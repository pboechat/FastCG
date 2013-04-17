#ifndef SHADER_H
#define SHADER_H

#include <Pointer.h>
#include <Texture.h>
#include <Exception.h>

#include <cstdlib>
#include <string>
#include <map>

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/freeglut.h>
#include <glm/glm.hpp>

enum ShaderType
{
	ST_VERTEX, ST_FRAGMENT
};

class Shader
{
public:
	static const unsigned int VERTICES_ATTRIBUTE_INDEX = 0;
	static const unsigned int NORMALS_ATTRIBUTE_INDEX = 1;
	static const unsigned int UVS_ATTRIBUTE_INDEX = 2;

	Shader();
	Shader(const std::string& rName);
	~Shader();

	inline unsigned int GetProgramId()
	{
		return mProgramId;
	}

	inline void Bind() const
	{
		glUseProgram(mProgramId);
	}

	inline void Unbind() const
	{
		glUseProgram(0);
	}

	void BindAttributeLocation(const std::string& rAttribute, unsigned int location) const;

	void SetInt(const std::string& rParameterName, int value) const;
	void SetFloat(const std::string& rParameterName, float value) const;
	void SetBool(const std::string& rParameterName, bool value) const;
	void SetVec2(const std::string& rParameterName, float x, float y) const;
	void SetVec2(const std::string& rParameterName, const glm::vec2& rVector) const;
	void SetVec3(const std::string& rParameterName, float x, float y, float z) const;
	void SetVec3(const std::string& rParameterName, const glm::vec3& rVector) const;
	void SetVec4(const std::string& rParameterName, float x, float y, float z, float w) const;
	void SetVec4(const std::string& rParameterName, const glm::vec4& rVector) const;
	void SetMat3(const std::string& rParameterName, const glm::mat3& rMatrix) const;
	void SetMat4(const std::string& rParameterName, const glm::mat4& rMatrix) const;
	void SetTexture(const std::string& rParameterName, const TexturePtr& spTexture, unsigned int textureUnit) const;
	void Compile(const std::string& rShaderFileName, ShaderType shaderType);
	void Link();

	void operator =(const Shader& rOther)
	{
		mProgramId = rOther.mProgramId;
		mName = rOther.mName;
		mShadersIds = rOther.mShadersIds;
	}

private:
	int mProgramId;
	std::string mName;
	std::map<ShaderType, unsigned int> mShadersIds;

	void CheckCompile(int shaderObjectId, const std::string& rShaderFileName) const;
	void CheckLink();
	void CheckValidate();
	void DetachShaders();
	void DeleteShaders();

	inline unsigned int GetUniformLocation(const std::string& rParameterName) const
	{
		return glGetUniformLocation(mProgramId, rParameterName.c_str());
	}

	inline unsigned int GetShaderTypeMapping(ShaderType shaderType)
	{
		if (shaderType == ST_VERTEX)
		{
			return GL_VERTEX_SHADER;
		}
		else if (shaderType == ST_FRAGMENT)
		{
			return GL_FRAGMENT_SHADER;
		}
		else
		{
			THROW_EXCEPTION(Exception, "Unknown shader type: %d", shaderType);
			return 0;
		}
	}

};

typedef Pointer<Shader> ShaderPtr;

#endif
