#ifndef FASTCG_FILE_READER_H
#define FASTCG_FILE_READER_H

#include <string>
#include <memory>

namespace FastCG
{
	class FileReader final
	{
	public:
		inline static std::unique_ptr<char[]> ReadText(const std::string &rFileName, size_t &rFileSize);
		inline static std::unique_ptr<uint8_t[]> ReadBinary(const std::string &rFileName, size_t &rFileSize);

	private:
		FileReader() = delete;
		~FileReader() = delete;
	};

}

#include <FastCG/Platform/FileReader.inc>

#endif
