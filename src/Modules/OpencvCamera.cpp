#include "OpencvCamera.h"
#include "Tools/Debugging/Debugging.h"
#include <opencv2/imgproc/imgproc.hpp>
#include <sstream>

MAKE_MODULE(OpencvCamera, Common2)

OpencvCamera::OpencvCamera(): index(0)
{
    /**
     * Read image configuration
     * */
    cv::FileStorage file( std::string(File::getGTDir())+"/Config/cameraConfig.xml", cv::FileStorage::READ);
    if(!file.isOpened())
    {
      std::cout << "Could not open the camera configuration file"<< std::endl;
    }
    file["imgWidth" ] >> width;
    file["imgHeight"] >> height;
    file.release();


    /**
     * Prepare cameras
    */
    //video0 = cv::VideoCapture(0);
    video0 = cv::VideoCapture(std::string(File::getGTDir()) + "/Data/vid/caja_muchos.mp4");
    if(!video0.isOpened())  // check if we succeeded
    {
        cam1.available = false;
        std::cout << "Camera 1 is unavailable"<< std::endl;
    }
    else // the camera is available
    {
        std::cout << "Camera 1 was successfully opened"<< std::endl;
        cam1.available = true;

        video0.set(CV_CAP_PROP_FRAME_HEIGHT, height);
        video0.set(CV_CAP_PROP_FRAME_WIDTH, width);
    }
    // open second camera
    //video1 = cv::VideoCapture(1);
    if(!video1.isOpened())  // check if we succeeded
    {
        cam2.available = false;
        std::cout << "Camera 2 is unavailable"<< std::endl;
    }
    else // the camera is available
    {
        std::cout << "Camera 2 was successfully opened"<< std::endl;
        cam2.available = true;
        video1.set(CV_CAP_PROP_FRAME_HEIGHT, height);
        video1.set(CV_CAP_PROP_FRAME_WIDTH, width);
    }

    numCameras = (cam1.available? 1 : 0) + (cam2.available? 1 : 0);
    // std::cout << "numCameras: " << numCameras << std::endl;

    /**
     * Prepare Camera Info
     * */
    cv::Mat K, d;
    cv::Point fieldCenter;
    float pix2World;
    // Load Camera 1 config
    cv::FileStorage file1( std::string(File::getGTDir()) + "/Config/cameraCalibration1.yml", cv::FileStorage::READ);
    if(!file1.isOpened())
    {
      std::cout << "Could not open the camera 1 calibration file"<< std::endl;
    }
    file1["Camera_Matrix" ] >> K;
    file1["Distortion_Coefficients"] >> d;
    file1["Pixel_to_World"] >> pix2World;
    file1["Field_Center"] >> fieldCenter;
    file1.release();
    cam1 = CameraInfo(CameraInfo::eastCam, "Camera 1", K, d, fieldCenter, pix2World);


    // Load Camera 2 config
    cv::FileStorage file2( std::string(File::getGTDir()) + "/Config/cameraCalibration2.yml", cv::FileStorage::READ);
    if(!file2.isOpened())
    {
      std::cout << "Could not open the camera 2 calibration file"<< std::endl;
    }
    file2["Camera_Matrix" ] >> K;
    file2["Distortion_Coefficients"] >> d;
    file2["Pixel_to_World"] >> pix2World;
    file2["Field_Center"] >> fieldCenter;
    file2.release();
    cam2 = CameraInfo(CameraInfo::westCam, "Camera 2", K, d, fieldCenter, pix2World);

    // fill the arrays
    cameras[0] = &video0;
    cameras[1] = &video1;
    camerasInfo[0] = &cam1;
    camerasInfo[1] = &cam2;
}

void OpencvCamera::update(CameraInfo& cameraInfo)
{
  index = (index + 1)%numCameras;
  cameraInfo = *camerasInfo[index];
}

void OpencvCamera::update(ImageBGR& image)
{
  cv::Mat tmp, undistorted;
  int i = 0;
  do{
    *cameras[index] >> tmp;
    i++;
    if (tmp.empty()) {
        cameras[index]->set(CV_CAP_PROP_POS_AVI_RATIO , 0);
        *cameras[index] >> tmp;
    }
  }
  while(tmp.empty() || i < 0);

  cv::resize(tmp,tmp,cv::Size(1500,750));

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

void OpencvCamera::update(Image& image)
{
    cv::cvtColor(theImageBGR, image, CV_BGR2YCrCb);
}

void OpencvCamera::update(GrayScaleImage& image)
{
  cv::cvtColor(theImageBGR, image, CV_BGR2GRAY);
}


void OpencvCamera::rotateImage90(cv::Mat &src, cv::Mat &dst, int angle)
{
    dst.create(src.size(), src.type());
    if(angle == 270 || angle == -90){
        // Rotate clockwise 270 degrees
        cv::transpose(src, dst);
        cv::flip(dst, dst, 0);
    }else if(angle == 180 || angle == -180){
        // Rotate clockwise 180 degrees
        cv::flip(src, dst, -1);
    }else if(angle == 90 || angle == -270){
        // Rotate clockwise 90 degrees
        cv::transpose(src, dst);
        cv::flip(dst, dst, 1);
    }else if(angle == 360 || angle == 0){
        if(src.data != dst.data){
            src.copyTo(dst);
        }
    }
}
