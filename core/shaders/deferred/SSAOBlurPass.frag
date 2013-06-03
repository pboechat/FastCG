#define NOISE_TEXTURE_WIDTH 4
#define NOISE_TEXTURE_HEIGHT 4
#define NOISE_TEXTURE_SIZE 16.0

uniform vec2 _ScreenSize;
uniform vec2 _TexelSize;
uniform sampler2D _OcclusionMap;

void main()
{
	vec2 uv = gl_FragCoord.xy / _ScreenSize;

	float result;
	for (int i = 0; i < NOISE_TEXTURE_WIDTH; i++) 
	{
		for (int j = 0; j < NOISE_TEXTURE_HEIGHT; j++) 
		{
			vec2 offset = vec2(_TexelSize.x * j, _TexelSize.y * i);
			result += texture2D(_OcclusionMap, uv + offset).x;
		}
	}

	gl_FragColor = vec4(result / NOISE_TEXTURE_SIZE);
}