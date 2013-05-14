#ifdef USE_PROGRAMMABLE_PIPELINE

#include <DeferredRenderingStrategy.h>
#include <ShaderRegistry.h>

DeferredRenderingStrategy::DeferredRenderingStrategy(unsigned int& rScreenWidth,
													 unsigned int& rScreenHeight,
													 std::vector<Light*>& rLights,
													 glm::vec4& rGlobalAmbientLight,
													 std::vector<RenderBatch*>& rRenderingGroups,
													 std::vector<LineRenderer*>& rLineRenderers,
													 std::vector<PointsRenderer*>& rPointsRenderer,
													 RenderingStatistics& rRenderingStatistics) :
	RenderingStrategy(rLights, rGlobalAmbientLight, rRenderingGroups, rLineRenderers, rPointsRenderer, rRenderingStatistics)
{
	mpGBuffer = new GBuffer(rScreenWidth, rScreenHeight);
	mpGeometryPassShader = ShaderRegistry::Find("GeometryPass");
	mpLineStripShader = ShaderRegistry::Find("LineStrip");
	mpPointsShader = ShaderRegistry::Find("Points");
}

DeferredRenderingStrategy::~DeferredRenderingStrategy()
{
	delete mpGBuffer;
}

void DeferredRenderingStrategy::Render(const Camera* pCamera)
{
	mrRenderingStatistics.drawCalls = 0;

	// geometry pass
	mpGBuffer->BindForWriting();

	glm::mat4& view = pCamera->GetView();
	glm::mat4& projection = pCamera->GetProjection();

	for (unsigned int i = 0; i < mrRenderBatches.size(); i++)
	{
		RenderBatch* pRenderingGroup = mrRenderBatches[i];
		std::vector<MeshFilter*>& rMeshFilters = pRenderingGroup->meshFilters;
		Shader* pShader = mpGeometryPassShader;
		pShader->Bind();
		pShader->SetMat4("_View", view);
		pShader->SetMat4("_Projection", projection);

		for (unsigned int j = 0; j < rMeshFilters.size(); j++)
		{
			MeshFilter* pMeshFilter = rMeshFilters[j];

			if (!pMeshFilter->GetGameObject()->IsActive())
			{
				continue;
			}

			Renderer* pRenderer = pMeshFilter->GetGameObject()->GetRenderer();

			if (pRenderer == 0)
			{
				continue;
			}

			const glm::mat4& rModel = pRenderer->GetGameObject()->GetTransform()->GetModel();
			glm::mat4 modelView = view * rModel;
			pShader->SetMat4("_Model", rModel);
			pShader->SetMat4("_ModelView", modelView);
			pShader->SetMat3("_ModelViewInverseTranspose", glm::transpose(glm::inverse(glm::mat3(modelView))));
			pShader->SetMat4("_ModelViewProjection", projection * modelView);
			pShader->SetVec4("_GlobalLightAmbientColor", mrGlobalAmbientLight);
			pRenderer->Render();
			mrRenderingStatistics.drawCalls++;
		}

		pShader->Unbind();
		// FIXME: shouldn't be necessary if we could guarantee that all textures are unbound after use!
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	// lighting pass
	mpGBuffer->BindForReading();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	RenderUnlitGeometries(view, projection);
}

void DeferredRenderingStrategy::RenderUnlitGeometries(const glm::mat4& view, const glm::mat4& projection)
{
	if (mrLineRenderers.size() > 0)
	{
		mpLineStripShader->Bind();
		mpLineStripShader->SetMat4("_View", view);
		mpLineStripShader->SetMat4("_Projection", projection);

		for (unsigned int i = 0; i < mrLineRenderers.size(); i++)
		{
			LineRenderer* pLineRenderer = mrLineRenderers[i];

			if (!pLineRenderer->GetGameObject()->IsActive())
			{
				continue;
			}

			const glm::mat4& rModel = pLineRenderer->GetGameObject()->GetTransform()->GetModel();
			glm::mat4 modelView = view * rModel;
			mpLineStripShader->SetMat4("_Model", rModel);
			mpLineStripShader->SetMat4("_ModelView", modelView);
			mpLineStripShader->SetMat3("_ModelViewInverseTranspose", glm::transpose(glm::inverse(glm::mat3(modelView))));
			mpLineStripShader->SetMat4("_ModelViewProjection", projection * modelView);
			mpLineStripShader->SetVec4("_GlobalLightAmbientColor", mrGlobalAmbientLight);
			pLineRenderer->Render();
			mrRenderingStatistics.drawCalls++;
		}

		mpLineStripShader->Unbind();
	}

	if (mrPointsRenderer.size() > 0)
	{
		mpPointsShader->Bind();
		mpPointsShader->SetMat4("_View", view);
		mpPointsShader->SetMat4("_Projection", projection);

		for (unsigned int i = 0; i < mrPointsRenderer.size(); i++)
		{
			PointsRenderer* pPointsRenderer = mrPointsRenderer[i];

			if (!pPointsRenderer->GetGameObject()->IsActive())
			{
				continue;
			}

			const glm::mat4& rModel = pPointsRenderer->GetGameObject()->GetTransform()->GetModel();
			glm::mat4 modelView = view * rModel;
			mpPointsShader->SetMat4("_Model", rModel);
			mpPointsShader->SetMat4("_ModelView", modelView);
			mpPointsShader->SetMat3("_ModelViewInverseTranspose", glm::transpose(glm::inverse(glm::mat3(modelView))));
			mpPointsShader->SetMat4("_ModelViewProjection", projection * modelView);
			mpPointsShader->SetVec4("_GlobalLightAmbientColor", mrGlobalAmbientLight);
			Points* pPoints = pPointsRenderer->GetPoints();

			if (pPoints != 0)
			{
				mpPointsShader->SetFloat("size", pPoints->GetSize());
			}

			pPointsRenderer->Render();
			mrRenderingStatistics.drawCalls++;
		}

		mpPointsShader->Unbind();
	}
}

#endif
