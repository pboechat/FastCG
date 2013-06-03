#version 330

#include "shaders/deferred/FastCG.glsl"

uniform float _Debug;

float DistanceAttenuation(vec3 position)
{
	float _distance = distance(_Light0Position, position);
	return 1.0 / max(_Light0ConstantAttenuation + _Light0LinearAttenuation * _distance + _Light0QuadraticAttenuation * pow(_distance, 2), 1.0);
}

vec4 BlinnPhongLighting(vec4 materialDiffuseColor,
						vec4 materialSpecularColor,
						float materialShininess,
						vec3 lightDirection,
					    vec3 viewerDirection,
						vec3 position,
					    vec3 normal)
{
    vec4 ambientContribution = _GlobalLightAmbientColor * _Light0AmbientColor * _Light0Intensity;

    float diffuseAttenuation = max(dot(lightDirection, normal), 0.0);
    vec4 diffuseContribution = _Light0DiffuseColor * _Light0Intensity * materialDiffuseColor * diffuseAttenuation;

	vec4 specularContribution = vec4(0.0);
	if (diffuseAttenuation > 0.0)
	{
        vec3 reflectionDirection = normalize(reflect(-lightDirection, normal));
        float specularAttenuation = pow(max(dot(reflectionDirection, viewerDirection), 0.0), materialShininess);
        specularContribution = _Light0SpecularColor * _Light0Intensity * materialSpecularColor * specularAttenuation;
    }

    return (ambientContribution + diffuseContribution + specularContribution) * DistanceAttenuation(position);
}

void main()
{
	vec2 uv = gl_FragCoord.xy / _ScreenSize;

	vec3 position = texture2D(_PositionMap, uv).xyz;
	vec4 diffuseColor = texture2D(_DiffuseMap, uv);
	vec4 normalAndShininess = texture2D(_NormalMap, uv);
	vec4 specularColor = texture2D(_SpecularMap, uv);
	vec3 normal = normalAndShininess.xyz;
	float shininess = normalAndShininess.w;
	float ambientOcclusion = texture2D(_AmbientOcclusionMap, uv).x;
	ambientOcclusion = max(ambientOcclusion, 1.0 - _AmbientOcclusionFlag);

	vec3 lightDirection = normalize(_Light0Position - position);
	vec3 viewerDirection = normalize(-position);

	gl_FragColor = vec4(_Debug) + (BlinnPhongLighting(diffuseColor, specularColor, shininess, lightDirection, viewerDirection, position, normal) * ambientOcclusion);
}