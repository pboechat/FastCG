#include "FastCG.glsl"
#include "Scene.glsl"
#include "Lighting.glsl"
#include "Material.glsl"

layout(location = 0) in vec3 vLightDirection;
layout(location = 1) in float vViewerDistance;
layout(location = 2) in vec3 vViewerDirection;
layout(location = 3) in vec3 vWorldPosition;
layout(location = 4) in vec3 vNormal;
layout(location = 5) in vec2 vUV;

layout(location = 0) out vec4 oColor;

void main()
{
	vec4 diffuse = uDiffuseColor * texture(uColorMap, (vUV * uColorMapTiling));
	oColor = Lighting(diffuse, uSpecularColor, uShininess, vLightDirection, vViewerDistance, vViewerDirection, vWorldPosition, vNormal, GetScreenCoordinates());
}