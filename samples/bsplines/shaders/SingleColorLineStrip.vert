#version 330

#include "shaders/FastCG.glsl"

in vec4 position;

void main()
{
	gl_Position = _ModelViewProjection * position;
}