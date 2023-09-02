#include "Frame.h"

FrameI420::FrameI420(int width, int height) :
    mSize(width * height * 3/2),
    mWidth(width), mHeight(height), 
    mData(new uint8_t[mSize])
{
    mStride[0] = mWidth;
    mStride[1] = mStride[2] = mWidth / 2;
}

// TODO Looks strange. should rewrite this as con above
uint8_t* FrameI420::operator[](int index) const
{
    switch (index) {
        case 0:     return mData.get(); 
        case 1:     return mData.get() + mWidth * mHeight;
        case 2:     return mData.get() + mWidth * mHeight * 5 / 4;
        default:    return nullptr;
    }
}
