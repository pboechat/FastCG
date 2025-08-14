#ifndef FASTCG_TONEMAP_TONEMAP_GLSL
#define FASTCG_TONEMAP_TONEMAP_GLSL

#include "../FastCG.glsl"

vec3 ACES(vec3 x) 
{
  const float a = 2.51;
  const float b = 0.03;
  const float c = 2.43;
  const float d = 0.59;
  const float e = 0.14;
  return clamp((x * (a * x + b)) / (x * (c * x + d) + e), 0.0, 1.0);
}

vec3 Reinhard(vec3 color)
{
    float L = max(dot(color, vec3(0.2126, 0.7152, 0.0722)), 1e-6);
    float Lout = L / (1.0 + L);
    return color * (Lout / L);
}

vec3 CheapReinhard(vec3 color)
{
    return color * (1.0 + color);
}

#endif