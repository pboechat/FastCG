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

const float NEAR = 0.3f;
const float FAR = 1000.0f;
const float DEPTH_RANGE_NEAR = 0;
const float DEPTH_RANGE_FAR = 1;

float DistanceAttenuation(vec3 position)
{
	float d = distance(_Light0Position, position);
	return 1.0 / max(_Light0ConstantAttenuation + _Light0LinearAttenuation * d + _Light0QuadraticAttenuation * pow(d, 2), 1.0);
}

vec3 UnpackNormal(vec4 packedNormal)
{
	return (packedNormal.xyz - 0.5) * 2.0;
}

float LinearizeDepth(float depth) 
{
	return _Projection[3][2] / (depth - (_Projection[2][2] / _Projection[2][3]));
}

vec3 GetPositionFromWindowCoordinates(vec3 windowCoordinates)
{
	vec3 normalizedDeviceCoordinatesPosition;
    normalizedDeviceCoordinatesPosition.xy = (2.0 * windowCoordinates.xy) / _ScreenSize - 1;
    normalizedDeviceCoordinatesPosition.z = (2.0 * windowCoordinates.z - DEPTH_RANGE_NEAR - DEPTH_RANGE_FAR) / (DEPTH_RANGE_FAR - DEPTH_RANGE_NEAR);
 
    vec4 clipSpacePosition;
    clipSpacePosition.w = LinearizeDepth(normalizedDeviceCoordinatesPosition.z);
    clipSpacePosition.xyz = normalizedDeviceCoordinatesPosition * clipSpacePosition.w;
 
    vec4 eyePosition = _InverseProjection * clipSpacePosition;

	return eyePosition.xyz / eyePosition.w;
}