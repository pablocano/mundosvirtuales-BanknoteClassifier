cmake_minimum_required(VERSION 2.8) 

PROJECT (GroundTruth)
find_package(OpenCV REQUIRED )

set( SEGMENT_SRC
    src/Processes/Segmentation.cpp
    src/Representations/ColorModel/ColorModel.cpp
    src/Representations/ColorModel/ColorModel.h
    src/Tools/ColorModelConversions.h
    src/Tools/ColorRangeCreator.h
    src/Tools/Range.h
)

set( GROUNDTRUTH_SRC
	src/Processes/GroundTruth.cpp
	src/Representations/ColorModel/ColorModel.cpp
	src/Representations/ColorModel/ColorModel.h
	src/Tools/ColorModelConversions.h
	src/Tools/Range.h
	src/Tools/Vector2.h
)
source_group(src REGULAR_EXPRESSION "src/Processes/*")
source_group(src\\Representations REGULAR_EXPRESSION "Representations/*")
source_group(src\\Representations\\ColorModel REGULAR_EXPRESSION "Representations/ColorModel/*")
source_group(src\\Tools REGULAR_EXPRESSION "Tools/*")

include_directories(${CMAKE_INSTALL_PREFIX}/include ${CMAKE_CURRENT_SOURCE_DIR}/src)
add_executable( createColorTable ${SEGMENT_SRC} ${OpenCV_INCLUDE})
target_link_libraries( createColorTable ${OpenCV_LIBS} )

add_executable( groundTruth ${GROUNDTRUTH_SRC} ${OpenCV_INCLUDE})
target_link_libraries( groundTruth ${OpenCV_LIBS} )
