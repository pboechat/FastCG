#ifndef FASTCG_SCENE_CONSTANTS_GLGL
#define FASTCG_SCENE_CONSTANTS_GLGL

#include "FastCG.glsl"

layout(BINDING(0, 0)) uniform SceneConstants 
{
	mat4 uView;
	mat4 uInverseView;
	mat4 uProjection;
	mat4 uInverseProjection;
	vec2 uScreenSize;
	float uAspectRatio;
	float uTanHalfFov;
	float uPointSize;
};

vec3 GetViewerPosition()
{
	return uView[3].xyz;
} 

#define GetScreenCoordinates() (gl_FragCoord.xy / uScreenSize)

#endif