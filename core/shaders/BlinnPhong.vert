#include "shaders/FastCG.glsl"

in vec4 position;
in vec3 normal;
in vec2 uv;

out vec4 eyeSpacePosition;
out vec3 vertexNormal;

void main()
{
	vertexNormal = normalize(_ModelViewInverseTranspose * normal);
	eyeSpacePosition = _ModelView * position;
	gl_Position = _ModelViewProjection * position;
}