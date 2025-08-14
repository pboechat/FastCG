#include "FastCG.glsl"
#include "Material.glsl"
#include "VertexData.glsl"

layout(location = 0) in VertexData vData;

layout(location = 0) out vec4 oDiffuse;
layout(location = 1) out vec4 oNormal;
layout(location = 2) out vec4 oSpecular;

void main()
{
	oDiffuse = uDiffuseColor;
	oNormal = vec4(PackNormalToColor(vData.normal), 0.0);
	oSpecular = vec4(uSpecularColor.xyz, PackToNormalizedValue(uShininess));
}