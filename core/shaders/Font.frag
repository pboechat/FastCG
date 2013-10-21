#version 330

uniform sampler2D font;
uniform vec4 color;

in vec2 textureCoordinates;

void main()
{
	float alpha = texture(font, textureCoordinates).r;
	gl_FragColor = vec4(color.rgb, alpha);
}