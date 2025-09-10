cmake_minimum_required(VERSION 3.16)
project(video_metadata_reader VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(videoinfo src/main.cpp)


set(VCPKG_TRIPLET_DIR "G:/Tools/vcpkg/installed/x64-windows")

# Headers
target_include_directories(videoinfo PRIVATE
  "${VCPKG_TRIPLET_DIR}/include"
)

# Libraries 
target_link_directories(videoinfo PRIVATE
  "${VCPKG_TRIPLET_DIR}/lib"
  "${VCPKG_TRIPLET_DIR}/debug/lib"
)

# FFMPEG Libs
target_link_libraries(videoinfo PRIVATE
  avformat
  avcodec
  avutil
  swresample
  swscale
)
