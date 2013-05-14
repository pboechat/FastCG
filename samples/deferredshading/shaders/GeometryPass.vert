#version 330

uniform mat3 _ModelViewInverseTranspose;
uniform mat4 _ModelViewProjection;

in vec4 position;
in vec3 normal;
in vec2 uv;

out vec4 vertexPosition;
out vec4 vertexNormal;
out vec4 vertexTextureCoordinates;

void main()
{
	vertexPosition = _ModelViewProjection * position;
	vertexNormal = vec4(normalize(_ModelViewInverseTranspose * normal), 0.0);
	vertexTextureCoordinates = vec4(uv, 0.0, 0.0);
	gl_Position = vertexPosition;
}