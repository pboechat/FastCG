#version 430 core

#ifdef VULKAN
#extension GL_GOOGLE_include_directive : enable
#endif

#include "FastCG.glsl"
#include "SceneConstants.glsl"
#include "SSAOHighFrequencyPassConstants.glsl"

layout(location = 0) in vec2 vUV;
layout(location = 1) noperspective in vec3 vViewRay;

layout(location = 0) out vec4 oAmbientOcclusion;

void main()
{
	float depth = LinearizeDepth(uProjection, texture(uDepth, vUV).x);
	vec3 position = vViewRay * depth;

	vec2 noiseUv = vec2(textureSize(uNormalMap, 0)) / vec2(textureSize(uNoiseMap, 0)) * vUV;
	vec3 randomVector = UnpackNormalFromColor(texture(uNoiseMap, noiseUv).xyz);

	vec3 normal = UnpackNormalFromColor(texture(uNormalMap, vUV)).xyz;
	vec3 tangent = normalize(randomVector - normal * dot(randomVector, normal)); // gram-schmidt orthonormalization
	vec3 binormal = cross(normal, tangent);
	mat3 tangentSpaceMatrix = mat3(tangent, binormal, normal);

	float occlusion = 0;
	for (int i = 0; i < NUMBER_OF_RANDOM_SAMPLES; i++)
	{
		vec3 samplePosition = (tangentSpaceMatrix * uRandomSamples[i].xyz) * uRadius + position;

		vec4 sampleUv = uProjection * vec4(samplePosition, 1.0);
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

	oAmbientOcclusion = vec4(occlusion, occlusion, occlusion, 1.0);
}