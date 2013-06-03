#version 330

#define NUMBER_OF_RANDOM_SAMPLES 30

uniform mat4 _Projection;
uniform mat4 _InverseProjection;
uniform vec2 _ScreenSize;
uniform sampler2D _NormalMap;
uniform sampler2D _DepthMap;
uniform sampler2D _NoiseMap;
uniform float _RayLength;
uniform vec3 _RandomSamplesInAHemisphere[NUMBER_OF_RANDOM_SAMPLES];

float LinearizeDepth(float depth) 
{
	return (2.0 * depth - gl_DepthRange.near - gl_DepthRange.far) / (gl_DepthRange.far - gl_DepthRange.near);
}

void main()
{
	vec2 uv = gl_FragCoord.xy / _ScreenSize;
	float depth = texture2D(_DepthMap, uv).x;

	float x = uv.x * 2.0 - 1.0;
	float y = (1.0 - uv.y) * 2.0 - 1.0;
	vec4 projectedPosition = vec4(x, y, depth, 1.0);
	vec4 position = _InverseProjection * projectedPosition;

	vec3 origin = position.xyz / position.w;

	vec3 normal = texture2D(_NormalMap, uv).xyz; // * 2.0 - 1.0;
	normal = normalize(normal);

	vec2 noiseScale = _ScreenSize / 4;
	vec3 randomOrientation = texture2D(_NoiseMap, uv * noiseScale).xyz; // * 2.0 - 1.0;

	vec3 tangent = normalize(randomOrientation - normal * dot(randomOrientation, normal));
	vec3 binormal = cross(normal, tangent);

	mat3 tangentSpaceMatrix = mat3(tangent, binormal, normal);

	float occlusion = 0.0;
	for (int i = 0; i < NUMBER_OF_RANDOM_SAMPLES; i++)
	{
		vec3 randomSampleInAHemisphere = tangentSpaceMatrix * _RandomSamplesInAHemisphere[i];
		randomSampleInAHemisphere = randomSampleInAHemisphere * _RayLength + origin;

		vec4 offset = vec4(randomSampleInAHemisphere, 1.0);
		offset = _Projection * offset;
		offset.xy /= offset.w;
		offset.xy = offset.xy * 0.5 + 0.5;

		float sampleDepth = LinearizeDepth(texture2D(_DepthMap, offset.st).x);
		float rangeCheck = smoothstep(0.0, 1.0, _RayLength / abs(origin.z - sampleDepth));
		occlusion += rangeCheck * step(sampleDepth, randomSampleInAHemisphere.z);
	}

	occlusion = 1.0 - (occlusion / NUMBER_OF_RANDOM_SAMPLES);

	gl_FragColor = vec4(occlusion);
}