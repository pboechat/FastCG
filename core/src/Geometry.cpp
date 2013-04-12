#include <Geometry.h>

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/freeglut.h>
#include <GL/glext.h>

Geometry::Geometry(const std::vector<glm::vec3>& vertices, const std::vector<unsigned int>& indexes, const std::vector<glm::vec2>& uvs, const Material& material)
	: mVertices(vertices), mIndexes(indexes), mUVs(uvs), mMaterial(material)
{
	AllocateResources();
}

Geometry::~Geometry()
{
	DeallocateResources();
}

void Geometry::AllocateResources()
{
	glGenVertexArrays(1, &mGeometryVAOId);

	// TODO: check for errors

	glBindVertexArray(mGeometryVAOId);

	glGenBuffers(1, &mVerticesVBOId);
	glBindBuffer(GL_ARRAY_BUFFER, mVerticesVBOId);
	glBufferData(GL_ARRAY_BUFFER, mVertices.size() * sizeof(float) * 3, &mVertices[0], GL_STATIC_DRAW);

	glVertexAttribPointer(VERTICES_ATTRIBUTE_INDEX, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glEnableVertexAttribArray(VERTICES_ATTRIBUTE_INDEX);
	glBindVertexArray(VERTICES_ATTRIBUTE_INDEX);

	glBindVertexArray(mGeometryVAOId);

	glGenBuffers(1, &mUVsVBOId);
	glBindBuffer(GL_ARRAY_BUFFER, mUVsVBOId);
	glBufferData(GL_ARRAY_BUFFER, mUVs.size() * sizeof(float) * 2, &mUVs[0], GL_STATIC_DRAW);

	glVertexAttribPointer(UVS_ATTRIBUTE_INDEX, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glEnableVertexAttribArray(UVS_ATTRIBUTE_INDEX);
	glBindVertexArray(UVS_ATTRIBUTE_INDEX);

	// TODO: really necessary?
	glBindVertexArray(0);
}

void Geometry::DeallocateResources()
{
	glDeleteBuffers(1, &mUVsVBOId);
	glDeleteBuffers(1, &mVerticesVBOId);
	glDeleteBuffers(1, &mGeometryVAOId);
}

void Geometry::Draw()
{
	mMaterial.Bind();

	glBindVertexArray(mGeometryVAOId);
	glDrawArrays(GL_TRIANGLES, 0, mVertices.size());
	// TODO: really necessary?
	glBindVertexArray(0);

	mMaterial.Unbind();
}

