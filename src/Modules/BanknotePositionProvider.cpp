#include "BanknotePositionProvider.h"
#include <opencv2/highgui.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/highgui.hpp>
#include "Tools/Debugging/DebugDrawings.h"
#include <algorithm>
#include <iostream>

MAKE_MODULE(BanknotePositionProvider, BanknoteClassifier)

bool nonzero(int i) { return i !=0;}

void resize_image(std::map<std::string,cv::Mat>* dict, float scale, cv::Ptr<cv::CLAHE>* clahe_){
    for(auto& x : *dict){
        std::cout<<x.first<<std::endl;
        //resize
        cv::resize(x.second,x.second,cv::Size(), scale, scale, CV_INTER_AREA);

        //Equalize histogram
        (*clahe_)->apply(x.second, x.second);
    }
}

std::vector<cv::Point2f> create_corners(cv::Mat* img_, std::vector<cv::Point2f>* obj_corners_){
    obj_corners_->resize(4);
    (*obj_corners_)[0] = cv::Point(0,0);
    (*obj_corners_)[1] = cv::Point( (*img_).cols, 0 );
    (*obj_corners_)[2] = cv::Point( (*img_).cols, (*img_).rows );
    (*obj_corners_)[3] = cv::Point( 0, (*img_).rows );
    return *obj_corners_;

}

std::string compare(const cv::Mat* des_query,std::map<std::string, cv::Mat>* dict_des_model,cv::BFMatcher* matcher, std::vector<cv::DMatch>*matches){
    std::string aux_name = "Ninguno";
    int aux_n_matches = 0;
    std::vector<cv::DMatch> aux_matches;

    for(auto& x : *dict_des_model){
        matcher->match(*des_query,x.second,aux_matches);

        //ordenar maches, tomar 50 primeros y promediar

        if(aux_matches.size()>aux_n_matches){

            aux_name = x.first;
            std::cout<<aux_name<<std::endl;
            aux_n_matches = aux_matches.size();
            *matches = aux_matches;
        }
    }

    return aux_name;
}



BanknotePositionProvider::BanknotePositionProvider()
{
    imageModel["1000_Cara"] = cv::imread(std::string(File::getGTDir())+"/Config/Banknotes/mil_real.jpg", CV_LOAD_IMAGE_GRAYSCALE);
    imageModel["1000_Sello"]= cv::imread(std::string(File::getGTDir())+"/Config/Banknotes/milB_real.jpg", CV_LOAD_IMAGE_GRAYSCALE);
    imageModel["2000_Cara"] = cv::imread(std::string(File::getGTDir())+"/Config/Banknotes/dosmil_real.jpg", CV_LOAD_IMAGE_GRAYSCALE);
    imageModel["2000_Sello"] = cv::imread(std::string(File::getGTDir())+"/Config/Banknotes/dosmilB_real.jpg", CV_LOAD_IMAGE_GRAYSCALE);
    imageModel["5000_Cara"] = cv::imread(std::string(File::getGTDir())+"/Config/Banknotes/cincomil_real.jpg", CV_LOAD_IMAGE_GRAYSCALE);
    imageModel["5000_Sello"] = cv::imread(std::string(File::getGTDir())+"/Config/Banknotes/cincomilB_real.jpg", CV_LOAD_IMAGE_GRAYSCALE);
    imageModel["10000_Cara"] = cv::imread(std::string(File::getGTDir())+"/Config/Banknotes/diezmil_real.jpg", CV_LOAD_IMAGE_GRAYSCALE);
    imageModel["10000_Sello"] = cv::imread(std::string(File::getGTDir())+"/Config/Banknotes/diezmilB_real.jpg", CV_LOAD_IMAGE_GRAYSCALE);
    imageModel["20000_Cara"] = cv::imread(std::string(File::getGTDir())+"/Config/Banknotes/veintemil_real.jpg", CV_LOAD_IMAGE_GRAYSCALE);
    imageModel["20000_Sello"] = cv::imread(std::string(File::getGTDir())+"/Config/Banknotes/veintemilB_real.jpg", CV_LOAD_IMAGE_GRAYSCALE);

    // CLAHE
    clahe = cv::createCLAHE(2.0, cv::Size(4,4));

    resize_image(&imageModel, 0.18, &clahe);

    //Matcher
    matcher.create(cv::NORM_L2, true);


    //Descriptor
    surf = cv::xfeatures2d::SURF::create(400,3,2,false,false);
    for(auto& img : imageModel){
        surf->detectAndCompute(img.second,cv::noArray(),keyPointsModel1A,descriptorModel1A,false);
        dict_kp[img.first] = keyPointsModel1A;
        dict_des[img.first] = descriptorModel1A;

        dict_corners[img.first] = create_corners(&(img.second), &obj_corners);

    }
}

void BanknotePositionProvider::update(BanknotePosition &banknotePosition)
{
    DECLARE_DEBUG_DRAWING("module:BanknotePosition:position","drawingOnImage");


    if (!theFeatures.descriptors.empty()){
        //Matching
        std::string best_match = compare(&(theFeatures.descriptors), &dict_des, &matcher, &matches);
        //-- Localize the object
        std::vector<cv::Point2f> obj;
        std::vector<cv::Point2f> scene;
        for( int i = 0; i < matches.size(); i++ ){
            //-- Get the keypoints from the good matches
            obj.push_back( (dict_kp[best_match])[ matches[i].trainIdx ].pt );
            scene.push_back( theFeatures.keypoints[ matches[i].queryIdx ].pt );
        }
        std::vector<int> mask;
        cv::Mat H = cv::findHomography(obj, scene, mask,CV_RANSAC,5.0 );

        //int count = count_if(mask.begin(),mask.end(),nonzero);


        if (!H.empty()){
            std::vector<cv::Point2f> scene_corners(4);
            perspectiveTransform( dict_corners[best_match], scene_corners, H);
            //-- Draw lines between the corners (the mapped object in the scene - image_2 )
            LINE( "module:BanknotePosition:position", scene_corners[0].x, scene_corners[0].y , scene_corners[1].x, scene_corners[1].y, 3, Drawings::ps_solid, ColorRGBA::red );
            LINE( "module:BanknotePosition:position", scene_corners[1].x, scene_corners[1].y , scene_corners[2].x, scene_corners[2].y, 3, Drawings::ps_solid, ColorRGBA::red );
            LINE( "module:BanknotePosition:position", scene_corners[2].x, scene_corners[2].y , scene_corners[3].x, scene_corners[3].y, 3, Drawings::ps_solid, ColorRGBA::red );
            LINE( "module:BanknotePosition:position", scene_corners[3].x, scene_corners[3].y , scene_corners[0].x, scene_corners[0].y, 3, Drawings::ps_solid, ColorRGBA::red );

        }


    }










    /*std::vector<cv::DMatch> matches;
    matcher.match(descriptorModel1A, theFeatures.descriptors,matches);

    double max_dist = 0; double min_dist = 1000;

    //-- Quick calculation of max and min distances between keypoints
    for( int i = 0; i < descriptorModel1A.rows; i++ )
    {
        double dist = matches[i].distance;
        if( dist < min_dist ) min_dist = dist;
        if( dist > max_dist ) max_dist = dist;
    }

    //-- Draw only "good" matches (i.e. whose distance is less than 3*min_dist )
    std::vector< cv::DMatch > good_matches;

    for( int i = 0; i < descriptorModel1A.rows; i++ )
    {
        if( matches[i].distance < 3*min_dist )
        {
            good_matches.push_back( matches[i]);
        }
    }

    //-- Localize the object
    std::vector<cv::Point2f> obj;
    std::vector<cv::Point2f> scene;

    for( int i = 0; i < good_matches.size(); i++ )
    {
        //-- Get the keypoints from the good matches
        obj.push_back( keyPointsModel1A[ good_matches[i].queryIdx ].pt );
        scene.push_back( theFeatures.keypoints[ good_matches[i].trainIdx ].pt );
    }

    if(obj.empty() || scene.empty())
        return;

    cv::Mat H = cv::findHomography( obj, scene, CV_RANSAC );

    //-- Get the corners from the image_1 ( the object to be "detected" )
    std::vector<cv::Point2f> scene_corners(4);

    perspectiveTransform( obj_corners, scene_corners, H);

    //-- Draw lines between the corners (the mapped object in the scene - image_2 )
    LINE( "module:BanknotePosition:position", scene_corners[0].x, scene_corners[0].y , scene_corners[1].x, scene_corners[1].y, 3, Drawings::ps_solid, ColorRGBA::red );
    LINE( "module:BanknotePosition:position", scene_corners[1].x, scene_corners[1].y , scene_corners[2].x, scene_corners[2].y, 3, Drawings::ps_solid, ColorRGBA::red );
    LINE( "module:BanknotePosition:position", scene_corners[2].x, scene_corners[2].y , scene_corners[3].x, scene_corners[3].y, 3, Drawings::ps_solid, ColorRGBA::red );
    LINE( "module:BanknotePosition:position", scene_corners[3].x, scene_corners[3].y , scene_corners[0].x, scene_corners[0].y, 3, Drawings::ps_solid, ColorRGBA::red );
    */
}
