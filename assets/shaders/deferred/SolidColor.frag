#version 330

#include "FastCG.glsl"

uniform vec4 diffuseColor;
uniform vec4 specularColor;
uniform float shininess;

in vec3 vertexNormal;

void main()
{
	gl_FragData[0] = diffuseColor;
	gl_FragData[1] = vec4(PackNormalToColor(vertexNormal), 0.0);
	gl_FragData[2] = vec4(specularColor.xyz, shininess);
	gl_FragData[3] = vec4(0.0);
}