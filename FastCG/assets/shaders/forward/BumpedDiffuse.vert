#include "VertexData.glsl"

layout(location = 0) in vec3 iPosition;
layout(location = 1) in vec3 iNormal;
layout(location = 2) in vec2 iUV;
layout(location = 3) in vec4 iTangent;

layout(location = 0) out VertexData vData;

void main()
{
	vData = ComputeVertexData(iPosition, iNormal, iUV, iTangent);
	gl_Position = vData.clipPosition;
}