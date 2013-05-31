#version 330

uniform mat4 _Projection;
uniform mat4 _InverseProjection;
uniform vec2 _ScreenSize;
uniform sampler2D _NormalMap;
uniform sampler2D _DepthMap;
uniform sampler2D _NoiseMap;
uniform float _RayLength;
uniform vec3 _RandomSamplesInAHemisphere[10];

void main()
{
	vec2 uv = gl_FragCoord.xy / _ScreenSize;
	float depth = texture2D(_DepthMap, uv).x;

	float x = uv.x * 2.0 - 1.0;
	float y = (1.0 - uv.y) * 2.0 - 1.0;
	vec4 projectedPosition = vec4(x, y, depth, 1.0);
	vec4 position = _InverseProjection * projectedPosition;

	vec3 origin = position.xyz / position.w;

	vec3 normal = texture2D(_NormalMap, uv).xyz * 2.0 - 1.0;
	normal = normalize(normal);

	vec2 noiseScale = _ScreenSize / 4;
	vec3 randomOrientation = texture2D(_NoiseMap, uv * noiseScale).xyz * 2.0 - 1.0;

	vec3 tangent = normalize(randomOrientation - normal * dot(randomOrientation, normal));
	vec3 binormal = cross(normal, tangent);

	/*mat3 tangentSpaceMatrix = mat3(tangent.x, binormal.x, normal.x,
								   tangent.y, binormal.y, normal.y,
								   tangent.z, binormal.z, normal.z);*/

	mat3 tangentSpaceMatrix = mat3(tangent, binormal, normal);

	float occlusion = 0.0;
	for (int i = 0; i < 10; i++)
	{
		vec3 randomSampleInAHemisphere = tangentSpaceMatrix * _RandomSamplesInAHemisphere[i];
		randomSampleInAHemisphere = randomSampleInAHemisphere * _RayLength + origin;

		vec4 offset = vec4(randomSampleInAHemisphere, 1.0);
		offset = _Projection * offset;
		offset.xy /= offset.w;
		offset.xy = offset.xy * 0.5 + 0.5;

		float sampleDepth = texture2D(_DepthMap, offset.st).x;
		float rangeCheck = abs(origin.z - sampleDepth) < _RayLength ? 1.0 : 0.0;
		occlusion += ((sampleDepth <= randomSampleInAHemisphere.z) ? 1.0 : 0.0) * rangeCheck;
	}

	occlusion = 1.0 - (occlusion / 10);

	gl_FragColor = vec4(vec3(occlusion), 1.0);
}