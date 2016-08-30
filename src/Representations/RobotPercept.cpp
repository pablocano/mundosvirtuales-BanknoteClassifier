#include "RobotPercept.h"
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <ostream>

void RobotPercept::draw(cv::Mat &image) const
{
    int fontFace = cv::FONT_HERSHEY_SCRIPT_SIMPLEX;
    double fontScale = 0.3f;
    int thickness = 0.5;
    for(auto& robot : robots)
    {
        std::stringstream textStream;
        textStream << robot.center.x <<  ", " << robot.center.y;
        std::string text = textStream.str();
        int baseline=0;
        cv::Size textSize = cv::getTextSize(text, fontFace,
                                    fontScale, thickness, &baseline);
        baseline += thickness;

        // center the text
        cv::Point textOrg(robot.centerInImage.x - textSize.width/2,
                      robot.centerInImage.y + textSize.height/2);

        cv::putText(image, text, textOrg, fontFace, fontScale,
                cv::Scalar::all(0), thickness, 8);
        cv::rectangle(image,cv::Point2i(robot.leftUpper.x, robot.leftUpper.y),cv::Point2i(robot.rightBottom.x, robot.rightBottom.y), cv::Scalar(255,0,0));
    }
}
