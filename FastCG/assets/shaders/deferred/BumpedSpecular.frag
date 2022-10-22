#version 430 core

#ifdef VULKAN
#extension GL_GOOGLE_include_directive : enable
#endif

#include "FastCG.glsl"
#include "MaterialConstants.glsl"

layout(location = 0) in vec3 vNormal;
layout(location = 1) in vec4 vTangent;
layout(location = 2) in vec2 vUV;

layout(location = 0) out vec4 oDiffuse;
layout(location = 1) out vec4 oNormal;
layout(location = 2) out vec4 oSpecular;
layout(location = 3) out vec4 oTangent;
layout(location = 4) out vec4 oExtraData;

void main()
{
	oDiffuse = uDiffuseColor * texture(uColorMap, vUV * uColorMapTiling);
	oNormal = vec4(PackNormalToColor(vNormal), 0.0);
	oSpecular = vec4(uSpecularColor.xyz, uShininess);
	oTangent = PackNormalToColor(vTangent);
	oExtraData = vec4(texture(uBumpMap, vUV * uBumpMapTiling).xyz, 0.0);
}