
#include <fstream>
#include <iostream>
#include <sstream>
#include <cstdlib>
#include "SemanticSeg.h"
#include "Tools/Debugging/Debugging.h"
#include "Tools/Debugging/DebugImages.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "Platform/File.h"

#include <opencv2/imgproc/imgproc.hpp>

MAKE_MODULE(SemanticSeg, BanknoteClassifier)


SemanticSeg::SemanticSeg()
{

        char* datacfg = "/home/nicolas/barcode/mundosvirtuales-BanknoteClassifier/Config/Darknet/cfg/products.data";




        int names_size = 0;
        list *options = read_data_cfg(datacfg);
        char *name_list = option_find_str(options, "names", "data/names.list");
        names = get_labels_custom(name_list, &names_size); //get_labels(name_list);

        thresh = 0.25;
        hier_thresh = 0.5f;
        fullscreen = 0;

        alphabet = load_alphabet();
        char* cfgfile = "/home/nicolas/barcode/mundosvirtuales-BanknoteClassifier/Config/Darknet/cfg/yolov3-tiny-products.cfg";
        net=parse_network_cfg_custom(cfgfile, 1,1); // set batch=1

        char* weightfile = "/home/nicolas/barcode/mundosvirtuales-BanknoteClassifier/Config/Darknet/weights/yolov3-tiny-products_92000.weights";
        load_weights(&net, weightfile);

        fuse_conv_batchnorm(net);
        calculate_binary_weights(net);


        if (net.layers[net.n - 1].classes != names_size) {
            ASSERT(false);
            printf(" Error: in the file %s number of names %d that isn't equal to classes=%d in the file %s \n",
                name_list, names_size, net.layers[net.n - 1].classes, cfgfile);
            if (net.layers[net.n - 1].classes > names_size) getchar();
        }





        bufferImgIn= (float *)malloc(416*416*3 * sizeof(float));//espacio de entrada
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

            bufferImgIn[0*w*h + j*w + i] = (r)/(255.f);//noramlizacion (valor-media)/(std*255) y transposicion
            bufferImgIn[1*w*h + j*w + i] = (g)/(255.f);
            bufferImgIn[2*w*h + j*w + i] = (b)/(255.f);
        }
    }

    return;
}



void SemanticSeg::transposeToMat(cv::Mat tarjet, image source)//de (W,H,C)->(C,W,H)
{
    int h = tarjet.rows;
    int w = tarjet.cols;

    for(int j = 0; j < h; j++)
    {
        for(int i = 0; i < w; i++)
        {
            float b = source.data[2*w*h + j*w + i];
            float g = source.data[1*w*h + j*w + i];
            float r = source.data[0*w*h + j*w + i];

            tarjet.at<cv::Vec3b>(j, i)[2] = (unsigned char)((r)*(255.f));//noramlizacion (valor-media)/(std*255) y transposicion
            tarjet.at<cv::Vec3b>(j, i)[1] = (unsigned char)((g)*(255.f));
            tarjet.at<cv::Vec3b>(j, i)[0] = (unsigned char)((b)*(255.f));
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
            int index=(int)b;
            if(b==0 || b>5)
                continue;

            colored.at<cv::Vec3b>(j, i) = coloursMap[index-1];




        }
    }
    return;
}

int compare_by_lefts(const void *a_ptr, const void *b_ptr) {
    const detection* a = (detection*)a_ptr;
    const detection* b = (detection*)b_ptr;
    const float delta = (a->bbox.x - a->bbox.w/2) - (b->bbox.x - b->bbox.w/2);
    return delta < 0 ? -1 : delta > 0 ? 1 : 0;
}

void SemanticSeg::update(SegmentedImage &image)
{
    DECLARE_DEBUG_DRAWING("module:SemanticSeg:enable", "drawingOnImage");

    if(theImage.empty())
      return;

    cv::Mat resized;

    cv::resize(theImage, resized, cv::Size(416,416), 0, 0, cv::INTER_AREA);
    //imwrite("background.png",theImage);
    transpose(resized);

    struct image im;
    im.h = net.h;
    im.w = net.w;
    im.c = net.c;
    im.data = bufferImgIn;




    srand(2222222);
    char buff[256];
    int j;
    float nms=.45;
    //std::vector<boundingBox> bBoxes;
    struct image sized = resize_image(im, net.w, net.h);
    save_image(sized, "imleft");
    int letterbox = 0;
    layer l = net.layers[net.n-1];

    float *X = sized.data;
    network_predict(net, X);
    int nboxes = 0;
    detection *dets = get_network_boxes(&net, im.w, im.h, thresh, hier_thresh, 0, 1, &nboxes, letterbox);
    if (nms)
        do_nms_sort(dets, nboxes, l.classes, nms);


    draw_detections_v3(im, dets, nboxes, thresh, names, alphabet, l.classes, 0);
    qsort(dets, nboxes, sizeof(*dets), compare_by_lefts);

    std::vector<detection> dets_norm;

    int i;
    for (i = 0; i < nboxes; ++i) {
        int class_id = -1;
        float prob = 0;
        for (j = 0; j < l.classes; ++j) {
            if (dets[i].prob[j] > thresh && dets[i].prob[j] > prob) {
                prob = dets[i].prob[j];
                class_id = j;
            }
        }
        if (class_id >= 0) {
            dets[i].sort_class=class_id;
            dets_norm.push_back(dets[i]);
        }
    }
    free_detections(dets, nboxes);



    COMPLEX_IMAGE("semanticSegmentation")
    {
        cv::Mat imageRGB(416,416, CV_8UC3);//(theImage.rows,theImage.cols, CV_8UC3);
        cv::Mat imageFinal(2000,2000, CV_8UC3);
        imageRGB=0;
        transposeToMat(imageRGB, im);
        cv::resize(imageRGB, imageFinal, cv::Size(2000,2000), 0, 0, cv::INTER_CUBIC);

        //cv::resize(im, im,  cv::Size(1024,1024), 0, 0, cv::INTER_NEAREST);//nearest para no perder la clase
        //colored(im,imageRGB);//obtener imagen coloreada
        //cv::Mat BGRResized;
        //cv::resize(theImage, BGRResized,  cv::Size(1024,1024), 0, 0);
        //addWeighted( imageRGB, alpha, BGRResized, beta, 0.0, imageRGB);//superponer imagenes

        SEND_DEBUG_IMAGE("semanticSegmentation", imageFinal);
    }
    //free_image(im);
    //free_image(sized);


}


void SemanticSeg::draw(cv::Mat src, cv::Mat colored) //Obtener colores en RGB solo para debugeo
{

    return;
}




