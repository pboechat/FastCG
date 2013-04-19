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
	vec3 normalAxis = normalize(_ModelViewInverseTranspose * normal);
	vec3 tangentAxis = normalize(_ModelViewInverseTranspose * tangent.xyz);
	vec3 binormalAxis = normalize(cross(normalAxis, tangentAxis)) * tangent.w;

	mat3 tangentSpaceMatrix = mat3(tangentAxis.x, binormalAxis.x, normalAxis.x,
								   tangentAxis.y, binormalAxis.y, normalAxis.y,
								   tangentAxis.z, binormalAxis.z, normalAxis.z);

	vec3 vertexPosition = vec3(_ModelView * position);

	lightDirection = normalize(tangentSpaceMatrix * (_Light0Position - vertexPosition));
	viewerDirection = tangentSpaceMatrix * normalize(-vertexPosition);
	textureCoordinates = uv;

	gl_Position = _ModelViewProjection * position;
}