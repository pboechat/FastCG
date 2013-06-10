#version 330

#include "shaders/FastCG.glsl"
#include "shaders/Lighting.glsl"

uniform vec4 ambientColor;
uniform vec4 diffuseColor;

in vec3 vertexPosition;
in vec3 vertexNormal;
in vec2 vertexUV;

void main()
{
	vec3 lightDirection;
	if (_Light0Type == 0.0) // directional
	{
		lightDirection = normalize(_Light0Position);
	}
	else
	{
		vec3 lightPosition = vec3(_View * vec4(_Light0Position, 1.0));
		lightDirection = normalize(lightPosition - vertexPosition);
	}

	gl_FragColor = BlinnPhongLighting(ambientColor,
									  diffuseColor,
									  lightDirection,
									  vertexPosition,
									  vertexNormal);
}