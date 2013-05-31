#version 330

uniform vec4 diffuseColor;
uniform vec4 specularColor;
uniform float shininess;

in vec4 vertexPosition;
in vec3 vertexNormal;

void main()
{
	gl_FragData[0] = vertexPosition;
	gl_FragData[1] = diffuseColor;
	gl_FragData[2] = vec4(vertexNormal, shininess);
	gl_FragData[3] = specularColor;
}