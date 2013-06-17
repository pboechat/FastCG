uniform mat4 _Projection;
uniform mat4 _InverseProjection;
uniform vec2 _ScreenSize;
uniform sampler2D _DiffuseMap;
uniform sampler2D _NormalMap;
uniform sampler2D _SpecularMap;
uniform sampler2D _DepthMap;
uniform sampler2D _AmbientOcclusionMap;
uniform vec4 _GlobalLightAmbientColor;
uniform vec3 _Light0Position;
uniform vec4 _Light0AmbientColor;
uniform vec4 _Light0DiffuseColor;
uniform vec4 _Light0SpecularColor;
uniform float _Light0Intensity;
uniform float _Light0ConstantAttenuation;
uniform float _Light0LinearAttenuation;
uniform float _Light0QuadraticAttenuation;
uniform float _AmbientOcclusionFlag;

float DistanceAttenuation(vec3 position)
{
	float d = distance(_Light0Position, position);
	return 1.0 / max(_Light0ConstantAttenuation + _Light0LinearAttenuation * d + _Light0QuadraticAttenuation * pow(d, 2), 1.0);
}

float LinearizeDepth(float depth) 
{
	return (2.0 * depth - gl_DepthRange.near - gl_DepthRange.far) / (gl_DepthRange.far - gl_DepthRange.near);
}

vec3 GetPositionFromDepth(vec2 uv)
{
	float depth = texture2D(_DepthMap, uv).x;

	vec3 normalizedDeviceCoordinatesPosition;
    normalizedDeviceCoordinatesPosition.xy = 2.0 * uv - 1;
    normalizedDeviceCoordinatesPosition.z = LinearizeDepth(depth);
 
    vec4 clipSpacePosition;
    clipSpacePosition.w = _Projection[2][2] / (normalizedDeviceCoordinatesPosition.z - (_Projection[3][2] / _Projection[2][3]));
    clipSpacePosition.xyz = normalizedDeviceCoordinatesPosition * clipSpacePosition.w;
 
    vec4 eyePosition = _InverseProjection * clipSpacePosition;

	return eyePosition.xyz / eyePosition.w;
}