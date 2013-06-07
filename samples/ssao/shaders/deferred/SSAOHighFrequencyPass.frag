#version 330

#define NOISE_TEXTURE_WIDTH 4
#define NOISE_TEXTURE_HEIGHT 4
#define NUMBER_OF_RANDOM_SAMPLES 30

uniform mat4 _Projection;
uniform mat4 _InverseProjection;
uniform vec2 _ScreenSize;
uniform sampler2D _NormalMap;
uniform sampler2D _DepthMap;
uniform sampler2D _NoiseMap;
uniform float _RayLength;
uniform float _OcclusionExponent;
uniform vec3 _RandomSamples[NUMBER_OF_RANDOM_SAMPLES];

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

	vec3 normal = texture2D(_NormalMap, uv).xyz;

	vec2 noiseScale = vec2(_ScreenSize.x / NOISE_TEXTURE_WIDTH, _ScreenSize.y / NOISE_TEXTURE_HEIGHT);
	vec3 randomOrientation = texture2D(_NoiseMap, uv * noiseScale).xyz;

	//vec3 tangent = normalize(randomOrientation - normal * dot(randomOrientation, normal));
	vec3 tangent = normalize(cross(normal, cross(randomOrientation, normal)));
	vec3 binormal = cross(normal, tangent);

	mat3 tangentSpaceMatrix = mat3(tangent, binormal, normal);

	float occlusion = 0.0;
	for (int i = 0; i < NUMBER_OF_RANDOM_SAMPLES; i++)
	{
		vec3 randomSample = tangentSpaceMatrix * _RandomSamples[i];
		randomSample = randomSample * _RayLength;

		vec4 offset = _Projection * vec4(randomSample, 1.0);
		offset.xy /= offset.w;
		offset.xy = offset.xy * 0.5 + 0.5;
		offset.y = 1 - offset.y;

		float sampleDepth = LinearizeDepth(texture2D(_DepthMap, offset.st).x);
		float rangeCheck = smoothstep(0.0, 1.0, _RayLength / abs(origin.z - sampleDepth));
		occlusion += rangeCheck * step(sampleDepth, randomSample.z);
	}

	occlusion /= NUMBER_OF_RANDOM_SAMPLES;

	gl_FragColor = vec4(pow(occlusion, _OcclusionExponent));
}