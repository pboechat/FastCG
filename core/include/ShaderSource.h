#if (!defined(SHADERSOURCE_H_) && !defined(FIXED_FUNCTION_PIPELINE))
#define SHADERSOURCE_H_

#include <string>

class ShaderSource
{
public:
	static std::string Parse(const std::string& rFileName);

private:
	static std::string s_mShaderFolderPath;

	ShaderSource();
	~ShaderSource();

};

#endif
