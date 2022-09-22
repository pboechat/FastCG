#version 330

uniform mat4 _ModelView;
uniform mat3 _ModelViewInverseTranspose;
uniform mat4 _ModelViewProjection;

in vec4 position;
in vec3 normal;
in vec4 tangent;
in vec2 uv;

out vec3 vertexNormal;
out vec4 vertexTangent;
out vec2 vertexUV;

void main()
{
	vertexNormal = normalize(_ModelViewInverseTranspose * normal);
	vertexTangent = vec4(normalize(_ModelViewInverseTranspose * tangent.xyz), tangent.w);
	vertexUV = uv;
	gl_Position = _ModelViewProjection * position;
}