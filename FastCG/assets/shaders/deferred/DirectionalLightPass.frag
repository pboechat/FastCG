#version 430 core

#ifdef VULKAN
#extension GL_GOOGLE_include_directive : enable
#endif

#include "FastCG.glsl"
#include "Scene.glsl"
#include "GetLightType"
#include "Lighting.glsl"

layout(location = 0) in vec2 vUV;

layout(location = 0) out vec4 oColor;

void main()
{
	float depth = texture(uDepth, vUV).x;
	vec3 viewPosition = GetViewPositionFromScreenCoordsAndDepth(uProjection, uInverseProjection, uScreenSize, vec3(gl_FragCoord.xy, depth));
	vec3 worldPosition = vec3(uInverseView * vec4(viewPosition, 1));
	vec4 diffuseColor = texture(uDiffuseMap, vUV);
	vec3 normal = UnpackNormalFromColor(texture(uNormalMap, vUV)).xyz;
	vec4 specularColor = texture(uSpecularMap, vUV);
	float shininess = specularColor.w;
	specularColor = vec4(specularColor.xyz, 1.0);
	vec4 tangent = UnpackNormalFromColor(texture(uTangentMap, vUV));
	vec4 extraData = texture(uExtraData, vUV);

	mat3 tangentSpaceMatrix;
	// FIXME: divergence control
	if (HasBump(tangent, extraData))
	{
		vec3 binormal = normalize(cross(normal, tangent.xyz) * tangent.w);
		tangentSpaceMatrix = transpose(mat3(tangent.xyz, binormal, normal));
		normal = UnpackNormalFromColor(extraData.xyz);
	}
	else
	{
		tangentSpaceMatrix = mat3(1.0);
	}

	vec3 lightDirection = tangentSpaceMatrix * normalize(uLight0ViewPosition.xyz);
	vec3 viewerDirection = tangentSpaceMatrix * normalize(-viewPosition);

	oColor = Lighting(diffuseColor, specularColor, shininess, lightDirection, viewerDirection, worldPosition, normal, vUV);
}