#include "FastCG.glsl"
#include "Scene.glsl"
#include "Lighting.glsl"
#include "Material.glsl"

layout(location = 0) in vec3 vLightDirection;
layout(location = 1) in float vViewerDistance;
layout(location = 2) in vec3 vWorldPosition;
layout(location = 3) in vec3 vNormal;
layout(location = 4) in vec4 vTangent;
layout(location = 5) in vec2 vUV;

layout(location = 0) out vec4 oColor;

void main()
{
	vec4 diffuse = uDiffuseColor * texture(uColorMap, (vUV * uColorMapTiling));
	vec3 bitangent = normalize(cross(vNormal, vTangent.xyz) * vTangent.w);
	mat3 TBN = mat3(vTangent.xyz, bitangent, vNormal);
	vec3 normal = normalize(TBN * UnpackNormalFromColor(texture(uBumpMap, vUV * uBumpMapTiling).xyz));
	oColor = Lighting(diffuse, vLightDirection, vViewerDistance, vWorldPosition, normal, GetScreenCoordinates());
}