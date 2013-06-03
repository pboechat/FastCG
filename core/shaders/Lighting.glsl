float DistanceAttenuation(vec3 position)
{
	float _distance = distance(_Light0Position, position);
	float attenuation = 1.0 / max(_Light0ConstantAttenuation + _Light0LinearAttenuation * _distance + _Light0QuadraticAttenuation * pow(_distance, 2.0), 1.0);
	// returns 1.0 if 'directional' and attenuation if 'point'
	return min(1.0 - _Light0Type + attenuation, 1.0);
}

vec4 BlinnPhongLighting(vec4 materialAmbientColor,
						vec4 materialDiffuseColor,
						vec3 lightDirection,
						vec3 position,
						vec3 normal)
{
    vec4 ambientContribution = _GlobalLightAmbientColor * _Light0AmbientColor * _Light0Intensity;

    float diffuseAttenuation = max(dot(lightDirection, normal), 0.0);
    vec4 diffuseContribution = _Light0DiffuseColor * _Light0Intensity * materialDiffuseColor * diffuseAttenuation;

    return (ambientContribution + diffuseContribution) * DistanceAttenuation(position);
}

vec4 BlinnPhongLighting(vec4 materialAmbientColor,
						vec4 materialDiffuseColor,
						vec4 materialSpecularColor,
						float materialShininess,
						vec3 lightDirection,
						vec3 viewerDirection,
						vec3 position,
						vec3 normal)
{
    vec4 ambientContribution = _GlobalLightAmbientColor * _Light0AmbientColor * _Light0Intensity * materialAmbientColor;

    float diffuseAttenuation = max(dot(lightDirection, normal), 0.0);
    vec4 diffuseContribution = _Light0DiffuseColor * _Light0Intensity * materialDiffuseColor * diffuseAttenuation;

	vec4 specularContribution = vec4(0.0);
	if (diffuseAttenuation > 0.0)
	{
		vec3 reflectionDirection = normalize(reflect(-lightDirection, normal));
		float specularAttenuation = pow(max(dot(reflectionDirection, viewerDirection), 0.0), materialShininess);
		specularContribution = _Light0SpecularColor * _Light0Intensity * materialSpecularColor * specularAttenuation;
	}

    return (ambientContribution + diffuseContribution + specularContribution) * DistanceAttenuation(position);
}
