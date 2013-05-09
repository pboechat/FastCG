#version 330

#include "shaders/FastCG.glsl"

uniform sampler2D colorMap;
uniform sampler2D bumpMap;
uniform vec2 colorMapTiling;
uniform vec2 bumpMapTiling;
uniform vec4 specularColor;
uniform float shininess;

in vec3 lightDirection;
in vec3 viewerDirection;
in vec2 textureCoordinates;

void main()
{
	vec4 diffuseColor = texture2D(colorMap, (textureCoordinates * colorMapTiling));

	vec3 normal = normalize(texture2D(bumpMap, (textureCoordinates * bumpMapTiling)).xyz * 2.0 - 1.0);

	vec4 ambientContribution = _GlobalLightAmbientColor;

	float diffuseAttenuation = max(dot(normal, lightDirection), 0.0); 
	vec4 diffuseContribution = _Light0DiffuseColor * diffuseColor * diffuseAttenuation;

	vec4 specularContribution = vec4(0.0);
	if (diffuseAttenuation > 0.0) 
	{
		vec3 reflection = reflect(-lightDirection, normal);
		float specularAttenuation = pow(max(dot(reflection, normalize(viewerDirection)), 0.0), shininess);
		specularContribution = _Light0SpecularColor * specularColor * specularAttenuation;
	}

	gl_FragColor = ambientContribution + diffuseContribution + specularContribution;
}