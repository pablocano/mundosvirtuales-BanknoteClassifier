cmake_minimum_required(VERSION 2.8) 

PROJECT (GroundTruth)
find_package(OpenCV REQUIRED )
set( NAME_SRC
    src/main.cpp
    src/BackgroundModel.cpp
    src/BackgroundModel.h
    src/ColorModel.cpp
    src/ColorModel.h
    src/ColorModelConversions.h;
    src/Line.cpp
    src/Line.h
    src/Object.cpp
    src/Object.h
    src/Vector2.h
    src/Range.h
)

include_directories(${CMAKE_INSTALL_PREFIX}/include)
add_executable( groundTruth ${NAME_SRC} ${OpenCV_INCLUDE})

target_link_libraries( groundTruth ${OpenCV_LIBS} )
