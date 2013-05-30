#version 330

#include "shaders/FastCG.glsl"

in vec4 position;
in vec3 normal;
in vec2 uv;

out vec3 vertexPosition;
out vec3 vertexNormal;
out vec2 vertexUV;

void main()
{
	vertexPosition = vec3(_ModelView * position);
	vertexNormal = normalize(_ModelViewInverseTranspose * normal);
	vertexUV = uv;
	gl_Position = _ModelViewProjection * position;
}