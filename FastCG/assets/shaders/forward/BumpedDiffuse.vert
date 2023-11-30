#include "FastCG.glsl"
#include "Scene.glsl"
#include "Instance.glsl"
#include "Lighting.glsl"

layout(location = 0) in vec3 iPosition;
layout(location = 1) in vec3 iNormal;
layout(location = 2) in vec2 iUV;
layout(location = 3) in vec4 iTangent;

layout(location = 0) out vec3 vLightDirection;
layout(location = 1) out float vViewDistance;
layout(location = 2) out vec3 vWorldPosition;
layout(location = 3) out vec2 vUV;

void main()
{
	vec3 normal = normalize(mat3(GetInstanceData().modelViewInverseTranspose) * iNormal);
	vec3 tangent = normalize(mat3(GetInstanceData().modelViewInverseTranspose) * iTangent.xyz);
	vec3 binormal = normalize(cross(normal, tangent) * iTangent.w);

	mat3 tangentSpaceMatrix = transpose(mat3(tangent, binormal, normal));

	vec4 worldPosition = GetInstanceData().model * vec4(iPosition, 1);
	vec3 viewPosition = vec3(uView * worldPosition);
	vViewDistance = length(viewPosition);
	vLightDirection = tangentSpaceMatrix * normalize(GetLightType() * uLight0ViewPosition.xyz - (step(0.0, GetLightType()) * viewPosition));
	vWorldPosition = worldPosition.xyz;
	vUV = iUV;

	gl_Position = GetInstanceData().modelViewProjection * vec4(iPosition, 1);
}