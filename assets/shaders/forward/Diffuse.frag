#version 330

#include "FastCG.glsl"
#include "../Lighting.glsl"

uniform sampler2D colorMap;
uniform vec2 colorMapTiling;
uniform vec4 diffuseColor;

in vec3 vertexPosition;
in vec3 vertexNormal;
in vec2 vertexUV;

layout(location = 0) out vec4 color;

void main()
{
	vec4 finalDiffuseColor = diffuseColor * texture(colorMap, (vertexUV * colorMapTiling));

	vec3 lightDirection = normalize(_Light0Position - (step(0.0, _Light0Type) * vertexPosition));
	
	color = Lighting(finalDiffuseColor, lightDirection, vertexPosition, vertexNormal);
}