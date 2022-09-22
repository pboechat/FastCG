#version 330

#include "FastCG.glsl"

uniform vec4 diffuseColor;
uniform vec4 specularColor;
uniform float shininess;

in vec3 vertexNormal;

layout(location = 0) out vec4 diffuse;
layout(location = 1) out vec4 normal;
layout(location = 2) out vec4 specular;

void main()
{
	diffuse = diffuseColor;
	normal = vec4(PackNormalToColor(vertexNormal), 0.0);
	specular = vec4(specularColor.xyz, shininess);
}