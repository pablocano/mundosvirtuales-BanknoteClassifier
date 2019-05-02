cmake_minimum_required(VERSION 3.1.0)

PROJECT (BanknoteClassifier)
find_package(OpenCV 4 REQUIRED)
find_package(Qt5 COMPONENTS Core Widgets Gui REQUIRED)
find_package (Eigen3 3.3 REQUIRED NO_MODULE)

find_package(CUDA 9.0 REQUIRED)
message(STATUS "Found CUDA ${CUDA_VERSION_STRING} at ${CUDA_TOOLKIT_ROOT_DIR}")

find_package(Torch REQUIRED PATHS Utils/libtorch/)#pytorch

add_definitions(-D_GLIBCXX_USE_CXX11_ABI=0)

if(WIN32)
        set(CMAKE_CXX_STANDARD 11)
else()
        set(CMAKE_CXX_FLAGS "-std=c++14")
endif()

set(CMAKE_VERBOSE_MAKEFILE OFF)

# Tell CMake to run moc when necessary:
set(CMAKE_AUTOMOC ON)
# As moc files are generated in the binary dir, tell CMake
# to always look for includes there:
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(PYLON_ROOT /opt/pylon5)
#include(${QT_USE_FILE})

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
  src/Modules/BanknoteDetector.cpp
  src/Modules/BanknoteDetector.h
  src/Modules/BanknoteClassifierConfiguration.cpp
  src/Modules/BanknoteClassifierConfiguration.h
  src/Modules/BanknoteTracker.cpp
  src/Modules/BanknoteTracker.h
  src/Modules/CameraProvider.cpp
  src/Modules/CameraProvider.h
  src/Modules/CameraPoseFilter.cpp
  src/Modules/CameraPoseFilter.h
  src/Modules/CameraPoseProvider.cpp
  src/Modules/CameraPoseProvider.h
  src/Modules/Corrector/BanknoteCorrectorConfiguration.cpp
  src/Modules/Corrector/BanknoteCorrectorConfiguration.h
  src/Modules/Corrector/BanknotePositionProvider.cpp
  src/Modules/Corrector/BanknotePositionProvider.h
  src/Modules/Corrector/CorrectorCamera.cpp
  src/Modules/Corrector/CorrectorCamera.h
  src/Modules/CustomCommunication.cpp
  src/Modules/CustomCommunication.h
  src/Modules/DummyImages.cpp
  src/Modules/DummyImages.h
  src/Modules/Equalizer.cpp
  src/Modules/Equalizer.h
  src/Modules/FeaturesProvider.cpp
  src/Modules/FeaturesProvider.h
  src/Modules/GrayImageProvider.cpp
  src/Modules/GrayImageProvider.h
  src/Modules/RobotFanucDataProvider.cpp
  src/Modules/RobotFanucDataProvider.h
  src/Modules/RobotFanucComm.cpp
  src/Modules/RobotFanucComm.h
  src/Modules/RobotStatusProvider.cpp
  src/Modules/RobotStatusProvider.h
  src/Modules/Segmentator.cpp
  src/Modules/Segmentator.h
  src/Modules/WorldCoordinatesPoseProvider.cpp
  src/Modules/WorldCoordinatesPoseProvider.h
  src/Modules/SemanticSeg.cpp
  src/Modules/SemanticSeg.h
  src/Platform/BCAssert.h
  src/Platform/Camera.cpp
  src/Platform/Camera.h
  src/Platform/DebugHandler.h
  src/Platform/File.cpp
  src/Platform/File.h
  src/Platform/Memory.h
  src/Platform/Semaphore.h
  src/Platform/SystemCall.cpp
  src/Platform/SystemCall.h
  src/Platform/Thread.h
  src/Platform/Time.h
  src/Processes/BanknoteClassifier.cpp
  src/Processes/BanknoteClassifier.h
  src/Processes/BanknoteCorrector.cpp
  src/Processes/BanknoteCorrector.h
  src/Processes/Debug.cpp
  src/Processes/Debug.h
  src/Representations/BanknoteDetections.cpp
  src/Representations/BanknoteDetections.h
  src/Representations/BanknoteDetectionParameters.h
  src/Representations/BanknoteModel.cpp
  src/Representations/BanknoteModel.h
  src/Representations/BanknotePosition.cpp
  src/Representations/BanknotePosition.h
  src/Representations/CameraInfo.cpp
  src/Representations/CameraInfo.h
  src/Representations/CameraSettings.cpp
  src/Representations/CameraSettings.h
  src/Representations/Classification.cpp
  src/Representations/Classification.h
  src/Representations/SemanticSegmented.cpp
  src/Representations/SemanticSegmented.h
  src/Representations/ColorModel/ColorCalibration.h
  src/Representations/ColorModel/ColorModel.cpp
  src/Representations/ColorModel/ColorModel.h
  src/Representations/Features.cpp
  src/Representations/Features.h
  src/Representations/FrameInfo.h
  src/Representations/GrabbingPosition.cpp
  src/Representations/GrabbingPosition.h
  src/Representations/Image.cpp
  src/Representations/Image.h
  src/Representations/Modeling/WorldCoordinatesPose.cpp
  src/Representations/Modeling/WorldCoordinatesPose.h
  src/Representations/Modeling/CameraPose.cpp
  src/Representations/Modeling/CameraPose.h
  src/Representations/RobotFanucStatus.cpp
  src/Representations/RobotFanucStatus.h
  src/Representations/RobotFanucRegisters.cpp
  src/Representations/RobotFanucRegisters.h
  src/Tools/AlignedMemory.cpp
  src/Tools/AlignedMemory.h
  src/Tools/ColorClasses.cpp
  src/Tools/ColorClasses.h
  src/Tools/ColorModelConversions.h
  src/Tools/Comm/Comm.h
  src/Tools/Comm/BanknoteClassifierMessageHandler.cpp
  src/Tools/Comm/BanknoteClassifierMessageHandler.h
  src/Tools/Debugging/ColorRGBA.cpp
  src/Tools/Debugging/ColorRGBA.h
  src/Tools/Debugging/DebugDataStreamer.cpp
  src/Tools/Debugging/DebugDataStreamer.h
  src/Tools/Debugging/DebugDataTable.cpp
  src/Tools/Debugging/DebugDataTable.h
  src/Tools/Debugging/Debugging.h
  src/Tools/Debugging/DebugDrawings.cpp
  src/Tools/Debugging/DebugDrawings.h
  src/Tools/Debugging/Debugging.h
  src/Tools/Debugging/DebugRequest.cpp
  src/Tools/Debugging/DebugRequest.h
  src/Tools/Debugging/Modify.h
  src/Tools/Debugging/QueueFillRequest.cpp
  src/Tools/Debugging/QueueFillRequest.h
  src/Tools/Debugging/Stopwatch.h
  src/Tools/Debugging/TcpConnection.cpp
  src/Tools/Debugging/TcpConnection.h
  src/Tools/Debugging/TimingManager.cpp
  src/Tools/Debugging/TimingManager.h
  src/Tools/Function.h
  src/Tools/FunctionList.cpp
  src/Tools/FunctionList.h
  src/Tools/Global.cpp
  src/Tools/Global.h
  src/Tools/Math/Angle.h
  src/Tools/Math/Constants.h
  src/Tools/Math/OpenCv.h
  src/Tools/Math/Eigen.h
  src/Tools/Math/EigenArrayExtensions.h
  src/Tools/Math/EigenMatrixBaseExtensions.h
  src/Tools/Math/FixedPoint.h
  src/Tools/Math/Line.cpp
  src/Tools/Math/Line.h
  src/Tools/Math/Geometry.cpp
  src/Tools/Math/Geometry.h
  src/Tools/Math/iou.cpp
  src/Tools/Math/iou.h
  src/Tools/Math/Kalman.h
  src/Tools/Math/Kalman.cpp
  src/Tools/Math/Pose2f.h
  src/Tools/Math/Random.cpp
  src/Tools/Math/Random.h
  src/Tools/Math/Transformation.cpp
  src/Tools/Math/Transformation.h
  src/Tools/Math/Vector2.h
  src/Tools/MessageQueue/InMessage.cpp
  src/Tools/MessageQueue/InMessage.h
  src/Tools/MessageQueue/MessageIDs.h
  src/Tools/MessageQueue/MessageQueue.cpp
  src/Tools/MessageQueue/MessageQueue.h
  src/Tools/MessageQueue/MessageQueueBase.cpp
  src/Tools/MessageQueue/MessageQueueBase.h
  src/Tools/MessageQueue/OutMessage.cpp
  src/Tools/MessageQueue/OutMessage.h
  src/Tools/ModuleManager/Blackboard.cpp
  src/Tools/ModuleManager/Blackboard.h
  src/Tools/ModuleManager/Module.cpp
  src/Tools/ModuleManager/Module.h
  src/Tools/ModuleManager/ModuleManager.cpp
  src/Tools/ModuleManager/ModuleManager.h
  src/Tools/ModuleManager/ModulePackage.h
  src/Tools/ModuleManager/Next.h
  src/Tools/ProcessFramework/PlatformProcess.h
  src/Tools/ProcessFramework/Process.cpp
  src/Tools/ProcessFramework/Process.h
  src/Tools/ProcessFramework/ProcessFramework.cpp
  src/Tools/ProcessFramework/ProcessFramework.h
  src/Tools/ProcessFramework/Receiver.cpp
  src/Tools/ProcessFramework/Receiver.h
  src/Tools/ProcessFramework/Sender.cpp
  src/Tools/ProcessFramework/Sender.h
  src/Tools/Range.h
  src/Tools/Streams/AutoStreamable.h
  src/Tools/Streams/Eigen.h
  src/Tools/Streams/Enum.h
  src/Tools/Streams/EnumIndexedArray.h
  src/Tools/Streams/InOut.cpp
  src/Tools/Streams/InOut.h
  src/Tools/Streams/InStreams.cpp
  src/Tools/Streams/InStreams.h
  src/Tools/Streams/OutStreams.cpp
  src/Tools/Streams/OutStreams.h
  src/Tools/Streams/SimpleMap.cpp
  src/Tools/Streams/SimpleMap.h
  src/Tools/Streams/Streamable.cpp
  src/Tools/Streams/Streamable.h
  src/Tools/Streams/TypeInfo.cpp
  src/Tools/Streams/TypeInfo.h
  src/Tools/Streams/TypeRegistry.cpp
  src/Tools/Streams/TypeRegistry.h
  src/Tools/Settings.cpp
  src/Tools/Settings.h
)

IF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
set(BC_PLATFORM_SRC
  src/Platform/macOS/File.cpp
)
ELSEIF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
set(BC_PLATFORM_SRC
  src/Platform/Linux/BCAssert.cpp
  src/Platform/Linux/DebugHandler.cpp
  src/Platform/Linux/File.cpp
  src/Platform/Linux/Memory.cpp
  src/Platform/Linux/Semaphore.cpp
  src/Platform/Linux/Thread.cpp
  src/Platform/Linux/ThreadBase.cpp
  src/Platform/Linux/Time.cpp
  src/Platform/Linux/V4lCamera.cpp
)
ELSE(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
set(BC_PLATFORM_SRC
  src/Platform/Windows/File.cpp
  src/Platform/Windows/Time.cpp
)
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")


set( CAMERA_CALIBRATION
   Utils/CameraCalibration/calibration.cpp
   src/Platform/Linux/BCAssert.cpp
   src/Platform/Linux/File.cpp
   src/Platform/File.cpp
   src/Platform/File.h
)

#Calibrarion Tool

set( CALIBRATOR_SRC_DIR Utils/CalibratorTool)

#QT5_WRAP_CPP(CALIBRATION_TOOL_MOCSrcs ${CALIBRATOR_SRC_DIR}/ListViewsDockWidget.h ${CALIBRATOR_SRC_DIR}/MainWindow.h ${CALIBRATOR_SRC_DIR}/RegisteredDockWidget.h ${CALIBRATOR_SRC_DIR}/Views/ImageView.h ${CALIBRATOR_SRC_DIR}/Views/StatusView.h ${CALIBRATOR_SRC_DIR}/Views/ColorCalibrationView/ColorCalibrationView.h ${CALIBRATOR_SRC_DIR}/Views/ColorCalibrationView/RangeSelector.h ${CALIBRATOR_SRC_DIR}/Views/ColorCalibrationView/ThresholdSelector.h)

QT5_ADD_RESOURCES(CALIBRATION_TOOL_RESOURCES_RCC ${CALIBRATOR_SRC_DIR}/CalibratorTool.qrc)

set( CALIBRATOR_TOOL_SRC
  #${CALIBRATOR_SRC_DIR}/CalibratorTool.h
  ${CALIBRATOR_SRC_DIR}/CalibratorToolCtrl.cpp
  #${CALIBRATOR_SRC_DIR}/Controller.cpp
  #${CALIBRATOR_SRC_DIR}/Controller.h
  ${CALIBRATOR_SRC_DIR}/ConsoleController.cpp
  #${CALIBRATOR_SRC_DIR}/BanknoteClassifierWrapper.cpp
  #${CALIBRATOR_SRC_DIR}/BanknoteClassifierWrapper.h
  ${CALIBRATOR_SRC_DIR}/ListViewsDockWidget.cpp
  ${CALIBRATOR_SRC_DIR}/LocalRobot.cpp
  #${CALIBRATOR_SRC_DIR}/ListViewsDockWidget.h
  ${CALIBRATOR_SRC_DIR}/Main.cpp
  ${CALIBRATOR_SRC_DIR}/MainWindow.cpp
  #${CALIBRATOR_SRC_DIR}/MainWindow.h
  ${CALIBRATOR_SRC_DIR}/RegisteredDockWidget.cpp
  #${CALIBRATOR_SRC_DIR}/RegisteredDockWidget.h
  #${CALIBRATOR_SRC_DIR}/Synchronization.h
  ${CALIBRATOR_SRC_DIR}/Representations/TimeInfo.cpp
  ${CALIBRATOR_SRC_DIR}/Representations/ModuleInfo.cpp
  ${CALIBRATOR_SRC_DIR}/RobotConsole.cpp
  ${CALIBRATOR_SRC_DIR}/Views/ImageView.cpp
  #${CALIBRATOR_SRC_DIR}/Views/ImageView.h
  #${CALIBRATOR_SRC_DIR}/Views/ColorCalibrationView/ColorCalibrationView.cpp
  #${CALIBRATOR_SRC_DIR}/Views/ColorCalibrationView/ColorCalibrationView.h
  #${CALIBRATOR_SRC_DIR}/Views/ColorCalibrationView/RangeSelector.cpp
  #${CALIBRATOR_SRC_DIR}/Views/ColorCalibrationView/RangeSelector.h
  #${CALIBRATOR_SRC_DIR}/Views/ColorCalibrationView/ThresholdSelector.cpp
  ${CALIBRATOR_SRC_DIR}/Views/ConsoleView.cpp
  #${CALIBRATOR_SRC_DIR}/Views/ColorCalibrationView/ThresholdSelector.h
  ${CALIBRATOR_SRC_DIR}/Views/DataView/DataView.cpp
  #${CALIBRATOR_SRC_DIR}/Views/DataView/DataView.h
  ${CALIBRATOR_SRC_DIR}/Views/DataView/DataWidget.cpp
  #${CALIBRATOR_SRC_DIR}/Views/DataView/DataWidget.h
  ${CALIBRATOR_SRC_DIR}/Views/DataView/EditorEventFilter.cpp
  #${CALIBRATOR_SRC_DIR}/Views/DataView/EditorEventFilter.h
  ${CALIBRATOR_SRC_DIR}/Views/DataView/PropertyEditorFactory.cpp
  #${CALIBRATOR_SRC_DIR}/Views/DataView/PropertyEditorFactory.h
  ${CALIBRATOR_SRC_DIR}/Views/DataView/PropertyManager.cpp
  #${CALIBRATOR_SRC_DIR}/Views/DataView/PropertyManager.h
  ${CALIBRATOR_SRC_DIR}/Views/DataView/PropertyTreeCreator.cpp
  #${CALIBRATOR_SRC_DIR}/Views/DataView/PropertyTreeCreator.h
  ${CALIBRATOR_SRC_DIR}/Views/DataView/PropertyTreeWriter.cpp
  #${CALIBRATOR_SRC_DIR}/Views/DataView/PropertyTreeWriter.h
  #${CALIBRATOR_SRC_DIR}/Views/DataView/TypeDeclarations.h
  ${CALIBRATOR_SRC_DIR}/Views/DataView/TypeDescriptor.cpp
  #${CALIBRATOR_SRC_DIR}/Views/DataView/TypeDescriptor.h
  ${CALIBRATOR_SRC_DIR}/Views/TimeView.cpp
  ${CALIBRATOR_SRC_DIR}/Views/StatusView.cpp
  #${CALIBRATOR_SRC_DIR}/Views/StatusView.h
  ${CALIBRATOR_SRC_DIR}/Visualization/DebugDrawing.cpp
  #${CALIBRATOR_SRC_DIR}/Visualization/DebugDrawing.h
  ${CALIBRATOR_SRC_DIR}/Visualization/PaintMethods.cpp
  #${CALIBRATOR_SRC_DIR}/Visualization/PaintMethods.h
  src/Platform/Linux/RobotCtrlSim.cpp
)

#Vision Tool Resurces
set( VISION_TOOL_SRC_DIR Utils/VisionTool)
#QT5_WRAP_CPP(VISION_TOOL_MOCSrcs ${VISION_TOOL_SRC_DIR}/VisionToolWidget.h ${VISION_TOOL_SRC_DIR}/MainWindow.h)
set( VISION_TOOL_SRC
  ${VISION_TOOL_SRC_DIR}/Main.cpp
  ${VISION_TOOL_SRC_DIR}/MainWindow.cpp
  #${VISION_TOOL_SRC_DIR}/MainWindow.h
  ${VISION_TOOL_SRC_DIR}/CommunicationHandler.cpp
  #${VISION_TOOL_SRC_DIR}/CommunicationHandler.h
  ${VISION_TOOL_SRC_DIR}/VisionTool.cpp
  #${VISION_TOOL_SRC_DIR}/VisionTool.h
  ${VISION_TOOL_SRC_DIR}/VisionToolWidget.cpp
  #${VISION_TOOL_SRC_DIR}/VisionToolWidget.h
  src/Platform/Linux/File.cpp
  src/Platform/File.cpp
  src/Platform/File.h
  src/Platform/Memory.cpp
  src/Platform/Memory.h
  src/Tools/AlignedMemory.cpp
  src/Tools/AlignedMemory.h
  src/Tools/Comm/BanknoteClassifierMessageHandler.cpp
  src/Tools/Comm/BanknoteClassifierMessageHandler.h
  src/Tools/Streams/Enum.h
  src/Tools/Global.cpp
  src/Tools/Global.h
  src/Tools/Math/Pose2f.h
  src/Tools/Math/Vector2.h
  src/Tools/MessageQueue/MessageQueue.cpp
  src/Tools/MessageQueue/MessageQueue.h
  src/Tools/Settings.cpp
  src/Tools/Settings.h
  src/Tools/SystemCall.cpp
  src/Tools/SystemCall.h

  src/Tools/MessageQueue/MessageIDs.h
)

#Qtpropertybrowser Resources
set( QTPROPERTYBROWSER_SRC_DIR Utils/qtpropertybrowser)
QT5_ADD_RESOURCES(QTPROPERTYBROWSER_RESOURCES_RCC ${QTPROPERTYBROWSER_SRC_DIR}/qtpropertybrowser.qrc)

set( QTPROPERTYBROWSER_SRC
  ${QTPROPERTYBROWSER_SRC_DIR}/qtbuttonpropertybrowser.cpp
  ${QTPROPERTYBROWSER_SRC_DIR}/qteditorfactory.cpp
  ${QTPROPERTYBROWSER_SRC_DIR}/qtgroupboxpropertybrowser.cpp
  ${QTPROPERTYBROWSER_SRC_DIR}/qtpropertybrowser.cpp
  ${QTPROPERTYBROWSER_SRC_DIR}/qtpropertybrowserutils.cpp
  ${QTPROPERTYBROWSER_SRC_DIR}/qtpropertymanager.cpp
  ${QTPROPERTYBROWSER_SRC_DIR}/qttreepropertybrowser.cpp
  ${QTPROPERTYBROWSER_SRC_DIR}/qtvariantproperty.cpp
)


#LibQxt Resources
set( LIBQXT_SRC_DIR Utils/libqxt)
set( LIBQXT_SRC
  ${LIBQXT_SRC_DIR}/qxtspanslider.cpp
)



set( LIB_ETHERNETIP_FANUC_SRC
	src/Tools/Fanuc/PositionRegisterCartesian.cpp
	src/Tools/Fanuc/PositionRegisterJoint.cpp
	src/Tools/Comm/TcpComm.cpp
)

set( LIB_ETHERNETIP_FANUC_INCLUDE
	src/Tools/Fanuc/PositionRegisterCartesian.h
  src/Tools/Fanuc/PositionRegisterJoint.h
  src/Tools/Fanuc/PacketEthernetIPFanuc.cpp
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
link_directories(${PYLON_ROOT}/lib64 /usr/local/lib)
ELSE(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
add_definitions(-DWINDOWS)
set(PYLON_ROOT $ENV{PYLON_DEV_DIR})
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    link_directories(${PYLON_ROOT}/lib/x64 /usr/local/lib)
elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
    link_directories(${PYLON_ROOT}/lib/Win32 /usr/local/lib)
endif()

ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")

message(STATUS ${EIGEN3_INCLUDE_DIR})

include_directories(${OpenCV_INCLUDE} ${EIGEN3_INCLUDE_DIR} ${PYLON_ROOT}/include ${CMAKE_CURRENT_SOURCE_DIR}/src ${CMAKE_CURRENT_SOURCE_DIR}/Utils/libqxt /usr/local/incude ${TORCH_INCLUDE_DIRS})

message(STATUS "Pylon dir find in " ${PYLON_ROOT})

add_library( banknoteClassifier SHARED ${BANCKNOTECLASSIFIER_SRC} ${BC_PLATFORM_SRC} ${OpenCV_INCLUDE} ${LIB_ETHERNETIP_FANUC_SRC} ${LIB_ETHERNETIP_FANUC_INCLUDE})
IF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
target_link_libraries( banknoteClassifier ${OpenCV_LIBS} Eigen3::Eigen pylonbase pylonutility GenApi_gcc_v3_1_Basler_pylon_v5_1 GCBase_gcc_v3_1_Basler_pylon_v5_1 geos)
ELSEIF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
target_link_libraries( banknoteClassifier ${OpenCV_LIBS} "${TORCH_LIBRARIES}" Eigen3::Eigen pylonbase pylonutility GenApi_gcc_v3_1_Basler_pylon_v5_1 GCBase_gcc_v3_1_Basler_pylon_v5_1 geos)
ELSE(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
target_link_libraries( banknoteClassifier ${OpenCV_LIBS} Eigen3::Eigen PylonBase_MD_VC120_v5_1 PylonUtility_MD_VC120_v5_1 GenApi_MD_VC120_v3_0_Basler_pylon_v5_1 GCBase_MD_VC120_v3_1_Basler_pylon_v5_1 ws2_32 winmm geos)
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")


include_directories(${OpenCV_INCLUDE} ${EIGEN3_INCLUDE_DIR} ${PYLON_ROOT}/include ${CMAKE_CURRENT_SOURCE_DIR}/src ${CMAKE_CURRENT_SOURCE_DIR}/Utils/libqxt /usr/local/incude ${TORCH_INCLUDE_DIRS} ${Qt5Widgets_INCLUDES})
add_library( banknoteClassifierSim SHARED ${BANCKNOTECLASSIFIER_SRC} ${BC_PLATFORM_SRC} ${OpenCV_INCLUDE} ${LIB_ETHERNETIP_FANUC_SRC} ${LIB_ETHERNETIP_FANUC_INCLUDE})
target_compile_definitions(banknoteClassifierSim PRIVATE CALIBRATION_TOOL)
IF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
target_link_libraries( banknoteClassifierSim ${OpenCV_LIBS} Eigen3::Eigen ${Qt5Widgets_LIBRARIES} pylonbase pylonutility GenApi_gcc_v3_1_Basler_pylon_v5_1 GCBase_gcc_v3_1_Basler_pylon_v5_1 geos)
ELSEIF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
target_link_libraries( banknoteClassifierSim ${OpenCV_LIBS} ${TORCH_LIBRARIES} Eigen3::Eigen ${Qt5Widgets_LIBRARIES} pylonbase pylonutility GenApi_gcc_v3_1_Basler_pylon_v5_1 GCBase_gcc_v3_1_Basler_pylon_v5_1 geos)
ELSE(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
target_link_libraries( banknoteClassifierSim ${OpenCV_LIBS} Eigen3::Eigen ${Qt5Widgets_LIBRARIES} PylonBase_MD_VC120_v5_1 PylonUtility_MD_VC120_v5_1 GenApi_MD_VC120_v3_0_Basler_pylon_v5_1 GCBase_MD_VC120_v3_1_Basler_pylon_v5_1 ws2_32 winmm geos)
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")

include_directories(${Qt5Widgets_INCLUDES} ${CMAKE_CURRENT_SOURCE_DIR}/Utils/qtpropertybrowser ${CMAKE_CURRENT_BINARY_DIR}/Utils/qtpropertybrowser ${CMAKE_CURRENT_SOURCE_DIR})
add_library( qtpropertybrowser STATIC ${QTPROPERTYBROWSER_SRC} ${QTPROPERTYBROWSER_RESOURCES_RCC})
qt5_use_modules(qtpropertybrowser Widgets)
target_compile_definitions(qtpropertybrowser PRIVATE QXT_STATIC )

include_directories(${Qt5Widgets_INCLUDES} ${CMAKE_CURRENT_SOURCE_DIR}/Utils/libqxt ${CMAKE_CURRENT_BINARY_DIR}/Utils/libqxt ${CMAKE_CURRENT_SOURCE_DIR})
add_library( libqxt STATIC ${LIBQXT_SRC})
qt5_use_modules(libqxt Widgets)
target_compile_definitions(libqxt PRIVATE QXT_STATIC )

add_executable( cameraCalibration ${CAMERA_CALIBRATION} ${OpenCV_INCLUDE})
IF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
target_link_libraries( cameraCalibration ${OpenCV_LIBS} pylonbase pylonutility GenApi_gcc_v3_1_Basler_pylon_v5_1 GCBase_gcc_v3_1_Basler_pylon_v5_1)
ELSEIF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
target_link_libraries( cameraCalibration ${OpenCV_LIBS} pylonbase pylonutility GenApi_gcc_v3_1_Basler_pylon_v5_1 GCBase_gcc_v3_1_Basler_pylon_v5_1)
ELSE(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
target_link_libraries( cameraCalibration ${OpenCV_LIBS} pylonBase_MD_VC120_v5_1 PylonUtility_MD_VC120_v5_1 GenApi_MD_VC120_v3_1_Basler_pylon_v5_1 GCBase_MD_VC120_v3_1_Basler_pylon_v5_1)
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")

add_executable(bc src/Platform/Linux/Main.cpp src/Platform/Linux/RobotCtrl.cpp)
include (GenerateExportHeader)          
generate_export_header(banknoteClassifier BASE_NAME banknoteClassifier EXPORT_MACRO_NAME BANCKNOTECLASSIFIER_EXPORT EXPORT_FILE_NAME BanknoteClassifier_EXPORTS.h STATIC_DEFINE BANCKNOTECLASSIFIER_EXPORTS_BUILT_AS_STATIC)
generate_export_header(banknoteClassifier BASE_NAME Process EXPORT_MACRO_NAME PROCESS_EXPORT EXPORT_FILE_NAME Process_EXPORTS.h STATIC_DEFINE PROCESS_EXPORTS_BUILT_AS_STATIC)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(bc banknoteClassifier)


include_directories(${OpenCV_INCLUDE} ${CMAKE_CURRENT_SOURCE_DIR}/src ${CMAKE_CURRENT_SOURCE_DIR}/Utils/CalibratorTool  ${CMAKE_CURRENT_BINARY_DIR} ${Qt5Widgets_INCLUDES})

add_executable(calibrationTool ${CALIBRATOR_TOOL_SRC} ${CALIBRATION_TOOL_RESOURCES_RCC})
target_compile_definitions(calibrationTool PRIVATE QXT_STATIC)
IF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
target_link_libraries( calibrationTool banknoteClassifierSim libqxt qtpropertybrowser ${Qt5Widgets_LIBRARIES})
ELSEIF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
target_link_libraries( calibrationTool banknoteClassifierSim libqxt qtpropertybrowser ${Qt5Widgets_LIBRARIES} rt pthread)
ELSE(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
target_link_libraries( calibrationTool banknoteClassifierSim libqxt qtpropertybrowser ${Qt5Widgets_LIBRARIES})
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")

