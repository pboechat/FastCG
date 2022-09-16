uniform mat4 _Model;
uniform mat4 _View;
uniform mat4 _ModelView;
uniform mat4 _Projection;
uniform mat4 _ModelViewProjection;
uniform mat3 _ModelViewInverseTranspose;
uniform vec4 _GlobalLightAmbientColor;
uniform float _Light0Type;
uniform vec3 _Light0Position;
uniform vec4 _Light0AmbientColor;
uniform vec4 _Light0DiffuseColor;
uniform vec4 _Light0SpecularColor;
uniform float _Light0Intensity;
uniform float _Light0ConstantAttenuation;
uniform float _Light0LinearAttenuation;
uniform float _Light0QuadraticAttenuation;
uniform vec3 _Light0SpotDirection;
uniform float _Light0SpotCutoff;
uniform float _Light0SpotExponent;

float DistanceAttenuation(vec3 position)
{
	float d = distance(_Light0Position, position);

	// FIXME: divergence control
	if (_Light0Type == 1.0) 
	{
		return 1.0 / max(_Light0ConstantAttenuation + _Light0LinearAttenuation * d + _Light0QuadraticAttenuation * pow(d, 2.0), 1.0);
	}
	else
	{
		return 1.0;
	}
}

vec3 UnpackNormal(vec4 packedNormal)
{
	return (packedNormal.xyz - 0.5) * 2.0;
}