#version 330

uniform mat4 _ModelViewProjection;

in vec4 position;
in vec3 normal;
in vec2 uv;

void main()
{
	gl_Position = _ModelViewProjection * position;
}