#version 330

uniform vec2 screenSize;
uniform vec3 viewerPosition;
uniform vec3 lightPosition;
uniform vec4 lightAmbientColor;
uniform vec4 lightDiffuseColor;
uniform vec4 lightSpecularColor;
uniform float lightIntensity;
uniform float lightConstantAttenuation;
uniform float lightLinearAttenuation;
uniform float lightQuadraticAttenuation;
uniform sampler2D positionMap;
uniform sampler2D colorMap;
uniform sampler2D normalMap;

float DistanceAttenuation(float _distance)
{
	return 1.0 / min(1.0, (lightConstantAttenuation + lightLinearAttenuation * _distance + lightQuadraticAttenuation * pow(_distance, 2.0)));
}

vec4 BlinnPhongLighting(vec4 materialDiffuseColor,
						float materialShininess,
						vec3 lightDirection,
					    vec3 viewerDirection,
					    vec3 normal)
{
    vec4 ambientContribution = lightAmbientColor * lightIntensity;

    float diffuseAttenuation = max(dot(normal, -lightDirection), 0.0);
    vec4 diffuseContribution = lightDiffuseColor * lightIntensity * materialDiffuseColor * diffuseAttenuation;

	vec4 specularContribution = vec4(0.0);
	if (diffuseAttenuation > 0.0)
	{
        vec3 reflectionDirection = normalize(reflect(lightDirection, normal));
        float specularAttenuation = pow(max(dot(reflectionDirection, viewerDirection), 0.0), materialShininess);
        specularContribution = lightSpecularColor * lightIntensity * specularAttenuation;
    }

    return (ambientContribution + diffuseContribution + specularContribution);
}

void main()
{
	vec2 uv = gl_FragCoord.xy / screenSize;
	vec3 position = texture2D(positionMap, uv).xyz;
	vec4 diffuseColor = texture2D(colorMap, uv);
	vec3 normal = normalize(texture2D(normalMap, uv).xyz);

	vec3 lightDirection = position - lightPosition;
	float _distance = length(lightDirection);
	lightDirection = normalize(lightDirection);
	vec3 viewerDirection = normalize(viewerPosition - position);

	gl_FragColor = vec4(0.1, 0.1, 0.1, 0.3) + DistanceAttenuation(_distance) * BlinnPhongLighting(diffuseColor, 5.0, lightDirection, viewerDirection, normal);
}