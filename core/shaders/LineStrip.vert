#version 330

#include "shaders/FastCG.glsl"

in vec4 position;
in vec4 color;

out vec4 vertexColor;

void main()
{
	vertexColor = color;
	gl_Position = _ModelViewProjection * position;
}