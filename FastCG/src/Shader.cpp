#include <FastCG/ShaderSource.h>
#include <FastCG/Shader.h>

void CheckCompile(GLuint shaderObjectId, const std::string &rShaderName, const std::string &rShaderFileName)
{
	GLint compileStatus;
	glGetShaderiv(shaderObjectId, GL_COMPILE_STATUS, &compileStatus);

	if (compileStatus == GL_TRUE)
	{
		return;
	}

	GLint infoLogLength;
	glGetShaderiv(shaderObjectId, GL_INFO_LOG_LENGTH, &infoLogLength);

	std::string infoLog;
	infoLog.reserve(infoLogLength);
	glGetShaderInfoLog(shaderObjectId, infoLogLength, &infoLogLength, &infoLog[0]);

	FASTCG_THROW_EXCEPTION(FastCG::Exception, "Shader (%s) info log ('%s'): %s", rShaderName.c_str(), rShaderFileName.c_str(), infoLog.c_str());
}

void CheckLink(GLuint programId, const std::string &rShaderName)
{
	GLint linkStatus;
	glGetProgramiv(programId, GL_LINK_STATUS, &linkStatus);

	if (linkStatus == GL_TRUE)
	{
		return;
	}

	GLint infoLogLength;
	glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &infoLogLength);

	std::string infoLog;
	infoLog.reserve((size_t)infoLogLength);
	glGetProgramInfoLog(programId, infoLogLength, &infoLogLength, &infoLog[0]);

	FASTCG_THROW_EXCEPTION(FastCG::Exception, "Shader (%s) info log: %s", rShaderName.c_str(), infoLog.c_str());
}

void CheckValidate(GLuint programId, const std::string &rShaderName)
{
	GLint validateStatus;
	glGetProgramiv(programId, GL_VALIDATE_STATUS, &validateStatus);

	if (validateStatus == GL_TRUE)
	{
		return;
	}

	GLint infoLogLength;
	glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &infoLogLength);

	std::string infoLog;
	infoLog.reserve((size_t)infoLogLength);
	glGetProgramInfoLog(programId, infoLogLength, &infoLogLength, &infoLog[0]);

	FASTCG_THROW_EXCEPTION(FastCG::Exception, "Shader (%s) info log: %s", rShaderName.c_str(), infoLog.c_str());
}

#define CASE_RETURN_STRING(str) \
	case str:                   \
		return #str
const char *GetOpenGLShaderTypeString(GLenum shaderType)
{
	switch (shaderType)
	{
		CASE_RETURN_STRING(GL_VERTEX_SHADER);
		CASE_RETURN_STRING(GL_FRAGMENT_SHADER);
	default:
		FASTCG_THROW_EXCEPTION(FastCG::Exception, "Unhandled OpenGL shader type");
		return nullptr;
	}
}

GLenum GetOpenGLShaderType(FastCG::ShaderType shaderType)
{
	switch (shaderType)
	{
	case FastCG::ShaderType::ST_VERTEX:
		return GL_VERTEX_SHADER;
	case FastCG::ShaderType::ST_FRAGMENT:
		return GL_FRAGMENT_SHADER;
	default:
		FASTCG_THROW_EXCEPTION(FastCG::Exception, "Unhandled shader type");
		return 0;
	}
}

namespace FastCG
{
	Shader::Shader() : mName("Default")
	{
	}

	Shader::Shader(const std::string &rName) : mName(rName)
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

	void Shader::Compile(const std::string &rShaderFileName, ShaderType shaderType)
	{
		auto shaderSource = ShaderSource::Parse(rShaderFileName);
		auto glShaderType = GetOpenGLShaderType(shaderType);
		auto shaderId = glCreateShader(glShaderType);

		if (shaderSource.empty())
		{
			FASTCG_THROW_EXCEPTION(Exception, "Shader file not found: %s", rShaderFileName.c_str());
		}

		const auto *pShaderSource = shaderSource.c_str();
		glShaderSource(shaderId, 1, (const char **)&pShaderSource, 0);

		glCompileShader(shaderId);
		CheckCompile(shaderId, mName, rShaderFileName);

		mShadersIds.insert(std::make_pair(shaderType, shaderId));

		FASTCG_CHECK_OPENGL_ERROR();

#ifdef _DEBUG
		std::string shaderLabel = mName + " (" + GetOpenGLShaderTypeString(glShaderType) + ")";
		glObjectLabel(GL_SHADER, shaderId, (GLsizei)shaderLabel.size(), shaderLabel.c_str());
#endif
	}

	void Shader::Link()
	{
		mProgramId = glCreateProgram();

		auto it = mShadersIds.begin();
		while (it != mShadersIds.end())
		{
			glAttachShader(mProgramId, it->second);
			it++;
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

#ifdef _DEBUG
		std::string programLabel = mName + " (GL_PROGRAM)";
		glObjectLabel(GL_PROGRAM, mProgramId, (GLsizei)programLabel.size(), programLabel.c_str());
#endif
	}

	void Shader::DetachShaders()
	{
		auto it = mShadersIds.begin();
		while (it != mShadersIds.end())
		{
			glDetachShader(mProgramId, it->second);
			it++;
		}
	}

	void Shader::DeleteShaders()
	{
		auto it = mShadersIds.begin();
		while (it != mShadersIds.end())
		{
			glDeleteShader(it->second);
			it++;
		}
	}

}