#version 330

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

vec4 Lighting(vec3 lightDirection,
			  vec3 viewerDirection,
			  float shininess,
			  vec3 normal)
{
    vec4 ambientContribution = lightAmbientColor * lightIntensity;

	vec4 diffuseContribution = vec4(0.0);
	vec4 specularContribution = vec4(0.0);
    float diffuseAttenuation = dot(normal, -lightDirection);
    if (diffuseAttenuation > 0) 
	{
        diffuseContribution = lightDiffuseColor * lightIntensity * diffuseAttenuation;

        vec3 reflectionDirection = normalize(reflect(lightDirection, normal));
        float specularAttenuation = dot(viewerDirection, reflectionDirection);
        specularAttenuation = pow(specularAttenuation, shininess);
        if (specularAttenuation > 0) 
		{
            specularContribution = lightSpecularColor * lightIntensity * specularAttenuation;
        }
    }

    return (ambientContribution + diffuseContribution + specularContribution);
}

void main()
{
	vec2 textureCoordinates = gl_FragCoord.xy / screenSize;
	vec3 position = texture2D(positionMap, textureCoordinates).xyz;
	vec3 color = texture2D(colorMap, textureCoordinates).xyz;
	vec3 normal = normalize(texture2D(normalMap, textureCoordinates).xyz);

	vec3 lightDirection = lightPosition - position;
	vec3 viewerDirection = viewerPosition - position;

	gl_FragColor = vec4(color, 1.0) * Lighting(lightDirection, viewerDirection, 1.5, normal);
}