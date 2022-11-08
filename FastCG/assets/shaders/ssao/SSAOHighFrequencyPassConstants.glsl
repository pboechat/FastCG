#ifndef FASTCG_SSAO_SSAO_HIGH_FREQUENCY_PASS_CONSTANTS_GLSL
#define FASTCG_SSAO_SSAO_HIGH_FREQUENCY_PASS_CONSTANTS_GLSL

#include "../FastCG.glsl"

#define NOISE_TEXTURE_WIDTH 4
#define NOISE_TEXTURE_HEIGHT NOISE_TEXTURE_WIDTH
#define NOISE_TEXTURE_SIZE NOISE_TEXTURE_WIDTH * NOISE_TEXTURE_HEIGHT
#define NUMBER_OF_RANDOM_SAMPLES 32

layout(BINDING(0, 0)) uniform SSAOHighFrequencyPassConstants
{
	mat4 uProjection;
    vec4 uRandomSamples[NUMBER_OF_RANDOM_SAMPLES];
	float uRadius;
	float uDistanceScale;
	float uAspectRatio;
	float uTanHalfFov;
};
layout(BINDING(0, 0)) uniform sampler2D uNoiseMap;
layout(BINDING(0, 2)) uniform sampler2D uDepth;

#endif