#include <Geometry.h>
#include <Shader.h>

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/freeglut.h>

Geometry::Geometry(const std::vector<glm::vec3>& vertices, const std::vector<unsigned int>& indexes, const std::vector<glm::vec3>& normals, const std::vector<glm::vec2>& uvs, const MaterialPtr& materialPtr) :
		mVertices(vertices), mIndexes(indexes), mNormals(normals), mUVs(uvs), mMaterialPtr(materialPtr)
{
	AllocateResources();
}

Geometry::~Geometry()
{
	DeallocateResources();
}

void Geometry::AllocateResources()
{
	// TODO: check for errors
	glGenBuffers(1, &mVerticesVBOId);
	glBindBuffer(GL_ARRAY_BUFFER, mVerticesVBOId);
	glBufferData(GL_ARRAY_BUFFER, mVertices.size() * 3 * sizeof(float), &mVertices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &mNormalsVBOId);
	glBindBuffer(GL_ARRAY_BUFFER, mNormalsVBOId);
	glBufferData(GL_ARRAY_BUFFER, mNormals.size() * 3 * sizeof(float), &mNormals[0], GL_STATIC_DRAW);

	glGenBuffers(1, &mUVsVBOId);
	glBindBuffer(GL_ARRAY_BUFFER, mUVsVBOId);
	glBufferData(GL_ARRAY_BUFFER, mUVs.size() * 2 * sizeof(float), &mUVs[0], GL_STATIC_DRAW);

	glGenVertexArrays(1, &mGeometryVAOId);
	glBindVertexArray(mGeometryVAOId);

	glEnableVertexAttribArray(Shader::VERTICES_ATTRIBUTE_INDEX);
	glEnableVertexAttribArray(Shader::NORMALS_ATTRIBUTE_INDEX);
	glEnableVertexAttribArray(Shader::UVS_ATTRIBUTE_INDEX);

	glBindBuffer(GL_ARRAY_BUFFER, mVerticesVBOId);
	glVertexAttribPointer(Shader::VERTICES_ATTRIBUTE_INDEX, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, mNormalsVBOId);
	glVertexAttribPointer(Shader::NORMALS_ATTRIBUTE_INDEX, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, mUVsVBOId);
	glVertexAttribPointer(Shader::UVS_ATTRIBUTE_INDEX, 2, GL_FLOAT, GL_FALSE, 0, 0);
}

void Geometry::DeallocateResources()
{
	//glDeleteBuffers(1, &mIndexesVBOId);
	glDeleteBuffers(1, &mUVsVBOId);
	glDeleteBuffers(1, &mNormalsVBOId);
	glDeleteBuffers(1, &mVerticesVBOId);
	glDeleteBuffers(1, &mGeometryVAOId);
}

void Geometry::Draw()
{
	glBindTexture(GL_TEXTURE_2D, 0);

	// TODO: improve this!
	mMaterialPtr->Bind(*this);

	glBindVertexArray(mGeometryVAOId);
	glDrawElements(GL_TRIANGLES, mIndexes.size(), GL_UNSIGNED_INT, &mIndexes[0]);
	//glDrawArrays(GL_TRIANGLES, 0, 3);
	// TODO: really necessary?
	glBindVertexArray(0);

	mMaterialPtr->Unbind();
}

