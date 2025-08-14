#include "FastCG.glsl"
#include "Scene.glsl"
#include "Lighting.glsl"
#include "Material.glsl"
#include "VertexData.glsl"

layout(location = 0) in VertexData vData;

layout(location = 0) out vec4 oColor;

void main()
{
	vec4 diffuse = uDiffuseColor * texture(uColorMap, (vData.uv * uColorMapTiling));
	oColor = Lighting(diffuse, uSpecularColor, uShininess, vData.lightDirection, vData.viewerDistance, vData.viewerDirection, vData.worldPosition, vData.normal, GetScreenCoordinates());
}