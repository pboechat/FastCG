#ifndef FASTCG_DEFERRED_SSAO_HIGH_FREQUENCY_PASS_CONSTANTS_GLSL
#define FASTCG_DEFERRED_SSAO_HIGH_FREQUENCY_PASS_CONSTANTS_GLSL

#include "FastCG.glsl"

#define NOISE_TEXTURE_WIDTH 4
#define NOISE_TEXTURE_HEIGHT NOISE_TEXTURE_WIDTH
#define NOISE_TEXTURE_SIZE NOISE_TEXTURE_WIDTH * NOISE_TEXTURE_HEIGHT
#define NUMBER_OF_RANDOM_SAMPLES 32

layout(BINDING(1, 0)) uniform SSAOHighFrequencyPassConstants
{
    vec4 uRandomSamples[NUMBER_OF_RANDOM_SAMPLES];
	float uRadius;
	float uDistanceScale;
};
layout(BINDING(0, 0)) uniform sampler2D uNoiseMap;
layout(BINDING(0, 1)) uniform sampler2D uNormalMap;
layout(BINDING(0, 2)) uniform sampler2D uDepth;

#endif