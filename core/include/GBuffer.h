#if (!defined(GBUFFER_H_) && !defined(FIXED_FUNCTION_PIPELINE))
#define GBUFFER_H_

#define NUMBER_OF_GBUFFER_COLOR_ATTACHMENTS 4

class GBuffer
{
public:
	enum GBufferTextureType
	{
		GTT_POSITION_TEXTURE,
		GTT_COLOR_TEXTURE,
		GTT_NORMAL_TEXTURE,
		GTT_SPECULAR_TEXTURE
	};

	GBuffer(unsigned int& rScreenWidth, unsigned int& rScreenHeight);
	~GBuffer();

	void StartFrame();
	void BindForGBufferDebugging();
	void BindForSSAODebugging();
	void BindForGeometryPass();
	void BindForStencilPass();
	void BindForLightPass();
	void BindForSSAOHighFrequencyPass();
	void BindForSSAOBlurPass();
	void BindForFinalPass();

private:
	unsigned int& mrScreenWidth;
	unsigned int& mrScreenHeight;
	unsigned int mGBufferFBOId;
	unsigned int mSSAOFBOId;
	unsigned int mSSAOBlurFBOId;
	unsigned int mTexturesIds[NUMBER_OF_GBUFFER_COLOR_ATTACHMENTS];
	unsigned int mDepthTextureId;
	unsigned int mAmbientTextureId;
	unsigned int mBlurredAmbientTextureId;
	unsigned int mFinalOutputTextureId;

	void AllocateResources();
	void DeallocateResources();

};

#endif