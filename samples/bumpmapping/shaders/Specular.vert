#version 330

#include "shaders/FastCG.glsl"

in vec4 position;
in vec3 normal;
in vec2 uv;

out vec4 eyeSpacePosition;
out vec3 vertexNormal;
out vec2 textureCoordinates;

void main()
{
	vertexNormal = normalize(_ModelViewInverseTranspose * normal);
	eyeSpacePosition = _ModelView * position;
	textureCoordinates = uv;
	gl_Position = _ModelViewProjection * position;
}