#include "FastCG.glsl"
#include "Scene.glsl"
#include "Lighting.glsl"

layout(location = 0) out vec4 oColor;

void main()
{
	vec2 screenCoords = GetScreenCoordinates();
	float depth = texture(uDepth, screenCoords).x;
	vec3 viewPosition = GetViewPositionFromScreenPositionAndDepth(uInverseProjection, uScreenSize, gl_FragCoord.xy, depth);
	vec3 worldPosition = vec3(uInverseView * vec4(viewPosition, 1));
	vec4 diffuseColor = texture(uDiffuseMap, screenCoords);
	vec3 normal = UnpackNormalFromColor(texture(uNormalMap, screenCoords)).xyz;
	vec4 specularColor = texture(uSpecularMap, screenCoords);
	float shininess = UnpackFromNormalizedValue(specularColor.w);
	specularColor = vec4(specularColor.xyz, 1.0);
	vec4 tangent = texture(uTangentMap, screenCoords);
	tangent = vec4(UnpackNormalFromColor(tangent.xyz), tangent.z * 2.0 - 1.0);
	vec4 extraData = texture(uExtraData, screenCoords);

	mat3 tangentSpaceMatrix;
	// FIXME: divergence control
	if (HasBump(extraData))
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
	float viewDistance = length(viewPosition);
	vec3 viewerDirection = tangentSpaceMatrix * (-viewPosition / viewDistance);

	oColor = Lighting(diffuseColor, specularColor, shininess, lightDirection, viewDistance, viewerDirection, worldPosition, normal, screenCoords);
}