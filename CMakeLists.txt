cmake_minimum_required(VERSION 3.1.0)

PROJECT (BanknoteClassifier)
find_package(OpenCV REQUIRED)
find_package(Qt4 REQUIRED)
find_package (Eigen3 3.3 REQUIRED NO_MODULE)

set(PYLON_ROOT /opt/pylon5)
include(${QT_USE_FILE})

set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build Type")

set(BC_WITH_CUDA OFF CACHE BOOL "Use CUDA")

if(${BC_WITH_CUDA})
    add_definitions(-DBC_WITH_CUDA)
endif(${BC_WITH_CUDA})

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

if(WINDOWS)
	set(CMAKE_SHARED_LINKER_FLAGS "/API_EXPORT")
endif()

set( BANCKNOTECLASSIFIER_SRC
  src/Modules/ArucoPoseEstimator.cpp
  src/Modules/ArucoPoseEstimator.h
  src/Modules/BlobProvider.cpp
  src/Modules/BlobProvider.h
  src/Modules/BlobFilter.cpp
  src/Modules/BlobFilter.h
  src/Modules/BanknoteDetector.cpp
  src/Modules/BanknoteDetector.h
  src/Modules/BanknoteClassifierConfiguration.cpp
  src/Modules/BanknoteClassifierConfiguration.h
  src/Modules/BanknotePositionProvider.cpp
  src/Modules/BanknotePositionProvider.h
  src/Modules/BanknoteClassifierProvider.cpp
  src/Modules/BanknoteClassifierProvider.h
  src/Modules/Camera.cpp
  src/Modules/Camera.h
  src/Modules/CameraPoseFilter.cpp
  src/Modules/CameraPoseFilter.h
  src/Modules/CameraPoseProvider.cpp
  src/Modules/CameraPoseProvider.h
  src/Modules/CustomCommunication.cpp
  src/Modules/CustomCommunication.h
  src/Modules/DummyImages.cpp
  src/Modules/DummyImages.h
  src/Modules/Equalizer.cpp
  src/Modules/Equalizer.h
  src/Modules/FeaturesProvider.cpp
  src/Modules/FeaturesProvider.h
  src/Modules/EstimatePosition.cpp
  src/Modules/EstimatePosition.h
  src/Modules/OpencvCamera.cpp
  src/Modules/OpencvCamera.h
  src/Modules/PreviousBanknoteCheck.cpp
  src/Modules/PreviousBanknoteCheck.h
  src/Modules/Regionizer.cpp
  src/Modules/Regionizer.h
  src/Modules/RobotFanucDataProvider.cpp
  src/Modules/RobotFanucDataProvider.h
  src/Modules/RobotFanucComm.cpp
  src/Modules/RobotFanucComm.h
  src/Modules/RobotStatus.cpp
  src/Modules/RobotStatus.h
  src/Modules/Segmentator.cpp
  src/Modules/Segmentator.h
  src/Modules/TemplateInspection.cpp
  src/Modules/TemplateInspection.h
  src/Modules/WorldCoordinatesPoseProvider.cpp
  src/Modules/WorldCoordinatesPoseProvider.h
  src/Platform/Memory.cpp
  src/Platform/Memory.h
  src/Processes/BanknoteClassifier.cpp
  src/Processes/BanknoteClassifier.h
  src/Processes/GripperCommunication.cpp
  src/Processes/GripperCommunication.h
  src/Representations/Blobs.cpp
  src/Representations/Blobs.h
  src/Representations/BanknoteDetections.h
  src/Representations/BanknotePosition.cpp
  src/Representations/BanknotePosition.h
  src/Representations/CameraInfo.cpp
  src/Representations/CameraInfo.h
  src/Representations/Classification.cpp
  src/Representations/Classification.h
  src/Representations/ErrorInfo.cpp
  src/Representations/ErrorInfo.h
  src/Representations/ColorModel/ColorCalibration.h
  src/Representations/ColorModel/ColorModel.cpp
  src/Representations/ColorModel/ColorModel.h
  src/Representations/Features.cpp
  src/Representations/Features.h
  src/Representations/FrameInfo.h
  src/Representations/FieldDimensions.h
  src/Representations/FieldDimensions.cpp
  src/Representations/GrabbingPosition.cpp
  src/Representations/GrabbingPosition.h
  src/Representations/BanknoteClassifierMessages.h
  src/Representations/Image.cpp
  src/Representations/Image.h
  src/Representations/Modeling/WorldCoordinatesPose.cpp
  src/Representations/Modeling/WorldCoordinatesPose.h
  src/Representations/Modeling/CameraPose.cpp
  src/Representations/Modeling/CameraPose.h
  src/Representations/Regions.cpp
  src/Representations/Regions.h
  src/Representations/RegState.cpp
  src/Representations/RegState.h
  src/Representations/RobotFanuc.cpp
  src/Representations/RobotFanuc.h
  src/Tools/AlignedMemory.cpp
  src/Tools/AlignedMemory.h
  src/Tools/ColorClasses.cpp
  src/Tools/ColorClasses.h
  src/Tools/ColorModelConversions.h
  src/Tools/Comm/Comm.h
  src/Tools/Comm/BanknoteClassifierMessageHandler.cpp
  src/Tools/Comm/BanknoteClassifierMessageHandler.h
  src/Tools/Debugging/Debugging.h
  src/Tools/Debugging/DebugRequest.cpp
  src/Tools/Debugging/DebugRequest.h
  src/Tools/Debugging/DebugDrawings.cpp
  src/Tools/Debugging/DebugDrawings.h
  src/Tools/Enum.cpp
  src/Tools/Enum.h
  src/Tools/File.cpp
  src/Tools/File.h
  src/Tools/Global.cpp
  src/Tools/Global.h
  src/Tools/Math/Angle.h
  src/Tools/Math/Constants.h
  src/Tools/Math/Eigen.h
  src/Tools/Math/EigenArrayExtensions.h
  src/Tools/Math/EigenMatrixBaseExtensions.h
  src/Tools/Math/Line.cpp
  src/Tools/Math/Line.h
  src/Tools/Math/Geometry.cpp
  src/Tools/Math/Geometry.h
  src/Tools/Math/Kalman.h
  src/Tools/Math/Kalman.cpp
  src/Tools/Math/Pose2D.h
  src/Tools/Math/Range.h
  src/Tools/Math/Transformation.cpp
  src/Tools/Math/Transformation.h
  src/Tools/Math/Vector2.h
  src/Tools/Messages/MessageQueue.cpp
  src/Tools/Messages/MessageQueue.h
  src/Tools/ModuleManager/Blackboard.cpp
  src/Tools/ModuleManager/Blackboard.h
  src/Tools/ModuleManager/Module.cpp
  src/Tools/ModuleManager/Module.h
  src/Tools/ModuleManager/ModuleManager.cpp
  src/Tools/ModuleManager/ModuleManager.h
  src/Tools/Process.cpp
  src/Tools/Process.h
  src/Tools/Settings.cpp
  src/Tools/Settings.h
  src/Tools/Streamable.h
  src/Tools/SystemCall.cpp
  src/Tools/SystemCall.h
  src/Tools/MessageIDs.h
)


set( CAMERA_CALIBRATION
   Utils/CameraCalibration/calibration.cpp
   src/Tools/File.cpp
   src/Tools/File.h
)

set( FIELD_CALIBRATOR
   Utils/FieldCalibrator/fieldCalibrator.cpp
   src/Representations/FieldDimensions.h
   src/Representations/FieldDimensions.cpp
)

set( CALIBRATOR_SRC_DIR Utils/CalibratorTool)

QT4_WRAP_CPP(CALIBRATION_TOOL_MOCSrcs ${CALIBRATOR_SRC_DIR}/ListViewsDockWidget.h ${CALIBRATOR_SRC_DIR}/MainWindow.h ${CALIBRATOR_SRC_DIR}/RegisteredDockWidget.h ${CALIBRATOR_SRC_DIR}/Views/ImageView.h ${CALIBRATOR_SRC_DIR}/Views/StatusView.h ${CALIBRATOR_SRC_DIR}/Views/ColorCalibrationView/ColorCalibrationView.h ${CALIBRATOR_SRC_DIR}/Views/ColorCalibrationView/RangeSelector.h ${CALIBRATOR_SRC_DIR}/Views/ColorCalibrationView/ThresholdSelector.h)

QT4_ADD_RESOURCES(CALIBRATION_TOOL_RESOURCES_RCC ${CALIBRATOR_SRC_DIR}/CalibratorTool.qrc)

set( CALIBRATOR_TOOL_SRC
  ${CALIBRATOR_SRC_DIR}/CalibratorTool.h
  ${CALIBRATOR_SRC_DIR}/Controller.cpp
  ${CALIBRATOR_SRC_DIR}/Controller.h
  ${CALIBRATOR_SRC_DIR}/BanknoteClassifierWrapper.cpp
  ${CALIBRATOR_SRC_DIR}/BanknoteClassifierWrapper.h
  ${CALIBRATOR_SRC_DIR}/ListViewsDockWidget.cpp
  ${CALIBRATOR_SRC_DIR}/ListViewsDockWidget.h
  ${CALIBRATOR_SRC_DIR}/Main.cpp
  ${CALIBRATOR_SRC_DIR}/MainWindow.cpp
  ${CALIBRATOR_SRC_DIR}/MainWindow.h
  ${CALIBRATOR_SRC_DIR}/RegisteredDockWidget.cpp
  ${CALIBRATOR_SRC_DIR}/RegisteredDockWidget.h
  ${CALIBRATOR_SRC_DIR}/Synchronization.h
  ${CALIBRATOR_SRC_DIR}/Views/ImageView.cpp
  ${CALIBRATOR_SRC_DIR}/Views/ImageView.h
  ${CALIBRATOR_SRC_DIR}/Views/ColorCalibrationView/ColorCalibrationView.cpp
  ${CALIBRATOR_SRC_DIR}/Views/ColorCalibrationView/ColorCalibrationView.h
  ${CALIBRATOR_SRC_DIR}/Views/ColorCalibrationView/RangeSelector.cpp
  ${CALIBRATOR_SRC_DIR}/Views/ColorCalibrationView/RangeSelector.h
  ${CALIBRATOR_SRC_DIR}/Views/ColorCalibrationView/ThresholdSelector.cpp
  ${CALIBRATOR_SRC_DIR}/Views/ColorCalibrationView/ThresholdSelector.h
  ${CALIBRATOR_SRC_DIR}/Views/StatusView.cpp
  ${CALIBRATOR_SRC_DIR}/Views/StatusView.h
  ${CALIBRATOR_SRC_DIR}/Visualization/DebugDrawing.cpp
  ${CALIBRATOR_SRC_DIR}/Visualization/DebugDrawing.h
  ${CALIBRATOR_SRC_DIR}/Visualization/PaintMethods.cpp
  ${CALIBRATOR_SRC_DIR}/Visualization/PaintMethods.h
)

set( VISION_TOOL_SRC_DIR Utils/VisionTool)

QT4_WRAP_CPP(VISION_TOOL_MOCSrcs ${VISION_TOOL_SRC_DIR}/VisionToolWidget.h ${VISION_TOOL_SRC_DIR}/MainWindow.h)

set( VISION_TOOL_SRC
  ${VISION_TOOL_SRC_DIR}/Main.cpp
  ${VISION_TOOL_SRC_DIR}/MainWindow.cpp
  ${VISION_TOOL_SRC_DIR}/MainWindow.h
  ${VISION_TOOL_SRC_DIR}/CommunicationHandler.cpp
  ${VISION_TOOL_SRC_DIR}/CommunicationHandler.h
  ${VISION_TOOL_SRC_DIR}/VisionTool.cpp
  ${VISION_TOOL_SRC_DIR}/VisionTool.h
  ${VISION_TOOL_SRC_DIR}/VisionToolWidget.cpp
  ${VISION_TOOL_SRC_DIR}/VisionToolWidget.h
  src/Platform/Memory.cpp
  src/Platform/Memory.h
  src/Tools/AlignedMemory.cpp
  src/Tools/AlignedMemory.h
  src/Tools/Comm/BanknoteClassifierMessageHandler.cpp
  src/Tools/Comm/BanknoteClassifierMessageHandler.h
  src/Tools/Enum.cpp
  src/Tools/Enum.h
  src/Tools/File.cpp
  src/Tools/File.h
  src/Tools/Global.cpp
  src/Tools/Global.h
  src/Tools/Math/Pose2D.h
  src/Tools/Math/Vector2.h
  src/Tools/Messages/MessageQueue.cpp
  src/Tools/Messages/MessageQueue.h
  src/Tools/Settings.cpp
  src/Tools/Settings.h
  src/Tools/Streamable.h
  src/Tools/SystemCall.cpp
  src/Tools/SystemCall.h

  src/Tools/MessageIDs.h
)

set( LIBQXT_SRC_DIR Utils/libqxt)

QT4_WRAP_CPP(LIBQXT_MOC_Srcs ${LIBQXT_SRC_DIR}/qxtspanslider_p.h ${LIBQXT_SRC_DIR}/qxtspanslider.h)

set( LIBQXT_SRC
  ${LIBQXT_SRC_DIR}/qxtglobal.h
  ${LIBQXT_SRC_DIR}/qxtnamespace.h
  ${LIBQXT_SRC_DIR}/qxtspanslider_p.h
  ${LIBQXT_SRC_DIR}/qxtspanslider.cpp
  ${LIBQXT_SRC_DIR}/qxtspanslider.h
)

set( LIB_ETHERNETIP_FANUC_SRC
	src/Tools/Fanuc/PositionRegisterCartesian.cpp
	src/Tools/Fanuc/PositionRegisterJoint.cpp
	src/Tools/Comm/TcpComm.cpp
)

set( LIB_ETHERNETIP_FANUC_INCLUDE
	src/Tools/Fanuc/PositionRegisterCartesian.h
	src/Tools/Fanuc/PositionRegisterJoint.h
	src/Tools/Fanuc/PacketEthernetIPFanuc.h
	src/Tools/Comm/TcpComm.h
)

source_group(src\\Processes REGULAR_EXPRESSION "src/Processes/*")
source_group(src\\Modules REGULAR_EXPRESSION "Modules/*")
source_group(src\\Representations REGULAR_EXPRESSION "Representations/*")
source_group(src\\Representations\\ColorModel REGULAR_EXPRESSION "Representations/ColorModel/*")
source_group(src\\Tools REGULAR_EXPRESSION "Tools/*")
source_group(src\\Tools\\ModuleManager REGULAR_EXPRESSION "Tools/ModuleManager/*")
source_group(src\\Tools\\Comm REGULAR_EXPRESSION "Tools/Comm/*")
source_group(src\\Tools\\Debugging REGULAR_EXPRESSION "Tools/Debugging/*")
source_group(src\\Tools\\Messages REGULAR_EXPRESSION "Tools/Messages/*")
source_group(src\\Tools\\Math REGULAR_EXPRESSION "Tools/Math/*")
source_group(Utils\\CalibratorTool REGULAR_EXPRESSION "Utils/CalibratorTool/*")
source_group(Utils\\CalibratorTool\\Views REGULAR_EXPRESSION "Utils/CalibratorTool/Views/*")
source_group(Utils\\CalibratorTool\\Visualization REGULAR_EXPRESSION "Utils/CalibratorTool/Visualization/*")
source_group(Utils\\CalibratorTool\\Views\\ColorCalibrationView REGULAR_EXPRESSION "Utils/CalibratorTool/Views/ColorCalibrationView/*")
source_group(Utils\\libqxt REGULAR_EXPRESSION "Utils/libqxt*")
source_group(Utils\\VisionTool REGULAR_EXPRESSION "Utils/VisionTool/*")

if(WIN32)
	set(CMAKE_CXX_STANDARD 11)
else()
	set(CMAKE_CXX_FLAGS "-std=c++11")
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

IF(WIN32)
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Zm200 -wd4503")
	add_definitions(-D_CRT_SECURE_NO_DEPRECATE -D_SCL_SECURE_NO_WARNINGS)
	message(STATUS "Windows computer")
	SET(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} /NODEFAULTLIB:msvcrt.lib")
	SET(CMAKE_SHARED_LINKER_FLAGS_DEBUG "${CMAKE_SHARED_LINKER_FLAGS_DEBUG} /NODEFAULTLIB:msvcrt.lib")
ENDIF(WIN32)

IF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
add_definitions(-DOSX)
ELSEIF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
add_definitions(-DLINUX)
set(PYLON_ROOT /opt/pylon5)
link_directories(${PYLON_ROOT}/lib64)
ELSE(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
add_definitions(-DWINDOWS)
set(PYLON_ROOT $ENV{PYLON_DEV_DIR})
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    link_directories(${PYLON_ROOT}/lib/x64)
elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
    link_directories(${PYLON_ROOT}/lib/Win32)
endif()

ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")

message(STATUS ${EIGEN3_INCLUDE_DIR})

include_directories(${OpenCV_INCLUDE} ${EIGEN3_INCLUDE_DIR} ${PYLON_ROOT}/include ${CMAKE_CURRENT_SOURCE_DIR}/src ${CMAKE_CURRENT_SOURCE_DIR}/Utils/libqxt)

message(STATUS "Pylon dir find in " ${PYLON_ROOT})

add_library( banknoteClassifier SHARED ${BANCKNOTECLASSIFIER_SRC} ${OpenCV_INCLUDE} ${LIB_ETHERNETIP_FANUC_SRC} ${LIB_ETHERNETIP_FANUC_INCLUDE})
IF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
target_link_libraries( banknoteClassifier ${OpenCV_LIBS} Eigen3::Eigen pylonbase pylonutility GenApi_gcc_v3_1_Basler_pylon_v5_1 GCBase_gcc_v3_1_Basler_pylon_v5_1)
ELSEIF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
target_link_libraries( banknoteClassifier ${OpenCV_LIBS} Eigen3::Eigen pylonbase pylonutility GenApi_gcc_v3_1_Basler_pylon_v5_1 GCBase_gcc_v3_1_Basler_pylon_v5_1)
ELSE(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
target_link_libraries( banknoteClassifier ${OpenCV_LIBS} Eigen3::Eigen PylonBase_MD_VC120_v5_1 PylonUtility_MD_VC120_v5_1 GenApi_MD_VC120_v3_0_Basler_pylon_v5_1 GCBase_MD_VC120_v3_1_Basler_pylon_v5_1 ws2_32 winmm)
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")


add_library( libqxt STATIC ${LIBQXT_SRC} ${LIBQXT_MOC_Srcs})
target_link_libraries(libqxt ${QT_LIBRARIES})
target_compile_definitions(libqxt PRIVATE QXT_STATIC )

add_executable( cameraCalibration ${CAMERA_CALIBRATION} ${OpenCV_INCLUDE})
IF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
target_link_libraries( cameraCalibration ${OpenCV_LIBS} pylonbase pylonutility GenApi_gcc_v3_1_Basler_pylon_v5_1 GCBase_gcc_v3_1_Basler_pylon_v5_1)
ELSEIF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
target_link_libraries( cameraCalibration ${OpenCV_LIBS} pylonbase pylonutility GenApi_gcc_v3_1_Basler_pylon_v5_1 GCBase_gcc_v3_1_Basler_pylon_v5_1)
ELSE(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
target_link_libraries( cameraCalibration ${OpenCV_LIBS} pylonBase_MD_VC120_v5_1 PylonUtility_MD_VC120_v5_1 GenApi_MD_VC120_v3_1_Basler_pylon_v5_1 GCBase_MD_VC120_v3_1_Basler_pylon_v5_1)
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")

add_executable( fieldCalibrator ${FIELD_CALIBRATOR} ${OpenCV_INCLUDE})
target_link_libraries( fieldCalibrator ${OpenCV_LIBS} )

add_executable(bc src/Processes/main.cpp)
include (GenerateExportHeader)          
generate_export_header(banknoteClassifier BASE_NAME banknoteClassifier EXPORT_MACRO_NAME BANCKNOTECLASSIFIER_EXPORT EXPORT_FILE_NAME BanknoteClassifier_EXPORTS.h STATIC_DEFINE BANCKNOTECLASSIFIER_EXPORTS_BUILT_AS_STATIC)
generate_export_header(banknoteClassifier BASE_NAME Process EXPORT_MACRO_NAME PROCESS_EXPORT EXPORT_FILE_NAME Process_EXPORTS.h STATIC_DEFINE PROCESS_EXPORTS_BUILT_AS_STATIC)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(bc banknoteClassifier)


add_executable(gripperComm src/Processes/mainGripperComm.cpp)
target_link_libraries(gripperComm banknoteClassifier)

include_directories(${OpenCV_INCLUDE} ${CMAKE_CURRENT_SOURCE_DIR}/src ${CMAKE_CURRENT_SOURCE_DIR}/Utils/CalibratorTool  ${CMAKE_CURRENT_BINARY_DIR})

add_executable(calibrationTool ${CALIBRATOR_TOOL_SRC} ${CALIBRATION_TOOL_MOCSrcs} ${CALIBRATION_TOOL_RESOURCES_RCC})
IF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
target_link_libraries( calibrationTool banknoteClassifier libqxt ${QT_LIBRARIES})
ELSEIF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
target_link_libraries( calibrationTool banknoteClassifier libqxt ${QT_LIBRARIES} rt pthread)
ELSE(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
target_link_libraries( calibrationTool banknoteClassifier libqxt ${QT_LIBRARIES})
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
target_compile_definitions(calibrationTool PRIVATE QXT_STATIC)

add_executable(visionTool ${VISION_TOOL_SRC} ${VISION_TOOL_MOCSrcs} ${OpenCV_INCLUDE})
target_link_libraries( visionTool banknoteClassifier ${OpenCV_LIBS} ${QT_LIBRARIES} )
