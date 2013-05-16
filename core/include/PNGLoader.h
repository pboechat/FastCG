#ifndef PNGLOADER_H_
#define PNGLOADER_H_

#include <string>

class PNGLoader
{
public:
	static void Load(const std::string& rFileName, unsigned int& rWidth, unsigned int& rHeight, bool& rTransparency, unsigned char** ppData);

private:
	PNGLoader()
	{
	}

	~PNGLoader()
	{
	}

};

#endif