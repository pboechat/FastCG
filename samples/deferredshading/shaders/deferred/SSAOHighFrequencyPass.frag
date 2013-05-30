#version 330

uniform vec2 screenSize;
uniform sampler2D normalMap;
uniform sampler2D depthMap;

void main()
{
	vec2 uv = gl_FragCoord.xy / screenSize;
	float depth = texture2D(depthMap, uv).x;
	gl_FragColor = vec4(depth);
}