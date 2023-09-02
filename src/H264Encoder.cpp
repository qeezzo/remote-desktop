#include "H264Encoder.h"
#include "Frame.h"

using std::cerr, std::endl;

H264Encoder::H264Encoder(int width, int height) 
        : mFrame(width, height), mWidth(width), mHeight(height)
{
    WelsCreateSVCEncoder(&mEncoder);
    if (!mEncoder)
        throw "Error: failed to create encoder";


    SEncParamBase param{};
    param.iUsageType = CAMERA_VIDEO_REAL_TIME;
    //param.iRCMode = RC_QUALITY_MODE;
    param.iTargetBitrate = 1024*1024*8;
    param.fMaxFrameRate = 60.0f;
    param.iPicWidth = mWidth;
    param.iPicHeight = mHeight;

    if (mEncoder->Initialize(&param))
        throw "Error: can't initialize encoder with params";

    mPic.iPicWidth = mWidth;
    mPic.iPicHeight = mHeight;
    mPic.iColorFormat = videoFormatI420;
    mPic.iStride[0] = mFrame.stride()[0];
    mPic.iStride[1] = mFrame.stride()[1];
    mPic.iStride[2] = mFrame.stride()[2];
    mPic.pData[0] = mFrame[0];
    mPic.pData[1] = mFrame[1];
    mPic.pData[2] = mFrame[2];
    //mPic.iStride[0] = mWidth;
    //mPic.iStride[1] = mWidth / 2;
    //mPic.iStride[2] = mWidth / 2;
    //mPic.pData[0] = mpI420Buffer;
    //mPic.pData[1] = mPic.pData[0] + mWidth * mHeight;
    //mPic.pData[2] = mPic.pData[1] + mWidth * mHeight / 4;


#ifdef DEBUG
    int logLevel = WELS_LOG_INFO;
    mEncoder->SetOption (ENCODER_OPTION_TRACE_LEVEL, &logLevel);
#endif
}

H264Encoder::~H264Encoder()
{
    mEncoder->Uninitialize();
    WelsDestroySVCEncoder(mEncoder);
}

Packet* H264Encoder::encodeFrame(const uint8_t* frameRGBA) {
    //libyuv::ABGRToI420( data, mWidth * 4,
    //        mpI420Buffer, mWidth,
    //        mpI420Buffer + mWidth * mHeight, mWidth /2,
    //        mpI420Buffer + mWidth * mHeight * 5 / 4, mWidth / 2,
    //        mWidth, mHeight);
    libyuv::ABGRToI420( frameRGBA, mWidth * 4,
            mFrame[0], mFrame.stride()[0],
            mFrame[1], mFrame.stride()[1],
            mFrame[2], mFrame.stride()[2],
            mWidth, mHeight);

    mPic.uiTimeStamp = mTimestamp;
    mTimestamp += 1000 / 60;

    if (mEncoder->EncodeFrame(&mPic, &mInfo)) {
        cerr << "Warning: frame wasn't encoded" << endl;
        return nullptr;
    }

    size_t size = mInfo.iFrameSizeInBytes;
    mPack.resize(size);

    if (mInfo.eFrameType == videoFrameTypeSkip) {
            //|| mInfo.eFrameType == videoFrameTypeInvalid) {
        cerr << "Waring: skipping frame." << endl;
        return nullptr; 
    }

    int offset{};
    for (int iLayer=0; iLayer < mInfo.iLayerNum; iLayer++) {

        int iLayerSize = 0;
        for (int nal{}; nal < mInfo.sLayerInfo[iLayer].iNalCount; nal++)
            iLayerSize += mInfo.sLayerInfo[iLayer].pNalLengthInByte[nal];

        //memcpy(sPack.data() + offset, mInfo.sLayerInfo[iLayer].pBsBuf, iLayerSize);
        memcpy(mPack.getPtrToData() + offset, mInfo.sLayerInfo[iLayer].pBsBuf, iLayerSize);
        offset += iLayerSize;
    }

    return &mPack;
    //return &sPack;
}
