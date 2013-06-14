#version 330

#include "FastCG.glsl"

uniform sampler2D colorMap;
uniform sampler2D bumpMap;
uniform vec2 colorMapTiling;
uniform vec2 bumpMapTiling;
uniform vec4 ambientColor;
uniform vec4 diffuseColor;
uniform vec4 specularColor;
uniform float shininess;

in vec3 lightDirection;
in vec3 viewerDirection;
in vec3 vertexPosition;
in vec2 vertexUV;

float DistanceAttenuation(vec3 position)
{
	float _distance = distance(_Light0Position, position);
	float attenuation = min(_Light0ConstantAttenuation + _Light0LinearAttenuation * _distance + _Light0QuadraticAttenuation * pow(_distance, 2.0), 1.0);
	attenuation = max(_Light0Type * attenuation, abs(_Light0Type));
	return 1.0 / attenuation;
}

vec4 BlinnPhongLighting(vec4 materialAmbientColor,
						vec4 materialDiffuseColor,
						vec4 materialSpecularColor,
						float materialShininess,
						vec3 lightDirection,
						vec3 viewerDirection,
						vec3 position,
						vec3 normal)
{
    vec4 ambientContribution = _GlobalLightAmbientColor + _Light0AmbientColor * _Light0Intensity * materialAmbientColor;

	float selfShadowing = step(0.0, lightDirection.z);
    float diffuseAttenuation = max(dot(lightDirection, normal), 0.0);
    vec4 diffuseContribution = _Light0DiffuseColor * _Light0Intensity * materialDiffuseColor * diffuseAttenuation * selfShadowing;

	vec3 reflectionDirection = normalize(reflect(-lightDirection, normal));
	float specularAttenuation = pow(max(dot(reflectionDirection, viewerDirection), 0.0), materialShininess);
	vec4 specularContribution = _Light0SpecularColor * _Light0Intensity * materialSpecularColor * specularAttenuation * step(0.0, diffuseAttenuation) * selfShadowing;

    return DistanceAttenuation(position) * (ambientContribution + diffuseContribution + specularContribution);
}

void main()
{
	vec4 texelColor = texture2D(colorMap, (vertexUV * colorMapTiling));
	vec4 finalDiffuseColor = diffuseColor * texelColor;
	vec3 normal = texture2D(bumpMap, (vertexUV * bumpMapTiling)).rgb * 2.0 - 1.0;

	gl_FragColor = BlinnPhongLighting(ambientColor,
									  finalDiffuseColor,
									  specularColor,
									  shininess,
									  lightDirection,
									  viewerDirection,
									  vertexPosition,
									  normal);
}