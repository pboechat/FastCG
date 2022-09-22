vec4 Phong(vec4 diffuse, vec3 lightDirection, vec3 position, vec3 normal)
{
    float diffuseAttenuation = max(dot(normal, lightDirection), 0.0);
    vec4 diffuseContribution = _Light0DiffuseColor * _Light0Intensity * diffuse * diffuseAttenuation;

    return DistanceAttenuation(position) * (_AmbientColor + diffuseContribution);
}

vec4 Phong(vec4 diffuse, vec4 specular, float shininess, vec3 lightDirection, vec3 viewerDirection, vec3 normal)
{
    float diffuseAttenuation = max(dot(lightDirection, normal), 0.0);
    vec4 diffuseContribution = _Light0DiffuseColor * _Light0Intensity * diffuse * diffuseAttenuation;

    vec3 reflectionDirection = normalize(reflect(-lightDirection, normal));
    float specularAttenuation = max(pow(max(dot(reflectionDirection, viewerDirection), 0.0), shininess), 0);
    vec4 specularContribution = _Light0SpecularColor * _Light0Intensity * specular * specularAttenuation;

    return _AmbientColor + diffuseContribution + specularContribution;
}

vec4 Phong(vec4 diffuse, vec4 specular, float shininess, vec3 lightDirection, vec3 viewerDirection, vec3 position, vec3 normal)
{
    return DistanceAttenuation(position) * Phong(diffuse, specular, shininess, lightDirection, viewerDirection, normal);
}

vec4 BlinnPhong(vec4 diffuse, vec3 lightDirection, vec3 position, vec3 normal)
{
    float diffuseAttenuation = max(dot(normal, lightDirection), 0.0);
    vec4 diffuseContribution = _Light0DiffuseColor * _Light0Intensity * diffuse * diffuseAttenuation;

    return DistanceAttenuation(position) * (_AmbientColor + diffuseContribution);
}

vec4 BlinnPhong(vec4 diffuse, vec4 specular, float shininess, vec3 lightDirection, vec3 viewerDirection, vec3 normal)
{
    float diffuseAttenuation = max(dot(lightDirection, normal), 0.0);
    vec4 diffuseContribution = _Light0DiffuseColor * _Light0Intensity * diffuse * diffuseAttenuation;

    vec3 halfwayVector = normalize(lightDirection + viewerDirection);
    float specularAttenuation = max(pow(max(dot(halfwayVector, normal), 0.0), shininess), 0);
    vec4 specularContribution = _Light0SpecularColor * _Light0Intensity * specular * specularAttenuation;

    return _AmbientColor + diffuseContribution + specularContribution;
}

vec4 BlinnPhong(vec4 diffuse, vec4 specular, float shininess, vec3 lightDirection, vec3 viewerDirection, vec3 position, vec3 normal)
{
    return DistanceAttenuation(position) * BlinnPhong(diffuse, specular, shininess, lightDirection, viewerDirection, normal);
}

#define Lighting BlinnPhong
