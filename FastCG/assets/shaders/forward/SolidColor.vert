#include "Lighting.glsl"
#include "FastCG.glsl"
#include "Scene.glsl"
#include "Instance.glsl"

layout(location = 0) in vec3 iPosition;
layout(location = 1) in vec3 iNormal;
layout(location = 2) in vec2 iUV;

layout(location = 0) out vec3 vLightDirection;
layout(location = 1) out float vViewDistance;
layout(location = 2) out vec3 vViewerDirection;
layout(location = 3) out vec3 vWorldPosition;
layout(location = 4) out vec3 vNormal;
layout(location = 5) out vec2 vUV;

void main()
{
	vec4 worldPosition = GetInstanceData().model * vec4(iPosition, 1);
	vec3 viewPosition = vec3(uView * worldPosition);
	vLightDirection = normalize(GetLightType() * uLight0ViewPosition.xyz - (step(0.0, GetLightType()) * viewPosition));
	vViewDistance = length(viewPosition);
	vViewerDirection = -viewPosition / vViewDistance;
	vWorldPosition = worldPosition.xyz;
	vNormal = normalize(mat3(GetInstanceData().modelViewInverseTranspose) * iNormal);
	vUV = iUV;
	gl_Position = GetInstanceData().modelViewProjection * vec4(iPosition, 1);
}