#version 330

#include "shaders/FastCG.glsl"

in vec4 eyeSpacePosition;
in vec3 vertexNormal;
in vec2 textureCoordinates;

uniform sampler2D colorMap;
uniform vec2 colorMapTiling;

void main()
{
	vec4 diffuseColor = texture(colorMap, (textureCoordinates * colorMapTiling));

	vec3 lightPosition = vec3(_View * vec4(_Light0Position, 1.0));
	vec3 lightDirection = normalize(lightPosition - eyeSpacePosition.xyz);
	
	vec4 ambientContribution = _GlobalLightAmbientColor;
	
	float diffuseAttenuation = max(dot(lightDirection, vertexNormal), 0.0);
	vec4 diffuseContribution = _Light0DiffuseColor * diffuseColor * diffuseAttenuation;

	gl_FragColor = ambientContribution + diffuseContribution;
}