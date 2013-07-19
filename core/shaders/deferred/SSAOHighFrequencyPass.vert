#version 330

#include "FastCG.glsl"

in vec4 position;
in vec3 normal;
in vec2 uv;

out vec2 vertexUv;
noperspective out vec3 viewRay;

void main()
{
	vertexUv = uv;
	viewRay = normalize(vec3(position.x * _TanHalfFov * _AspectRatio,
				   position.x * _TanHalfFov,
				   1));
	gl_Position = position;
}