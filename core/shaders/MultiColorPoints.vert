#version 330

#include "shaders/FastCG.glsl"

in vec4 position;
in vec4 color;

uniform float size;

out vec4 vertexColor;

void main()
{
	vertexColor = color;
	gl_PointSize = size;
	gl_Position = _ModelViewProjection * position;
}