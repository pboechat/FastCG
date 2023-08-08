#ifndef FASTCG_FASTCG_GLSL
#define FASTCG_FASTCG_GLSL

#ifdef VULKAN
	#define BINDING_0_0 set = 0, binding = 0x0
	#define BINDING_0_1 set = 0, binding = 0x1
	#define BINDING_0_2 set = 0, binding = 0x2
	#define BINDING_0_3 set = 0, binding = 0x3
	#define BINDING_0_4 set = 0, binding = 0x4
	#define BINDING_0_5 set = 0, binding = 0x5
	#define BINDING_0_6 set = 0, binding = 0x6
	#define BINDING_0_7 set = 0, binding = 0x7
	#define BINDING_0_8 set = 0, binding = 0x8
	#define BINDING_0_9 set = 0, binding = 0x9
	#define BINDING_0_A set = 0, binding = 0xa
	#define BINDING_0_B set = 0, binding = 0xb
	#define BINDING_0_C set = 0, binding = 0xc
	#define BINDING_0_D set = 0, binding = 0xd
	#define BINDING_0_E set = 0, binding = 0xe
	#define BINDING_0_F set = 0, binding = 0xf
	#define BINDING_1_0 set = 1, binding = 0x0
	#define BINDING_1_1 set = 1, binding = 0x1
	#define BINDING_1_2 set = 1, binding = 0x2
	#define BINDING_1_3 set = 1, binding = 0x3
	#define BINDING_1_4 set = 1, binding = 0x4
	#define BINDING_1_5 set = 1, binding = 0x5
	#define BINDING_1_6 set = 1, binding = 0x6
	#define BINDING_1_7 set = 1, binding = 0x7
	#define BINDING_1_8 set = 1, binding = 0x8
	#define BINDING_1_9 set = 1, binding = 0x9
	#define BINDING_1_A set = 1, binding = 0xa
	#define BINDING_1_B set = 1, binding = 0xb
	#define BINDING_1_C set = 1, binding = 0xc
	#define BINDING_1_D set = 1, binding = 0xd
	#define BINDING_1_E set = 1, binding = 0xe
	#define BINDING_1_F set = 1, binding = 0xf
	#define BINDING_2_0 set = 2, binding = 0x0
	#define BINDING_2_1 set = 2, binding = 0x1
	#define BINDING_2_2 set = 2, binding = 0x2
	#define BINDING_2_3 set = 2, binding = 0x3
	#define BINDING_2_4 set = 2, binding = 0x4
	#define BINDING_2_5 set = 2, binding = 0x5
	#define BINDING_2_6 set = 2, binding = 0x6
	#define BINDING_2_7 set = 2, binding = 0x7
	#define BINDING_2_8 set = 2, binding = 0x8
	#define BINDING_2_9 set = 2, binding = 0x9
	#define BINDING_2_A set = 2, binding = 0xa
	#define BINDING_2_B set = 2, binding = 0xb
	#define BINDING_2_C set = 2, binding = 0xc
	#define BINDING_2_D set = 2, binding = 0xd
	#define BINDING_2_E set = 2, binding = 0xe
	#define BINDING_2_F set = 2, binding = 0xf
#else
	#define BINDING_0_0 binding = 0x0
	#define BINDING_0_1 binding = 0x1
	#define BINDING_0_2 binding = 0x2
	#define BINDING_0_3 binding = 0x3
	#define BINDING_0_4 binding = 0x4
	#define BINDING_0_5 binding = 0x5
	#define BINDING_0_6 binding = 0x6
	#define BINDING_0_7 binding = 0x7
	#define BINDING_0_8 binding = 0x8
	#define BINDING_0_9 binding = 0x9
	#define BINDING_0_A binding = 0xa
	#define BINDING_0_B binding = 0xb
	#define BINDING_0_C binding = 0xc
	#define BINDING_0_D binding = 0xd
	#define BINDING_0_E binding = 0xe
	#define BINDING_0_F binding = 0xf
	#define BINDING_1_0 binding = 0x10
	#define BINDING_1_1 binding = 0x11
	#define BINDING_1_2 binding = 0x12
	#define BINDING_1_3 binding = 0x13
	#define BINDING_1_4 binding = 0x14
	#define BINDING_1_5 binding = 0x15
	#define BINDING_1_6 binding = 0x16
	#define BINDING_1_7 binding = 0x17
	#define BINDING_1_8 binding = 0x18
	#define BINDING_1_9 binding = 0x19
	#define BINDING_1_A binding = 0x1a
	#define BINDING_1_B binding = 0x1b
	#define BINDING_1_C binding = 0x1c
	#define BINDING_1_D binding = 0x1d
	#define BINDING_1_E binding = 0x1e
	#define BINDING_1_F binding = 0x1f
	#define BINDING_2_0 binding = 0x20
	#define BINDING_2_1 binding = 0x21
	#define BINDING_2_2 binding = 0x22
	#define BINDING_2_3 binding = 0x23
	#define BINDING_2_4 binding = 0x24
	#define BINDING_2_5 binding = 0x25
	#define BINDING_2_6 binding = 0x26
	#define BINDING_2_7 binding = 0x27
	#define BINDING_2_8 binding = 0x28
	#define BINDING_2_9 binding = 0x29
	#define BINDING_2_A binding = 0x2a
	#define BINDING_2_B binding = 0x2b
	#define BINDING_2_C binding = 0x2c
	#define BINDING_2_D binding = 0x2d
	#define BINDING_2_E binding = 0x2e
	#define BINDING_2_F binding = 0x2f
#endif

#define MAX_INSTANCES 256

#ifdef VULKAN
#define GetInstanceIndex() gl_InstanceIndex
#else
#define GetInstanceIndex() gl_InstanceID
#endif

float GetViewSpaceZ(mat4 projection, float depth) 
{
	float zNear = projection[3][2] / (projection[2][2] - 1.0);
    float zFar = projection[3][2] / (projection[2][2] + 1.0);
    return (2.0 * zNear * zFar) / (zFar + zNear - (2.0 * depth - 1.0) * (zFar - zNear));
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