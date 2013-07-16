#version 330

#include "FastCG.glsl"

#define NOISE_TEXTURE_WIDTH 4
#define NOISE_TEXTURE_HEIGHT 4
#define NUMBER_OF_RANDOM_SAMPLES 30

uniform sampler2D _NoiseMap;
uniform float _RayLength;
uniform float _OcclusionExponent;
uniform vec3 _RandomSamples[NUMBER_OF_RANDOM_SAMPLES];

void main()
{
	vec2 uv = gl_FragCoord.xy / _ScreenSize;

	float depth = texture2D(_DepthMap, uv).x;

	vec3 position = GetPositionFromDepth(depth);

	vec2 noiseTexCoords = vec2(textureSize(_NormalMap, 0)) / vec2(textureSize(_NoiseMap, 0));
	noiseTexCoords *= uv;
	vec3 rvec = texture2D(_NoiseMap, noiseTexCoords).xyz * 2.0 - 1.0;

	vec3 normal = UnpackNormal(texture2D(_NormalMap, uv));
	vec3 tangent = normalize(rvec - normal * dot(rvec, normal));
	vec3 binormal = cross(normal, tangent);

	mat3 tangentSpaceMatrix = mat3(tangent, binormal, normal);

	float occlusion = 0;
	for (int i = 0; i < NUMBER_OF_RANDOM_SAMPLES; i++)
	{
		vec3 randomSample = tangentSpaceMatrix * _RandomSamples[i];
		randomSample = position + (randomSample * _RayLength);

		vec4 offset = _Projection * vec4(randomSample, 1.0);
		offset.xy /= offset.w;
		offset.xy = offset.xy * 0.5 + 0.5;

		float sampleDepth = LinearizeDepth(texture2D(_DepthMap, offset.xy).x);
		float rangeCheck = smoothstep(0.0, 1.0, _RayLength / abs(position.z - sampleDepth));
		occlusion += rangeCheck * step(sampleDepth, randomSample.z);
	}

	occlusion = 1.0 - (occlusion / NUMBER_OF_RANDOM_SAMPLES);

	gl_FragColor = vec4(pow(occlusion, _OcclusionExponent));
}