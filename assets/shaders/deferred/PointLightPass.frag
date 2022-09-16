#version 330

#include "FastCG.glsl"

uniform float _Debug;

vec4 BlinnPhongLighting(vec4 materialDiffuseColor,
						vec4 materialSpecularColor,
						float materialShininess,
						vec3 lightDirection,
					    vec3 viewerDirection,
						vec3 position,
					    vec3 normal)
{
    vec4 ambientContribution = _Light0AmbientColor * _Light0Intensity * materialDiffuseColor;

    float diffuseAttenuation = max(dot(normal, lightDirection), 0.0);
    vec4 diffuseContribution = _Light0DiffuseColor * _Light0Intensity * materialDiffuseColor * diffuseAttenuation;

    vec3 reflectionDirection = normalize(reflect(-lightDirection, normal));
    float specularAttenuation = pow(max(dot(reflectionDirection, viewerDirection), 0.0), materialShininess);
    vec4 specularContribution = _Light0SpecularColor * _Light0Intensity * materialSpecularColor * specularAttenuation * step(0.0, diffuseAttenuation);

    return DistanceAttenuation(position) * (ambientContribution + diffuseContribution + specularContribution);
}

void main()
{
	vec2 uv = gl_FragCoord.xy / _ScreenSize;
	float depth = texture2D(_DepthMap, uv).x;

	vec3 position = GetPositionFromWindowCoordinates(vec3(gl_FragCoord.xy, depth));
	vec4 diffuseColor = texture2D(_DiffuseMap, uv);
	vec3 normal = UnpackNormal(texture2D(_NormalMap, uv));
	vec4 specularColor = texture2D(_SpecularMap, uv);
	float shininess = specularColor.w;
	specularColor = vec4(specularColor.xyz, 1.0);

	float ambientOcclusion = texture2D(_AmbientOcclusionMap, uv).x;
	ambientOcclusion = max(ambientOcclusion, 1.0 - _AmbientOcclusionFlag);

	vec3 lightDirection = normalize(_Light0Position - position);
	vec3 viewerDirection = normalize(-position);

	gl_FragColor = vec4(_Debug) + (BlinnPhongLighting(diffuseColor, specularColor, shininess, lightDirection, viewerDirection, position, normal) * ambientOcclusion);
}