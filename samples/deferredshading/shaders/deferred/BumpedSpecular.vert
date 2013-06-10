#version 330

uniform mat4 _ModelView;
uniform mat3 _ModelViewInverseTranspose;
uniform mat4 _ModelViewProjection;

in vec4 position;
in vec3 normal;
in vec4 tangent;
in vec2 uv;

out vec4 vertexPosition;
out vec3 vertexNormal;
out vec3 vertexTangent;
out vec3 vertexBinormal;
out vec2 vertexUV;

void main()
{
	vertexPosition = _ModelView * position;
	vertexNormal = _ModelViewInverseTranspose * normal;
	vertexTangent = _ModelViewInverseTranspose * tangent.xyz;
	vertexBinormal = normalize(cross(vertexNormal, vertexTangent)) * tangent.w;
	vertexUV = uv;
	gl_Position = _ModelViewProjection * position;
}