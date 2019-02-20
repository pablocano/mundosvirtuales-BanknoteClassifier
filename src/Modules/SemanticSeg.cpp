
#include <fstream>
#include <iostream>
#include <sstream>
#include <cstdlib>
#include "SemanticSeg.h"
#include "Tools/Debugging/Debugging.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "Platform/File.h"

MAKE_MODULE(SemanticSeg, BanknoteClassifier)


SemanticSeg::SemanticSeg()
{

    if (torch::cuda::is_available())
    {
        string darknetFolder = string(File::getBCDir()) + "/Config/LibTorch/";
        const string traceFile = darknetFolder + "generativeSim.pt";
        //const char *cstr = traceFile.c_str();

        device_type = torch::kCUDA;
        torch::Device device(device_type);
        moduleTorch = torch::jit::load(traceFile);
        moduleTorch->to(at::kCUDA);
    }
    else
    {
        ASSERT(false);
    }

//assert(moduleTorch != nullptr);

}

void SemanticSeg::update(SegmentedImage &image)
{

    auto output = (moduleTorch->forward({torch::from_blob(theImageBGR.data, {3, 512, 1024}, at::kFloat).to(at::kCUDA)}).toTensor());

    //.slice(/*dim=*/0, /*start=*/0, /*end=*/1); //[1, 3 , 256 512]

    cv::Mat image2(512,1024,CV_8UC3);
    image2.data=&((output.data<unsigned char>())[0]);

    DRAW_
}
