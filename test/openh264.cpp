// Compile settings
// g++ -o %< % Screen.cpp -lopenh264 -lxcb -lxcb-image -lyuv -ltimer -lImlib2

#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <cstring>
#include <climits>
using namespace std;

#include "Screen.h"

#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include <wels/codec_api.h>
#include <wels/codec_app_def.h>
#include <wels/codec_def.h>
#include <wels/codec_ver.h>

#include <libyuv.h>
#include <Imlib2.h>

class Encoder {
private:

    ISVCEncoder *encoder;
    SSourcePicture pic;
    SFrameBSInfo info;
    uint8_t* pI420Buffer;
    int timestamp{};

    int width;
    int height;

public:
    
    Encoder(int w, int h) 
        : width(w), height(h)
    {
        WelsCreateSVCEncoder(&encoder);

        SEncParamBase param;
        memset(&param, 0, sizeof(SEncParamBase));
        param.iUsageType = CAMERA_VIDEO_REAL_TIME;
        param.iRCMode = RC_QUALITY_MODE;
        param.iTargetBitrate = 5'000'000;
        param.fMaxFrameRate = 60.0f;
        param.iPicWidth = width;
        param.iPicHeight = height;

        if ( encoder->Initialize(&param) ) {
            cerr << "Can't initialize encoder with params" << endl;
            exit(EXIT_FAILURE);
        }

#ifdef VERBOSE
        int logLevel = WELS_LOG_INFO;
        encoder->SetOption (ENCODER_OPTION_TRACE_LEVEL, &logLevel);
        //encoder->SetOption (ENCODER_OPTION_ENABLE_PREFIX_NAL_ADDING, &t);
#endif

        pI420Buffer = new uint8_t[width * height * 3 / 2];

        memset(&info, 0, sizeof(SFrameBSInfo));

    }

    ~Encoder()
    {
        encoder->Uninitialize();
        WelsDestroySVCEncoder(encoder);
        delete [] pI420Buffer;
    }

    int encodeFrame(const uint8_t* data, uint8_t* packet)
    {
        libyuv::ABGRToI420( data, width * 4,
                            pI420Buffer, width,
                            pI420Buffer + width * height, width /2,
                            pI420Buffer + width * height * 5 / 4, width / 2,
                            width, height);

        memset(&pic, 0, sizeof(SSourcePicture));

        pic.iPicWidth = width;
        pic.iPicHeight = height;
        pic.iColorFormat = videoFormatI420;
        pic.iStride[0] = width;
        pic.iStride[1] = width / 2;
        pic.iStride[2] = width / 2;
        pic.pData[0] = pI420Buffer;
        pic.pData[1] = pic.pData[0] + width * height;
        pic.pData[2] = pic.pData[1] + width * height / 4;
        pic.uiTimeStamp = timestamp;
        timestamp += 1000 / 60;

        if (encoder->EncodeFrame(&pic, &info)) {
            cout << "Frame wasn't encoded" << endl;
            return -1;
        }

        if (info.eFrameType != videoFrameTypeSkip) {

            int offset{};
            for (int iLayer=0; iLayer < info.iLayerNum; iLayer++)
            {
                int iLayerSize = 0;
                for (int nal{}; nal < info.sLayerInfo[iLayer].iNalCount; nal++)
                    iLayerSize += info.sLayerInfo[iLayer].pNalLengthInByte[nal];

                memcpy(&packet[offset], info.sLayerInfo[iLayer].pBsBuf, iLayerSize);
                offset += iLayerSize;
            }
        } else { 
            cout << "Waring, skipping frame." << endl;
            return 0; 
        }

        return info.iFrameSizeInBytes;
        //cout << "Packet size in bytes " << info.iFrameSizeInBytes << endl;
    }

};

class Decoder {
private:

    ISVCDecoder *decoder;
    SBufferInfo info;
    uint8_t* YUVframe[3];
    int width;
    int height;

public:
    
    Decoder(int w, int h)
        : width(w), height(h)
    {
        WelsCreateDecoder(&decoder);

        memset(&info, 0, sizeof(SBufferInfo));

        SDecodingParam param;
        memset(&param, 0, sizeof(SDecodingParam));
        param.sVideoProperty.eVideoBsType = VIDEO_BITSTREAM_DEFAULT;
        param.bParseOnly = false;
        param.eEcActiveIdc = ERROR_CON_SLICE_COPY;
        param.uiTargetDqLayer = UCHAR_MAX;
        //param.sVideoProperty.eVideoBsType = VIDEO_BITSTREAM_AVC;
        //param.sVideoProperty.size = sizeof(SVideoProperty);
        //param.uiCpuLoad = 1;
        
        if (decoder->Initialize(&param)) {
            cout << "Can't Initialize decoder" << endl;
            exit(EXIT_FAILURE);
        }

#ifdef VERBOSE
        int logLevel = WELS_LOG_INFO;
        decoder->SetOption(DECODER_OPTION_TRACE_LEVEL, &logLevel);
#endif
    }

    ~Decoder()
    {
        decoder->Uninitialize();
        WelsDestroyDecoder(decoder);
    }

    int decodePacket(uint8_t* RGBAframe, const uint8_t* packet, int size)
    {

        if (int rv = decoder->DecodeFrameNoDelay(packet, size, YUVframe, &info)) {
            cout << "Can't decode packet" << endl;
            return -1;
        }

        libyuv::I420ToABGR( info.pDst[0], info.UsrData.sSystemBuffer.iStride[0],
                            info.pDst[1], info.UsrData.sSystemBuffer.iStride[1],
                            info.pDst[2], info.UsrData.sSystemBuffer.iStride[1],
                            RGBAframe, width * 4,
                            width, height);

        return 0;
    }

};

int ximage(uint8_t* rgba_data);

int main()
{
    const int width { 1920 };
    const int height { 1080 };

    unsigned char* packet { new unsigned char[1920 * 1080] };
    unsigned char* RGBAframe { new unsigned char[1920 * 1080 * 4] };

    scr::Screen screen;
    Encoder encoder(width, height);
    Decoder decoder(width, height);


    for (int i{}; i < 10; ++i) {
        Timer t;
        int count{};
        while (t.elapsed() < 1.0) {
            uint8_t* screen_data = screen.screen_data(width, height);
            int size = encoder.encodeFrame(screen_data, packet);
            //cout << "Packet size " << size << endl;
            decoder.decodePacket(RGBAframe, packet, size);
            screen.screen_free_data();
            ++count;
        }
        cout << "Encoded Frames : " << count << endl;
    }
    
    cout << "Openning window..." << endl;
    //ximage(RGBAframe);
    cout << "Closing window..." << endl;

    delete[] packet;
    delete[] RGBAframe;
}

int ximage(uint8_t* rgba_data) {
    Display *display;
    Window window;
    XEvent event;
    GC gc;
    XImage *image;
    int width = 1920;
    int height = 1080;

    // Connect to the X server
    display = XOpenDisplay(NULL);
    if (!display) {
        fprintf(stderr, "Unable to open the X display.\n");
        return 1;
    }

    // Create the window
    int screen = DefaultScreen(display);
    window = XCreateSimpleWindow(display, RootWindow(display, screen), 0, 0, width, height, 0, BlackPixel(display, screen), WhitePixel(display, screen));

    // Select the events to listen for
    XSelectInput(display, window, ExposureMask | KeyPressMask);

    // Map the window
    XMapWindow(display, window);

    // Create a graphics context
    gc = XCreateGC(display, window, 0, NULL);

    // Allocate memory for the RGBA image data
    //rgba_data = (uint32_t *)malloc(width * height * sizeof(uint32_t));
    if (!rgba_data) {
        fprintf(stderr, "Memory allocation failed.\n");
        return 1;
    }

    // Create an XImage to hold the RGBA data
    image = XCreateImage(display, DefaultVisual(display, screen), DefaultDepth(display, screen), ZPixmap, 0, (char *)rgba_data, width, height, 32, 0);

    // Main event loop
    while (1) {
        XNextEvent(display, &event);
        if (event.type == Expose) {
            // Draw the image on the window
            XPutImage(display, window, gc, image, 0, 0, 0, 0, width, height);
        } else if (event.type == KeyPress) {
            break; // Exit the loop when any key is pressed
        }
    }

    // Clean up and close the X server connection
    XDestroyImage(image);
    XFreeGC(display, gc);
    XDestroyWindow(display, window);
    XCloseDisplay(display);
    free(rgba_data);

    return 0;
}
