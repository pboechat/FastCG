#include "FastCG.glsl"
#include "ImGui.glsl"

layout(location = 0) in vec2 iPosition;
layout(location = 1) in vec2 iUV;
layout(location = 2) in vec4 iColor;

layout(location = 0) out vec2 vUV;
layout(location = 1) out vec4 vColor;

void main()
{
    vUV = iUV;
    vColor = iColor;
    gl_Position = uProjection * vec4(iPosition, 0, 1);
}