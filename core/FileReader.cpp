#include "include/FileReader.h"
#include "include/IOExceptions.h"

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

char* FileReader::Read(const std::string& rFileName, FileMode mode)
{
	int unusedFileSize;
	return Read(rFileName, unusedFileSize, mode);
}

char* FileReader::Read(const std::string& rFileName, int& fileSize, FileMode mode)
{
	char* pContent = NULL;
	fileSize = 0;

	if (!rFileName.empty())
	{
		FILE* file = fopen(rFileName.c_str(), GetFileModeString(mode));

		if (file == NULL)
		{
			//THROW_EXCEPTION(FileNotFoundException, rFileName.c_str());
			FileNotFoundException exception(rFileName.c_str());
			exception.SetFunction(__FUNCTION__);
			exception.SetFile(__FILE__);
			exception.SetLine(__LINE__);
			throw exception;
		}

		fseek(file, 0, SEEK_END);
		int size = ftell(file);
		fileSize = size;
		rewind(file);

		if (size > 0)
		{
			pContent = (char*)malloc(sizeof(char) * (size + 1));
			size = fread(pContent, sizeof(char), size, file);
			pContent[size] = '\0';
		}

		fclose(file);
	}

	return pContent;
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
