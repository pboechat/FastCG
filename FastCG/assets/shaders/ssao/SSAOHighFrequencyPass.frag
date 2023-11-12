#include "../FastCG.glsl"
#include "SSAOHighFrequencyPass.glsl" 

layout(BINDING_0_1) uniform sampler2D uNoiseMap;
layout(BINDING_0_2) uniform sampler2D uDepth;

layout(location = 0) in vec2 vUV;
layout(location = 1) in vec3 vViewRay;

layout(location = 0) out float oAmbientOcclusion;

// based on: https://learnopengl.com/Advanced-Lighting/SSAO

void main()
{
	vec2 noiseUv = vec2(textureSize(uDepth, 0) / textureSize(uNoiseMap, 0)) * vUV;
	vec3 randomVector = texture(uNoiseMap, noiseUv).xyz;

	float z = GetViewSpaceZ(uProjection, texture(uDepth, vUV).x);

	vec3 viewPos = vViewRay * z;

	// estimate the view position of neighboring fragments 
	// (doesn't take into account discontinuities)
	vec3 viewPosRight = dFdx(viewPos);
	vec3 viewPosBottom = dFdy(viewPos);

	// determine the sampling surface normal with a cross product 
	// between the edges of the triangle formed by the world position 
	// of the current and neighbouring fragments
	vec3 normal = normalize(cross(viewPosRight, viewPosBottom));

#ifdef VULKAN
	// FIXME:
	normal *= -1.0;
#endif

	// compute the sampling tangent space matrix from normal, 
	// random vector and their cross product (gram-schmidt orthonormalization)
	vec3 tangent = normalize(randomVector - normal * dot(randomVector, normal));
	vec3 binormal = cross(normal, tangent);
	mat3 tangentSpaceMatrix = mat3(tangent, binormal, normal);

	float occlusion = 0.0;
	for (int i = 0; i < NUMBER_OF_RANDOM_SAMPLES; i++)
	{
		vec3 sampleViewPos = viewPos + (tangentSpaceMatrix * uRandomSamples[i].xyz) * uAspectRatio;

		vec4 sampleClipPos = uProjection * vec4(sampleViewPos, 1.0);

		vec2 sampleUv = sampleClipPos.xy / sampleClipPos.w;
		sampleUv.x = sampleUv.x * 0.5 + 0.5;
#ifdef VULKAN
		sampleUv.y = sampleUv.y * -0.5 + 0.5;
#else
		sampleUv.y = sampleUv.y * 0.5 + 0.5;
#endif

		float sampleZ = GetViewSpaceZ(uProjection, texture(uDepth, sampleUv.xy).x);
		
		vec3 actualViewPos = normalize(sampleViewPos) * sampleZ;

		float rangeCheck = smoothstep(0.0, 1.0, uAspectRatio / abs(viewPos.z - actualViewPos.z));
		occlusion += (actualViewPos.z <= sampleViewPos.z - uBias ? 1.0 : 0.0) * rangeCheck;  
	}

	oAmbientOcclusion = 1.0 - (occlusion / float(NUMBER_OF_RANDOM_SAMPLES));
}