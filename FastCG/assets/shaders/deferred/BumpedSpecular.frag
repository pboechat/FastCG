#include "FastCG.glsl"
#include "Material.glsl"
#include "VertexData.glsl"

layout(location = 0) in VertexData vData;

layout(location = 0) out vec4 oDiffuse;
layout(location = 1) out vec4 oNormal;
layout(location = 2) out vec4 oSpecular;
layout(location = 3) out vec4 oTangent;
layout(location = 4) out vec4 oExtraData;

void main()
{
	oDiffuse = uDiffuseColor * texture(uColorMap, vData.uv * uColorMapTiling);
	oNormal = vec4(PackNormalToColor(vData.normal), 0.0);
	oSpecular = vec4(uSpecularColor.xyz, PackToNormalizedValue(uShininess));
	oTangent = vec4(PackNormalToColor(vData.tangent.xyz), vData.tangent.w * 0.5 + 0.5);
	oExtraData = vec4(texture(uBumpMap, vData.uv * uBumpMapTiling).xyz, 0.0);
}