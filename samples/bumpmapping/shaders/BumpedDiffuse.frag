#version 330

#include "shaders/FastCG.glsl"

uniform sampler2D colorMap;
uniform sampler2D bumpMap;
uniform vec2 colorMapTiling;
uniform vec2 bumpMapTiling;

in vec3 lightDirection;
in vec3 viewerDirection;
in vec2 textureCoordinates;

void main()
{
	vec4 diffuseColor = texture(colorMap, (textureCoordinates * colorMapTiling));

	vec3 normal = normalize(texture(bumpMap, (textureCoordinates * bumpMapTiling)).xyz * 2.0 - 1.0);

	vec4 ambientContribution = _GlobalLightAmbientColor;

	float diffuseAttenuation = max(dot(lightDirection, normal), 0.0); 
	vec4 diffuseContribution = _Light0DiffuseColor * diffuseColor * diffuseAttenuation;

	gl_FragColor = ambientContribution + diffuseContribution;
}