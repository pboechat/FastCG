#version 330

in vec4 position;
in vec3 normal;
in vec2 uv;

noperspective out vec2 vTexcoord;

void main()
{
	vTexcoord = position.xy * 0.5 + 0.5;
	gl_Position = vec4(position.xy, 0.0, 1.0);
}