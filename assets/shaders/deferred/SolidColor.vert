#version 330

uniform mat4 _ModelView;
uniform mat3 _ModelViewInverseTranspose;
uniform mat4 _ModelViewProjection;

in vec4 position;
in vec3 normal;
in vec2 uv;

out vec3 vertexNormal;

void main()
{
	vertexNormal = normalize(_ModelViewInverseTranspose * normal);
	gl_Position = _ModelViewProjection * position;
}