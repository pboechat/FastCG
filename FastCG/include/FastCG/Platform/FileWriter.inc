#include <FastCG/Core/Exception.h>

#include <fstream>

namespace
{
	template <typename T>
	void Write(const std::string &rFilePath, std::ios_base::openmode openMode, const T *pData, size_t dataSize)
	{
		if (rFilePath.empty())
		{
			return;
		}

		std::ofstream fileStream(rFilePath.c_str(), openMode);

		if (!fileStream.is_open())
		{
			FASTCG_THROW_EXCEPTION(FastCG::Exception, "Couldn't open file: %s", rFilePath.c_str());
		}

		fileStream.write((const char *)pData, dataSize);
		fileStream.close();
	}

}

namespace FastCG
{
	void FileWriter::WriteText(const std::string &rFilePath, const char *pData, size_t dataSize)
	{
		Write(rFilePath, std::ios_base::out, pData, dataSize);
	}

	void FileWriter::WriteBinary(const std::string &rFilePath, const uint8_t *pData, size_t dataSize)
	{
		Write(rFilePath, std::ios_base::out | std::ios_base::binary, pData, dataSize);
	}

}