#include <FastCG/ShaderSource.h>
#include <FastCG/Shader.h>

void CheckCompile(GLuint shaderObjectId, const std::string& rShaderName, const std::string& rShaderFileName)
{
	GLint compileStatus;
	glGetShaderiv(shaderObjectId, GL_COMPILE_STATUS, &compileStatus);

	if (compileStatus == GL_TRUE)
	{
		return;
	}

	int bufferLength;
	glGetShaderiv(shaderObjectId, GL_INFO_LOG_LENGTH, &bufferLength);

	if (bufferLength > 1)
	{
		char pBuffer[4096];
		glGetShaderInfoLog(shaderObjectId, bufferLength, &bufferLength, pBuffer);
		FASTCG_THROW_EXCEPTION(FastCG::Exception, "Shader (%s) info log ('%s'): %s", rShaderName.c_str(), rShaderFileName.c_str(), pBuffer);
	}
}

void CheckLink(GLuint programId, const std::string& rShaderName)
{
	GLint linkStatus;
	glGetProgramiv(programId, GL_LINK_STATUS, &linkStatus);

	if (linkStatus == GL_TRUE)
	{
		return;
	}

	GLint bufferLength;
	glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &bufferLength);

	char* pBuffer = new char[bufferLength];
	glGetProgramInfoLog(programId, bufferLength, &bufferLength, pBuffer);
	FASTCG_THROW_EXCEPTION(FastCG::Exception, "Shader (%s) info log: %s", rShaderName.c_str(), pBuffer);
	delete[] pBuffer;
}

void CheckValidate(GLuint programId, const std::string& rShaderName)
{
	GLint validateStatus;
	glGetProgramiv(programId, GL_VALIDATE_STATUS, &validateStatus);

	if (validateStatus == GL_TRUE)
	{
		return;
	}

	int bufferLength;
	glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &bufferLength);

	char* pBuffer = new char[bufferLength];
	glGetProgramInfoLog(programId, bufferLength, &bufferLength, pBuffer);
	FASTCG_THROW_EXCEPTION(FastCG::Exception, "Shader (%s) info log: %s", rShaderName.c_str(), pBuffer);
	delete[] pBuffer;
}

namespace FastCG
{
	Shader::Shader() :
		mName("Default")
	{
	}

	Shader::Shader(const std::string& rName) :
		mName(rName)
	{
	}

	Shader::~Shader()
	{
		DetachShaders();
		DeleteShaders();
		mShadersIds.clear();

		if (mProgramId != ~0u)
		{
			glDeleteProgram(mProgramId);
		}
	}

	void Shader::Compile(const std::string& rShaderFileName, ShaderType shaderType)
	{
		auto shaderSource = ShaderSource::Parse(rShaderFileName);
		auto shaderId = glCreateShader(GetShaderTypeMapping(shaderType));

		if (shaderSource.empty())
		{
			FASTCG_THROW_EXCEPTION(Exception, "Shader file not found: %s", rShaderFileName.c_str());
		}

		const auto* pShaderSource = shaderSource.c_str();
		glShaderSource(shaderId, 1, (const char**)&pShaderSource, 0);
		glCompileShader(shaderId);
		CheckCompile(shaderId, mName, rShaderFileName);
		mShadersIds.insert(std::make_pair(shaderType, shaderId));

		FASTCG_CHECK_OPENGL_ERROR();
	}

	void Shader::Link()
	{
		mProgramId = glCreateProgram();
		auto shaderIdsCursor = mShadersIds.begin();

		while (shaderIdsCursor != mShadersIds.end())
		{
			glAttachShader(mProgramId, shaderIdsCursor->second);
			shaderIdsCursor++;
		}

		BindAttributeLocation("position", VERTICES_ATTRIBUTE_INDEX);
		BindAttributeLocation("normal", NORMALS_ATTRIBUTE_INDEX);
		BindAttributeLocation("uv", UVS_ATTRIBUTE_INDEX);
		BindAttributeLocation("tangent", TANGENTS_ATTRIBUTE_INDEX);
		BindAttributeLocation("color", COLORS_ATTRIBUTE_INDEX);

		glLinkProgram(mProgramId);
		CheckLink(mProgramId, mName);
		glValidateProgram(mProgramId);
		CheckValidate(mProgramId, mName);
	}

	void Shader::DetachShaders()
	{
		auto shaderIdsCursor = mShadersIds.begin();
		while (shaderIdsCursor != mShadersIds.end())
		{
			glDetachShader(mProgramId, shaderIdsCursor->second);
			shaderIdsCursor++;
		}
	}

	void Shader::DeleteShaders()
	{
		auto shaderIdsCursor = mShadersIds.begin();
		while (shaderIdsCursor != mShadersIds.end())
		{
			glDeleteShader(shaderIdsCursor->second);
			shaderIdsCursor++;
		}
	}

}