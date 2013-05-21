#version 330

#include "shaders/FastCG.glsl"
#include "shaders/Lighting.glsl"

uniform sampler2D colorMap;
uniform sampler2D bumpMap;
uniform vec2 colorMapTiling;
uniform vec2 bumpMapTiling;
uniform vec4 specularColor;
uniform float shininess;

in vec3 lightDirection;
in vec3 viewerDirection;
in vec3 vertexPosition;
in vec2 vertexUV;

void main()
{
	vec4 diffuseColor = texture2D(colorMap, (vertexUV * colorMapTiling));
	vec3 normal = normalize(texture2D(bumpMap, (vertexUV * bumpMapTiling)).rgb * 2.0 - 1.0);

	gl_FragColor = BlinnPhongLighting(vec4(1.0),
									  diffuseColor,
									  specularColor,
									  shininess,
									  lightDirection,
									  viewerDirection,
									  vertexPosition,
									  normal);
}