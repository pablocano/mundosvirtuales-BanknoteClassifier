#include "Camera.h"
#include "Tools/Debugging/Debugging.h"
#include <opencv2/imgproc/imgproc.hpp>
#include <sstream>
#include <string>
#include <regex>

#ifdef WINDOWS
	#define VALID_PATH(s) std::regex_replace(s, std::regex("\/"), "\\\\")
#else
	#define VALID_PATH(s) s
#endif 

MAKE_MODULE(Camera, Common)

Camera::Camera()
{
    //Needed for the basler camera to work
    Pylon::PylonInitialize();

    try
    {
        // Find the type of camera of the current camera
        Pylon::CDeviceInfo info;
        info.SetDeviceClass( Pylon::CBaslerUsbInstantCamera::DeviceClass());

        // Find the first camera of the previous type
        camera = new Pylon::CBaslerUsbInstantCamera(Pylon::CTlFactory::GetInstance().CreateFirstDevice(info));

        // Open the camera
        camera->Open();

        // Load the persistent configuration
        std::string nodeFile = std::string(File::getGTDir()) + "/Config/NodeMap.pfs";
        Pylon::CFeaturePersistence::Load(nodeFile.c_str(), &camera->GetNodeMap(), true );

        // Initialice the pixel converter
        fc = new Pylon::CImageFormatConverter();
        fc->OutputPixelFormat = Pylon::PixelType_BGR8packed;

        // Initialization of a pylon image
        grabbedImage = new Pylon::CPylonImage();

        // Start the adquisition of images
        camera->StartGrabbing(Pylon::GrabStrategy_LatestImageOnly);

        std::cout << "Camera OK" << std::endl;
    }
    catch (GenICam::GenericException &e)
    {
        std::cerr << "An exception occurred." << std::endl << e.GetDescription() << std::endl;
    }
}
Camera::~Camera()
{
    camera->StopGrabbing();
    camera->Close();

    delete camera;

    Pylon::PylonTerminate();
}

void Camera::update(CameraInfo& cameraInfo)
{
  cameraInfo = info;
}

void Camera::update(Image& image)
{
    Pylon::CGrabResultPtr ptrGrabResult;
    while(!camera->IsGrabbing())
        cv::waitKey(1);

    try{
        camera->RetrieveResult( 5000, ptrGrabResult, Pylon::TimeoutHandling_ThrowException);

        // Image grabbed successfully?
        if (ptrGrabResult->GrabSucceeded())
        {
            std::cout << "new image" << std::endl;
            fc->Convert(*grabbedImage, ptrGrabResult);
            currentImage = cv::Mat(ptrGrabResult->GetHeight(), ptrGrabResult->GetWidth(), CV_8UC3,(uint8_t*)grabbedImage->GetBuffer());
        }
    }
    catch (GenICam::GenericException &e)
    {
        std::cerr << "An exception occurred." << std::endl << e.GetDescription() << std::endl;
    }

    currentImage.timeStamp = theFrameInfo.time;

    DEBUG_RESPONSE("representation:ImageBGR",
    {
        OUTPUT(idImage,currentImage);
    });

    cv::cvtColor(currentImage, image, CV_BGR2YCrCb);
  
}

void Camera::update(GrayScaleImage& image)
{
    cv::extractChannel(theImage,image, 0);
}
