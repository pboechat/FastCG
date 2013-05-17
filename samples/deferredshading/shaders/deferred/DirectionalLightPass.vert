#version 330

in vec4 position;
in vec3 normal;
in vec2 uv;

void main()
{
	gl_Position = position;
}