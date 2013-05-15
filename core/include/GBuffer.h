#if (!defined(GBUFFER_H_) && defined(USE_PROGRAMMABLE_PIPELINE))
#define GBUFFER_H_

class GBuffer
{
public:
	enum GBufferTextureType
	{
		GBUFFER_TEXTURE_TYPE_POSITION,
		GBUFFER_TEXTURE_TYPE_DIFFUSE,
		GBUFFER_TEXTURE_TYPE_NORMAL,
		GBUFFER_TEXTURE_TYPE_TEXCOORD,
		GBUFFER_NUM_TEXTURES
	};

	GBuffer(unsigned int& rScreenWidth, unsigned int& rScreenHeight);
	~GBuffer();

	void BindForWriting();
	void BindForReading();
	void SetReadBuffer(GBufferTextureType textureType);

private:
	unsigned int& mrScreenWidth;
	unsigned int& mrScreenHeight;
	unsigned int mFBOId;
	unsigned int mTexturesIds[GBUFFER_NUM_TEXTURES];
	unsigned int mDepthTextureId;

	void AllocateResources();
	void DeallocateResources();

};

#endif