#ifndef H264_DECODER_H
#define H264_DECODER_H

#include <climits>
#include <cstring>
#include <iostream>

#include <wels/codec_api.h>
#include <wels/codec_app_def.h>
#include <wels/codec_def.h>
#include <wels/codec_ver.h>

#include <libyuv.h>

#include "Packet.h"
#include "Frame.h"

class H264Decoder {
private:

    int             mWidth;
    int             mHeight;
    uint8_t*        YUVframe[3];
    ISVCDecoder*    mDecoder;
    SBufferInfo     mInfo{};

public:
    
    H264Decoder(int width, int height);
    ~H264Decoder();

    FrameRGBA* decodePacket(const Packet* packet);
};

#endif
