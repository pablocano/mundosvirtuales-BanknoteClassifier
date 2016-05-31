/**
 * @file FieldCalibrator.cpp
 * FieldCalibrator based on perspective projections
 * @author Matías Mattamala
 */

#include <iostream>
#include <time.h>
#include <stdio.h>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "Representations/FieldDimensions.h"

#define CAM1 1
#define CAM2 0

using namespace cv;
using namespace std;

// program state enum
enum
{
    IDLE = 0,
    CALIBRATING = 1,
    DONE = 3
};

// rotation enum
enum
{
    CLOCKWISE = 90,
    COUNTERCLOCKWISE = -90
};

// Field Dimensions
FieldDimensions fieldDimensions;

static int width = 640;
static int height = 480;

static int xMouse1;
static int yMouse1;
static int xMouse2;
static int yMouse2;

// vector of captured points
int cam1Counter = 0;
int cam2Counter = 0;
std::vector<cv::Point2d> cam1Points;
std::vector<cv::Point2d> cam2Points;

bool isCam1Ready = false;
bool isCam2Ready = false;

// vectors of real 3D points
std::vector<cv::Point3d> field1Points;
std::vector<cv::Point3d> field2Points;


// calibrating info
// camera 1
cv::Mat image1, image2;
cv::Mat K1, d1;
// camera 2
cv::Mat K2, d2;

void readCameraCalibration(int cam)
{
    if(cam == 1)
    {
        cv::FileStorage file("../../Config/cameraCalibrationTmp1.yml", cv::FileStorage::READ);
        if(!file.isOpened())
        {
          std::cout << "Could not open the camera calibration file"<< std::endl;
        }
        file["Camera_Matrix" ] >> K1;
        file["Distortion_Coefficients"] >> d1;
        file.release();

        cout << K1 << endl;
        cout << d1 << endl;
    }
    else
    {
        cv::FileStorage file("../../Config/cameraCalibrationTmp2.yml", cv::FileStorage::READ);
        if(!file.isOpened())
        {
          std::cout << "Could not open the camera calibration file"<< std::endl;
        }
        file["Camera_Matrix" ] >> K2;
        file["Distortion_Coefficients"] >> d2;
        file.release();

        cout << K2 << endl;
        cout << d2 << endl;
    }
}

void rotateImage90(cv::Mat &src, cv::Mat &dst, int angle)
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

void combineImages(cv::Mat& img1, cv::Mat& img2, cv::Mat& combined)
{
    combined = cv::Mat(img1.rows, 2*img1.cols, img1.type());

    combined.adjustROI(0, 0, 0, -img1.cols);
    img1.copyTo(combined);
    combined.adjustROI(0,0,0,height);
    combined.adjustROI(0, 0, -img1.cols, img1.cols);
    img2.copyTo(combined);
    combined.adjustROI(0,0, img1.cols, 0);
}


cv::Point img1_P1(0,0);
cv::Point img1_P2(0,0);
cv::Point img2_P1(0,0);
cv::Point img2_P2(0,0);
bool clicked1 = false;
bool clicked2 = false;

void showImage(char cam)
{
    if(cam == 1)
    {
        cv::Mat img = image1.clone();
        line(img, img1_P1, img1_P2, Scalar(0, 0, 255), 3);
        imshow("cam1", img);
    }
    else if (cam == 2)
    {
        cv::Mat img = image2.clone();
        line(img, img2_P1, img2_P2, cv::Scalar(0, 0, 255), 3);
        imshow("cam2", img);
    }
    else
    {
        cout << "Error displaying the image" << endl;
    }
}

void onMouseRuler1( int event, int x, int y, int flags, void* userdata)
{
    switch(event){

        case  CV_EVENT_LBUTTONDOWN:
            clicked1 = true;
            img1_P1.x = x;
            img1_P1.y = y;
            img1_P2.x = x;
            img1_P2.y = y;
            cout << "Camera 1 - Mouse position (" << x << ", " << y << ")" << endl;
            break;

        case  CV_EVENT_LBUTTONUP:
            img1_P2.x = x;
            img1_P2.y = y;
            clicked1 = false;
            break;

        case  CV_EVENT_MOUSEMOVE:
            if(clicked1){
                img1_P2.x = x;
                img1_P2.y = y;
            }
            break;

        default: break;
    }
    if(clicked1)
        showImage(1);
}

void onMouseRuler2( int event, int x, int y, int flags, void* userdata)
{
    switch(event){

        case  CV_EVENT_LBUTTONDOWN:
            clicked2 = true;
            img2_P1.x = x;
            img2_P1.y = y;
            img2_P2.x = x;
            img2_P2.y = y;
            cout << "Camera 2 - Mouse position (" << x << ", " << y << ")" << endl;
            break;

        case  CV_EVENT_LBUTTONUP:
            img2_P2.x = x;
            img2_P2.y = y;
            clicked2 = false;
            break;

        case  CV_EVENT_MOUSEMOVE:
            if(clicked2){
                img2_P2.x = x;
                img2_P2.y = y;
            }
            break;

        default: break;
    }
    if(clicked2)
        showImage(2);
}

void computeImageFieldRatiosAndSave()
{
    Point d1 = img1_P1-img1_P2;
    Point d2 = img2_P1-img2_P2;

    float distImg1 = sqrt(d1.dot(d1));
    float distImg2 = sqrt(d2.dot(d2));

    float fieldDist = 2*fieldDimensions.yLimit;

    float pix2World1 = fieldDist/distImg1;
    float pix2World2 = fieldDist/distImg2;

    cout << "Pixel-to-World, Cam1: " << pix2World1 << endl;
    cout << "Pixel-to-World, Cam2: " << pix2World2 << endl;


    // save configuration
    cv::FileStorage fs1( "../../Config/cameraCalibration1.yml", FileStorage::WRITE );
    cv::FileStorage fs2( "../../Config/cameraCalibration2.yml", FileStorage::WRITE );

    fs1 << "Camera_Matrix" << K1;
    fs1 << "Distortion_Coefficients" << d1;
    fs1 << "Pixel_to_World" << pix2World1;

    fs2 << "Camera_Matrix" << K2;
    fs2 << "Distortion_Coefficients" << d2;
    fs2 << "Pixel_to_World" << pix2World2;
}

// main program
int main(int argc, char* argv[])
{
    // some state variables
    int state = IDLE;

    // read camera calibration files
    readCameraCalibration(1);
    readCameraCalibration(2);

    // prepare cameras
    // open first camera
    cv::VideoCapture cam1(CAM1);
    cam1.set(CV_CAP_PROP_FRAME_HEIGHT, height);
    cam1.set(CV_CAP_PROP_FRAME_WIDTH, width);

    // open second camera
    cv::VideoCapture cam2(CAM2);
    cam2.set(CV_CAP_PROP_FRAME_HEIGHT, height);
    cam2.set(CV_CAP_PROP_FRAME_WIDTH, width);

    // create windows and set properties
    //Create a window
    namedWindow("cam1", WINDOW_NORMAL | CV_GUI_EXPANDED);
    namedWindow("cam2", WINDOW_NORMAL | CV_GUI_EXPANDED);
    namedWindow("combined", WINDOW_NORMAL);

    char key;
    cv::Mat img1, img2, undistorted1, undistorted2, rotated1, rotated2, combined;


    /** MAIN LOOP **/
    while (true){

        key = cv::waitKey(1);
        if(key == 27)
            break;

        if(state == IDLE)
        {
            // get images (we assume this run right everytime)
            cam1 >> img1;
            cam2 >> img2;

            undistorted1 = img1.clone();
            undistorted2 = img2.clone();

            cv::undistort(img1, undistorted1, K1, d1);
            cv::undistort(img2, undistorted2, K2, d2);

            rotateImage90(undistorted1, rotated1, CLOCKWISE);
            rotateImage90(undistorted2, rotated2, COUNTERCLOCKWISE);
            combineImages(rotated1, rotated2, combined);

            // show images
            imshow("cam1", rotated1);
            imshow("cam2", rotated2);
            imshow("combined", combined);

            if(key == 'c')
            {
                image1 = rotated1;
                image2 = rotated2;
                state = CALIBRATING;
            }
        }

        if(state == CALIBRATING)
        {
            setMouseCallback( "cam1", onMouseRuler1, NULL);
            setMouseCallback( "cam2", onMouseRuler2, NULL);

            // cancel calibrationg of field depth
            if(key == 'x')
            {
                computeImageFieldRatiosAndSave();
                state = IDLE;
            }
        }

    }

    return 0;
}
