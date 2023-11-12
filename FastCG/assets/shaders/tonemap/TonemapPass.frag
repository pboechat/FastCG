#include "../FastCG.glsl"

layout(BINDING_0_0) uniform sampler2D uSource;

layout(location = 0) in vec2 vUV;

layout(location = 0) out vec4 oDestination;

void main()
{
    vec4 color = texture(uSource, vUV);

    // reinhard tone mapping
    vec3 mappedColor = color.rgb / (color.rgb + vec3(1.0));

	oDestination = vec4(mappedColor, color.a);
}