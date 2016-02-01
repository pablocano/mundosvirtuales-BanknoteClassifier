cmake_minimum_required(VERSION 2.8) 

PROJECT (GroundTruth)
find_package(OpenCV REQUIRED )
set( NAME_SRC
    src/main.cpp
    src/ColorModel/ColorModel.cpp
    src/ColorModel/ColorModel.h
    src/ColorModel/ColorModelConversions.h
    src/ColorModel/ColorRangeCreator.h
    src/Tools/Vector2.h
    src/Tools/Range.h
)

include_directories(${CMAKE_INSTALL_PREFIX}/include ${CMAKE_CURRENT_SOURCE_DIR}/src)
add_executable( groundTruth ${NAME_SRC} ${OpenCV_INCLUDE})

target_link_libraries( groundTruth ${OpenCV_LIBS} )
