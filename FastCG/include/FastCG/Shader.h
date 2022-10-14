#ifndef FASTCG_SHADER_H_
#define FASTCG_SHADER_H_

#include <FastCG/Texture.h>
#include <FastCG/OpenGLExceptions.h>
#include <FastCG/Exception.h>

#include <glm/glm.hpp>

#include <GL/glew.h>
#include <GL/gl.h>

#include <string>
#include <map>
#include <cstdlib>

namespace FastCG
{
	enum class ShaderType : uint8_t
	{
		ST_VERTEX = 0,
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
		Shader(const std::string &rName);
		~Shader();

		inline const std::string &GetName() const
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
			FASTCG_CHECK_OPENGL_ERROR();
		}

		inline void Unbind() const
		{
			glUseProgram(0);
		}

		inline void BindAttributeLocation(const std::string &rAttribute, unsigned int location) const
		{
			glBindAttribLocation(mProgramId, location, rAttribute.c_str());
			FASTCG_CHECK_OPENGL_ERROR();
		}

		inline void SetInt(const std::string &rParameterName, int value) const
		{
			glUniform1i(GetUniformLocation(rParameterName), value);
			FASTCG_CHECK_OPENGL_ERROR();
		}

		inline void SetFloat(const std::string &rParameterName, float value) const
		{
			glUniform1f(GetUniformLocation(rParameterName), value);
			FASTCG_CHECK_OPENGL_ERROR();
		}

		inline void SetBool(const std::string &rParameterName, bool value) const
		{
			glUniform1i(GetUniformLocation(rParameterName), value);
			FASTCG_CHECK_OPENGL_ERROR();
		}

		inline void SetVec2(const std::string &rParameterName, float x, float y) const
		{
			glUniform2f(GetUniformLocation(rParameterName), x, y);
			FASTCG_CHECK_OPENGL_ERROR();
		}

		inline void SetVec2(const std::string &rParameterName, const glm::vec2 &rVector) const
		{
			glUniform2fv(GetUniformLocation(rParameterName), 1, &rVector[0]);
			FASTCG_CHECK_OPENGL_ERROR();
		}

		inline void SetVec3(const std::string &rParameterName, float x, float y, float z) const
		{
			glUniform3f(GetUniformLocation(rParameterName), x, y, z);
			FASTCG_CHECK_OPENGL_ERROR();
		}

		inline void SetVec3(const std::string &rParameterName, const glm::vec3 &rVector) const
		{
			glUniform3fv(GetUniformLocation(rParameterName), 1, &rVector[0]);
			FASTCG_CHECK_OPENGL_ERROR();
		}

		inline void SetVec3Array(const std::string &rParameterName, unsigned int arraySize, const glm::vec3 *pVectorArray) const
		{
			glUniform3fv(GetUniformLocation(rParameterName), arraySize, &pVectorArray[0][0]);
			FASTCG_CHECK_OPENGL_ERROR();
		}

		inline void SetVec4(const std::string &rParameterName, float x, float y, float z, float w) const
		{
			glUniform4f(GetUniformLocation(rParameterName), x, y, z, w);
			FASTCG_CHECK_OPENGL_ERROR();
		}

		inline void SetVec4(const std::string &rParameterName, const glm::vec4 &rVector) const
		{
			glUniform4fv(GetUniformLocation(rParameterName), 1, &rVector[0]);
			FASTCG_CHECK_OPENGL_ERROR();
		}

		inline void SetMat3(const std::string &rParameterName, const glm::mat3 &rMatrix) const
		{
			glUniformMatrix3fv(GetUniformLocation(rParameterName), 1, GL_FALSE, &rMatrix[0][0]);
			FASTCG_CHECK_OPENGL_ERROR();
		}

		inline void SetMat4(const std::string &rParameterName, const glm::mat4 &rMatrix) const
		{
			glUniformMatrix4fv(GetUniformLocation(rParameterName), 1, GL_FALSE, &rMatrix[0][0]);
			FASTCG_CHECK_OPENGL_ERROR();
		}

		inline void SetTexture(const std::string &rParameterName, const std::shared_ptr<Texture> &pTexture, GLint textureUnit) const
		{
			assert(pTexture != nullptr);
			glActiveTexture(GL_TEXTURE0 + textureUnit);
			pTexture->Bind();
			glUniform1i(GetUniformLocation(rParameterName), textureUnit);
		}

		inline void SetTexture(const std::string &rParameterName, unsigned int textureId, GLint textureUnit) const
		{
			glActiveTexture(GL_TEXTURE0 + textureUnit);
			glBindTexture(GL_TEXTURE_2D, textureId);
			glUniform1i(GetUniformLocation(rParameterName), textureUnit);
			FASTCG_CHECK_OPENGL_ERROR();
		}

		inline void SetTexture(const std::string &rParameterName, GLint textureUnit) const
		{
			glUniform1i(GetUniformLocation(rParameterName), textureUnit);
			FASTCG_CHECK_OPENGL_ERROR();
		}

		void Compile(const std::string &rShaderFileName, ShaderType shaderType);
		void Link();

		void operator=(const Shader &rOther)
		{
			mProgramId = rOther.mProgramId;
			mName = rOther.mName;
			mShadersIds = rOther.mShadersIds;
		}

	private:
		GLuint mProgramId{~0u};
		std::string mName;
		std::map<ShaderType, GLuint> mShadersIds;

		void DetachShaders();
		void DeleteShaders();

		inline int GetUniformLocation(const std::string &rParameterName) const
		{
			auto location = glGetUniformLocation(mProgramId, rParameterName.c_str());
			FASTCG_CHECK_OPENGL_ERROR();
			return location;
		}
	};

}

#endif
