#include "FastCG.glsl"
#include "Material.glsl"
#include "VertexData.glsl"

layout(location = 0) in VertexData vData;

layout(location = 0) out vec4 oDiffuse;
layout(location = 1) out vec4 oNormal;

void main()
{
	oDiffuse = uDiffuseColor * texture(uColorMap, vData.uv * uColorMapTiling);
	oNormal = vec4(PackNormalToColor(vData.normal), 0.0);
}