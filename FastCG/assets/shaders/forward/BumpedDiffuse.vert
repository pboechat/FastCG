#include "FastCG.glsl"
#include "Scene.glsl"
#include "Instance.glsl"
#include "Lighting.glsl"

layout(location = 0) in vec3 iPosition;
layout(location = 1) in vec3 iNormal;
layout(location = 2) in vec2 iUV;
layout(location = 3) in vec4 iTangent;

layout(location = 0) out vec3 vLightDirection;
layout(location = 1) out float vViewerDistance;
layout(location = 2) out vec3 vWorldPosition;
layout(location = 3) out vec3 vNormal;
layout(location = 4) out vec4 vTangent;
layout(location = 5) out vec2 vUV;

void main()
{
	vec4 worldPosition = GetInstanceData().model * vec4(iPosition, 1.0);
	vec3 viewPosition = vec3(uView * worldPosition);
	vViewerDistance = length(GetViewerPosition() - worldPosition.xyz);
	vWorldPosition = worldPosition.xyz;
	vLightDirection = normalize(GetLightType() * uLight0Position.xyz - (step(0.0, GetLightType()) * vWorldPosition));
	mat3 MIT = mat3(GetInstanceData().modelInverseTranspose);
	vNormal = normalize(MIT * iNormal);
	vTangent = vec4(normalize(MIT * iTangent.xyz), iTangent.w);
	vUV = iUV;
	gl_Position = GetInstanceData().modelViewProjection * vec4(iPosition, 1.0);
}