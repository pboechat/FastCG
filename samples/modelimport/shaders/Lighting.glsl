float DistanceAttenuation(vec3 position)
{
	float d = distance(_Light0Position, position);
	return 1 / min(_Light0ConstantAttenuation + _Light0LinearAttenuation * d + _Light0QuadraticAttenuation * pow(d, 2.0), 1);
}

vec4 BlinnPhongLighting(vec4 materialDiffuseColor,
						vec3 lightDirection,
						vec3 position,
						vec3 normal)
{
    vec4 ambientContribution = _GlobalLightAmbientColor * _Light0AmbientColor * _Light0Intensity;

    float diffuseAttenuation = max(dot(lightDirection, normal), 0.0);
    vec4 diffuseContribution = _Light0DiffuseColor * _Light0Intensity * materialDiffuseColor * diffuseAttenuation;

	float distanceAttenuation = 1.0f;
	if (_Light0Type == 1.0) // point
	{
		distanceAttenuation = DistanceAttenuation(position);
	}

    return distanceAttenuation * (ambientContribution + diffuseContribution);
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
		vec3 reflectionDirection = normalize(reflect(lightDirection, normal));
		float specularAttenuation = pow(max(dot(reflectionDirection, viewerDirection), 0.0), materialShininess);
		specularContribution = _Light0SpecularColor * _Light0Intensity * materialSpecularColor * specularAttenuation;
	}

	float distanceAttenuation = 1.0f;
	if (_Light0Type == 1.0) // point
	{
		distanceAttenuation = DistanceAttenuation(position);
	}

    return distanceAttenuation * (ambientContribution + diffuseContribution + specularContribution);
}
