#version 330

uniform mat4 _ModelViewProjection;

in vec4 position;
in vec4 color;

uniform float size;

out vec4 vertexColor;

void main()
{
	vertexColor = color;
	gl_PointSize = size;
	gl_Position = _ModelViewProjection * position;
}