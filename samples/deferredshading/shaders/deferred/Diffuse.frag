#version 330

uniform sampler2D colorMap;
uniform vec4 diffuseColor;

in vec4 vertexPosition;
in vec4 vertexNormal;
in vec4 vertexUV;

void main()
{
	gl_FragData[0] = vertexPosition;
	gl_FragData[1] = diffuseColor * texture2D(colorMap, vertexUV.st);
	gl_FragData[2] = vertexNormal;
	gl_FragData[3] = vec4(0.0);
}