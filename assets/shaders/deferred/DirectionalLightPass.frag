#version 330

#include "FastCG.glsl"
#include "../Lighting.glsl"

in vec2 vertexUv;

layout(location = 0) out vec4 color;

void main()
{
	float depth = texture2D(_Depth, vertexUv).x;
	vec3 position = GetWorldPositionFromScreenCoordsAndDepth(vec3(gl_FragCoord.xy, depth));
	vec4 diffuseColor = texture2D(_DiffuseMap, vertexUv);
	vec3 normal = UnpackNormalFromColor(texture2D(_NormalMap, vertexUv)).xyz;
	vec4 specularColor = texture2D(_SpecularMap, vertexUv);
	float shininess = specularColor.w;
	specularColor = vec4(specularColor.xyz, 1.0);
	vec4 tangent = UnpackNormalFromColor(texture2D(_TangentMap, vertexUv));
	vec4 extraData = texture2D(_ExtraData, vertexUv);

	mat3 tangentSpaceMatrix;
	// FIXME: divergence control
	if (tangent.w != 0.0)
	{
		vec3 binormal = normalize(cross(normal, tangent.xyz) * tangent.w);
		tangentSpaceMatrix = transpose(mat3(tangent.xyz, binormal, normal));
		normal = UnpackNormalFromColor(extraData.xyz);
	}
	else
	{
		tangentSpaceMatrix = mat3(1.0);
	}

	vec3 lightDirection = tangentSpaceMatrix * normalize(_Light0Position);
	vec3 viewerDirection = tangentSpaceMatrix * normalize(-position);

	float ambientOcclusion = texture2D(_AmbientOcclusionMap, vertexUv).x;
	ambientOcclusion = max(ambientOcclusion, 1.0 - _AmbientOcclusionFlag);

	color = Lighting(diffuseColor, specularColor, shininess, lightDirection, viewerDirection, normal) * ambientOcclusion;
}