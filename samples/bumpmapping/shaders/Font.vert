#version 330

uniform mat4 _ModelViewProjection;

in vec4 position;
in vec3 normal;
in vec2 uv;

out vec2 textureCoordinates;

void main()
{
	textureCoordinates = uv;
	gl_Position = _ModelViewProjection * position;
}