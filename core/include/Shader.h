#ifndef SHADER_H
#define SHADER_H

#include <stdlib.h>

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/freeglut.h>
#include <GL/glext.h>

#include <string>
#include <map>

class Shader
{
public:
	Shader(const std::string& rName);
	~Shader();

	inline unsigned int GetProgramId()
	{
		return mProgramId;
	}

	inline unsigned int GetUniformLocation(const std::string& rParameterName) const
	{
		return glGetUniformLocation(mProgramId, rParameterName.c_str());
	}

	inline void Bind() const
	{
		glUseProgram(mProgramId);
	}

	inline void Unbind() const
	{
		glUseProgram(0);
	}

	void SetUniformInt(const std::string& rParameterName, int value) const;
	void SetUniformFloat(const std::string& rParameterName, float value) const;
	void SetUniformBool(const std::string& rParameterName, bool value) const;
	void SetUniformVec2(const std::string& rParameterName, float x, float y) const;
	void SetUniformVec2(const std::string& rParameterName, const float* pVector) const;
	void SetUniformVec4(const std::string& rParameterName, float x, float y, float z, float w) const;
	void SetUniformVec4(const std::string& rParameterName, const float* pVector) const;
	void SetUniformMat4(const std::string& rParameterName, const float* pMatrix) const;
	void SetUniformTexture(const std::string& rParameterName, unsigned int textureTarget, unsigned int textureId, unsigned int textureUnit) const;
	void Compile(const std::string& rShaderFileName, unsigned int shaderType);
	void Link();

private:
	std::string mName;
	std::map<unsigned int, unsigned int> mShadersIds;
	int mProgramId;

	void CheckCompile(int shaderObjectId, const std::string& rShaderFileName) const;
	void CheckLink();
	void CheckValidate();
	void DetachShaders();
	void DeleteShaders();

};

#endif
