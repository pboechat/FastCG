#version 430 core

#ifdef VULKAN
#extension GL_GOOGLE_include_directive : enable
#endif

#include "FastCG.glsl"
#include "SceneConstants.glsl"
#include "LightingConstants.glsl"
#include "../Lighting.glsl"

layout(location = 0) out vec4 oColor;

void main()
{
	vec2 iUV = gl_FragCoord.xy / uScreenSize;

	float depth = texture(uDepth, iUV).x;
	vec3 position = GetWorldPositionFromScreenCoordsAndDepth(uProjection, uInverseProjection, uScreenSize, vec3(gl_FragCoord.xy, depth));
	vec4 diffuseColor = texture(uDiffuseMap, iUV);
	vec3 normal = UnpackNormalFromColor(texture(uNormalMap, iUV)).xyz;
	vec4 specularColor = texture(uSpecularMap, iUV);
	float shininess = specularColor.w;
	specularColor = vec4(specularColor.xyz, 1.0);
	vec4 tangent = UnpackNormalFromColor(texture(uTangentMap, iUV));
	vec4 extraData = texture(uExtraData, iUV);

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

	vec3 lightDirection = tangentSpaceMatrix * normalize(uLight0Position.xyz - position);
	vec3 viewerDirection = tangentSpaceMatrix * normalize(-position);

	float ambientOcclusion = texture(uAmbientOcclusionMap, iUV).x;
	ambientOcclusion = min(ambientOcclusion, 1.0);

	oColor = Lighting(diffuseColor, specularColor, shininess, lightDirection, viewerDirection, position, normal) * ambientOcclusion;
}