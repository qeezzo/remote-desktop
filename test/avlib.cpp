#include <iostream>
#include <libavutil/pixfmt.h>
#include <libyuv.h>
using namespace std;

// g++ -o %< % Screen.cpp -lavcodec -lavutil -lyuv -lxcb

#include "Screen.h"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavutil/imgutils.h>
#include <libavcodec/packet.h>
#include <libavutil/frame.h>
}

class AVapi {
private:

    const AVCodec *pCodec;
    AVCodecContext *pCodecContext;

    AVFrame *pFrameRGBA;
    AVFrame *pFrameYUV;
    AVPacket *pPacket;

    scr::Screen src;

public:

    AVapi(const char* codec_name, int width, int height) :
        pCodec(avcodec_find_encoder_by_name(codec_name))
    {
        if (!pCodec) {
            cerr << "Can't find " << codec_name << " codec" << endl;
            throw;
        }

        pCodecContext = avcodec_alloc_context3(pCodec);
        pCodecContext->bit_rate = 5'000'000;
        pCodecContext->width = width;
        pCodecContext->height = height;
        pCodecContext->time_base = AVRational{1, 60};
        pCodecContext->framerate = AVRational{60, 1};
        pCodecContext->pix_fmt = AV_PIX_FMT_YUV420P;
        avcodec_open2(pCodecContext, pCodec, NULL);

        pFrameYUV = av_frame_alloc();
        pFrameYUV->format = pCodecContext->pix_fmt;
        pFrameYUV->width = pCodecContext->width;
        pFrameYUV->height = pCodecContext->height;
        int r1, r2;
        r1 = av_frame_get_buffer(pFrameYUV, 0);
        r2 = av_image_fill_arrays(pFrameYUV->data, pFrameYUV->linesize, nullptr,
                pCodecContext->pix_fmt, pFrameYUV->width, pFrameYUV->height, 64);
        if (r1 < 0 || r2 < 0)
            cerr << "EROROR" << endl;

        pFrameRGBA = av_frame_alloc();
        pFrameRGBA->format = AV_PIX_FMT_RGBA;
        pFrameRGBA->width = pCodecContext->width;
        pFrameRGBA->height = pCodecContext->height;
        pFrameRGBA->linesize[0] = width * 4;
        //av_frame_get_buffer(pFrameRGBA, 0);
        //av_image_fill_arrays(pFrameRGBA->data, pFrameRGBA->linesize, nullptr,
        //        AV_PIX_FMT_RGBA, pFrameRGBA->width, pFrameRGBA->height, 64);


        pPacket = av_packet_alloc();
    }

    ~AVapi()
    {
        avcodec_free_context(&pCodecContext);
        av_packet_free(&pPacket);
        av_frame_free(&pFrameYUV);
        av_frame_free(&pFrameRGBA);
    }

    void getPacket()
    {
        //r1 = av_frame_get_buffer(pFrameRGBA, 0);
        //r2 = av_image_fill_arrays(pFrameRGBA->data, pFrameRGBA->linesize, src.screen_data(),
        //       AV_PIX_FMT_RGBA, pFrameRGBA->width, pFrameRGBA->height, 1);
        //if (r1 < 0 || r2 < 0)
        //    cerr << "ERROR" << endl;

        //pFrameRGBA->data[0] = src.screen_data();
        //sws_scale(pSwsContext,pFrameRGBA->data, pFrameRGBA->linesize, 0, 1080, 
        //                        pFrameYUV->data, pFrameYUV->linesize);

        int width = 1920;
        int height = 1080;
        cout << "here" << endl;
        libyuv::ABGRToI420( src.screen_data(), width * 4,
                            (uint8_t*)pFrameYUV->data, width,
                            (uint8_t*)pFrameYUV->data + width * height, width /2,
                            (uint8_t*)pFrameYUV->data + width * height * 5 / 4, width / 2,
                            width, height);
        cout << "linesize : " << *pFrameYUV->linesize << endl;

        if (avcodec_send_frame(pCodecContext, pFrameYUV) != 0)
            cerr << "error send frame" << endl;
        cerr << "here" << endl;
        if (avcodec_receive_packet(pCodecContext, pPacket) < 0)
            cerr << "error receive packet" << endl;

        cout << "Sizeof packet : " << pPacket->size << endl;

        av_packet_unref(pPacket);
        src.screen_free_data();
    }
};


int main()
{
    AVapi api("libx264", 1920, 1080);

    api.getPacket();
    cout << "end" << endl;
    api.getPacket();
    cout << "end" << endl;
    api.getPacket();
    cout << "end" << endl;
    api.getPacket();

    cout << "end" << endl;
}
