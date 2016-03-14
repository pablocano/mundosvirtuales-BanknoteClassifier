#include "RobotPercept.h"
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <ostream>

void RobotPercept::draw(cv::Mat &image) const
{
    int fontFace = cv::FONT_HERSHEY_SCRIPT_SIMPLEX;
    double fontScale = 0.3f;
    int thickness = 0.5;
    cv::circle(image, cv::Point(306,466), 5, cv::Scalar(255,0,0));
    for(auto& robot : robots)
    {
        std::stringstream textStream;
        textStream << robot.posInField.x <<  ", " << robot.posInField.y;
        std::string text = textStream.str();
        int baseline=0;
        cv::Size textSize = cv::getTextSize(text, fontFace,
                                    fontScale, thickness, &baseline);
        baseline += thickness;

        // center the text
        cv::Point textOrg(robot.center.y - textSize.width/2,
                      robot.center.x + textSize.height/2);

        cv::putText(image, text, textOrg, fontFace, fontScale,
                cv::Scalar::all(0), thickness, 8);
        cv::rectangle(image,cv::Point2i(robot.leftUpper.y, robot.leftUpper.x),cv::Point2i(robot.rightBottom.y, robot.rightBottom.x), cv::Scalar(255,0,0));
    }
}
