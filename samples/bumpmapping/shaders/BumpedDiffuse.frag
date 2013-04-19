#version 330

#include "shaders/FastCG.glsl"

uniform sampler2D colorMap;
uniform sampler2D bumpMap;
uniform vec4 color;

in vec3 lightDirection;
in vec3 viewerDirection;
in vec2 textureCoordinates;

void main()
{
	vec4 texel = texture(colorMap, textureCoordinates);
	vec3 normal = texture(bumpMap, textureCoordinates).xyz;
	
	float attenuation = max(dot(lightDirection, normal), 0.0); 

	gl_FragColor = _Light0DiffuseColor * texel * color * attenuation;
}