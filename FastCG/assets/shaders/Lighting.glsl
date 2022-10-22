#ifndef FASTCG_LIGHTING_GLSL
#define FASTCG_LIGHTING_GLSL

float DistanceAttenuation(vec3 position)
{
	float d = distance(uLight0Position.xyz, position);

	// FIXME: divergence control
	if (FASTCG_LIGHT_TYPE() == 1.0) 
	{
		return 1.0 / max(uLight0ConstantAttenuation + uLight0LinearAttenuation * d + uLight0QuadraticAttenuation * pow(d, 2.0), 1.0);
	}
	else
	{
		return 1.0;
	}
}

vec4 Phong(vec4 diffuse, vec3 lightDirection, vec3 position, vec3 normal)
{
    float diffuseAttenuation = max(dot(normal, lightDirection), 0.0);
    vec4 diffuseContribution = uLight0DiffuseColor * uLight0Intensity * diffuse * diffuseAttenuation;

    return DistanceAttenuation(position) * (uAmbientColor + diffuseContribution);
}

vec4 Phong(vec4 diffuse, vec4 specular, float shininess, vec3 lightDirection, vec3 viewerDirection, vec3 normal)
{
    float diffuseAttenuation = max(dot(lightDirection, normal), 0.0);
    vec4 diffuseContribution = uLight0DiffuseColor * uLight0Intensity * diffuse * diffuseAttenuation;

    vec3 reflectionDirection = normalize(reflect(-lightDirection, normal));
    float specularAttenuation = max(pow(max(dot(reflectionDirection, viewerDirection), 0.0), shininess), 0);
    vec4 specularContribution = uLight0SpecularColor * uLight0Intensity * specular * specularAttenuation;

    return uAmbientColor + diffuseContribution + specularContribution;
}

vec4 Phong(vec4 diffuse, vec4 specular, float shininess, vec3 lightDirection, vec3 viewerDirection, vec3 position, vec3 normal)
{
    return DistanceAttenuation(position) * Phong(diffuse, specular, shininess, lightDirection, viewerDirection, normal);
}

vec4 BlinnPhong(vec4 diffuse, vec3 lightDirection, vec3 position, vec3 normal)
{
    float diffuseAttenuation = max(dot(normal, lightDirection), 0.0);
    vec4 diffuseContribution = uLight0DiffuseColor * uLight0Intensity * diffuse * diffuseAttenuation;

    return DistanceAttenuation(position) * (uAmbientColor + diffuseContribution);
}

vec4 BlinnPhong(vec4 diffuse, vec4 specular, float shininess, vec3 lightDirection, vec3 viewerDirection, vec3 normal)
{
    float diffuseAttenuation = max(dot(lightDirection, normal), 0.0);
    vec4 diffuseContribution = uLight0DiffuseColor * uLight0Intensity * diffuse * diffuseAttenuation;

    vec3 halfwayVector = normalize(lightDirection + viewerDirection);
    float specularAttenuation = max(pow(max(dot(halfwayVector, normal), 0.0), shininess), 0);
    vec4 specularContribution = uLight0SpecularColor * uLight0Intensity * specular * specularAttenuation;

    return uAmbientColor + diffuseContribution + specularContribution;
}

vec4 BlinnPhong(vec4 diffuse, vec4 specular, float shininess, vec3 lightDirection, vec3 viewerDirection, vec3 position, vec3 normal)
{
    return DistanceAttenuation(position) * BlinnPhong(diffuse, specular, shininess, lightDirection, viewerDirection, normal);
}

#define Lighting BlinnPhong

#endif