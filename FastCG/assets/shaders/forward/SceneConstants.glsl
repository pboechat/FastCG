#ifndef FASTCG_FORWARD_SCENE_CONSTANTS_GLGL
#define FASTCG_FORWARD_SCENE_CONSTANTS_GLGL

#include "FastCG.glsl"

layout(BINDING(0, 0)) uniform SceneConstants 
{
	mat4 uView;
	mat4 uProjection;
	mat4 uInverseProjection;
	float uPointSize;
};

#endif