#version 330

#include "FastCG.glsl"
#include "../Lighting.glsl"

uniform float _Debug;

void main()
{
	vec2 uv = gl_FragCoord.xy / _ScreenSize;

	float depth = texture2D(_Depth, uv).x;
	vec3 position = GetWorldPositionFromScreenCoordsAndDepth(vec3(gl_FragCoord.xy, depth));
	vec4 diffuseColor = texture2D(_DiffuseMap, uv);
	vec3 normal = UnpackNormalFromColor(texture2D(_NormalMap, uv)).xyz;
	vec4 specularColor = texture2D(_SpecularMap, uv);
	float shininess = specularColor.w;
	specularColor = vec4(specularColor.xyz, 1.0);
	vec4 tangent = UnpackNormalFromColor(texture2D(_TangentMap, uv));
	vec4 extraData = texture2D(_ExtraData, uv);

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

	vec3 lightDirection = tangentSpaceMatrix * normalize(_Light0Position - position);
	vec3 viewerDirection = tangentSpaceMatrix * normalize(-position);

	float ambientOcclusion = texture2D(_AmbientOcclusionMap, uv).x;
	ambientOcclusion = max(ambientOcclusion, 1.0 - _AmbientOcclusionFlag);

	gl_FragColor = vec4(_Debug) + (Lighting(diffuseColor, specularColor, shininess, lightDirection, viewerDirection, position, normal) * ambientOcclusion);
}