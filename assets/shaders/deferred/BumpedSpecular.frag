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

layout(location = 0) out vec4 diffuse;
layout(location = 1) out vec4 normal;
layout(location = 2) out vec4 specular;
layout(location = 3) out vec4 tangent;
layout(location = 4) out vec4 extraData;

void main()
{
	diffuse = diffuseColor * texture2D(colorMap, vertexUV * colorMapTiling);

	vec3 bump = texture2D(bumpMap, vertexUV * bumpMapTiling).xyz;

	normal = vec4(PackNormalToColor(vertexNormal), 0.0);
	specular = vec4(specularColor.xyz, shininess);
	tangent = PackNormalToColor(vertexTangent);
	extraData = vec4(bump, 0.0);
}