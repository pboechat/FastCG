#version 330

#include "FastCG.glsl"

uniform sampler2D colorMap;
uniform vec2 colorMapTiling;
uniform vec4 diffuseColor;

in vec4 vertexPosition;
in vec3 vertexNormal;
in vec2 vertexUV;

layout(location = 0) out vec4 diffuse;
layout(location = 1) out vec4 normal;

void main()
{
	diffuse = diffuseColor * texture2D(colorMap, vertexUV * colorMapTiling);
	normal = vec4(PackNormalToColor(vertexNormal), 0.0);
}