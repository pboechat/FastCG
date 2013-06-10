#define NOISE_TEXTURE_WIDTH 4
#define NOISE_TEXTURE_HEIGHT 4
#define NOISE_TEXTURE_SIZE 16

uniform vec2 _ScreenSize;
uniform vec2 _TexelSize;
uniform sampler2D _AmbientOcclusionMap;

void main()
{
	vec2 uv = gl_FragCoord.xy / _ScreenSize;

	vec3 result;
	for (int x = 0; x < NOISE_TEXTURE_WIDTH; x++) 
	{
		for (int y = 0; y < NOISE_TEXTURE_HEIGHT; y++) 
		{
			vec2 offset = vec2(_TexelSize.x * x, _TexelSize.y * y);
			result += texture2D(_AmbientOcclusionMap, uv + offset).rgb;
		}
	}

	gl_FragColor = vec4(result / NOISE_TEXTURE_SIZE, 1.0);
}