#include <FileReader.h>
#include <IOExceptions.h>

#include <string.h>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>

FileReader::FileReader()
{
}

FileReader::~FileReader()
{
}

std::string FileReader::Read(const std::string& rFileName, FileMode mode)
{
	int unusedFileSize;
	return Read(rFileName, unusedFileSize, mode);
}

std::string FileReader::Read(const std::string& rFileName, int& fileSize, FileMode mode)
{
	char* pBuffer = 0;
	fileSize = 0;

	if (!rFileName.empty())
	{
		FILE* file = fopen(rFileName.c_str(), GetFileModeString(mode));

		if (file == 0)
		{
			THROW_EXCEPTION(FileNotFoundException, "File not found: %s", rFileName.c_str());
		}

		fseek(file, 0, SEEK_END);
		int size = ftell(file);
		fileSize = size;
		rewind(file);

		if (size > 0)
		{
			pBuffer = (char*) malloc(sizeof(char) * (size + 1));
			size = fread(pBuffer, sizeof(char), size, file);
			pBuffer[size] = '\0';
		}

		fclose(file);
	}


	std::string content(pBuffer);
	free(pBuffer);

	return content;
}

char* GetFileModeString(FileMode mode)
{
	switch (mode)
	{
	case FM_TEXT:
		return "rt";

	case FM_BINARY:
		return "rb";

	default:
		return NULL;
	}
}
