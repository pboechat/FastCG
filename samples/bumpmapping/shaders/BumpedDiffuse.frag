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
	vec3 diffuseColor = texture(colorMap, (textureCoordinates * colorMapTiling)).xyz;
	vec3 normal = texture(bumpMap, (textureCoordinates * bumpMapTiling)).xyz;

	vec3 ambientContribution = vec3(_GlobalLightAmbientColor + _Light0AmbientColor);

	float diffuseAttenuation = max(dot(lightDirection, normal), 0.0); 
	vec3 diffuseContribution = vec3(_Light0DiffuseColor) * diffuseColor * diffuseAttenuation;

	gl_FragColor = vec4(ambientContribution + diffuseContribution, 1.0);
}