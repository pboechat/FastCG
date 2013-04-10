#ifndef SHADERSOURCE_H
#define SHADERSOURCE_H

#include <string>

class ShaderSource
{
public:
	static void SetShaderFolderPath(const std::string& rShaderFolderPath);
	static std::string Parse(const std::string& rFileName);

private:
	static std::string s_mShaderFolderPath;

	ShaderSource();
	~ShaderSource();

};

#endif
