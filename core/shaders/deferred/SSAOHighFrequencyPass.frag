#version 330

#include "FastCG.glsl"

#define NOISE_TEXTURE_WIDTH 4
#define NOISE_TEXTURE_HEIGHT 4
#define NUMBER_OF_RANDOM_SAMPLES 30

in vec2 vertexUv;
noperspective in vec3 viewRay;

uniform sampler2D _NoiseMap;
uniform float _Radius;
uniform vec3 _RandomSamples[NUMBER_OF_RANDOM_SAMPLES];

void main()
{
	float depth = texture2D(_DepthMap, vertexUv).x;
	vec3 position = GetPositionFromWindowCoordinates(vec3(gl_FragCoord.xy, depth));
	depth = LinearizeDepth(depth);

	vec2 noiseUv = vec2(textureSize(_NormalMap, 0)) / vec2(textureSize(_NoiseMap, 0)) * vertexUv;
	vec3 randomVector = texture2D(_NoiseMap, noiseUv).xyz * 2.0 - 1.0;

	vec3 normal = UnpackNormal(texture2D(_NormalMap, vertexUv));
	vec3 tangent = normalize(randomVector - normal * dot(randomVector, normal)); // gram-schmidt orthonormalization
	vec3 binormal = cross(normal, tangent);
	mat3 tangentSpaceMatrix = mat3(tangent, binormal, normal);

	float occlusion = 0;
	for (int i = 0; i < NUMBER_OF_RANDOM_SAMPLES; i++)
	{
		vec3 samplePosition = position + (tangentSpaceMatrix * _RandomSamples[i]) * _Radius;

		vec4 sampleUv = _Projection * vec4(samplePosition, 1.0);
		sampleUv.xy /= sampleUv.w;
		sampleUv.xy = sampleUv.xy * 0.5 + 0.5;

		float sampleDepth = LinearizeDepth(texture2D(_DepthMap, sampleUv).x);
		float rangeCheck = smoothstep(0.0, 1.0, _Radius / abs(sampleDepth - depth));
		occlusion += rangeCheck * step(depth, sampleDepth);

		/*if (sampleDepth == 1.0)
		{
			occlusion++;
		}
		else
		{		
			float decay = 29000 * max(depth - sampleDepth, 0.0f);
			occlusion  += 1.0f / (1.0f + decay * decay * 0.1);
		}*/
	}
	occlusion /= NUMBER_OF_RANDOM_SAMPLES;

	gl_FragColor = vec4(occlusion, occlusion, occlusion, 1.0);
}