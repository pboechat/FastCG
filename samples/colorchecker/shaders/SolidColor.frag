#version 330

#include "shaders/FastCG.glsl"

uniform vec4 solidColor;

void main()
{
	gl_FragColor = solidColor;
}