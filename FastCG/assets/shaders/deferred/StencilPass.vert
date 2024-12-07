#include "FastCG.glsl"
#include "Instance.glsl"

layout(location = 0) in vec3 iPosition;
layout(location = 1) in vec3 iNormal;
layout(location = 2) in vec2 iUV;

void main()
{
	gl_Position = GetInstanceData().modelViewProjection * vec4(iPosition, 1.0);
}