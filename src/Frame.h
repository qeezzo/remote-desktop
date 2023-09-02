#ifndef FRAME_H
#define FRAME_H

#include <memory>

class FrameRGBA {
private:

    int mSize;
    int mWidth;
    int mHeight;
    int mStride;

    std::unique_ptr<uint8_t> mData;

public:

    FrameRGBA(int width, int height) :
        mSize(width * height * 4),
        mWidth(width), mHeight(height), mStride(width * 4),
        mData(new uint8_t[mSize])
    {}

    size_t size() const { return mSize; }
    size_t stride() const { return mStride; }
    int width() const { return mWidth; }
    int height() const { return mHeight; }

    uint8_t* data() const { return mData.get(); }
};

class FrameI420 {
private:

    int mSize;
    int mWidth;
    int mHeight;
    int mStride[3];


    std::unique_ptr<uint8_t> mData;

public:

    FrameI420(int width, int height);

    size_t size() const { return mSize; }
    int width() const { return mWidth; }
    int height() const { return mHeight; }

    uint8_t* operator[](int index) const;
    const int* stride() const { return mStride; }
};

#endif
