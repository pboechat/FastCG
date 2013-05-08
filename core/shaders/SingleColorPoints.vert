#version 330

#include "shaders/FastCG.glsl"

in vec4 position;

uniform float size;

void main()
{
	gl_PointSize = size;
	gl_Position = _ModelViewProjection * position;
}