#version 330

#include "FastCG.glsl"
#include "../Lighting.glsl"

uniform sampler2D colorMap;
uniform sampler2D bumpMap;
uniform vec2 colorMapTiling;
uniform vec2 bumpMapTiling;
uniform vec4 diffuseColor;

in vec3 lightDirection;
in vec3 vertexPosition;
in vec2 vertexUV;

layout(location = 0) out vec4 color;

void main()
{
	vec4 finalDiffuseColor = diffuseColor * texture2D(colorMap, (vertexUV * colorMapTiling));

	vec3 normal = UnpackNormalFromColor(texture2D(bumpMap, (vertexUV * bumpMapTiling))).xyz;

	color = Lighting(finalDiffuseColor, lightDirection, vertexPosition, normal);
}