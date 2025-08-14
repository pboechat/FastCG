#include "FastCG.glsl"
#include "Instance.glsl"

layout(location = 0) in vec3 iPosition;

layout(location = 0) out vec3 vRayDir;

void main()
{
    vRayDir = iPosition;
	gl_Position = GetInstanceData().modelViewProjection * vec4(iPosition, 1);
}