#ifndef FASTCG_BINARY_STREAM_H
#define FASTCG_BINARY_STREAM_H

#include <vector>
#include <cstdint>

namespace FastCG
{
	class InputBinaryStream
	{
	public:
		InputBinaryStream(uint8_t *data, size_t size);

		template <typename T>
		inline bool Read(T *array, size_t length);
		template <typename T>
		inline bool Read(T &object);

	private:
		const uint8_t *mpData;
		size_t mSize;
		size_t mCurrent{0};
	};

	class OutputBinaryStream
	{
	public:
		template <typename T>
		inline void Write(T *array, size_t length);
		template <typename T>
		inline void Write(T &object);
		inline const uint8_t *GetData() const;
		inline size_t GetSize() const;

	private:
		std::vector<uint8_t> mData;
	};

}

#include <FastCG/BinaryStream.inc>

#endif
