#version 330

#include "FastCG.glsl"

uniform sampler2D colorMap;
uniform sampler2D bumpMap;
uniform vec2 colorMapTiling;
uniform vec2 bumpMapTiling;
uniform vec4 ambientColor;
uniform vec4 diffuseColor;

in vec3 lightDirection;
in vec3 vertexPosition;
in vec2 vertexUV;

vec4 BlinnPhongLighting(vec4 materialAmbientColor,
						vec4 materialDiffuseColor,
						vec3 lightDirection,
						vec3 position,
						vec3 normal)
{
    vec4 ambientContribution = _GlobalLightAmbientColor + _Light0AmbientColor * _Light0Intensity;

	float selfShadowing = step(0.0, lightDirection.z);
    float diffuseAttenuation = max(dot(lightDirection, normal), 0.0);
    vec4 diffuseContribution = _Light0DiffuseColor * _Light0Intensity * materialDiffuseColor * diffuseAttenuation * selfShadowing;

    return DistanceAttenuation(position) * (ambientContribution + diffuseContribution);
}

void main()
{
	vec4 texelColor = texture2D(colorMap, (vertexUV * colorMapTiling));
	vec4 finalDiffuseColor = diffuseColor * texelColor;
	vec3 normal = UnpackNormal(texture2D(bumpMap, (vertexUV * bumpMapTiling)));

	gl_FragColor = BlinnPhongLighting(ambientColor,
									  finalDiffuseColor,
									  lightDirection,
									  vertexPosition,
									  normal);
}