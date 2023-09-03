cmake_minimum_required(VERSION 3.2)

project(remote-desktop LANGUAGES CXX)
set(SrcDir "src")

add_subdirectory(src/timer)

add_executable(sender 
    ${SrcDir}/sender.cpp
    ${SrcDir}/NetServer.cpp
    ${SrcDir}/NetIO.cpp
    ${SrcDir}/ScreenData.cpp
    ${SrcDir}/H264Encoder.cpp
    ${SrcDir}/Frame.cpp
)

add_executable(reciever 
    ${SrcDir}/reciever.cpp
    ${SrcDir}/NetClient.cpp
    ${SrcDir}/NetIO.cpp
    ${SrcDir}/H264Decoder.cpp
    ${SrcDir}/Frame.cpp
    ${SrcDir}/ScreenData.cpp

    #${SrcDir}/H264Encoder.cpp
)

target_link_libraries(sender PRIVATE xcb xcb-image yuv timer openh264 X11 xcb-xtest)
target_link_libraries(reciever PRIVATE xcb xcb-image yuv timer openh264 X11 xcb-xtest)
