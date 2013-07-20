#ifndef DEFERREDRENDERINGSTRATEGY_H_
#define DEFERREDRENDERINGSTRATEGY_H_

#include <RenderingStrategy.h>
#include <Texture.h>
#include <Shader.h>
#include <Mesh.h>

#include <glm/glm.hpp>

#include <vector>

class DeferredRenderingStrategy : public RenderingStrategy
{
public:
	DeferredRenderingStrategy(std::vector<Light*>& rLights,
							  std::vector<DirectionalLight*>& rDirectionalLights,
							  std::vector<PointLight*>& rPointLights,
							  glm::vec4& rGlobalAmbientLight,
							  std::vector<RenderBatch*>& rRenderingGroups,
							  std::vector<LineRenderer*>& rLineRenderers,
							  std::vector<PointsRenderer*>& rPointsRenderer,
							  RenderingStatistics& rRenderingStatistics,
							  unsigned int& rScreenWidth,
							  unsigned int& rScreenHeight);

	virtual ~DeferredRenderingStrategy();

	inline bool IsDisplayGBufferEnabled() const
	{
		return mDisplayGBufferEnabled;
	}

	inline void SetDisplayGBufferEnabled(bool displayGBufferEnabled)
	{
		mDisplayGBufferEnabled = displayGBufferEnabled;
	}

	inline bool IsDisplaySSAOTextureEnabled() const
	{
		return mDisplaySSAOTextureEnabled;
	}

	inline void SetDisplaySSAOTextureEnabled(bool displaySSAOTextureEnabled)
	{
		mDisplaySSAOTextureEnabled = displaySSAOTextureEnabled;
	}

	inline bool IsShowPointLightsEnabled() const
	{
		return mShowPointLightsEnabled;
	}

	inline void SetShowPointLightsEnabled(bool showPointLightsEnabled)
	{
		mShowPointLightsEnabled = showPointLightsEnabled;
	}

	inline void SetSSAORadius(float ssaoRayLength)
	{
		mSSAORadius = ssaoRayLength;
	}

	inline float GetSSAORadius() const
	{
		return mSSAORadius;
	}

	inline void SetSSAODistanceScale(float ssaoDistanceScale)
	{
		mSSAODistanceScale = ssaoDistanceScale;
	}

	inline float GetSSAODistanceScale() const
	{
		return mSSAODistanceScale;
	}

	inline bool IsSSAOBlurEnabled() const
	{
		return mSSAOBlurEnabled;
	}

	inline void SetSSAOBlurEnabled(bool ssaoBlurEnabled)
	{
		mSSAOBlurEnabled = ssaoBlurEnabled;
	}
	
	virtual void Render(const Camera* pCamera);

private:
	const static unsigned int NUMBER_OF_RANDOM_SAMPLES;
	const static unsigned int LIGHT_MESH_DETAIL;
	const static float DEFAULT_SSAO_RADIUS; 
	const static float DEFAULT_SSAO_DISTANCE_SCALE;
	const static unsigned int DEFAULT_SSAO_EXPONENT; 
	const static unsigned int NOISE_TEXTURE_WIDTH;
	const static unsigned int NOISE_TEXTURE_HEIGHT;
	const static unsigned int NOISE_TEXTURE_SIZE;

	unsigned int& mrScreenWidth;
	unsigned int& mrScreenHeight;
	unsigned int mHalfScreenWidth;
	unsigned int mHalfScreenHeight;
	float mAspectRatio;
	unsigned int mGBufferFBOId;
	unsigned int mSSAOFBOId;
	unsigned int mSSAOBlurFBOId;
	unsigned int mNormalTextureId;
	unsigned int mDiffuseTextureId;
	unsigned int mSpecularTextureId;
	unsigned int mDepthTextureId;
	unsigned int mSSAOTextureId;
	unsigned int mBlurredSSAOTextureId;
	unsigned int mFinalOutputTextureId;
	Shader* mpStencilPassShader;
	Shader* mpDirectionalLightPassShader;
	Shader* mpPointLightPassShader;
	Shader* mpSSAOHighFrequencyPassShader;
	Shader* mpSSAOBlurPassShader;
	Shader* mpLineStripShader;
	Shader* mpPointsShader;
	Texture* mpNoiseTexture;
	Mesh* mpQuadMesh;
	Mesh* mpSphereMesh;
	bool mDisplayGBufferEnabled;
	bool mDisplaySSAOTextureEnabled;
	bool mShowPointLightsEnabled;
	float mSSAORadius;
	float mSSAODistanceScale;
	bool mSSAOBlurEnabled;
	std::vector<glm::vec3> mRandomSamples;

	void BuildAuxiliaryMeshes();
	void FindShaders();
	void GenerateNoiseTexture();
	void GenerateRandomSamplesInAHemisphere();
	void AllocateTexturesAndFBOs();
	void RenderUnlitGeometries(const glm::mat4& view, const glm::mat4& projection);
	float CalculateLightBoundingBoxScale(PointLight* pLight);

};

#endif