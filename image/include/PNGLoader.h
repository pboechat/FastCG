#ifndef PNGLOADER_H_
#define PNGLOADER_H_

#include <string>

class PNGLoader
{
public:
	static void Load(const std::string& rFileName, unsigned int* pWidth, unsigned int* pHeight, bool* pTransparency, unsigned char** pData);

private:
	PNGLoader()
	{
	}

	~PNGLoader()
	{
	}

};

#endif