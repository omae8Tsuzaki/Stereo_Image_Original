cmake_minimum_required(VERSION 3.5)
project(Stereo_Image_Original)

FIND_PACKAGE( OpenCV REQUIRED )


set(SOURCE_FILES Stereo_Image_Original.cpp)

add_executable(Stereo_Image_Original ${SOURCE_FILES})

target_link_libraries(Stereo_Image_Original ${OpenCV_LIBS} )