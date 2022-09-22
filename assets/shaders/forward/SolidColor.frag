#version 330

#include "FastCG.glsl"
#include "../Lighting.glsl"

uniform vec4 diffuseColor;
uniform vec4 specularColor;
uniform float shininess;

in vec3 viewerDirection;
in vec3 vertexPosition;
in vec3 vertexNormal;
in vec2 vertexUV;

layout(location = 0) out vec4 color;

void main()
{
	vec3 lightDirection = normalize(_Light0Position - (step(0.0, _Light0Type) * vertexPosition));

	color = Lighting(diffuseColor, specularColor, shininess, lightDirection, viewerDirection, vertexPosition, vertexNormal);
}