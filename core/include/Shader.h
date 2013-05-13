#if (!defined(SHADER_H_) && defined(USE_PROGRAMMABLE_PIPELINE))
#define SHADER_H_

#include <Texture.h>
#include <Exception.h>

#include <glm/glm.hpp>

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/freeglut.h>

#include <cstdlib>
#include <string>
#include <map>

enum ShaderType
{
	ST_VERTEX, 
	ST_FRAGMENT
};

class Shader
{
public:
	static const unsigned int VERTICES_ATTRIBUTE_INDEX = 0;
	static const unsigned int NORMALS_ATTRIBUTE_INDEX = 1;
	static const unsigned int UVS_ATTRIBUTE_INDEX = 2;
	static const unsigned int TANGENTS_ATTRIBUTE_INDEX = 3;
	static const unsigned int COLORS_ATTRIBUTE_INDEX = 4;

	Shader();
	Shader(const std::string& rName);
	~Shader();

	inline const std::string& GetName() const
	{
		return mName;
	}

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

	inline void BindAttributeLocation(const std::string& rAttribute, unsigned int location) const
	{
		glBindAttribLocation(mProgramId, location, rAttribute.c_str());
	}

	inline void SetInt(const std::string& rParameterName, int value) const
	{
		glUniform1i(GetUniformLocation(rParameterName), value);
	}

	inline void SetFloat(const std::string& rParameterName, float value) const
	{
		glUniform1f(GetUniformLocation(rParameterName), value);
	}

	inline void SetBool(const std::string& rParameterName, bool value) const
	{
		glUniform1i(GetUniformLocation(rParameterName), value);
	}

	inline void SetVec2(const std::string& rParameterName, float x, float y) const
	{
		glUniform2f(GetUniformLocation(rParameterName), x, y);
	}

	inline void SetVec2(const std::string& rParameterName, const glm::vec2& rVector) const
	{
		glUniform2fv(GetUniformLocation(rParameterName), 2, &rVector[0]);
	}

	inline void SetVec3(const std::string& rParameterName, float x, float y, float z) const
	{
		glUniform3f(GetUniformLocation(rParameterName), x, y, z);
	}

	inline void SetVec3(const std::string& rParameterName, const glm::vec3& rVector) const
	{
		glUniform3fv(GetUniformLocation(rParameterName), 3, &rVector[0]);
	}

	inline void SetVec4(const std::string& rParameterName, float x, float y, float z, float w) const
	{
		glUniform4f(GetUniformLocation(rParameterName), x, y, z, w);
	}

	inline void SetVec4(const std::string& rParameterName, const glm::vec4& rVector) const
	{
		glUniform4fv(GetUniformLocation(rParameterName), 4, &rVector[0]);
	}

	inline void SetMat3(const std::string& rParameterName, const glm::mat3& rMatrix) const
	{
		glUniformMatrix3fv(GetUniformLocation(rParameterName), 1, GL_FALSE, &rMatrix[0][0]);
	}

	inline void SetMat4(const std::string& rParameterName, const glm::mat4& rMatrix) const
	{
		glUniformMatrix4fv(GetUniformLocation(rParameterName), 1, GL_FALSE, &rMatrix[0][0]);
	}

	inline void SetTexture(const std::string& rParameterName, const Texture* pTexture, unsigned int textureUnit) const
	{
		glActiveTexture(GL_TEXTURE0 + (int)textureUnit);
		pTexture->Bind();
		glUniform1i(GetUniformLocation(rParameterName), (int)textureUnit);
		// TODO:
		//texturePtr->Unbind();
	}

	inline void SetTexture(const std::string& rParameterName, unsigned int textureId, unsigned int textureUnit) const
	{
		glActiveTexture(GL_TEXTURE0 + (int)textureUnit);
		glBindTexture(GL_TEXTURE_2D, textureId);
		glUniform1i(GetUniformLocation(rParameterName), (int)textureUnit);
	}

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

#endif
