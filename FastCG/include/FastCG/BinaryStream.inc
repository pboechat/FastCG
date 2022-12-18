#include <cassert>

namespace FastCG
{
    InputBinaryStream::InputBinaryStream(uint8_t *data, size_t size) : mpData(data), mSize(size)
    {
        assert(data != nullptr);
    }

    template <typename T>
    bool InputBinaryStream::Read(T *array, size_t length)
    {
        assert(array != nullptr);
        assert(length > 0);
        size_t size = length * sizeof(T);
        if (mCurrent + size > mSize)
        {
            return false;
        }
        memcpy((void *)array, &mpData[mCurrent], size);
        mCurrent += size;
        return true;
    }

    template <typename T>
    bool InputBinaryStream::Read(T &object)
    {
        if (mCurrent + sizeof(T) > mSize)
        {
            return false;
        }
        memcpy((void *)&object, &mpData[mCurrent], sizeof(T));
        mCurrent += sizeof(T);
        return true;
    }

    template <typename T>
    inline void OutputBinaryStream::Write(T *array, size_t length)
    {
        size_t size = length * sizeof(T);
        size_t current = mData.size();
        mData.resize(mData.size() + size);
        memcpy(&mData[current], (void *)array, size);
    }

    template <typename T>
    inline void OutputBinaryStream::Write(T &object)
    {
        size_t current = mData.size();
        mData.resize(mData.size() + sizeof(T));
        memcpy(&mData[current], (void *)&object, sizeof(T));
    }

    inline uint8_t *OutputBinaryStream::GetData()
    {
        return &mData[0];
    }

    inline const uint8_t *OutputBinaryStream::GetData() const
    {
        return const_cast<OutputBinaryStream *>(this)->GetData();
    }

    inline size_t OutputBinaryStream::GetSize() const
    {
        return mData.size();
    }
}