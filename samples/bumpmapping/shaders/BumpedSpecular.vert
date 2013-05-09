#version 330

#include "shaders/FastCG.glsl"

in vec4 position;
in vec3 normal;
in vec2 uv;
in vec4 tangent;

out vec3 lightDirection;
out vec3 viewerDirection;
out vec2 textureCoordinates;

void main()
{
	vec3 vertexNormal = normalize(_ModelViewInverseTranspose * normal);
	vec3 vertexTangent = normalize(_ModelViewInverseTranspose * tangent.xyz);
	vec3 vertexBinormal = cross(vertexNormal, vertexTangent) * tangent.w;

	mat3 tangentSpaceMatrix = mat3(vertexTangent.x, vertexBinormal.x, vertexNormal.x,
								   vertexTangent.y, vertexBinormal.y, vertexNormal.y,
								   vertexTangent.z, vertexBinormal.z, vertexNormal.z);

	vec3 vertexPosition = vec3(_ModelView * position);

	vec3 lightPosition = vec3(_View * vec4(_Light0Position, 1.0));
	lightDirection = tangentSpaceMatrix * normalize(lightPosition - vertexPosition);

	viewerDirection = tangentSpaceMatrix * normalize(-vertexPosition);

	textureCoordinates = uv;

	gl_Position = _ModelViewProjection * position;
}