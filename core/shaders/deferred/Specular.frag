#version 330

uniform sampler2D colorMap;
uniform vec4 diffuseColor;
uniform vec4 specularColor;
uniform float shininess;

in vec4 vertexPosition;
in vec3 vertexNormal;
in vec2 vertexUV;

void main()
{
	gl_FragData[0] = vertexPosition;
	gl_FragData[1] = diffuseColor * texture2D(colorMap, vertexUV);
	gl_FragData[2] = vec4(vertexNormal, 0.0);
	gl_FragData[3] = specularColor;
}