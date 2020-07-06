project(AQ_002_CONSOLE)
cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 17)

find_package(OpenCV 3.4.6 REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

aux_source_directory(./src SRC_LIST)

include_directories(include lib_include)

link_directories(lib/64)

file(GLOB _SRC
        "*.h"
        "*.cpp"
        )

#add_library(aq SHARED ${_SRC} ${SRC_LIST})
#target_link_libraries(aq MvCameraControl)

add_executable(${PROJECT_NAME} ${_SRC} ${SRC_LIST} )

target_link_libraries(${PROJECT_NAME} pthread MvCameraControl X11 ${OpenCV_LIBS}  opencv_highgui)


