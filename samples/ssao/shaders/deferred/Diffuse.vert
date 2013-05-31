#version 330

uniform mat4 _ModelView;
uniform mat3 _ModelViewInverseTranspose;
uniform mat4 _ModelViewProjection;

in vec4 position;
in vec3 normal;
in vec2 uv;

out vec4 vertexPosition;
out vec3 vertexNormal;
out vec2 vertexUV;

void main()
{
	vertexPosition = _ModelView * position;
	vertexNormal = normalize(_ModelViewInverseTranspose * normal);
	vertexUV = uv;
	gl_Position = _ModelViewProjection * position;
}