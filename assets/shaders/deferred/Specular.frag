#version 330

uniform sampler2D colorMap;
uniform vec2 colorMapTiling;
uniform vec4 diffuseColor;
uniform vec4 specularColor;
uniform float shininess;

in vec3 vertexNormal;
in vec2 vertexUV;

void main()
{
	gl_FragData[0] = diffuseColor * texture2D(colorMap, vertexUV * colorMapTiling);
	gl_FragData[1] = vec4((vertexNormal.xyz + 1.0) * 0.5, 0.0);
	gl_FragData[2] = vec4(specularColor.xyz, shininess);
}