#include <Geometry.h>
#include <Shader.h>
#include <OpenGLExceptions.h>

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/freeglut.h>

Geometry::Geometry(const std::vector<glm::vec3>& vertices, const std::vector<unsigned int>& indexes, const std::vector<glm::vec3>& normals, const std::vector<glm::vec2>& uvs, const MaterialPtr& materialPtr) :
	mVertices(vertices), 
	mIndexes(indexes), 
	mNormals(normals), 
	mUVs(uvs), 
	mMaterialPtr(materialPtr)
{
#ifdef USE_OPENGL4
	AllocateResources();
#else
	mDisplayListId = 0;
#endif
}

Geometry::~Geometry()
{
	DeallocateResources();
}

void Geometry::AllocateResources()
{
#ifdef USE_OPENGL4
	// TODO: check for errors

	// create vertex buffer object and attach data
	glGenBuffers(1, &mVerticesVBOId);
	glBindBuffer(GL_ARRAY_BUFFER, mVerticesVBOId);
	glBufferData(GL_ARRAY_BUFFER, mVertices.size() * 3 * sizeof(float), &mVertices[0], GL_STATIC_DRAW);

	// create normals buffer object and attach data
	glGenBuffers(1, &mNormalsVBOId);
	glBindBuffer(GL_ARRAY_BUFFER, mNormalsVBOId);
	glBufferData(GL_ARRAY_BUFFER, mNormals.size() * 3 * sizeof(float), &mNormals[0], GL_STATIC_DRAW);

	// create uvs buffer object and attach data
	glGenBuffers(1, &mUVsVBOId);
	glBindBuffer(GL_ARRAY_BUFFER, mUVsVBOId);
	glBufferData(GL_ARRAY_BUFFER, mUVs.size() * 2 * sizeof(float), &mUVs[0], GL_STATIC_DRAW);

	// create vertex array object with all previous vbos attached
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
#else
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	// create vertex array buffer and attach data
	glVertexPointer(3, GL_FLOAT, 0, &mVertices[0]);

	// create normal array buffer and attach data
	glNormalPointer(GL_FLOAT, 0, &mNormals[0]);

	// create uvs array buffer and attach data
	glTexCoordPointer(2, GL_FLOAT, 0, &mUVs[0]);
#endif
}

void Geometry::DeallocateResources()
{
#ifdef USE_OPENGL4
	glDeleteBuffers(1, &mUVsVBOId);
	glDeleteBuffers(1, &mNormalsVBOId);
	glDeleteBuffers(1, &mVerticesVBOId);
	glDeleteBuffers(1, &mGeometryVAOId);
#else
	glDeleteLists(mDisplayListId, 1);
#endif
}

void Geometry::Draw()
{
#ifdef USE_OPENGL4
	mMaterialPtr->Bind(*this);

	glBindVertexArray(mGeometryVAOId);
	glDrawElements(GL_TRIANGLES, mIndexes.size(), GL_UNSIGNED_INT, &mIndexes[0]);
	glBindVertexArray(0);

	mMaterialPtr->Unbind();
#else
	if (mDisplayListId == 0)
	{
		AllocateResources();

		mDisplayListId = glGenLists(1);

		CHECK_FOR_OPENGL_ERRORS();

		glNewList(mDisplayListId, GL_COMPILE);
		glDrawElements(GL_TRIANGLES, mIndexes.size(), GL_UNSIGNED_INT, &mIndexes[0]);
		glEndList();
	}

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glMultMatrixf(&GetModel()[0][0]);

	mMaterialPtr->Bind();

	glCallList(mDisplayListId);

	glPopMatrix();
#endif
}

