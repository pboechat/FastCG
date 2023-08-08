#version 430 core

#ifdef ENABLE_INCLUDE_EXTENSION_DIRECTIVE
#extension GL_GOOGLE_include_directive : enable 
#endif

#include "FastCG.glsl"
#include "Instance.glsl"

layout(location = 0) in vec3 iPosition;

void main()
{
	gl_Position = GetInstanceData().modelViewProjection * vec4(iPosition, 1);
}