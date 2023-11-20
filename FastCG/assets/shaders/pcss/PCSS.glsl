#ifndef FASTCG_PCSS_PCSS_GLSL
#define FASTCG_PCSS_PCSS_GLSL

#include "../FastCG.glsl"
#include "../Shadow.glsl"
#include "../Noise.glsl"

// based on: https://developer.download.nvidia.com/whitepapers/2008/PCSS_Integration.pdf

struct PCSSData
{
    ShadowMapData shadowMapData;
	float uvScale;
	float nearClip;
	int blockerSearchSamples;
	int pcfSamples;
};

layout(BINDING_0_3) uniform PCSSConstants 
{
    PCSSData uPCSSData;
};

void FindBlocker(PCSSData pcssData, sampler2D shadowMap, vec3 shadowMapCoords, out float avgBlockerDistance, out int numBlockers)
{
	numBlockers = 0;
	float blockerDistanceSum = 0.0;
    float zThreshold = shadowMapCoords.z - pcssData.shadowMapData.bias;
	for (int i = 0; i < pcssData.blockerSearchSamples; i++)
	{
		float z = texture(shadowMap, shadowMapCoords.xy + PoissonDiskSample(float(i) / float(pcssData.blockerSearchSamples)) * pcssData.uvScale).x;
		if (z < zThreshold)
		{
			blockerDistanceSum += z;
			numBlockers++;
		}
	}
	avgBlockerDistance = blockerDistanceSum / float(numBlockers);
}

float PCF(PCSSData pcssData, sampler2D shadowMap, vec3 shadowMapCoords, float uvRadius)
{
	float sum = 0.0;
    float zThreshold = shadowMapCoords.z - pcssData.shadowMapData.bias;
	for (int i = 0; i < pcssData.pcfSamples; i++)
	{
		float z = texture(shadowMap, shadowMapCoords.xy + PoissonDiskSample(float(i) / float(pcssData.pcfSamples)) * uvRadius).x;
		sum += float(z <= zThreshold);
	}
	return sum / float(pcssData.pcfSamples);
}

float GetPCSS(PCSSData pcssData, sampler2D shadowMap, vec3 worldPosition)
{
    vec3 shadowMapCoords = GetShadowMapCoordinates(pcssData.shadowMapData, worldPosition);

    // blocker search
	float avgBlockerDistance;
	int numBlockers;
	FindBlocker(pcssData, shadowMap, shadowMapCoords, avgBlockerDistance, numBlockers);
	if (numBlockers < 1)
	{
		return 1.0;
	}

    // penumbra estimation
	float penumbraWidth = (shadowMapCoords.z - avgBlockerDistance) / avgBlockerDistance;

	// percentage-close filtering
	float uvRadius = penumbraWidth * pcssData.uvScale;
	return 1.0 - PCF(pcssData, shadowMap, shadowMapCoords, uvRadius);
}

// for debugging purposes
float GetPCSS_HardShadow(PCSSData pcssData, sampler2D shadowMap, vec3 worldPosition)
{
	return GetHardShadow(pcssData.shadowMapData, shadowMap, worldPosition);
}

#ifdef GetShadow
#undef GetShadow
#endif
#define GetShadow GetPCSS

#endif