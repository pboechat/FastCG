#include <Shader.h>
#include <ShaderSource.h>

Shader::Shader() :
	mProgramId(-1), mName("Default")
{
}

Shader::Shader(const std::string& rName) :
	mProgramId(-1), mName(rName)
{
}

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

void Shader::Compile(const std::string& rShaderFileName, ShaderType shaderType)
{
	std::string shaderSource = ShaderSource::Parse(rShaderFileName);
	unsigned int shaderId = glCreateShader(GetShaderTypeMapping(shaderType));

	if (shaderSource.empty())
	{
		// TODO:
		THROW_EXCEPTION(Exception, "Shader file not found: %s\n", rShaderFileName.c_str());
	}

	const char* pShaderSource = shaderSource.c_str();
	glShaderSource(shaderId, 1, (const char**) &pShaderSource, 0);
	glCompileShader(shaderId);
	CheckCompile(shaderId, rShaderFileName);
	mShadersIds.insert(std::make_pair(shaderType, shaderId));
}

void Shader::Link()
{
	mProgramId = glCreateProgram();
	std::map<ShaderType, unsigned int>::iterator shaderIdsCursor = mShadersIds.begin();

	while (shaderIdsCursor != mShadersIds.end())
	{
		glAttachShader(mProgramId, shaderIdsCursor->second);
		shaderIdsCursor++;
	}

	BindAttributeLocation("position", VERTICES_ATTRIBUTE_INDEX);
	BindAttributeLocation("normal", NORMALS_ATTRIBUTE_INDEX);
	BindAttributeLocation("uv", UVS_ATTRIBUTE_INDEX);
	BindAttributeLocation("tangent", TANGENTS_ATTRIBUTE_INDEX);

	glLinkProgram(mProgramId);
	CheckLink();
	glValidateProgram(mProgramId);
	CheckValidate();
}

void Shader::BindAttributeLocation(const std::string& rAttribute, unsigned int location) const
{
	glBindAttribLocation(mProgramId, location, rAttribute.c_str());
}

void Shader::SetInt(const std::string& rParameterName, int value) const
{
	glUniform1i(GetUniformLocation(rParameterName), value);
}

void Shader::SetFloat(const std::string& rParameterName, float value) const
{
	glUniform1f(GetUniformLocation(rParameterName), value);
}

void Shader::SetBool(const std::string& rParameterName, bool value) const
{
	glUniform1i(GetUniformLocation(rParameterName), value);
}

void Shader::SetVec2(const std::string& rParameterName, float x, float y) const
{
	glUniform2f(GetUniformLocation(rParameterName), x, y);
}

void Shader::SetVec2(const std::string& rParameterName, const glm::vec2& rVector) const
{
	glUniform2fv(GetUniformLocation(rParameterName), 2, &rVector[0]);
}

void Shader::SetVec3(const std::string& rParameterName, float x, float y, float z) const
{
	glUniform3f(GetUniformLocation(rParameterName), x, y, z);
}

void Shader::SetVec3(const std::string& rParameterName, const glm::vec3& rVector) const
{
	glUniform3fv(GetUniformLocation(rParameterName), 3, &rVector[0]);
}

void Shader::SetVec4(const std::string& rParameterName, float x, float y, float z, float w) const
{
	glUniform4f(GetUniformLocation(rParameterName), x, y, z, w);
}

void Shader::SetVec4(const std::string& rParameterName, const glm::vec4& rVector) const
{
	glUniform4fv(GetUniformLocation(rParameterName), 4, &rVector[0]);
}

void Shader::SetMat3(const std::string& rParameterName, const glm::mat3& rMatrix) const
{
	glUniformMatrix3fv(GetUniformLocation(rParameterName), 1, GL_FALSE, &rMatrix[0][0]);
}

void Shader::SetMat4(const std::string& rParameterName, const glm::mat4& rMatrix) const
{
	glUniformMatrix4fv(GetUniformLocation(rParameterName), 1, GL_FALSE, &rMatrix[0][0]);
}

void Shader::SetTexture(const std::string& rParameterName, const TexturePtr& texturePtr, unsigned int textureUnit) const
{
	int unitId = GL_TEXTURE0 + (int)textureUnit;
	glActiveTexture(unitId);
	texturePtr->Bind();
	glUniform1i(GetUniformLocation(rParameterName), (int)textureUnit);
	//spTexture->Unbind();
}

void Shader::DetachShaders()
{
	std::map<ShaderType, unsigned int>::iterator shaderIdsCursor = mShadersIds.begin();

	while (shaderIdsCursor != mShadersIds.end())
	{
		glDetachShader(mProgramId, shaderIdsCursor->second);
		shaderIdsCursor++;
	}
}

void Shader::DeleteShaders()
{
	std::map<ShaderType, unsigned int>::iterator shaderIdsCursor = mShadersIds.begin();

	while (shaderIdsCursor != mShadersIds.end())
	{
		glDeleteShader(shaderIdsCursor->second);
		shaderIdsCursor++;
	}
}

void Shader::CheckCompile(int shaderObjectId, const std::string& rShaderFileName) const
{
	int compileStatus;
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
		// TODO:
		THROW_EXCEPTION(Exception, "Shader (%s) info log ('%s'): %s\n", mName.c_str(), rShaderFileName.c_str(), pBuffer);
	}
}

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

