layout(location = 0) in vec3 iPosition;
layout(location = 2) in vec2 iUV;

layout(location = 0) out vec2 vUV;

void main()
{
	vUV = iUV;
#ifdef VULKAN
	vUV.y = 1.0 - vUV.y;
#endif
	gl_Position = vec4(iPosition.xy, 0.0, 1.0);
}