#include "include/Shader.h"
#include "include/ShaderSource.h"
#include "include/Exception.h"

// ====================================================================================================
Shader::Shader(const std::string& rName)
	:
	mProgramId(-1),
	mName(rName)
{
}

// ====================================================================================================
Shader::~Shader()
{
	DetachShaders();
	DeleteShaders();
	mShadersIds.clear();

	if (mProgramId != -1)
	{
		glDeleteProgram(mProgramId);
	}
}

// ====================================================================================================
void Shader::Compile(const std::string& rShaderFileName, unsigned int shaderType)
{
	std::string shaderSource = ShaderSource::Parse(rShaderFileName);
	unsigned int shaderId = glCreateShader(shaderType);

	if (shaderSource.empty())
	{
		// TODO:
		THROW_EXCEPTION(Exception, "Shader file not found: %s\n", rShaderFileName.c_str());
	}

	const char* pShaderSource = shaderSource.c_str();

	glShaderSource(shaderId, 1, (const char**)&pShaderSource, 0);
	glCompileShader(shaderId);

	CheckCompile(shaderId, rShaderFileName);

	mShadersIds.insert(std::make_pair(shaderType, shaderId));
}

// ====================================================================================================
void Shader::Link()
{
	mProgramId = glCreateProgram();
	std::map<unsigned int, unsigned int>::iterator shaderIdsCursor = mShadersIds.begin();

	while (shaderIdsCursor != mShadersIds.end())
	{
		glAttachShader(mProgramId, shaderIdsCursor->second);
		shaderIdsCursor++;
	}

	glLinkProgram(mProgramId);

	CheckLink();

	glValidateProgram(mProgramId);

	CheckValidate();
}

// ====================================================================================================
void Shader::SetUniformInt(const std::string& rParameterName, int value) const
{
	glUniform1i(GetUniformLocation(rParameterName), value);
}

// ====================================================================================================
void Shader::SetUniformFloat(const std::string& rParameterName, float value) const
{
	glUniform1f(GetUniformLocation(rParameterName), value);
}

// ====================================================================================================
void Shader::SetUniformBool(const std::string& rParameterName, bool value) const
{
	glUniform1i(GetUniformLocation(rParameterName), value);
}

// ====================================================================================================
void Shader::SetUniformVec2(const std::string& rParameterName, float x, float y) const
{
	glUniform2f(GetUniformLocation(rParameterName), x, y);
}

// ====================================================================================================
void Shader::SetUniformVec2(const std::string& rParameterName, const float* pVector) const
{
	glUniform2fv(GetUniformLocation(rParameterName), 2, pVector);
}

// ====================================================================================================
void Shader::SetUniformVec4(const std::string& rParameterName, float x, float y, float z, float w) const
{
	glUniform4f(GetUniformLocation(rParameterName), x, y, z, w);
}

// ====================================================================================================
void Shader::SetUniformVec4(const std::string& rParameterName, const float* pVector) const
{
	glUniform4fv(GetUniformLocation(rParameterName), 4, pVector);
}

// ====================================================================================================
void Shader::SetUniformMat4(const std::string& rParameterName, const float* pMatrix) const
{
	glUniformMatrix4fv(GetUniformLocation(rParameterName), 1, GL_FALSE, pMatrix);
}

// ====================================================================================================
void Shader::SetUniformTexture(const std::string& rParameterName, unsigned int textureTarget, unsigned int textureId, unsigned int textureUnit) const
{
	glBindTexture(textureTarget, textureId);
	glUniform1i(GetUniformLocation(rParameterName), (int)textureUnit);
}

// ====================================================================================================
void Shader::DetachShaders()
{
	std::map<unsigned int, unsigned int>::iterator shaderIdsCursor = mShadersIds.begin();

	while (shaderIdsCursor != mShadersIds.end())
	{
		glDetachShader(mProgramId, shaderIdsCursor->second);
		shaderIdsCursor++;
	}
}

// ====================================================================================================
void Shader::DeleteShaders()
{
	std::map<unsigned int, unsigned int>::iterator shaderIdsCursor = mShadersIds.begin();

	while (shaderIdsCursor != mShadersIds.end())
	{
		glDeleteShader(shaderIdsCursor->second);
		shaderIdsCursor++;
	}
}


// ====================================================================================================
void Shader::CheckCompile(int shaderObjectId, const std::string& rShaderFileName) const
{
	int compileStatus;
	glGetShaderiv(shaderObjectId, GL_COMPILE_STATUS, &compileStatus);

	if (compileStatus == GL_TRUE)
	{
		return;
	}

	int bufferLength;
	glGetShaderiv(shaderObjectId, GL_INFO_LOG_LENGTH , &bufferLength);

	if (bufferLength > 1)
	{
		char pBuffer[4096];
		glGetShaderInfoLog(shaderObjectId, bufferLength, &bufferLength, pBuffer);
		// TODO:
		THROW_EXCEPTION(Exception, "Shader (%s) info log ('%s'): %s\n", mName.c_str(), rShaderFileName.c_str(), pBuffer);
	}
}

// ====================================================================================================
void Shader::CheckLink()
{
	int linkStatus;

	glGetProgramiv(mProgramId, GL_LINK_STATUS, &linkStatus);

	if (linkStatus == GL_TRUE)
	{
		return;
	}

	int bufferLength;
	glGetProgramiv(mProgramId, GL_INFO_LOG_LENGTH, &bufferLength);

	if (bufferLength > 1)
	{
		char pBuffer[4096];
		glGetProgramInfoLog(mProgramId, bufferLength, &bufferLength, pBuffer);
		// TODO:
		THROW_EXCEPTION(Exception, "Shader (%s) info log: %s\n", mName.c_str(), pBuffer);
	}
}

// ====================================================================================================
void Shader::CheckValidate()
{
	int validateStatus;
	glGetProgramiv(mProgramId, GL_VALIDATE_STATUS, &validateStatus);

	if (validateStatus == GL_TRUE)
	{
		return;
	}

	int bufferLength;
	glGetProgramiv(mProgramId, GL_INFO_LOG_LENGTH, &bufferLength);

	if (bufferLength > 1)
	{
		char pBuffer[4096];
		glGetProgramInfoLog(mProgramId, bufferLength, &bufferLength, pBuffer);
		// TODO:
		THROW_EXCEPTION(Exception, "Shader (%s) info log: %s\n", mName.c_str(), pBuffer);
	}
}

