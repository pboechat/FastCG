#include "FastCG.glsl"

layout (location = 0) in vec3 vRayDir;

layout(location = 0) out vec4 oColor;

layout(BINDING_1_0) uniform samplerCube uSkybox;

void main()
{    
    oColor = texture(uSkybox, vRayDir);
}