#version 330

#include "FastCG.glsl"

in vec4 position;
in vec3 normal;
in vec2 uv;

out vec2 vertexUv;

void main()
{
	vertexUv = uv;
	gl_Position = position;
}