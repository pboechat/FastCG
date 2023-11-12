#include "FastCG.glsl"
#include "Material.glsl"

layout(location = 0) in vec3 vNormal;
layout(location = 1) in vec2 vUV;

layout(location = 0) out vec4 oDiffuse;
layout(location = 1) out vec4 oNormal;

void main()
{
	oDiffuse = uDiffuseColor * texture(uColorMap, vUV * uColorMapTiling);
	oNormal = vec4(PackNormalToColor(vNormal), 0.0);
}