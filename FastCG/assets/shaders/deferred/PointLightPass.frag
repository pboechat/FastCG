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
	vec2 uv = gl_FragCoord.xy / uScreenSize;
	float depth = texture(uDepth, uv).x;
	vec3 viewPosition = GetViewPositionFromScreenCoordsAndDepth(uProjection, uInverseProjection, uScreenSize, vec3(gl_FragCoord.xy, depth));
	vec3 worldPosition = vec3(uInverseView * vec4(viewPosition, 1));
	vec4 diffuseColor = texture(uDiffuseMap, uv);
	vec3 normal = UnpackNormalFromColor(texture(uNormalMap, uv)).xyz;
	vec4 specularColor = texture(uSpecularMap, uv);
	float shininess = specularColor.w;
	specularColor = vec4(specularColor.xyz, 1.0);
	vec4 tangent = UnpackNormalFromColor(texture(uTangentMap, uv));
	vec4 extraData = texture(uExtraData, uv);

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

	vec3 lightDirection = tangentSpaceMatrix * normalize(uLight0ViewPosition.xyz - viewPosition);
	vec3 viewerDirection = tangentSpaceMatrix * normalize(-viewPosition);

	float ambientOcclusion = texture(uAmbientOcclusionMap, uv).x;
	ambientOcclusion = min(ambientOcclusion, 1.0);

	oColor = Lighting(diffuseColor, specularColor, shininess, lightDirection, viewerDirection, worldPosition, normal) * ambientOcclusion;
}