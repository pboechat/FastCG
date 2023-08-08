#ifndef FASTCG_INSTANCE_GLSL
#define FASTCG_INSTANCE_GLSL

#include "FastCG.glsl"

struct InstanceData
{
	mat4 model;
	mat4 modelView;
	mat4 modelViewInverseTranspose;
	mat4 modelViewProjection;
};

layout(BINDING_0_1) uniform InstanceConstants
{
    InstanceData uInstanceData[MAX_INSTANCES];
};

#define GetInstanceData() uInstanceData[GetInstanceIndex()]

#endif