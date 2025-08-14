layout(location = 0) in vec3 iPosition;

layout(location = 0) out vec2 vUV;

void main()
{
	vUV = iPosition.xy * 0.5 + 0.5;
#ifdef VULKAN
	vUV.y = 1.0 - vUV.y;
#endif
	gl_Position = vec4(iPosition.xy, 0.0, 1.0);
}