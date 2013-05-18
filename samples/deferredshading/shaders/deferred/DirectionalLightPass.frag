#version 330

uniform vec4 _GlobalLightAmbientColor;
uniform vec2 screenSize;
uniform vec3 viewerPosition;
uniform vec3 lightPosition;
uniform vec4 lightAmbientColor;
uniform vec4 lightDiffuseColor;
uniform vec4 lightSpecularColor;
uniform float lightIntensity;
uniform sampler2D positionMap;
uniform sampler2D colorMap;
uniform sampler2D normalMap;
uniform sampler2D specularMap;

vec4 BlinnPhongLighting(vec4 materialDiffuseColor,
						vec4 materialSpecularColor,
						float materialShininess,
						vec3 lightDirection,
					    vec3 viewerDirection,
					    vec3 normal)
{
    vec4 ambientContribution = _GlobalLightAmbientColor * lightAmbientColor * lightIntensity;

    float diffuseAttenuation = max(dot(normal, -lightDirection), 0.0);
    vec4 diffuseContribution = lightDiffuseColor * lightIntensity * materialDiffuseColor * diffuseAttenuation;

	vec4 specularContribution = vec4(0.0);
	if (diffuseAttenuation > 0.0)
	{
        vec3 reflectionDirection = normalize(reflect(lightDirection, normal));
        float specularAttenuation = pow(max(dot(reflectionDirection, viewerDirection), 0.0), materialShininess);
        specularContribution = lightSpecularColor * lightIntensity * materialSpecularColor * specularAttenuation;
    }

    return (ambientContribution + diffuseContribution + specularContribution);
}

void main()
{
	vec2 uv = gl_FragCoord.xy / screenSize;
	vec3 position = texture2D(positionMap, uv).xyz;
	vec4 diffuseColor = texture2D(colorMap, uv);
	vec4 normalAndShininess = texture2D(normalMap, uv);
	vec4 specularColor = texture2D(specularMap, uv);
	vec3 normal = normalize(normalAndShininess.xyz);
	float shininess = normalAndShininess.w;

	vec3 lightDirection = normalize(lightPosition - position);
	vec3 viewerDirection = normalize(viewerPosition - position);

	gl_FragColor = BlinnPhongLighting(diffuseColor, specularColor, shininess, lightDirection, viewerDirection, normal);
}