#ifndef FASTCG_AMBIENT_OCCLUSION_GLSL
#define FASTCG_AMBIENT_OCCLUSION_GLSL

float _GetAmbientOcclusion(sampler2D ambientOcclusionMap, vec2 screenCoords)
{
	return min(texture(ambientOcclusionMap, screenCoords).x, 1.0);
}

#define GetAmbientOcclusion _GetAmbientOcclusion

#endif