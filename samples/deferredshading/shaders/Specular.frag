#version 330

#include "shaders/FastCG.glsl"

in vec4 eyeSpacePosition;
in vec3 vertexNormal;
in vec2 textureCoordinates;

uniform sampler2D colorMap;
uniform vec2 colorMapTiling;
uniform vec4 specularColor;
uniform float shininess;

void main()
{
	vec4 diffuseColor = texture(colorMap, (textureCoordinates * colorMapTiling));

	vec3 lightPosition = vec3(_View * vec4(_Light0Position, 1.0));
	vec3 lightDirection = normalize(lightPosition - eyeSpacePosition.xyz);
	
	vec4 ambientContribution = _GlobalLightAmbientColor;
	
	float diffuseAttenuation = max(dot(lightDirection, vertexNormal), 0.0);
	vec4 diffuseContribution = _Light0DiffuseColor * diffuseColor * diffuseAttenuation;
	
	vec4 specularContribution = vec4(0.0);
	if (diffuseAttenuation > 0.0)
	{
		vec3 viewerDirection = normalize(-eyeSpacePosition.xyz);
		vec3 reflection = reflect(-lightDirection, vertexNormal);
		float specularAttenuation = pow(max(dot(reflection, viewerDirection), 0.0), shininess);
		specularContribution = _Light0SpecularColor * specularColor * specularAttenuation;
	}  

	gl_FragColor = ambientContribution + diffuseContribution + specularContribution;
}