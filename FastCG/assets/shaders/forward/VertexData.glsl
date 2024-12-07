#ifndef FASTCG_FORWARD_VERTEX_DATA_GLGL
#define FASTCG_FORWARD_VERTEX_DATA_GLGL

#include "FastCG.glsl"
#include "Scene.glsl"
#include "Instance.glsl"
#include "Lighting.glsl"

struct VertexData
{
    vec3 lightDirection;
    float viewerDistance;
    vec4 clipPosition;
    vec4 tangent;
    vec3 viewerDirection;
    vec3 worldPosition;
    vec3 normal;
    vec2 uv;
};

#ifdef FASTCG_VERTEX_SHADER
VertexData ComputeVertexData(vec3 position, vec3 normal, vec2 uv, vec4 tangent)
{
    VertexData vertexData;
    vec4 worldPosition = GetInstanceData().model * vec4(position, 1.0);
    vertexData.worldPosition = worldPosition.xyz;
    vertexData.clipPosition = GetInstanceData().viewProjection * worldPosition;
	vertexData.viewerDirection = GetViewerPosition() - vertexData.worldPosition;
	vertexData.viewerDistance = length(vertexData.viewerDirection);
	vertexData.viewerDirection = vertexData.viewerDirection / max(vertexData.viewerDistance, 1e-8);
	vertexData.lightDirection = normalize(GetLightType() * uLight0Position.xyz - (step(0.0, GetLightType()) * vertexData.worldPosition));
	mat3 MIT = mat3(GetInstanceData().modelInverseTranspose);
	vertexData.normal = normalize(MIT * normal);
	vertexData.tangent = vec4(normalize(MIT * tangent.xyz), tangent.w);
	vertexData.uv = uv;
    return vertexData;
}
#endif

#endif