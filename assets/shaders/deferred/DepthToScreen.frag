#version 330

uniform sampler2D _Depth;

in vec2 uv;

void main()
{
	gl_FragColor = vec4(vec3(texture(_Depth, uv).x), 1);
}