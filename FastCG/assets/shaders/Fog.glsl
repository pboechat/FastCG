#ifndef FASTCG_FOG_H
#define FASTCG_FOG_H

#include "FastCG.glsl"

#define FOG_MODE_NONE 0
#define FOG_MODE_LINEAR 1
#define FOG_MODE_EXP 2
#define FOG_MODE_EXP2 3

struct FogData
{
	vec4 color;
    uint mode;
	float density;
	float start;
	float end;
};

layout(BINDING_0_2) uniform FogConstants
{
	FogData uFogData;
};

// based on: https://registry.khronos.org/OpenGL-Refpages/gl2.1/xhtml/glFog.xml

float GetLinearFogFactor(float viewDistance)
{
    // (end - c) / (end - start)
    return (uFogData.end - viewDistance) / (uFogData.end - uFogData.start);
}

float GetExpFogFactor(float viewDistance)
{
    float c =  viewDistance / (uFogData.end - uFogData.start);
    // e^(-density*c)
    return exp(-uFogData.density * c);
}

float GetExp2FogFactor(float viewDistance)
{
    float c =  viewDistance / (uFogData.end - uFogData.start);
    // e^(-density*c^2)
    return exp(-uFogData.density * (c * c));
}

vec4 ApplyFog(float viewDistance, vec4 fragColor)
{
    float f = mix(
        clamp(
            mix(
                mix(
                    GetExp2FogFactor(viewDistance), // exp2 f
                    GetExpFogFactor(viewDistance), // exp f
                    float(uFogData.mode == FOG_MODE_EXP) // exp f if uFogData.mode == FOG_MODE_EXP
                ),
                GetLinearFogFactor(viewDistance), // linear f
                float(uFogData.mode == FOG_MODE_LINEAR) // linear f if uFogData.mode == FOG_MODE_LINEAR
            ), 
            0.0, 
            1.0
        ), 
        1.0, // f = 1
        float(uFogData.mode == FOG_MODE_NONE) // f == 1.0 if uFogData.mode == FOG_MODE_NONE
    );
    return vec4(
        mix(
            uFogData.color.rgb, 
            fragColor.rgb, 
            f
        ), // (0=frag, 1=fog)
        fragColor.a
    );
}

#endif