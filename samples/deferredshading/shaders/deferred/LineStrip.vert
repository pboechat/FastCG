#version 330

uniform mat4 _ModelViewProjection;

in vec4 position;
in vec4 color;

out vec4 vertexColor;

void main()
{
	vertexColor = color;
	gl_Position = _ModelViewProjection * position;
}