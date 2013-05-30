#if (!defined(GBUFFER_H_) && !defined(FIXED_FUNCTION_PIPELINE))
#define GBUFFER_H_

class GBuffer
{
public:
	enum GBufferTextureType
	{
		GBUFFER_TEXTURE_TYPE_POSITION,
		GBUFFER_TEXTURE_TYPE_DIFFUSE,
		GBUFFER_TEXTURE_TYPE_NORMAL,
		GBUFFER_TEXTURE_TYPE_SPECULAR,
		GBUFFER_NUM_TEXTURES
	};

	GBuffer(unsigned int& rScreenWidth, unsigned int& rScreenHeight);
	~GBuffer();

	void StartFrame();
	void BindForDebugging();
	void BindForGeometryPass();
	void BindForStencilPass();
	void BindForLightPass();
	void BindForFinalPass();
	void SetReadBuffer(GBufferTextureType textureType);

private:
	unsigned int& mrScreenWidth;
	unsigned int& mrScreenHeight;
	unsigned int mFBOId;
	unsigned int mTexturesIds[GBUFFER_NUM_TEXTURES];
	unsigned int mDepthTextureId;
	unsigned int mFinalTextureId;

	void AllocateResources();
	void DeallocateResources();

};

#endif