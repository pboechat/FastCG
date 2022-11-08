#ifndef FASTCG_FASTCG_GLSL
#define FASTCG_FASTCG_GLSL

#ifdef VULKAN
	#define BINDING(a, b) set = a, binding = b
#else
	#define BINDING(a, b) binding = 0x##a##b
#endif

float LinearizeDepth(mat4 projection, float depth) 
{
	return projection[3][2] / (depth - (projection[2][2] / projection[2][3]));
}

vec3 PackNormalToColor(vec3 value)
{
	return value.xyz * 0.5 + 0.5;
}

vec4 PackNormalToColor(vec4 value)
{
	return vec4(PackNormalToColor(value.xyz), value.w);
}

vec3 UnpackNormalFromColor(vec3 color)
{
	return (color.xyz - 0.5) * 2.0;
}

vec4 UnpackNormalFromColor(vec4 color)
{
	return vec4((color.xyz - 0.5) * 2.0, color.w);
}

#endif