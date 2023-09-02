#ifndef PACKET_H
#define PACKET_H

#include <cstddef>
#include <cstdint>
#include <iostream>

class Packet {
private:

    enum { mHeaderSize = 4 };

    uint8_t*    mData{};
    int*        mSize{};
    int         mCapacity{};


public:

    // Assumes that data has memory allocated for header
    // Destructor will delete that data anyway
    Packet(void* data, int size) :
        mData(static_cast<uint8_t*>(data)),
        mSize(reinterpret_cast<int*>(mData)),
        mCapacity(size)
    {
        *mSize = size;
    }

    // Additionally allocate memory for header
    Packet(size_t size) :
        mData(new uint8_t[size + mHeaderSize]),
        mSize(reinterpret_cast<int*>(mData)),
        mCapacity(size + mHeaderSize)
    {
        *mSize = size + mHeaderSize;
    }

    Packet(){}

    // PROVIDE COPY & MOVE CTOR!!!
    //Packet(const Packet& pack);
    //Packet(const Packet&& pack);

    ~Packet()
    { if (mData) delete[] mData; }

    int getSizeWithHeader() const { return *mSize; }
    int getSizeWithoutHeader() const { return *mSize - mHeaderSize; }
    uint8_t* getPtrToData() const { return mData + mHeaderSize; }
    uint8_t* getPtrToHeader() const { return mData; }
    int getCapacity() const { return mCapacity; }

    void resize(int size)
    {
        size += mHeaderSize;
        if (size > mCapacity) {
            if(mData) delete[] mData;
            mData = new uint8_t[size];
            mCapacity = size;
        }
        mSize = reinterpret_cast<int*>(mData);
        *mSize = size;
    }

};

#endif
