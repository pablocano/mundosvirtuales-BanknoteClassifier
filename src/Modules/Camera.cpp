#include "Camera.h"
#include "Tools/Debugging/Debugging.h"
#include <opencv2/imgproc/imgproc.hpp>
#include <sstream>

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

        // Initialice the pixel converter
        fc = new Pylon::CImageFormatConverter();
        fc->OutputPixelFormat = Pylon::PixelType_BGR8packed;

        // Initialization of a pylon image
        grabbedImage = new Pylon::CPylonImage();

        // Start the adquisition of images
        camera->StartGrabbing(Pylon::GrabStrategy_LatestImageOnly);
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

void Camera::update(ImageBGR& image)
{
    cv::Mat tmp;
    Pylon::CGrabResultPtr ptrGrabResult;
    while(!camera->IsGrabbing())
        cv::waitKey(1);

    try{
        camera->RetrieveResult( 5000, ptrGrabResult, Pylon::TimeoutHandling_ThrowException);

        // Image grabbed successfully?
        if (ptrGrabResult->GrabSucceeded())
        {
            fc->Convert(*grabbedImage, ptrGrabResult);
            tmp = cv::Mat(ptrGrabResult->GetHeight(), ptrGrabResult->GetWidth(), CV_8UC3,(uint8_t*)grabbedImage->GetBuffer());
        }
    }
    catch (GenICam::GenericException &e)
    {
        std::cerr << "An exception occurred." << std::endl << e.GetDescription() << std::endl;
    }


  //cv::resize(tmp,tmp,cv::Size(1500,750));

  // correct and rotate images
  //cv::undistort(tmp, undistorted, camerasInfo[index]->K, camerasInfo[index]->d);
  //rotateImage90(undistorted, rotated, index == 0? ANGLES::COUNTERCLOCKWISE : ANGLES::CLOCKWISE);
  image = ImageBGR(tmp);
  image.timeStamp = theFrameInfo.time;
  
  DEBUG_RESPONSE("representation:ImageBGR",
  {
    OUTPUT(idImage,image);
  });
  
}

void Camera::update(Image& image)
{
    cv::cvtColor(theImageBGR, image, CV_BGR2YCrCb);
}

void Camera::update(GrayScaleImage& image)
{
  cv::cvtColor(theImageBGR, image, CV_BGR2GRAY);
}
