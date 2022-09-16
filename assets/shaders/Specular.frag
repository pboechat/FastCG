#version 330

#include "FastCG.glsl"
#include "Lighting.glsl"

uniform sampler2D colorMap;
uniform vec2 colorMapTiling;
uniform vec4 ambientColor;
uniform vec4 diffuseColor;
uniform vec4 specularColor;
uniform float shininess;

in vec3 viewerDirection;
in vec3 vertexPosition;
in vec3 vertexNormal;
in vec2 vertexUV;

void main()
{
	vec4 texelColor = texture(colorMap, (vertexUV * colorMapTiling));
	vec4 finalDiffuseColor = diffuseColor * texelColor;

	vec3 lightDirection = normalize(_Light0Position - (step(0.0, _Light0Type) * vertexPosition));

	gl_FragColor = BlinnPhongLighting(ambientColor,
	                                  finalDiffuseColor,
									  specularColor,
									  shininess,
									  lightDirection,
									  viewerDirection,
									  vertexPosition,
									  vertexNormal);
}