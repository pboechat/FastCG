#if (!defined(SHADERSOURCE_H_) && defined(USE_PROGRAMMABLE_PIPELINE))
#define SHADERSOURCE_H_

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
