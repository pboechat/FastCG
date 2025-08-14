#include "../FastCG.glsl"
#include "Tonemap.glsl"

layout(BINDING_0_0) uniform sampler2D uSource;

layout(location = 0) in vec2 vUV;

layout(location = 0) out vec4 oDestination;

void main()
{
    vec4 color = texture(uSource, vUV);
	oDestination = vec4(CheapReinhard(color.rgb), color.a);
}