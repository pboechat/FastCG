#version 330

uniform sampler2D colorMap;
uniform sampler2D bumpMap;
uniform vec2 colorMapTiling;
uniform vec2 bumpMapTiling;
uniform vec4 diffuseColor;
uniform vec4 specularColor;
uniform float shininess;

in vec4 vertexPosition;
in vec3 vertexNormal;
in vec3 vertexTangent;
in vec3 vertexBinormal;
in vec2 vertexUV;

void main()
{
	gl_FragData[0] = vertexPosition;
	gl_FragData[1] = diffuseColor * texture2D(colorMap, vertexUV * colorMapTiling);

	vec3 normal = texture2D(bumpMap, vertexUV * bumpMapTiling).xyz;
	normal -= 0.5;
	normal = normalize(normal.x * vertexTangent + normal.y * vertexBinormal + normal.z * vertexNormal);
	normal = normal * 0.5 + 0.5;

	gl_FragData[2] = vec4(normal, 0.0);
	gl_FragData[3] = vec4(specularColor.xyz, shininess);
}