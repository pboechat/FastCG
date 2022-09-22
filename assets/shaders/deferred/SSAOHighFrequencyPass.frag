#version 330

#include "FastCG.glsl"

#define NOISE_TEXTURE_WIDTH 4
#define NOISE_TEXTURE_HEIGHT 4
#define NUMBER_OF_RANDOM_SAMPLES 30

in vec2 uv;
noperspective in vec3 viewRay;

uniform sampler2D _NoiseMap;
uniform float _Radius;
uniform float _DistanceScale;
uniform vec3 _RandomSamples[NUMBER_OF_RANDOM_SAMPLES];

layout(location = 0) out vec4 ambientOcclusion;

void main()
{
	float depth = LinearizeDepth(texture2D(_Depth, uv).x);
	vec3 position = viewRay * depth;

	vec2 noiseUv = vec2(textureSize(_NormalMap, 0)) / vec2(textureSize(_NoiseMap, 0)) * uv;
	vec3 randomVector = texture2D(_NoiseMap, noiseUv).xyz * 2.0 - 1.0;

	vec3 normal = UnpackNormalFromColor(texture2D(_NormalMap, uv)).xyz;
	vec3 tangent = normalize(randomVector - normal * dot(randomVector, normal)); // gram-schmidt orthonormalization
	vec3 binormal = cross(normal, tangent);
	mat3 tangentSpaceMatrix = mat3(tangent, binormal, normal);

	float occlusion = 0;
	for (int i = 0; i < NUMBER_OF_RANDOM_SAMPLES; i++)
	{
		vec3 samplePosition = (tangentSpaceMatrix * _RandomSamples[i]) * _Radius + position;

		vec4 sampleUv = _Projection * vec4(samplePosition, 1.0);
		sampleUv.xy /= sampleUv.w;
		sampleUv.xy = sampleUv.xy * 0.5 + 0.5;

		float sampleDepth = LinearizeDepth(texture2D(_Depth, sampleUv.xy).x);

		if (sampleDepth == 1.0)
		{
			occlusion++;
		}
		else
		{		
			float rangeCheck = _DistanceScale * max(depth - sampleDepth, 0.0f);
			occlusion  += 1.0f / (1.0f + rangeCheck * rangeCheck * 0.1);
		}
	}
	occlusion /= NUMBER_OF_RANDOM_SAMPLES;

	ambientOcclusion = vec4(occlusion, occlusion, occlusion, 1.0);
}