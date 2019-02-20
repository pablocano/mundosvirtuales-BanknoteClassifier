
#include <fstream>
#include <iostream>
#include <sstream>
#include <cstdlib>
#include "SemanticSeg.h"
#include "Tools/Debugging/Debugging.h"
#include "Tools/Debugging/DebugDrawings.h"
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

            bufferImgIn[0*w*h + j*w + i] = (b-111.467575f)/(255.f*52.213707f);//noramlizacion (valor-media)/(std*255) y transposicion
            bufferImgIn[1*w*h + j*w + i] = (g-116.11815f)/(255.f*47.367764f);
            bufferImgIn[2*w*h + j*w + i] = (r-116.60477f)/(255.f*49.892235f);
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


    cv::Mat netInput;
    cv::Mat resized;

    cv::resize(theImageBGR, resized, cv::Size(1024,512), 0, 0);
    transpose(resized);

    auto output = (moduleTorch->forward({torch::from_blob(bufferImgIn, {1,3, 512, 1024}, at::kFloat).to(at::kCUDA)}).toTensor());//inferencia de la red en cuda

    cv::Mat netOut(512,1024, CV_8U);


    // de aca en adelante todo es dibujos
    netOut.data = output.data<unsigned char>();

    cv::Mat imageRGB(512,512, CV_8UC3);//(theImageBGR.rows,theImageBGR.cols, CV_8UC3);
    imageRGB=0;
    cv::resize(netOut, netOut,  cv::Size(512,512), 0, 0, cv::INTER_NEAREST);//nearest para no perder la clase
    colored(netOut,imageRGB);//obtener imagen coloreada
    cv::Mat BGRResized;
    cv::resize(theImageBGR, BGRResized,  cv::Size(512,512), 0, 0);
    addWeighted( imageRGB, alpha, BGRResized, beta, 0.0, imageRGB);//superponer imagenes

    CvMat outSeg = imageRGB;
    DRAW_IMAGE("semanticSegmentation", outSeg, theFrameInfo.time);
}