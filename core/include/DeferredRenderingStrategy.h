#if (!defined(DEFERREDRENDERINGSTRATEGY_H_) && defined(USE_PROGRAMMABLE_PIPELINE))
#define DEFERREDRENDERINGSTRATEGY_H_

#include <RenderingStrategy.h>
#include <GBuffer.h>
#include <Shader.h>
#include <Mesh.h>

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

	inline bool IsDebugEnabled() const
	{
		return mDebugEnabled;
	}

	inline void SetDebugEnabled(bool debugEnabled)
	{
		mDebugEnabled = debugEnabled;
	}

	virtual void Render(const Camera* pCamera);

private:
	unsigned int& mrScreenWidth;
	unsigned int& mrScreenHeight;
	GBuffer* mpGBuffer;
	Shader* mpStencilPassShader;
	Shader* mpDirectionalLightPassShader;
	Shader* mpPointLightPassShader;
	Shader* mpLineStripShader;
	Shader* mpPointsShader;
	Mesh* mpQuadMesh;
	Mesh* mpSphereMesh;
	bool mDebugEnabled;

	void RenderUnlitGeometries(const glm::mat4& view, const glm::mat4& projection);
	float CalculateLightBoundingBoxScale(Light* pLight);
};

#endif