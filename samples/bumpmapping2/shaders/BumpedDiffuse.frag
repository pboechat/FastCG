#version 330

#include "shaders/FastCG.glsl"
#include "shaders/Lighting.glsl"

uniform sampler2D colorMap;
uniform sampler2D bumpMap;
uniform vec2 colorMapTiling;
uniform vec2 bumpMapTiling;
uniform vec4 ambientColor;
uniform vec4 diffuseColor;

in vec3 lightDirection;
in vec3 vertexPosition;
in vec2 vertexUV;

void main()
{
	vec4 texelColor = texture2D(colorMap, (vertexUV * colorMapTiling));
	vec4 finalDiffuseColor = diffuseColor * texelColor;
	vec3 normal = texture2D(bumpMap, (vertexUV * bumpMapTiling)).rgb * 2.0 - 1.0;

	gl_FragColor = BlinnPhongLighting(ambientColor,
									  finalDiffuseColor,
									  lightDirection,
									  vertexPosition,
									  normal) * step(0.0, lightDirection.z);
}