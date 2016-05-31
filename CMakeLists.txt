cmake_minimum_required(VERSION 3.1.0)

PROJECT (GroundTruth)
find_package(OpenCV REQUIRED )

set(CMAKE_BUILD_TYPE Release)

set( SEGMENT_SRC
    src/Processes/Segmentation.cpp
    src/Modules/Camera.cpp
    src/Modules/Camera.h
    src/Modules/Segmentator.cpp
    src/Modules/Segmentator.h
    src/Representations/Blackboard.cpp
    src/Representations/Blackboard.h
    src/Representations/ColorModel/ColorModel.cpp
    src/Representations/ColorModel/ColorModel.h
    src/Representations/Image.h
    src/Tools/ColorModelConversions.h
    src/Tools/ColorRangeCreator.h
    src/Tools/ModuleManager/Module.h
    src/Tools/Range.h
)

set( GROUNDTRUTH_SRC
    src/Processes/GroundTruth.cpp
    src/Modules/Camera.cpp
    src/Modules/Camera.h
    src/Modules/BackgroundModel.cpp
    src/Modules/BackgroundModel.h
    src/Modules/BallPerceptor.cpp
    src/Modules/BallPerceptor.h
    src/Modules/MapRecorder.cpp
    src/Modules/MapRecorder.h
    src/Modules/Regionizer.cpp
    src/Modules/Regionizer.h
    src/Modules/RobotPerceptor.cpp
    src/Modules/RobotPerceptor.h
    src/Modules/RobotPoseProvider.cpp
    src/Modules/RobotPoseProvider.h
    src/Representations/BallPerception.cpp
    src/Representations/BallPerception.h
    src/Representations/Blackboard.cpp
    src/Representations/Blackboard.h
    src/Representations/CameraInfo.cpp
    src/Representations/CameraInfo.h
    src/Representations/ColorModel/ColorModel.cpp
    src/Representations/ColorModel/ColorModel.h
    src/Representations/FrameInfo.h
    src/Representations/FieldDimensions.h
    src/Representations/FieldDimensions.cpp
    src/Representations/Image.h
    src/Representations/Regions.cpp
    src/Representations/Regions.h
    src/Representations/RobotPercept.cpp
    src/Representations/RobotPercept.h
    src/Representations/RobotPose.cpp
    src/Representations/RobotPose.h
    src/Tools/ColorModelConversions.h
    src/Tools/ModuleManager/Module.h
    src/Tools/Line.cpp
    src/Tools/Line.h
    src/Tools/Range.h
    src/Tools/Transformation.cpp
    src/Tools/Transformation.h
    src/Tools/Vector2.h
)


set( CAMERA_CALIBRATION
   Utils/CameraCalibration/calibration.cpp
   Utils/CameraCalibration/settings.h
)

set( FIELD_CALIBRATOR
   Utils/FieldCalibrator/fieldCalibrator.cpp
   src/Representations/FieldDimensions.h
   src/Representations/FieldDimensions.cpp
)

source_group(src REGULAR_EXPRESSION "src/Processes/*")
source_group(src\\Modules REGULAR_EXPRESSION "Modules/*")
source_group(src\\Representations REGULAR_EXPRESSION "Representations/*")
source_group(src\\Representations\\ColorModel REGULAR_EXPRESSION "Representations/ColorModel/*")
source_group(src\\Tools REGULAR_EXPRESSION "Tools/*")
source_group(src\\Tools\\ModuleManager REGULAR_EXPRESSION "Tools/ModuleManager/*")

set(CMAKE_CXX_FLAGS "-std=c++11")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include_directories(${CMAKE_INSTALL_PREFIX}/include ${CMAKE_CURRENT_SOURCE_DIR}/src)

add_executable( createColorTable ${SEGMENT_SRC} ${OpenCV_INCLUDE})
target_link_libraries( createColorTable ${OpenCV_LIBS} )

add_executable( groundTruth ${GROUNDTRUTH_SRC} ${OpenCV_INCLUDE})
target_link_libraries( groundTruth ${OpenCV_LIBS} )

add_executable( calibration ${CAMERA_CALIBRATION} ${OpenCV_INCLUDE})
target_link_libraries( calibration ${OpenCV_LIBS} )

add_executable( fieldCalibrator ${FIELD_CALIBRATOR} ${OpenCV_INCLUDE})
target_link_libraries( fieldCalibrator ${OpenCV_LIBS} )
