#ifndef FASTCG_BINARY_STREAM_H
#define FASTCG_BINARY_STREAM_H

#include <vector>
#include <cstdint>
#include <cassert>

namespace FastCG
{
	class InputBinaryStream
	{
	public:
		InputBinaryStream(uint8_t *data, size_t size) : mpData(data), mSize(size)
		{
			assert(data != nullptr);
		}

		template <typename T>
		inline bool Read(T *array, size_t length)
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
		inline bool Read(T &object)
		{
			if (mCurrent + sizeof(T) > mSize)
			{
				return false;
			}
			memcpy((void *)&object, &mpData[mCurrent], sizeof(T));
			mCurrent += sizeof(T);
			return true;
		}

	private:
		const uint8_t *mpData;
		size_t mSize;
		size_t mCurrent{0};
	};

	class OutputBinaryStream
	{
	public:
		template <typename T>
		inline void Write(T *array, size_t length)
		{
			size_t size = length * sizeof(T);
			size_t current = mData.size();
			mData.resize(mData.size() + size);
			memcpy(&mData[current], (void *)array, size);
		}

		template <typename T>
		inline void Write(T &object)
		{
			size_t current = mData.size();
			mData.resize(mData.size() + sizeof(T));
			memcpy(&mData[current], (void *)&object, sizeof(T));
		}

		inline const uint8_t *GetData() const
		{
			return &mData[0];
		}

		inline size_t GetSize() const
		{
			return mData.size();
		}

	private:
		std::vector<uint8_t> mData;
	};

}

#endif
