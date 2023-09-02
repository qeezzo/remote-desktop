cmake_minimum_required(VERSION 3.5)

project(opencv LANGUAGES CXX)

add_executable(opencv newidea.cpp)

find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )
target_link_libraries( opencv ${OpenCV_LIBS} )
