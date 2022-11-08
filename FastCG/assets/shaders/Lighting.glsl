#ifndef FASTCG_LIGHTING_GLSL
#define FASTCG_LIGHTING_GLSL

float DistanceAttenuation(vec3 worldPosition)
{
	if (FASTCG_LIGHT_TYPE() == 1.0) 
	{
	    float d = distance(uLight0Position.xyz, worldPosition);
		return 1.0 / max(uLight0ConstantAttenuation + uLight0LinearAttenuation * d + uLight0QuadraticAttenuation * pow(d, 2.0), 1.0);
	}
	else
	{
		return 1.0;
	}
}

vec4 Phong(vec4 diffuse, vec3 lightDirection, vec3 worldPosition, vec3 normal, vec2 screenCoords)
{
    float diffuseAttenuation = max(dot(normal, lightDirection), 0.0);
    vec4 diffuseContribution = uLight0DiffuseColor * uLight0Intensity * diffuse * diffuseAttenuation;

    return DistanceAttenuation(worldPosition) * (uAmbientColor + diffuseContribution) * GetShadow(worldPosition);
}

vec4 Phong(vec4 diffuse, vec4 specular, float shininess, vec3 lightDirection, vec3 viewerDirection, vec3 worldPosition, vec3 normal, vec2 screenCoords)
{
    float diffuseAttenuation = max(dot(lightDirection, normal), 0.0);
    vec4 diffuseContribution = uLight0DiffuseColor * uLight0Intensity * diffuse * diffuseAttenuation;

    vec3 reflectionDirection = normalize(reflect(-lightDirection, normal));
    float specularAttenuation = max(pow(max(dot(reflectionDirection, viewerDirection), 0.0), shininess), 0);
    vec4 specularContribution = uLight0SpecularColor * uLight0Intensity * specular * specularAttenuation;

    return DistanceAttenuation(worldPosition) * (uAmbientColor + diffuseContribution + specularContribution) * GetShadow(worldPosition) * GetAmbientOcclusion(screenCoords);
}

vec4 BlinnPhong(vec4 diffuse, vec3 lightDirection, vec3 worldPosition, vec3 normal, vec2 screenCoords)
{
    float diffuseAttenuation = max(dot(normal, lightDirection), 0.0);
    vec4 diffuseContribution = uLight0DiffuseColor * uLight0Intensity * diffuse * diffuseAttenuation;

    return DistanceAttenuation(worldPosition) * (uAmbientColor + diffuseContribution) * GetShadow(worldPosition) * GetAmbientOcclusion(screenCoords);
}

vec4 BlinnPhong(vec4 diffuse, vec4 specular, float shininess, vec3 lightDirection, vec3 viewerDirection, vec3 worldPosition, vec3 normal, vec2 screenCoords)
{
    float diffuseAttenuation = max(dot(lightDirection, normal), 0.0);
    vec4 diffuseContribution = uLight0DiffuseColor * uLight0Intensity * diffuse * diffuseAttenuation;

    vec3 halfwayVector = normalize(lightDirection + viewerDirection);
    float specularAttenuation = max(pow(max(dot(halfwayVector, normal), 0.0), shininess), 0);
    vec4 specularContribution = uLight0SpecularColor * uLight0Intensity * specular * specularAttenuation;

    return DistanceAttenuation(worldPosition) * (uAmbientColor + diffuseContribution + specularContribution) * GetShadow(worldPosition) * GetAmbientOcclusion(screenCoords);
}

#define Lighting BlinnPhong

#endif