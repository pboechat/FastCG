#ifndef FASTCG_INSTANCE_GLSL
#define FASTCG_INSTANCE_GLSL

#include "FastCG.glsl"

layout(BINDING(0, 1)) uniform InstanceConstants
{
    mat4 uModel;
	mat4 uModelView;
	mat4 uModelViewInverseTranspose;
	mat4 uModelViewProjection;
};

#endif