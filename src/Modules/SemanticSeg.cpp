
#include <fstream>
#include <iostream>
#include <sstream>
#include <cstdlib>
#include "SemanticSeg.h"
#include "Tools/Debugging/Debugging.h"
#include "Tools/Debugging/DebugImages.h".h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "Platform/File.h"

#include <opencv2/imgproc/imgproc.hpp>

MAKE_MODULE(SemanticSeg, BanknoteClassifier)


SemanticSeg::SemanticSeg()
{

    if (torch::cuda::is_available())//hay gpu
    {
        string darknetFolder = string(File::getBCDir()) + "/Config/LibTorch/";
        const string traceFile = darknetFolder + "billetatorSegmentor.pt";
        device_type = torch::kCUDA;
        torch::Device device(device_type);
        moduleTorch = torch::jit::load(traceFile);//parseo
        moduleTorch->to(at::kCUDA);//modelo a cuda

        bufferImgIn= (float *)malloc(512*1024*3 * sizeof(float));//espacio de entrada

        alpha = 0.3;//que tan transparente son los labels
        beta = ( 1.0 - alpha );
    }
    else
    {
        ASSERT(false);
    }

    ASSERT(moduleTorch != nullptr);

}

void SemanticSeg::transpose(cv::Mat src)//de (W,H,C)->(C,W,H)
{
    int h = src.rows;
    int w = src.cols;

    for(int j = 0; j < h; j++)
    {
        for(int i = 0; i < w; i++)
        {
            unsigned char b = src.at<cv::Vec3b>(j, i)[0];
            unsigned char g = src.at<cv::Vec3b>(j, i)[1];
            unsigned char r = src.at<cv::Vec3b>(j, i)[2];

            bufferImgIn[0*w*h + j*w + i] = (b-113.8041f)/(255.f*52.70879f);//noramlizacion (valor-media)/(std*255) y transposicion
            bufferImgIn[1*w*h + j*w + i] = (g-118.58932f)/(255.f*44.97723f);
            bufferImgIn[2*w*h + j*w + i] = (r-119.293884f)/(255.f*46.73076f);
        }
    }

    return;
}

void SemanticSeg::colored(cv::Mat src, cv::Mat colored) //Obtener colores en RGB solo para debugeo
{
    int h = src.rows;
    int w = src.cols;

    for(int j = 0; j < h; j++)
    {
        for(int i = 0; i < w; i++)
        {

            unsigned char b = src.at<uchar>(j,i);
            if(b==1)
            {
                colored.at<cv::Vec3b>(j, i)[0] = 0;
                colored.at<cv::Vec3b>(j, i)[1] = 255;
                colored.at<cv::Vec3b>(j, i)[2] = 0;
            }
            else if(b==2)
            {
                colored.at<cv::Vec3b>(j, i)[0] = 255;
                colored.at<cv::Vec3b>(j, i)[1] = 0;
                colored.at<cv::Vec3b>(j, i)[2] = 255;
            }
            else if(b==3)
            {
                colored.at<cv::Vec3b>(j, i)[0] = 0;
                colored.at<cv::Vec3b>(j, i)[1] = 0;
                colored.at<cv::Vec3b>(j, i)[2] = 255;
            }
            else if(b==4)
            {
                colored.at<cv::Vec3b>(j, i)[0] = 255;
                colored.at<cv::Vec3b>(j, i)[1] = 0;
                colored.at<cv::Vec3b>(j, i)[2] = 0;
            }
            else if(b==5)
            {
                colored.at<cv::Vec3b>(j, i)[0] = 0;
                colored.at<cv::Vec3b>(j, i)[1] = 255;
                colored.at<cv::Vec3b>(j, i)[2] = 255;
            }

        }
    }
    return;
}

void SemanticSeg::update(SegmentedImage &image)
{
    DECLARE_DEBUG_DRAWING("module:SemanticSeg:enable", "drawingOnImage");

    if(theImage.empty())
      return;

    cv::Mat netInput;
    cv::Mat resized;

    cv::resize(theImage, resized, cv::Size(1024,512), 0, 0);
    //imwrite("background.png",theImage);
    transpose(resized);

    auto output = (moduleTorch->forward({torch::from_blob(bufferImgIn, {1,3, 512, 1024}, at::kFloat).to(at::kCUDA)}).toTensor());//inferencia de la red en cuda

    cv::Mat netOut(512,1024, CV_8U);


    image.map.resize(Classification::numOfBanknotes);
    image.map[Classification::NONE] = 0;
    image.map[Classification::UNO_C] = 1;
    image.map[Classification::UNO_S] = 1;
    image.map[Classification::DOS_C] = 2;
    image.map[Classification::DOS_S] = 2;
    image.map[Classification::CINCO_C] = 3;
    image.map[Classification::CINCO_S] = 3;
    image.map[Classification::DIEZ_C] = 4;
    image.map[Classification::DIEZ_S] = 4;
    image.map[Classification::VEINTE_C] = 5;
    image.map[Classification::VEINTE_S] = 5;

    // de aca en adelante todo es dibujos
    netOut.data = output.data<unsigned char>();
    image = netOut.clone();

    COMPLEX_IMAGE("semanticSegmentation")
    {
        cv::Mat imageRGB(512,512, CV_8UC3);//(theImage.rows,theImage.cols, CV_8UC3);
        imageRGB=0;
        cv::resize(netOut, netOut,  cv::Size(512,512), 0, 0, cv::INTER_NEAREST);//nearest para no perder la clase
        colored(netOut,imageRGB);//obtener imagen coloreada
        cv::Mat BGRResized;
        cv::resize(theImage, BGRResized,  cv::Size(512,512), 0, 0);
        addWeighted( imageRGB, alpha, BGRResized, beta, 0.0, imageRGB);//superponer imagenes

        SEND_DEBUG_IMAGE("semanticSegmentation", imageRGB);
    }

}
