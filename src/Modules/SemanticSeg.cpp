
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

        //alphabet = load_alphabet();
        char* cfgfile = "/home/nicolas/barcode/mundosvirtuales-BanknoteClassifier/Config/Darknet/cfg/yolov3-tiny-products.cfg";
        net=parse_network_cfg_custom(cfgfile, 1,1); // set batch=1

        char* weightfile = "/home/nicolas/barcode/mundosvirtuales-BanknoteClassifier/Config/Darknet/weights/yolov3-tiny-products_50000.weights";
        load_weights(&net, weightfile);

        fuse_conv_batchnorm(net);
        calculate_binary_weights(net);


        if (net.layers[net.n - 1].classes != names_size) {
            ASSERT(false);
            printf(" Error: in the file %s number of names %d that isn't equal to classes=%d in the file %s \n",
                name_list, names_size, net.layers[net.n - 1].classes, cfgfile);
            if (net.layers[net.n - 1].classes > names_size) getchar();
        }



        im.h = net.h;
        im.w = net.w;
        im.c = net.c;

        bufferImgIn= (float *)malloc(net.h*net.w*net.c * sizeof(float));//espacio de entrada



        char* datacfgV3 = "/home/nicolas/barcode/mundosvirtuales-BanknoteClassifier/Config/Darknet/cfg/bar.data";
        int names_sizeV3 = 0;
        list *optionsV3 = read_data_cfg(datacfgV3);
        char *name_listV3 = option_find_str(optionsV3, "names", "data/names.list");
        namesV3 = get_labels_custom(name_listV3, &names_sizeV3); //get_labels(name_list);

        char* cfgfileV3 = "/home/nicolas/barcode/mundosvirtuales-BanknoteClassifier/Config/Darknet/cfg/yolov3.cfg";
        netV3=parse_network_cfg_custom(cfgfileV3, 1,1); // set batch=1

        char* weightfileV3 = "/home/nicolas/barcode/mundosvirtuales-BanknoteClassifier/Config/Darknet/weights/yolov3_9000.weights";
        load_weights(&netV3, weightfileV3);

        fuse_conv_batchnorm(netV3);
        calculate_binary_weights(netV3);


        if (netV3.layers[netV3.n - 1].classes != names_sizeV3) {
            ASSERT(false);
            printf(" Error: in the file %s number of names %d that isn't equal to classes=%d in the file %s \n",
                name_listV3, names_sizeV3, netV3.layers[netV3.n - 1].classes, cfgfileV3);
            if (netV3.layers[netV3.n - 1].classes > names_sizeV3) getchar();
        }



        bufferImgInV3= (float *)malloc(netV3.h*netV3.w*netV3.c * sizeof(float));
}





int compare_by_lefts(const void *a_ptr, const void *b_ptr) {
    const detection* a = (detection*)a_ptr;
    const detection* b = (detection*)b_ptr;
    const float delta = (a->bbox.x - a->bbox.w/2) - (b->bbox.x - b->bbox.w/2);
    return delta < 0 ? -1 : delta > 0 ? 1 : 0;
}

void SemanticSeg::update(SegmentedImage &image)
{
    DECLARE_DEBUG_DRAWING("module:SemanticSeg", "drawingOnImage");

    if(theImage.empty())
      return;

    cv::Mat resized;

    cv::resize(theImage, resized, cv::Size(net.h,net.w), 0, 0, cv::INTER_AREA);
    //imwrite("background.png",theImage);
    transpose(resized);



    im.data = bufferImgIn;

    std::vector<detection> dets_norm;

    predict(dets_norm, net);

    std::vector<detection> dets_normV3;

    predict(dets_normV3, netV3);


    //free_image(im);
    draw(dets_norm);
    draw(dets_normV3);

}



void SemanticSeg::predict(std::vector<detection>& dets_norm, network choosenNet) //Obtener colores en RGB solo para debugeo
{
    srand(2222222);
    char buff[256];
    int j;
    float nms=.45;
    //std::vector<boundingBox> bBoxes;
    //save_image(sized, "imleft");
    int letterbox = 0;
    layer l = choosenNet.layers[choosenNet.n-1];

    float *X = im.data;
    network_predict(choosenNet, X);
    int nboxes = 0;
    detection *dets = get_network_boxes(&choosenNet, im.w, im.h, thresh, hier_thresh, 0, 1, &nboxes, letterbox);
    if (nms)
        do_nms_sort(dets, nboxes, l.classes, nms);


    //draw_detections_v3(im, dets, nboxes, thresh, names, alphabet, l.classes, 0);
    qsort(dets, nboxes, sizeof(*dets), compare_by_lefts);


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

void SemanticSeg::draw(std::vector<detection> dets_norm) //Obtener colores en RGB solo para debugeo
{

    int imageHeight=2046;
    int imageWidth=2040;
    for(int j=0;j<dets_norm.size();j++)
    {
       box b = dets_norm[j].bbox;

       int leftImg  = (b.x-b.w/2.f)*imageWidth;
       int rightImg = (b.x+b.w/2.f)*imageWidth;
       int topImg   = (b.y-b.h/2.f)*imageHeight;
       int botImg   = (b.y+b.h/2.f)*imageHeight;

       if(leftImg < 0) leftImg = 0;
       if(rightImg > imageHeight-1) rightImg = imageHeight-1;
       if(topImg < 0) topImg = 0;
       if(botImg > imageWidth-1) botImg = imageWidth-1;


       LINE("module:SemanticSeg", leftImg, topImg , leftImg, botImg, 3, Drawings::solidPen, ColorRGBA::green );
       LINE("module:SemanticSeg", leftImg, botImg , rightImg, botImg, 3, Drawings::solidPen, ColorRGBA::green );
       LINE("module:SemanticSeg", rightImg, topImg , rightImg, botImg, 3, Drawings::solidPen, ColorRGBA::green );
       LINE("module:SemanticSeg", leftImg, topImg , rightImg, topImg, 3, Drawings::solidPen, ColorRGBA::green );


    }

    // 3 channels RGB
    return;
}




