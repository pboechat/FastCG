#define NOISE_TEXTURE_SIDE 4
#define NOISE_TEXTURE_SIZE 16

noperspective in vec2 vertexUv;

uniform sampler2D _AmbientOcclusionMap;

void main()
{
	vec2 texelSize = 1.0 / vec2(textureSize(_AmbientOcclusionMap, 0));

	vec4 result = vec4(0);
	vec2 stride = vec2(-NOISE_TEXTURE_SIDE * 0.5 + 0.5);
	for (int x = 0; x < NOISE_TEXTURE_SIDE; x++) 
	{
		for (int y = 0; y < NOISE_TEXTURE_SIDE; y++) 
		{
			vec2 uvOffset = vec2(float(x), float(y));
			uvOffset += stride;
			uvOffset *= texelSize;

			result += texture2D(_AmbientOcclusionMap, vertexUv + uvOffset);
		}
	}

	gl_FragColor = result / NOISE_TEXTURE_SIZE;
}