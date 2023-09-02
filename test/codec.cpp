extern "C" {
#include <libavcodec/codec_par.h>
#include <libavcodec/packet.h>
#include <libavutil/avutil.h>
#include <libavutil/frame.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
}

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main() 
{
    // standard file for all testings
    const char filename[] = "ffmpeg/small_bunny_1080p_60fps.mp4";

    // getting header of container
    AVFormatContext* pFormatContext = avformat_alloc_context();
    avformat_open_input(&pFormatContext, filename, NULL, NULL);
    printf("Format %s, duration %ld us\n", pFormatContext->iformat->long_name, pFormatContext->duration);
    
    // get info of all streams
    avformat_find_stream_info(pFormatContext, NULL);

    AVCodecParameters *pCodecParameters = NULL;
    const AVCodec *pCodec = NULL;
    int video_stream_index = -1;

    // loop through all the streams
    for (unsigned int i = 0; i < pFormatContext->nb_streams; ++i) {

        // getting codec parameters 
        AVCodecParameters *pLocalCodecParameters = pFormatContext->streams[i]->codecpar;

        // find codec according codec parameters
        const AVCodec *pLocalCodec = avcodec_find_decoder(pLocalCodecParameters->codec_id);


        // output information about the codec
        // specific for video and audio
        if (pLocalCodecParameters->codec_type == AVMEDIA_TYPE_VIDEO) {
            printf("Video Codec: resolution %d x %d\n",   pLocalCodecParameters->width,
                    pLocalCodecParameters->height);

            if (video_stream_index == -1) {
              video_stream_index = i;
              pCodec = pLocalCodec;
              pCodecParameters = pLocalCodecParameters;
            }
        }
        else if (pLocalCodecParameters->codec_type == AVMEDIA_TYPE_AUDIO)
            printf("Audio Codec: %d channels, sample rate %d\n",  pLocalCodecParameters->channels,
                                                                pLocalCodecParameters->sample_rate);
        // more general for both
        printf("\tCodec %s ID %d bit_rate %ld\n", pLocalCodec->long_name, pLocalCodec->id,
                                                pLocalCodecParameters->bit_rate);
    }

    // memory for holding processing information
    AVCodecContext *pCodecContext = avcodec_alloc_context3(pCodec);
    avcodec_parameters_to_context(pCodecContext, pCodecParameters);
    avcodec_open2(pCodecContext, pCodec, NULL);

    // also memory for processing information packets and frames
    AVPacket *pPacket = av_packet_alloc();
    AVFrame *pFrame = av_frame_alloc();

    // reading packets...
    while (av_read_frame(pFormatContext, pPacket) >= 0) {

        // if its the video stream
        if (pPacket->stream_index == video_stream_index) {

            // decode packet and get frame
            avcodec_send_packet(pCodecContext, pPacket);
            avcodec_receive_frame(pCodecContext, pFrame);

            // frame information
            printf (
                    "Frame %c (%ld) pts %ld dts %ld key_frame %d [coded_picture_num %d, display_picture_num %d]\t",
                    av_get_picture_type_char(pFrame->pict_type),
                    pCodecContext->frame_num,
                    pFrame->pts,
                    pFrame->pkt_dts,
                    pFrame->key_frame,
                    pFrame->coded_picture_number,
                    pFrame->display_picture_number
                   );
            printf("Sizeof Packet %d\tSizeof Frame %d\n", pPacket->size, pFrame->pkt_size);
        }

        av_packet_unref(pPacket);
    }



    avformat_close_input(&pFormatContext);
    av_packet_free(&pPacket);
    av_frame_free(&pFrame);
    avcodec_free_context(&pCodecContext);
    return 0;
}
