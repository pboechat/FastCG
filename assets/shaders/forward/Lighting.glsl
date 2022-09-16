vec4 BlinnPhongLighting(vec4 materialAmbientColor,
						vec4 materialDiffuseColor,
						vec3 lightDirection,
						vec3 position,
						vec3 normal)
{
    vec4 ambientContribution = _GlobalLightAmbientColor + _Light0AmbientColor * _Light0Intensity * materialAmbientColor;

    float diffuseAttenuation = max(dot(normal, lightDirection), 0.0);
    vec4 diffuseContribution = _Light0DiffuseColor * _Light0Intensity * materialDiffuseColor * diffuseAttenuation;

    return DistanceAttenuation(position) * (ambientContribution + diffuseContribution);
}

vec4 PhongLighting(vec4 materialAmbientColor,
				   vec4 materialDiffuseColor,
				   vec4 materialSpecularColor,
				   float materialShininess,
				   vec3 lightDirection,
				   vec3 viewerDirection,
				   vec3 position,
				   vec3 normal)
{
    vec4 ambientContribution = _GlobalLightAmbientColor + _Light0AmbientColor * _Light0Intensity * materialAmbientColor;

    float diffuseAttenuation = max(dot(lightDirection, normal), 0.0);
    vec4 diffuseContribution = _Light0DiffuseColor * _Light0Intensity * materialDiffuseColor * diffuseAttenuation;

	vec3 reflectionDirection = normalize(reflect(-lightDirection, normal));
	float specularAttenuation = pow(max(dot(reflectionDirection, viewerDirection), 0.0), materialShininess);
	vec4 specularContribution = _Light0SpecularColor * _Light0Intensity * materialSpecularColor * specularAttenuation * step(0.0, diffuseAttenuation);

    return DistanceAttenuation(position) * (ambientContribution + diffuseContribution + specularContribution);
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
    vec4 ambientContribution = _GlobalLightAmbientColor + _Light0AmbientColor * _Light0Intensity * materialAmbientColor;

    float diffuseAttenuation = max(dot(lightDirection, normal), 0.0);
    vec4 diffuseContribution = _Light0DiffuseColor * _Light0Intensity * materialDiffuseColor * diffuseAttenuation;

	vec3 halfwayVector = normalize(lightDirection + viewerDirection);
	float specularAttenuation = pow(max(dot(halfwayVector, normal), 0.0), materialShininess);
	vec4 specularContribution = _Light0SpecularColor * _Light0Intensity * materialSpecularColor * specularAttenuation * step(0.0, diffuseAttenuation);

    return DistanceAttenuation(position) * (ambientContribution + diffuseContribution + specularContribution);
}
