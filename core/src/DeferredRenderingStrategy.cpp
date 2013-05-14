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
	RenderingStrategy(rLights, rGlobalAmbientLight, rRenderingGroups, rLineRenderers, rPointsRenderer, rRenderingStatistics),
	mrScreenWidth(rScreenWidth),
	mrScreenHeight(rScreenHeight)
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

	glm::mat4& rView = pCamera->GetView();
	glm::mat4& rProjection = pCamera->GetProjection();

	mpGeometryPassShader->Bind();
	for (unsigned int i = 0; i < mrRenderBatches.size(); i++)
	{
		RenderBatch* pRenderingGroup = mrRenderBatches[i];
		std::vector<MeshFilter*>& rMeshFilters = pRenderingGroup->meshFilters;

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
			glm::mat4 modelView = rView * rModel;
			mpGeometryPassShader->SetMat3("_ModelViewInverseTranspose", glm::transpose(glm::inverse(glm::mat3(modelView))));
			mpGeometryPassShader->SetMat4("_ModelViewProjection", rProjection * modelView);
			pRenderer->Render();
			mrRenderingStatistics.drawCalls++;
		}
	}
	mpGeometryPassShader->Unbind();

	// lighting pass
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	mpGBuffer->BindForReading();

	// DEBUG:

	unsigned int halfScreenWidth = mrScreenWidth / 2;
	unsigned int halfScreenHeight = mrScreenHeight / 2;

	mpGBuffer->SetReadBuffer(GBuffer::GBUFFER_TEXTURE_TYPE_POSITION); 
	glBlitFramebuffer(0, 0, mrScreenWidth, mrScreenHeight, 0, 0, halfScreenWidth, halfScreenHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);

	mpGBuffer->SetReadBuffer(GBuffer::GBUFFER_TEXTURE_TYPE_DIFFUSE); 
	glBlitFramebuffer(0, 0, mrScreenWidth, mrScreenHeight, 0, halfScreenHeight, halfScreenWidth, mrScreenHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);

	mpGBuffer->SetReadBuffer(GBuffer::GBUFFER_TEXTURE_TYPE_NORMAL); 
	glBlitFramebuffer(0, 0, mrScreenWidth, mrScreenHeight, halfScreenWidth, halfScreenHeight, mrScreenWidth, mrScreenHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);

	mpGBuffer->SetReadBuffer(GBuffer::GBUFFER_TEXTURE_TYPE_TEXCOORD); 
	glBlitFramebuffer(0, 0, mrScreenWidth, mrScreenHeight, halfScreenWidth, 0, mrScreenWidth, halfScreenHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);

	RenderUnlitGeometries(rView, rProjection);
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
