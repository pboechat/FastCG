#version 330

uniform sampler2D _Depth;

in vec2 uv;

layout(location = 0) out vec4 color;

void main()
{
	color = vec4(vec3(texture(_Depth, uv).x), 1);
}