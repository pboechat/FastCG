#version 430 core

layout(location = 0) out float oShadow;

void main()
{
	oShadow = gl_FragCoord.z;
}