#include <FastCG/FileWriter.h>
#include <FastCG/Exception.h>

#include <fstream>

template <typename T>
void Write(const std::string& rFileName, const T* pData, size_t dataSize)
{
	if (rFileName.empty())
	{
		return;
	}

	std::ofstream fileStream(rFileName.c_str());

	if (!fileStream.is_open())
	{
		FASTCG_THROW_EXCEPTION(FastCG::Exception, "Error opening file: %s", rFileName.c_str());
	}

	fileStream.write((const char*)pData, dataSize);
	fileStream.close();
}

namespace FastCG
{
	void FileWriter::WriteText(const std::string& rFileName, const char* pData, size_t dataSize)
	{
		Write(rFileName, pData, dataSize);
	}

	void FileWriter::WriteBinary(const std::string& rFileName, const uint8_t* pData, size_t dataSize)
	{
		Write(rFileName, pData, dataSize);
	}

}