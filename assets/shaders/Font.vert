#version 330

uniform vec4 _ScreenCoords;
uniform vec4 _MapCoords;
uniform vec2 _ScreenSize;

out vec2 uv;

void main()
{
	vec2 mask = vec2(gl_VertexID & 1, gl_VertexID >> 1);
	uv = _MapCoords.xy + mask * _MapCoords.zw;
	gl_Position = vec4((_ScreenCoords.xy + mask * _ScreenCoords.zw) / _ScreenSize.xy * 2 - vec2(1, 1), 0, 1);
}