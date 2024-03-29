#include "../FastCG.glsl"
#include "ShadowMapPass.glsl"

layout(location = 0) in vec3 iPosition;

void main()
{
    gl_Position = GetShadowMapPassInstanceData().modelViewProjection * vec4(iPosition, 1);
}