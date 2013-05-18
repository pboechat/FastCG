#version 330

uniform mat4 _Model;
uniform mat4 _ModelViewProjection;

in vec4 position;
in vec3 normal;
in vec2 uv;

out vec4 vertexPosition;
out vec4 vertexNormal;
out vec4 vertexUV;

void main()
{
	vertexPosition = _Model * position;
	vertexNormal = normalize(_Model * vec4(normal, 1.0));
	vertexUV = vec4(uv, 0.0, 0.0);
	gl_Position = _ModelViewProjection * position;
}