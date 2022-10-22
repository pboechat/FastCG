#version 430 core

layout(location = 0) in vec4 iPosition;

layout(location = 0) out vec2 vUV;

void main()
{
	vUV = iPosition.xy * 0.5 + 0.5;
	gl_Position = vec4(iPosition.xy, 0.0, 1.0);
}