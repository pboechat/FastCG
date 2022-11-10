#ifndef FASTCG_PCSS_PCSS_GLSL
#define FASTCG_PCSS_PCSS_GLSL

#include "../FastCG.glsl"
#include "../Shadow.glsl"
#include "../Noise.glsl"

// Based on:
// hhttps://developer.download.nvidia.com/whitepapers/2008/PCSS_Integration.pdf

#define SCALE 0.133333

struct PCSSData
{
    ShadowMapData shadowMapData;
	float near;
	int blockerSearchSamples;
	int pcfSamples;
};

float SearchWidth(PCSSData pcssData, float receiverDistance)
{
	return SCALE * (receiverDistance - pcssData.near) / receiverDistance;
}

void FindBlocker(PCSSData pcssData, sampler2D shadowMap, vec3 shadowMapCoords, out float avgBlockerDistance, out int numBlockers)
{
	numBlockers = 0;
	float blockerSum = 0;
	float searchWidth = SearchWidth(pcssData, shadowMapCoords.z);
    float zThreshold = shadowMapCoords.z - pcssData.shadowMapData.bias;
	for (int i = 0; i < pcssData.blockerSearchSamples; i++)
	{
		float z = texture(shadowMap, shadowMapCoords.xy + PoissonDiskSample(i / float(pcssData.blockerSearchSamples)) * searchWidth).x;
		if (z < zThreshold)
		{
			blockerSum += z;
			numBlockers++;
		}
	}
	avgBlockerDistance = blockerSum / numBlockers;
}

float PCF(PCSSData pcssData, sampler2D shadowMap, vec3 shadowMapCoords, float uvRadius)
{
	float sum = 0;
    float zThreshold = shadowMapCoords.z - pcssData.shadowMapData.bias;
	for (int i = 0; i < pcssData.pcfSamples; i++)
	{
		float z = texture(shadowMap, shadowMapCoords.xy + PoissonDiskSample(i / float(pcssData.pcfSamples)) * uvRadius).x;
		sum += float(z <= zThreshold);
	}
	return sum / pcssData.pcfSamples;
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
		return 1;
	}

    // penumbra estimation
	float penumbraWidth = (shadowMapCoords.z - avgBlockerDistance) / avgBlockerDistance;

	// percentage-close filtering
	float uvRadius = penumbraWidth * SCALE * pcssData.near / shadowMapCoords.z;
	return 1 - PCF(pcssData, shadowMap, shadowMapCoords, uvRadius);
}

#ifdef GetShadow
#undef GetShadow
#endif
#define GetShadow GetPCSS

#endif