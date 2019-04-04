#include "Camera.h"
#include "Platform/File.h"
#include "Tools/Debugging/Debugging.h"
#include <opencv2/cudaimgproc.hpp>
#include <sstream>
#include <string>
#include <regex>

#ifdef WINDOWS
#define VALID_PATH(s) std::regex_replace(s, std::regex("\/"), "\\\\")
#else
#define VALID_PATH(s) s
#endif 

MAKE_MODULE(Camera, BaslerCamera)

Camera* Camera::theInstance = 0;

Camera::Camera() : cameraLoaded(false)
{

  theInstance = this;

  cv::FileStorage cameraCalibrationFile(std::string(File::getBCDir()) + "/Config/cameracalibration.xml", cv::FileStorage::READ);

  cameraCalibrationFile["camera_matrix"] >> info.K;
  cameraCalibrationFile["distortion_coefficients"] >> info.d;

  //Needed for the basler camera to work
  Pylon::PylonInitialize();

  try
  {
    // Find the type of camera of the current camera
    //Pylon::CDeviceInfo info;
    //info.SetDeviceClass( Pylon::CBaslerUsbInstantCamera::DeviceClass());

    // Find the first camera of the previous type
    //camera = new Pylon::CBaslerUsbInstantCamera(Pylon::CTlFactory::GetInstance().CreateFirstDevice(info));
    camera = new Pylon::CInstantCamera(Pylon::CTlFactory::GetInstance().CreateFirstDevice());

    // Print the model name of the camera.
    OUTPUT_TEXT("Using device " + camera->GetDeviceInfo().GetModelName());

    // Initialization of a pylon image
    grabbedImage = new Pylon::CPylonImage();        // Open the camera
    camera->Open();

    // Load the persistent configuration
    //std::string nodeFile = std::string(File::getGTDir()) + "/Config/acA2040-90uc_22313646.pfs";
    //std::string nodeFile = std::string(File::getGTDir()) + "/Config/ubuntu_config.pfs";
    std::string nodeFile = std::string(File::getBCDir()) + "/Config/2019-02-21.pfs";
    Pylon::CFeaturePersistence::Load(nodeFile.c_str(), &camera->GetNodeMap(), true );

    // Initialice the pixel converter
    fc = new Pylon::CImageFormatConverter();
    fc->OutputPixelFormat = Pylon::PixelType_BGR8packed;

    // Initialization of a pylon image
    grabbedImage = new Pylon::CPylonImage();

    // Start the adquisition of images
    camera->StartGrabbing(Pylon::GrabStrategy_LatestImageOnly);

    cameraLoaded = true;
  }
  catch (GenICam::GenericException &e)
  {
    cameraLoaded = false;
    camera = nullptr;
    fc = nullptr;
    std::cerr << "An exception occurred." << std::endl << e.GetDescription() << std::endl;
  }
}
Camera::~Camera()
{
  if(camera)
  {
    camera->StopGrabbing();
    camera->Close();
    delete camera;
    delete fc;
    delete grabbedImage;
  }

  Pylon::PylonTerminate();
}

void Camera::update(CameraInfo& cameraInfo)
{
  cameraInfo = info;
}

void Camera::update(Image& image)
{
  if(!cameraLoaded)
    return;

  Pylon::CGrabResultPtr ptrGrabResult;
  while(!camera->IsGrabbing())
    cv::waitKey(1);

  try{
    do{
      camera->RetrieveResult( 5000, ptrGrabResult, Pylon::TimeoutHandling_ThrowException);

    }
    while(!ptrGrabResult->GrabSucceeded());

    // Image grabbed successfully?
    if (ptrGrabResult->GrabSucceeded())
    {
      fc->Convert(*grabbedImage, ptrGrabResult);
      currentImage = cv::Mat(ptrGrabResult->GetHeight(), ptrGrabResult->GetWidth(), CV_8UC3,(uint8_t*)grabbedImage->GetBuffer());
    }
  }
  catch (GenICam::GenericException &e)
  {
    std::cerr << "An exception occurred." << std::endl << e.GetDescription() << std::endl;
  }

  image = currentImage.clone();

  image.timeStamp = theFrameInfo.time;
  
}

CameraInfo& Camera::getCameraInfo()
{
  if(theInstance)
    return theInstance->info;
}
