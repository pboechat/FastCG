#ifndef ASSIMPUTILS_H_
#define ASSIMPUTILS_H_

#include <Texture.h>

#include <assimp/cimport.h>
#include <assimp/scene.h>

#include <glm/glm.hpp>

#include <string>

class AssimpUtils
{
public:
	inline static glm::mat4 Convert(const aiMatrix4x4& rMatrix)
	{
		return glm::mat4(rMatrix.a1, rMatrix.b1, rMatrix.c1, rMatrix.d1,
						 rMatrix.a2, rMatrix.b2, rMatrix.c2, rMatrix.d2,
						 rMatrix.a3, rMatrix.b3, rMatrix.c3, rMatrix.d3,
						 rMatrix.a4, rMatrix.b4, rMatrix.c4, rMatrix.d4);
	}

	inline static glm::vec3 Convert(const aiVector3D& rVector)
	{
		return glm::vec3(rVector[0], rVector[1], rVector[2]);
	}

	inline static glm::vec2 Convert(const aiVector2D& rVector)
	{
		return glm::vec2(rVector[0], rVector[1]);
	}

	inline static std::string Convert(const aiString& rString)
	{
		return std::string(rString.C_Str());
	}

private:
	AssimpUtils()
	{
	}

	~AssimpUtils()
	{
	}


};

#endif