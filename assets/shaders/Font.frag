#version 330

uniform sampler2D _Map;
uniform vec4 _Color;

in vec2 vertexUv;

layout(location = 0) out vec4 color;

void main()
{
	float alpha = texture(_Map, vertexUv).r;
	color = vec4(_Color.rgb, alpha);
}