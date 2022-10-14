#ifndef FASTCG_FILE_READER_H_
#define FASTCG_FILE_READER_H_

#include <string>
#include <memory>

namespace FastCG
{
	class FileReader
	{
	public:
		static std::unique_ptr<char[]> ReadText(const std::string &rFileName, size_t &rFileSize);
		static std::unique_ptr<uint8_t[]> ReadBinary(const std::string &rFileName, size_t &rFileSize);

	private:
		FileReader() = delete;
		~FileReader() = delete;
	};

}

#endif
