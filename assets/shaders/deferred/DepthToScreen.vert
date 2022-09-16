#version 330

in vec4 position;

out vec2 uv;

void main()
{
	uv = position.xy * 0.5 + 0.5;
	gl_Position = vec4(position.xy, 0.0, 1.0);
}