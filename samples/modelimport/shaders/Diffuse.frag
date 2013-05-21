#version 330

#include "shaders/FastCG.glsl"
#include "shaders/Lighting.glsl"

uniform sampler2D colorMap;
uniform vec2 colorMapTiling;

in vec3 vertexPosition;
in vec3 vertexNormal;
in vec2 vertexUV;

void main()
{
	vec4 diffuseColor = texture(colorMap, (vertexUV * colorMapTiling));

	vec3 lightPosition = vec3(_View * vec4(_Light0Position, 1.0));
	vec3 lightDirection = normalize(lightPosition - vertexPosition);
	
	gl_FragColor = BlinnPhongLighting(diffuseColor,
									  lightDirection,
									  vertexPosition,
									  vertexNormal);
}