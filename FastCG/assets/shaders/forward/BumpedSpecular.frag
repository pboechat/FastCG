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
	vec3 bitangent = normalize(cross(vData.normal, vData.tangent.xyz) * vData.tangent.w);
	mat3 TBN = mat3(vData.tangent.xyz, bitangent, vData.normal);
	vec3 normal = normalize(TBN * UnpackNormalFromColor(texture(uBumpMap, vData.uv * uBumpMapTiling).xyz));
	oColor = Lighting(diffuse, uSpecularColor, uShininess, vData.lightDirection, vData.viewerDistance, vData.viewerDirection, vData.worldPosition, normal, GetScreenCoordinates());
}