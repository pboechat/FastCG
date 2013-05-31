#if (!defined(DEFERREDRENDERINGSTRATEGY_H_) && !defined(FIXED_FUNCTION_PIPELINE))
#define DEFERREDRENDERINGSTRATEGY_H_

#include <RenderingStrategy.h>
#include <GBuffer.h>
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

	virtual void Render(const Camera* pCamera);

private:
	const static unsigned int NUMBER_OF_RANDOM_SAMPLES;
	const static unsigned int LIGHT_MESH_DETAIL;

	unsigned int& mrScreenWidth;
	unsigned int& mrScreenHeight;
	GBuffer* mpGBuffer;
	Shader* mpStencilPassShader;
	Shader* mpDirectionalLightPassShader;
	Shader* mpPointLightPassShader;
	Shader* mpSSAOHighFrequencyPassShader;
	Shader* mpLineStripShader;
	Shader* mpPointsShader;
	Texture* mpNoiseTexture;
	Mesh* mpQuadMesh;
	Mesh* mpSphereMesh;
	bool mDisplayGBufferEnabled;
	bool mDisplaySSAOTextureEnabled;
	bool mShowPointLightsEnabled;
	std::vector<glm::vec3> mRandomSamplesInAHemisphere;

	void RenderUnlitGeometries(const glm::mat4& view, const glm::mat4& projection);
	float CalculateLightBoundingBoxScale(PointLight* pLight);
	void BuildAuxiliaryMeshes();
	void FindShaders();
	void GenerateNoiseTexture();
	void GenerateRandomSamplesInAHemisphere();

};

#endif