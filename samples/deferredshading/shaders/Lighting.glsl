vec4 BlinnPhong(vec4 lightAmbientColor,
				vec4 lightDiffuseColor,
				vec4 lightSpecularcolor,
				float lightIntensity,
				vec4 materialAmbientColor,
				vec4 materialDiffuseColor,
				vec4 materialSpecularColor,
				vec3 lightDirection,
				vec3 viewerDirection,
				float shininess,
				vec3 position,
				vec3 normal)
{
    vec4 ambientContribution = lightAmbientColor * lightIntensity * materialAmbientColor;

	vec4 diffuseContribution = vec4(0.0);
	vec4 specularContribution = vec4(0.0);
    float diffuseAttenuation = dot(normal, -lightDirection);
    if (diffuseAttenuation > 0) 
	{
        diffuseContribution = lightDiffuseColor * lightIntensity * materialDiffuseColor * diffuseAttenuation;

        vec3 reflectionDirection = normalize(reflect(lightDirection, normal));
        float specularAttenuation = dot(viewerDirection, reflectionDirection);
        specularAttenuation = pow(specularAttenuation, shininess);
        if (specularAttenuation > 0) 
		{
            specularContribution = lightSpecularColor * materialSpecularColor * specularAttenuation;
        }
    }

    return (ambientContribution + diffuseContribution + specularContribution);
}
