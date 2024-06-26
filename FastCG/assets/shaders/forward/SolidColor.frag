#include "FastCG.glsl"
#include "Scene.glsl"
#include "Lighting.glsl"
#include "Material.glsl"

layout(location = 0) in vec3 vLightDirection;
layout(location = 1) in float vViewDistance;
layout(location = 2) in vec3 vViewerDirection;
layout(location = 3) in vec3 vWorldPosition;
layout(location = 4) in vec3 vNormal;
layout(location = 5) in vec2 vUV;

layout(location = 0) out vec4 oColor;

void main()
{
	oColor = Lighting(uDiffuseColor, uSpecularColor, uShininess, vLightDirection, vViewDistance, vViewerDirection, vWorldPosition, vNormal, GetScreenCoordinates());
}