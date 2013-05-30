#if (!defined(GBUFFER_H_) && !defined(FIXED_FUNCTION_PIPELINE))
#define GBUFFER_H_

#define NUMBER_OF_TEXTURES 4

class GBuffer
{
public:
	enum GBufferTextureType
	{
		GTT_POSITION_TEXTURE,
		GTT_DIFFUSE_TEXTURE,
		GTT_NORMAL_TEXTURE,
		GTT_SPECULAR_TEXTURE,
		GTT_SSAO_TEXTURE,
		GTT_FINAL_TEXTURE
	};

	GBuffer(unsigned int& rScreenWidth, unsigned int& rScreenHeight);
	~GBuffer();

	void StartFrame();
	void BindForDebugging();
	void BindForGeometryPass();
	void BindForStencilPass();
	void BindForLightPass();
	void BindForSSAOHighFrequencyPass();
	void BindForFinalPass();
	void SetReadBuffer(GBufferTextureType textureType);

private:
	unsigned int& mrScreenWidth;
	unsigned int& mrScreenHeight;
	unsigned int mFBOId;
	unsigned int mTexturesIds[NUMBER_OF_TEXTURES];
	unsigned int mDepthTextureId;
	unsigned int mSSAOTextureId;
	unsigned int mFinalTextureId;

	void AllocateResources();
	void DeallocateResources();

};

#endif