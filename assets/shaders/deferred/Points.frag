#version 330

in vec4 vertexColor;

layout(location = 0) out vec4 color;

void main()
{
	color = vertexColor;
}