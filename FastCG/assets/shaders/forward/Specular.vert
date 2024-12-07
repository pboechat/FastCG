#include "FastCG.glsl"
#include "Scene.glsl"
#include "Lighting.glsl"
#include "Instance.glsl"

layout(location = 0) in vec3 iPosition;
layout(location = 1) in vec3 iNormal;
layout(location = 2) in vec2 iUV;

layout(location = 0) out vec3 vLightDirection;
layout(location = 1) out float vViewerDistance;
layout(location = 2) out vec3 vViewerDirection;
layout(location = 3) out vec3 vWorldPosition;
layout(location = 4) out vec3 vNormal;
layout(location = 5) out vec2 vUV;

void main()
{
	vec4 worldPosition = GetInstanceData().model * vec4(iPosition, 1.0);
	vec3 viewPosition = vec3(uView * worldPosition);
	vViewerDirection = GetViewerPosition() - worldPosition.xyz;
	vViewerDistance = length(vViewerDirection);
	vViewerDirection = vViewerDirection / max(vViewerDistance, 1e-8);
	vWorldPosition = worldPosition.xyz;
	vLightDirection = normalize(GetLightType() * uLight0Position.xyz - (step(0.0, GetLightType()) * vWorldPosition));
	vNormal = normalize(mat3(GetInstanceData().modelInverseTranspose) * iNormal);
	vUV = iUV;
	gl_Position = GetInstanceData().modelViewProjection * vec4(iPosition, 1.0);
}