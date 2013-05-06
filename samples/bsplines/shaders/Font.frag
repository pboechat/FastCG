#version 330

uniform sampler2D font;
uniform vec4 color;

in vec2 textureCoordinates;

void main()
{
	vec4 texel = texture(font, textureCoordinates);
	gl_FragColor = texel * color;
}