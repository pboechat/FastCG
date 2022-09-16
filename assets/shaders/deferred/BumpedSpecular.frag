#version 330

uniform sampler2D colorMap;
uniform sampler2D bumpMap;
uniform vec2 colorMapTiling;
uniform vec2 bumpMapTiling;
uniform vec4 diffuseColor;
uniform vec4 specularColor;
uniform float shininess;

in vec3 vertexNormal;
in vec3 vertexTangent;
in vec3 vertexBinormal;
in vec2 vertexUV;

void main()
{
	gl_FragData[0] = diffuseColor * texture2D(colorMap, vertexUV * colorMapTiling);

	vec3 normal = texture2D(bumpMap, vertexUV * bumpMapTiling).xyz;
	normal = normalize(normal.x * vertexTangent + normal.y * vertexBinormal + normal.z * vertexNormal);

	gl_FragData[1] = vec4((normal + 1.0) * 0.5, 0.0);
	gl_FragData[2] = vec4(specularColor.xyz, shininess);
}