#version 330

#include "FastCG.glsl"

in vec4 position;
in vec3 normal;
in vec2 uv;
in vec4 tangent;

out vec3 lightDirection;
out vec3 viewerDirection;
out vec3 vertexPosition;
out vec2 vertexUV;

void main()
{
	vec3 vertexNormal = normalize(_ModelViewInverseTranspose * normal);
	vec3 vertexTangent = normalize(_ModelViewInverseTranspose * tangent.xyz);
	vec3 vertexBinormal = normalize(cross(vertexNormal, vertexTangent) * tangent.w);

	mat3 tangentSpaceMatrix = transpose(mat3(vertexTangent, vertexBinormal, vertexNormal));

	vertexPosition = vec3(_ModelView * position);
	lightDirection = tangentSpaceMatrix * normalize(_Light0Position - (step(0.0, _Light0Type) * vertexPosition));
	viewerDirection = tangentSpaceMatrix * normalize(-vertexPosition);
	vertexUV = uv;

	gl_Position = _ModelViewProjection * position;
}