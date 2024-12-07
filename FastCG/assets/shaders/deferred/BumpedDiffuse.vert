#include "FastCG.glsl"
#include "Instance.glsl"

layout(location = 0) in vec3 iPosition;
layout(location = 1) in vec3 iNormal;
layout(location = 2) in vec2 iUV;
layout(location = 3) in vec4 iTangent;

layout(location = 0) out vec3 vNormal;
layout(location = 1) out vec4 vTangent;
layout(location = 2) out vec2 vUV;

void main()
{
	mat3 MIT = mat3(GetInstanceData().modelInverseTranspose);
	vNormal = normalize(MIT * iNormal);
	vTangent = vec4(normalize(MIT * iTangent.xyz), iTangent.w);
	vUV = iUV;
	gl_Position = GetInstanceData().modelViewProjection * vec4(iPosition, 1.0);
}