#version 330

#include "FastCG.glsl"
#include "../Lighting.glsl"

uniform sampler2D colorMap;
uniform sampler2D bumpMap;
uniform vec2 colorMapTiling;
uniform vec2 bumpMapTiling;
uniform vec4 diffuseColor;
uniform vec4 specularColor;
uniform float shininess;

in vec3 lightDirection;
in vec3 viewerDirection;
in vec3 vertexPosition;
in vec2 vertexUV;

void main()
{
	vec4 finalDiffuseColor = diffuseColor * texture2D(colorMap, (vertexUV * colorMapTiling));

	vec3 normal = UnpackNormalFromColor(texture2D(bumpMap, (vertexUV * bumpMapTiling))).xyz;

	gl_FragColor = Lighting(finalDiffuseColor, specularColor, shininess, lightDirection, viewerDirection, vertexPosition, normal);
}