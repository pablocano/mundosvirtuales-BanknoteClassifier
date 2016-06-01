cmake_minimum_required(VERSION 3.1.0)

PROJECT (GroundTruth)
find_package(OpenCV REQUIRED )

set( MessageIDsPath ${CMAKE_CURRENT_SOURCE_DIR}/../uchile-robotics-team/Src/Tools/MessageQueue CACHE PATH "Path to MessageIDs file used by BHuman code")

set( MessageIDsPath)

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
    src/Tools/File.cpp
    src/Tools/File.h
    src/Tools/Math/Range.h
    src/Tools/SystemCall.cpp
    src/Tools/SystemCall.h
)

set( GROUNDTRUTH_SRC
    src/Processes/GroundTruth.cpp
    src/Modules/Camera.cpp
    src/Modules/Camera.h
    src/Modules/BackgroundModel.cpp
    src/Modules/BackgroundModel.h
    src/Modules/BallPerceptor.cpp
    src/Modules/BallPerceptor.h
    src/Modules/GroundTruthProvider.cpp
    src/Modules/GroundTruthProvider.h
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
    src/Representations/GroundTruthMessages.h
    src/Representations/Image.h
    src/Representations/Regions.cpp
    src/Representations/Regions.h
    src/Representations/RobotPercept.cpp
    src/Representations/RobotPercept.h
    src/Representations/RobotPose.cpp
    src/Representations/RobotPose.h
    src/Tools/ColorModelConversions.h
    src/Tools/Comm/GroundTruthMessageHandler.cpp
    src/Tools/Comm/GroundTruthMessageHandler.h
    src/Tools/Comm/SPLStandardMessage.h
    src/Tools/Comm/SPLStandardMessageWrapper.cpp
    src/Tools/Comm/SPLStandardMessageWrapper.h
    src/Tools/Comm/UdpComm.cpp
    src/Tools/Comm/UdpComm.h
    src/Tools/Enum.cpp
    src/Tools/Enum.h
    src/Tools/File.cpp
    src/Tools/File.h
    src/Tools/Math/Line.cpp
    src/Tools/Math/Line.h
    src/Tools/Math/Pose2D.h
    src/Tools/Math/Range.h
    src/Tools/Math/Transformation.cpp
    src/Tools/Math/Transformation.h
    src/Tools/Math/Vector2.h
    src/Tools/Messages/MessageQueue.cpp
    src/Tools/Messages/MessageQueue.h
    src/Tools/ModuleManager/Module.h
    src/Tools/SystemCall.cpp
    src/Tools/SystemCall.h
    
    ${MessageIDsPath}/MessageIDs.h
)


set( CAMERA_CALIBRATION
   Utils/CameraCalibration/calibration.cpp
   Utils/CameraCalibration/settings.h
   src/Tools/File.cpp
   src/Tools/File.h
)

source_group(src REGULAR_EXPRESSION "src/Processes/*")
source_group(src\\Modules REGULAR_EXPRESSION "Modules/*")
source_group(src\\Representations REGULAR_EXPRESSION "Representations/*")
source_group(src\\Representations\\ColorModel REGULAR_EXPRESSION "Representations/ColorModel/*")
source_group(src\\Tools REGULAR_EXPRESSION "Tools/*")
source_group(src\\Tools\\ModuleManager REGULAR_EXPRESSION "Tools/ModuleManager/*")
source_group(src\\Tools\\Comm REGULAR_EXPRESSION "Tools/Comm/*")
source_group(src\\Tools\\Messages REGULAR_EXPRESSION "Tools/Messages/*")
source_group(src\\Tools\\Math REGULAR_EXPRESSION "Tools/Math/*")

set(CMAKE_CXX_FLAGS "-std=c++11")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

IF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
add_definitions(-DOSX)
ELSEIF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
add_definitions(-DLINUX)
ELSEIF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
add_definitions(-DWINDOWS)
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")

include_directories(${CMAKE_INSTALL_PREFIX}/include ${CMAKE_CURRENT_SOURCE_DIR}/src ${MessageIDsPath})
add_executable( createColorTable ${SEGMENT_SRC} ${OpenCV_INCLUDE})
target_link_libraries( createColorTable ${OpenCV_LIBS} )

add_executable( groundTruth ${GROUNDTRUTH_SRC} ${OpenCV_INCLUDE})
target_link_libraries( groundTruth ${OpenCV_LIBS} )

add_executable( calibration ${CAMERA_CALIBRATION} ${OpenCV_INCLUDE})
target_link_libraries( calibration ${OpenCV_LIBS} )
