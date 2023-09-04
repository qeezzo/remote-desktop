#include "H264Decoder.h"

H264Decoder::H264Decoder(int width, int height)
    : mWidth(width), mHeight(height)
{
    WelsCreateDecoder(&mDecoder);
    if (!mDecoder)
        throw "Error: failed to create dceoder";

    SDecodingParam param{};
    param.sVideoProperty.eVideoBsType = VIDEO_BITSTREAM_DEFAULT;
    param.bParseOnly = false;
    param.eEcActiveIdc = ERROR_CON_SLICE_COPY;
    param.uiTargetDqLayer = UCHAR_MAX;

    if (mDecoder->Initialize(&param))
        throw "Error: failed to initialize dceoder";

    // TODO: implement debug case output
    //int logLevel = WELS_LOG_INFO;
    //mDecoder->SetOption(DECODER_OPTION_TRACE_LEVEL, &logLevel);
}

H264Decoder::~H264Decoder()
{
    mDecoder->Uninitialize();
    WelsDestroyDecoder(mDecoder);
}

FrameRGBA* H264Decoder::decodePacket(const Packet* pack)
{
    if (mDecoder->DecodeFrameNoDelay(pack->getPtrToData(),
                                    pack->getSizeWithoutHeader(),
                                    YUVframe, &mInfo)) 
    {
        std::cerr << "Can't decode packet" << std::endl;
        return nullptr;
    }

    static FrameRGBA frame{mWidth, mHeight};

    libyuv::I420ToABGR( mInfo.pDst[0], mInfo.UsrData.sSystemBuffer.iStride[0],
            mInfo.pDst[1], mInfo.UsrData.sSystemBuffer.iStride[1],
            mInfo.pDst[2], mInfo.UsrData.sSystemBuffer.iStride[1],
            frame.data(), mWidth * 4,
            mWidth, mHeight);

    return &frame;
}
