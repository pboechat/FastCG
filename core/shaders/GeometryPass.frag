#version 330

uniform sampler2D colorMap;

in vec4 vertexPosition;
in vec4 vertexNormal;
in vec4 vertexTextureCoordinates;

void main()
{
	gl_FragData[0] = vertexPosition;
	gl_FragData[1] = texture2D(colorMap, vertexTextureCoordinates);
	gl_FragData[2] = vertexNormal;
	gl_FragData[3] = vertexTextureCoordinates;
}