uniform mat4 _Projection;
uniform mat4 _InverseProjection;
uniform vec2 _ScreenSize;
uniform float _AspectRatio;
uniform float _TanHalfFov;
uniform sampler2D _DiffuseMap;
uniform sampler2D _NormalMap;
uniform sampler2D _SpecularMap;
uniform sampler2D _TangentMap;
uniform sampler2D _ExtraData;
uniform sampler2D _Depth;
uniform sampler2D _AmbientOcclusionMap;
uniform vec4 _AmbientColor;
uniform vec3 _Light0Position;
uniform vec4 _Light0DiffuseColor;
uniform vec4 _Light0SpecularColor;
uniform float _Light0Intensity;
uniform float _Light0ConstantAttenuation;
uniform float _Light0LinearAttenuation;
uniform float _Light0QuadraticAttenuation;
uniform float _AmbientOcclusionFlag;

const float DEPTH_RANGE_NEAR = 0;
const float DEPTH_RANGE_FAR = 1;

float DistanceAttenuation(vec3 position)
{
	float d = distance(_Light0Position, position);
	return 1.0 / max(_Light0ConstantAttenuation + _Light0LinearAttenuation * d + _Light0QuadraticAttenuation * pow(d, 2), 1.0);
}

vec3 PackNormalToColor(vec3 value)
{
	return value.xyz * 0.5 + 0.5;
}

vec4 PackNormalToColor(vec4 value)
{
	return vec4(PackNormalToColor(value.xyz), value.w);
}

vec3 UnpackNormalFromColor(vec3 color)
{
	return (color.xyz - 0.5) * 2.0;
}

vec4 UnpackNormalFromColor(vec4 color)
{
	return vec4((color.xyz - 0.5) * 2.0, color.w);
}

float LinearizeDepth(float depth) 
{
	return _Projection[3][2] / (depth - (_Projection[2][2] / _Projection[2][3]));
}

vec3 GetWorldPositionFromScreenCoordsAndDepth(vec3 screenCoords)
{
	vec3 ndc;
    ndc.xy = (2.0 * screenCoords.xy) / _ScreenSize - 1;
    ndc.z = (2.0 * screenCoords.z - DEPTH_RANGE_NEAR - DEPTH_RANGE_FAR) / (DEPTH_RANGE_FAR - DEPTH_RANGE_NEAR);
 
    vec4 clipCoords;
    clipCoords.w = _Projection[3][2] / (ndc.z - (_Projection[2][2] / _Projection[2][3]));
    clipCoords.xyz = ndc * clipCoords.w;
 
    vec4 eyePosition = _InverseProjection * clipCoords;

	return eyePosition.xyz / eyePosition.w;
}

bool HasBump(vec4 tangent, vec4 extraData)
{
	return extraData.x != 0 && extraData.y != 0 && extraData.z != 0;
}