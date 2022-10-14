#ifndef FASTCG_FILE_WRITER_H_
#define FASTCG_FILE_WRITER_H_

#include <string>
#include <cstdint>

namespace FastCG
{
	class FileWriter
	{
	public:
		static void WriteText(const std::string &rFileName, const char *pData, size_t dataSize);
		static void WriteBinary(const std::string &rFileName, const uint8_t *pData, size_t dataSize);

	private:
		FileWriter() = delete;
		~FileWriter() = delete;
	};

}

#endif
