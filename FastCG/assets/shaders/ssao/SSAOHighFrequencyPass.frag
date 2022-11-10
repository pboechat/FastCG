#version 430 core

#ifdef VULKAN
#extension GL_GOOGLE_include_directive : enable
#endif

#include "../FastCG.glsl"
#include "SSAOHighFrequencyPass.glsl"

layout(location = 0) in vec2 vUV;
layout(location = 1) noperspective in vec3 vViewRay;

layout(location = 0) out float oAmbientOcclusion;

void main()
{
	// fetch a random vector
	vec2 noiseUv = vec2(textureSize(uDepth, 0)) / vec2(textureSize(uNoiseMap, 0)) * vUV;
	vec3 randomVector = UnpackNormalFromColor(texture(uNoiseMap, noiseUv).xyz);

	// fetch depth for the current fragment and linearize it
	float depth = LinearizeDepth(uProjection, texture(uDepth, vUV).x);

	// reconstruct the fragment world position from the linearized depth and the view ray
	vec3 worldPosition = vViewRay * depth;

	// get the reconstructed world position from the neighboring fragments
	vec3 worldPositionRight = dFdx(worldPosition);
	vec3 worldPositionBottom = dFdy(worldPosition);

	// determine the sampling surface normal with a cross product 
	// between the edges of the triangle formed by the world position 
	// of the current and neighbouring fragments
	vec3 normal = normalize(cross(worldPositionRight - worldPosition, worldPositionBottom - worldPosition));

	// compute the sampling tangent space matrix from normal, random vector and their cross product (gram-schmidt orthonormalization)
	vec3 tangent = normalize(randomVector - normal * dot(randomVector, normal));
	vec3 binormal = cross(normal, tangent);
	mat3 tangentSpaceMatrix = mat3(tangent, binormal, normal);

	// compute occlusion from sampling the depth buffer at pseudo-random offsets from the reconstructed worldPosition
	float occlusion = 0;
	for (int i = 0; i < NUMBER_OF_RANDOM_SAMPLES; i++)
	{
		vec3 sampleworldPosition = (tangentSpaceMatrix * uRandomSamples[i].xyz) * uRadius + worldPosition;

		vec4 sampleUv = uProjection * vec4(sampleworldPosition, 1.0);
		sampleUv.xy /= sampleUv.w;
		sampleUv.xy = sampleUv.xy * 0.5 + 0.5;

		float sampleDepth = LinearizeDepth(uProjection, texture(uDepth, sampleUv.xy).x);

		if (sampleDepth == 1.0)
		{
			occlusion++;
		}
		else
		{		
			float rangeCheck = uDistanceScale * max(depth - sampleDepth, 0.0f);
			occlusion += 1.0f / (1.0f + rangeCheck * rangeCheck * 0.1);
		}
	}
	occlusion /= NUMBER_OF_RANDOM_SAMPLES;

	oAmbientOcclusion = occlusion;
}