#version 430 core

#ifdef ENABLE_INCLUDE_EXTENSION_DIRECTIVE
#extension GL_GOOGLE_include_directive : enable 
#endif

#define NOISE_TEXTURE_SIDE 4
#define NOISE_TEXTURE_SIZE 16

#include "../FastCG.glsl"

layout(BINDING_0_0) uniform sampler2D uAmbientOcclusionMap;

layout(location = 0) in vec2 vUV;

layout(location = 0) out float oAmbientOcclusion;

void main()
{
	vec2 texelSize = 1.0 / textureSize(uAmbientOcclusionMap, 0);

	float result = 0;
	vec2 stride = vec2(-NOISE_TEXTURE_SIDE * 0.5);
	for (int x = 0; x < NOISE_TEXTURE_SIDE; x++) 
	{
		for (int y = 0; y < NOISE_TEXTURE_SIDE; y++) 
		{
			vec2 uvOffset = vec2(float(x), float(y));
			uvOffset += stride;
			uvOffset *= texelSize;

			result += texture(uAmbientOcclusionMap, vUV + uvOffset).x;
		}
	}

	oAmbientOcclusion = result / NOISE_TEXTURE_SIZE;
}