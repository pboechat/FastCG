#version 330

#include "shaders/FastCG.glsl"

in vec4 position;
in vec3 normal;
in vec2 uv;
in vec4 tangent;

out vec3 lightDirection;
out vec3 vertexPosition;
out vec2 vertexUV;

void main()
{
	vec3 normalAxis = normalize(_ModelViewInverseTranspose * normal);
	vec3 tangentAxis = normalize(_ModelViewInverseTranspose * tangent.xyz);
	vec3 binormalAxis = normalize(cross(normalAxis, tangentAxis)) * tangent.w;

	mat3 tangentSpaceMatrix = mat3(tangentAxis.x, binormalAxis.x, normalAxis.x,
								   tangentAxis.y, binormalAxis.y, normalAxis.y,
								   tangentAxis.z, binormalAxis.z, normalAxis.z);

	vec3 vertexPosition = vec3(_ModelView * position);

	vec3 lightPosition = vec3(_View * vec4(_Light0Position, 1.0));
	lightDirection = tangentSpaceMatrix * normalize(lightPosition - vertexPosition);
	
	vertexPosition = vec3(_ModelView * position);
	vertexUV = uv;

	gl_Position = _ModelViewProjection * position;
}