#ifndef FASTCG_DEFERRED_VERTEX_DATA_GLGL
#define FASTCG_DEFERRED_VERTEX_DATA_GLGL

#include "FastCG.glsl"
#include "Scene.glsl"
#include "Instance.glsl"

struct VertexData
{
    vec4 clipPosition;
    vec4 tangent;
    vec3 normal;
    vec2 uv;
};

#ifdef GL_VERTEX_SHADER
VertexData ComputeVertexData(vec3 position, vec3 normal, vec2 uv, vec4 tangent)
{
    VertexData vertexData;
    vertexData.clipPosition = GetInstanceData().modelViewProjection * vec4(position, 1.0);
	mat3 MIT = mat3(GetInstanceData().modelInverseTranspose);
	vertexData.normal = normalize(MIT * normal);
	vertexData.tangent = vec4(normalize(MIT * tangent.xyz), tangent.w);
	vertexData.uv = uv;
    return vertexData;
}
#endif

#endif