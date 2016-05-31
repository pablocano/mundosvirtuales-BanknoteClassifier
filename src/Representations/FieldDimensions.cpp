#include "FieldDimensions.h"

#include <iostream>


FieldDimensions::FieldDimensions()
{
    cv::FileStorage file("../../Config/fieldDimensions.xml", cv::FileStorage::READ);

    if(!file.isOpened())
    {
      std::cout << "Could not open the field dimensions file"<< std::endl;
    }
    file["xLimit" ] >> xLimit;
    file["yLimit"] >> yLimit;
    file["xPenaltyAreaLimit"] >> xPenaltyAreaLimit;
    file["yPenaltyAreaLimit"] >> yPenaltyAreaLimit;
    file["lineWidth"] >> lineWidth;
    file["circleRadius"] >> circleRadius;

    imgCenter = Vector2<int>(0,0);
    scale = 1.f;

    // fill corner points
    p1.x =  xLimit;
    p1.y =  yLimit;

    p2.x = -xLimit;
    p2.y =  yLimit;

    p3.x = -xLimit;
    p3.y = -yLimit;

    p4.x =  xLimit;
    p4.y = -yLimit;

    p5.x = 0;
    p5.y =  yLimit;

    p6.x = 0;
    p6.y = -yLimit;

}

void FieldDimensions::draw(cv::Mat &image) const
{
    // adjust according to the image center
    cv::Point2d offset(lineWidth, lineWidth);

    cv::Point2d p1Img(p1.x + imgCenter.x, p1.y + imgCenter.y);
    cv::Point2d p2Img(p2.x + imgCenter.x, p2.y + imgCenter.y);
    cv::Point2d p3Img(p3.x + imgCenter.x, p3.y + imgCenter.y);
    cv::Point2d p4Img(p4.x + imgCenter.x, p4.y + imgCenter.y);
    cv::Point2d p5Img(p5.x + imgCenter.x, p5.y + imgCenter.y);
    cv::Point2d p6Img(p6.x + imgCenter.x, p6.y + imgCenter.y);



    cv::Scalar color(255,255,255);
    cv::line(image, p1Img+offset, p2Img+offset, color);
    cv::line(image, p1Img-offset, p2Img-offset, color);


}
