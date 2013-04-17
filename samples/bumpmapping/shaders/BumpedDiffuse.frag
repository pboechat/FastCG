#version 330

#include "shaders/FastCG.glsl"

in vec4 eyeSpacePosition;
in vec3 vertexNormal;
in vec2 textureCoordinates;

uniform sampler2D colorMap;
uniform sampler2D bumpMap;
uniform vec4 color;

void main()
{
	vec4 texel = texture(colorMap, textureCoordinates);
	vec4 bump = texture(bumpMap, textureCoordinates);

	vec3 lightPos = vec3(_View * vec4(_Light0Position, 1.0));
	vec3 lightDir = normalize(lightPos - vec3(eyeSpacePosition));
	
	float attenuation = max(dot(lightDir, vertexNormal), 0.0); 

	gl_FragColor = _Light0DiffuseColor * texel * color * attenuation;
}