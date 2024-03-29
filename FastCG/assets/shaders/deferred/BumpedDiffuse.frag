#include "FastCG.glsl"
#include "Material.glsl"

layout(location = 0) in vec3 vNormal;
layout(location = 1) in vec4 vTangent;
layout(location = 2) in vec2 vUV;

layout(location = 0) out vec4 oDiffuse;
layout(location = 1) out vec4 oNormal;
layout(location = 3) out vec4 oTangent;
layout(location = 4) out vec4 oExtraData;

void main()
{
	oDiffuse = uDiffuseColor * texture(uColorMap, vUV * uColorMapTiling);
	oNormal = vec4(PackNormalToColor(vNormal), 0.0);
	oTangent = vec4(PackNormalToColor(vTangent.xyz), vTangent.w * 0.5 + 0.5);
	oExtraData = vec4(texture(uBumpMap, vUV * uBumpMapTiling).xyz, 0.0);
}