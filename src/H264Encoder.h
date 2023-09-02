#ifndef H264_ENCODER_H
#define H264_ENCODER_H

#include <cstring>
#include <iostream>

#include <wels/codec_api.h>
#include <wels/codec_app_def.h>
#include <wels/codec_def.h>
#include <wels/codec_ver.h>

#include <libyuv.h>

#include "Packet.h"
#include "Frame.h"

class H264Encoder {
private:

    ISVCEncoder*    mEncoder    {};
    SSourcePicture  mPic        {};
    SFrameBSInfo    mInfo       {};
    FrameI420       mFrame        ;
    Packet          mPack       {};
    int             mTimestamp  {};
    int             mWidth      {};
    int             mHeight     {};

public:
    
    H264Encoder(int width, int height);
    ~H264Encoder();

    //Packet* encodeFrame(const uint8_t* frameRGBA);
    Packet* encodeFrame(const uint8_t* frameRGBA);
};

#endif
