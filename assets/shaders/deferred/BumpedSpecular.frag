#version 330

#include "FastCG.glsl"

uniform sampler2D colorMap;
uniform sampler2D bumpMap;
uniform vec2 colorMapTiling;
uniform vec2 bumpMapTiling;
uniform vec4 diffuseColor;
uniform vec4 specularColor;
uniform float shininess;

in vec3 vertexNormal;
in vec4 vertexTangent;
in vec2 vertexUV;

void main()
{
	gl_FragData[0] = diffuseColor * texture2D(colorMap, vertexUV * colorMapTiling);

	vec3 normal = texture2D(bumpMap, vertexUV * bumpMapTiling).xyz;

	gl_FragData[1] = vec4(PackNormalToColor(vertexNormal), 0.0);
	gl_FragData[2] = vec4(specularColor.xyz, shininess);
	gl_FragData[3] = PackNormalToColor(vertexTangent);
	gl_FragData[4] = vec4(normal, 0.0);
}