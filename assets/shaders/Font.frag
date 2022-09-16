#version 330

uniform sampler2D _Map;
uniform vec4 _Color;

in vec2 uv;

void main()
{
	float alpha = texture(_Map, uv).r;
	gl_FragColor = vec4(_Color.rgb, alpha);
}