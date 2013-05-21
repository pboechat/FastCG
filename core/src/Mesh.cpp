#include <Mesh.h>
#include <Shader.h>
#include <Exception.h>
//#include <OpenGLExceptions.h>

#include <GL/glew.h>
#include <GL/gl.h>

Mesh::Mesh(const std::vector<glm::vec3>& vertices, const std::vector<unsigned int>& indexes, const std::vector<glm::vec3>& normals, const std::vector<glm::vec2>& uvs) :
	mVertices(vertices), 
	mIndexes(indexes), 
	mNormals(normals), 
	mUVs(uvs)
{
#ifdef USE_PROGRAMMABLE_PIPELINE
	mTriangleMeshVAOId = 0;
	mUseTangents = false;
#else
	mDisplayListId = 0;
#endif
}

#ifdef USE_PROGRAMMABLE_PIPELINE
Mesh::Mesh(const std::vector<glm::vec3>& vertices, const std::vector<unsigned int>& indexes, const std::vector<glm::vec3>& normals, const std::vector<glm::vec4>& tangents, const std::vector<glm::vec2>& uvs) :
	mTriangleMeshVAOId(0),
	mVertices(vertices), 
	mIndexes(indexes), 
	mNormals(normals), 
	mTangents(tangents),
	mUVs(uvs), 
	mUseTangents(true)
{
}
#endif

Mesh::~Mesh()
{
	DeallocateResources();
}

void Mesh::AllocateResources()
{
#ifdef USE_PROGRAMMABLE_PIPELINE
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

	if (mUseTangents)
	{
		// create tangents buffer object and attach data
		glGenBuffers(1, &mTangentsVBOId);
		glBindBuffer(GL_ARRAY_BUFFER, mTangentsVBOId);
		glBufferData(GL_ARRAY_BUFFER, mTangents.size() * 4 * sizeof(float), &mTangents[0], GL_STATIC_DRAW);
	}

	// create vertex array object with all previous vbos attached
	glGenVertexArrays(1, &mTriangleMeshVAOId);
	glBindVertexArray(mTriangleMeshVAOId);

	glEnableVertexAttribArray(Shader::VERTICES_ATTRIBUTE_INDEX);
	glEnableVertexAttribArray(Shader::NORMALS_ATTRIBUTE_INDEX);
	glEnableVertexAttribArray(Shader::UVS_ATTRIBUTE_INDEX);

	if (mUseTangents)
	{
		glEnableVertexAttribArray(Shader::TANGENTS_ATTRIBUTE_INDEX);
	}

	glBindBuffer(GL_ARRAY_BUFFER, mVerticesVBOId);
	glVertexAttribPointer(Shader::VERTICES_ATTRIBUTE_INDEX, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, mNormalsVBOId);
	glVertexAttribPointer(Shader::NORMALS_ATTRIBUTE_INDEX, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, mUVsVBOId);
	glVertexAttribPointer(Shader::UVS_ATTRIBUTE_INDEX, 2, GL_FLOAT, GL_FALSE, 0, 0);

	if (mUseTangents)
	{
		glBindBuffer(GL_ARRAY_BUFFER, mTangentsVBOId);
		glVertexAttribPointer(Shader::TANGENTS_ATTRIBUTE_INDEX, 4, GL_FLOAT, GL_FALSE, 0, 0);
	}

	// TODO: check for errors!
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

	mDisplayListId = glGenLists(1);

	//CHECK_FOR_OPENGL_ERRORS();

	glNewList(mDisplayListId, GL_COMPILE);
	glDrawElements(GL_TRIANGLES, mIndexes.size(), GL_UNSIGNED_INT, &mIndexes[0]);
	glEndList();
#endif
}

void Mesh::DeallocateResources()
{
#ifdef USE_PROGRAMMABLE_PIPELINE
	if (mUseTangents)
	{
		glDeleteBuffers(1, &mTangentsVBOId);
	}

	glDeleteBuffers(1, &mUVsVBOId);
	glDeleteBuffers(1, &mNormalsVBOId);
	glDeleteBuffers(1, &mVerticesVBOId);
	glDeleteBuffers(1, &mTriangleMeshVAOId);
#else
	glDeleteLists(mDisplayListId, 1);
#endif
}

void Mesh::DrawCall()
{
#ifdef USE_PROGRAMMABLE_PIPELINE
	if (mTriangleMeshVAOId == 0)
	{
		AllocateResources();
	}

	glBindVertexArray(mTriangleMeshVAOId);
	glDrawElements(GL_TRIANGLES, mIndexes.size(), GL_UNSIGNED_INT, &mIndexes[0]);
	glBindVertexArray(0);

#else
	if (mDisplayListId == 0)
	{
		AllocateResources();
	}

	glCallList(mDisplayListId);
#endif
}

#ifdef USE_PROGRAMMABLE_PIPELINE
void Mesh::CalculateTangents()
{
	if (mTriangleMeshVAOId != 0) 
	{
		THROW_EXCEPTION(Exception, "Cannot calculate tangents after VAO is already created: %d", mTriangleMeshVAOId);
	}

	mTangents = std::vector<glm::vec4>(mVertices.size());
	std::vector<glm::vec3> tangents1(mVertices.size());
	std::vector<glm::vec3> tangents2(mVertices.size());

	for (unsigned int i = 0; i < mIndexes.size(); i += 3)
	{
		unsigned int i1 = mIndexes[i];
		unsigned int i2 = mIndexes[i + 1];
		unsigned int i3 = mIndexes[i + 2];

		glm::vec3 v1 = mVertices[i1];
		glm::vec3 v2 = mVertices[i2];
		glm::vec3 v3 = mVertices[i3];

		glm::vec2 w1 = mUVs[i1];
		glm::vec2 w2 = mUVs[i2];
		glm::vec2 w3 = mUVs[i3];

		float x1 = v2.x - v1.x;
		float x2 = v3.x - v1.x;

		float y1 = v2.y - v1.y;
		float y2 = v3.y - v1.y;

		float z1 = v2.z - v1.z;
		float z2 = v3.z - v1.z;

		float s1 = w2.x - w1.x;
		float s2 = w3.x - w1.x;

		float t1 = w2.y - w1.y;
		float t2 = w3.y - w1.y;

		float r = 1.0f / (s1 * t2 - s2 * t1);

		glm::vec3 sdir((t2 * x1 - t1 * x2) * r, 
			           (t2 * y1 - t1 * y2) * r,
			           (t2 * z1 - t1 * z2) * r);

		glm::vec3 tdir((s1 * x2 - s2 * x1) * r, 
					   (s1 * y2 - s2 * y1) * r,
					   (s1 * z2 - s2 * z1) * r);

		tangents1[i1] += sdir;
		tangents1[i2] += sdir;
		tangents1[i3] += sdir;

		tangents2[i1] += tdir;
		tangents2[i2] += tdir;
		tangents2[i3] += tdir;
	}

	for (unsigned int i = 0; i < mVertices.size(); i++)
	{
		glm::vec3 normal = mNormals[i];
		glm::vec3 tangent = tangents1[i];

		// gram-schmidt orthogonalization
		mTangents[i] = glm::vec4(glm::normalize(tangent - normal * glm::dot(normal, tangent)), 0.0f);

		// calculate handedness
		mTangents[i].w = (glm::dot(glm::cross(normal, tangent), tangents2[i]) < 0.0f) ? -1.0f : 1.0f;
	}

	mUseTangents = true;
}
#endif