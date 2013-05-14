#ifdef USE_PROGRAMMABLE_PIPELINE

#include <ForwardRenderingStrategy.h>
#include <ShaderRegistry.h>

ForwardRenderingStrategy::ForwardRenderingStrategy(std::vector<Light*>& rLights,
												   glm::vec4& rGlobalAmbientLight,
												   std::vector<RenderBatch*>& rRenderingGroups,
												   std::vector<LineRenderer*>& rLineRenderers,
												   std::vector<PointsRenderer*>& rPointsRenderer,
												   RenderingStatistics& rRenderingStatistics) :
	RenderingStrategy(rLights, rGlobalAmbientLight, rRenderingGroups, rLineRenderers, rPointsRenderer, rRenderingStatistics)
{
	mpLineStripShader = ShaderRegistry::Find("LineStrip");
	mpPointsShader = ShaderRegistry::Find("Points");
}

void ForwardRenderingStrategy::SetUpShaderLights(Shader* pShader)
{
	static char pLightVariableName[128];

	for (unsigned int i = 0; i < mrLights.size(); i++)
	{
		Light* pLight = mrLights[i];
		glm::vec3& lightPosition = pLight->GetGameObject()->GetTransform()->GetPosition();
		const glm::vec4& lightAmbientColor = pLight->GetAmbientColor();
		const glm::vec4& lightDiffuseColor = pLight->GetDiffuseColor();
		const glm::vec4& lightSpecularColor = pLight->GetSpecularColor();
		sprintf(pLightVariableName, "_Light%dPosition", i);
		pShader->SetVec3(pLightVariableName, lightPosition);
		sprintf(pLightVariableName, "_Light%dAmbientColor", i);
		pShader->SetVec4(pLightVariableName, lightAmbientColor);
		sprintf(pLightVariableName, "_Light%dDiffuseColor", i);
		pShader->SetVec4(pLightVariableName, lightDiffuseColor);
		sprintf(pLightVariableName, "_Light%dSpecularColor", i);
		pShader->SetVec4(pLightVariableName, lightSpecularColor);
	}
}

void ForwardRenderingStrategy::Render(const Camera* pCamera)
{
	mrRenderingStatistics.drawCalls = 0;

	glm::mat4& rView = pCamera->GetView();
	glm::mat4& rProjection = pCamera->GetProjection();

	for (unsigned int i = 0; i < mrRenderBatches.size(); i++)
	{
		RenderBatch* pRenderingGroup = mrRenderBatches[i];
		Material* pMaterial = pRenderingGroup->pMaterial;
		std::vector<MeshFilter*>& rMeshFilters = pRenderingGroup->meshFilters;
		Shader* pShader = pMaterial->GetShader();
		pShader->Bind();
		SetUpShaderLights(pShader);
		pShader->SetMat4("_View", rView);
		pShader->SetMat4("_Projection", rProjection);
		pMaterial->SetUpParameters();

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
			pShader->SetMat4("_Model", rModel);
			pShader->SetMat4("_ModelView", modelView);
			pShader->SetMat3("_ModelViewInverseTranspose", glm::transpose(glm::inverse(glm::mat3(modelView))));
			pShader->SetMat4("_ModelViewProjection", rProjection * modelView);
			pShader->SetVec4("_GlobalLightAmbientColor", mrGlobalAmbientLight);
			pRenderer->Render();
			mrRenderingStatistics.drawCalls++;
		}

		pShader->Unbind();
		// FIXME: shouldn't be necessary if we could guarantee that all textures are unbound after use!
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	if (mrLineRenderers.size() > 0)
	{
		mpLineStripShader->Bind();
		mpLineStripShader->SetMat4("_View", rView);
		mpLineStripShader->SetMat4("_Projection", rProjection);

		for (unsigned int i = 0; i < mrLineRenderers.size(); i++)
		{
			LineRenderer* pLineRenderer = mrLineRenderers[i];

			if (!pLineRenderer->GetGameObject()->IsActive())
			{
				continue;
			}

			const glm::mat4& rModel = pLineRenderer->GetGameObject()->GetTransform()->GetModel();
			glm::mat4 modelView = rView * rModel;
			mpLineStripShader->SetMat4("_Model", rModel);
			mpLineStripShader->SetMat4("_ModelView", modelView);
			mpLineStripShader->SetMat3("_ModelViewInverseTranspose", glm::transpose(glm::inverse(glm::mat3(modelView))));
			mpLineStripShader->SetMat4("_ModelViewProjection", rProjection * modelView);
			mpLineStripShader->SetVec4("_GlobalLightAmbientColor", mrGlobalAmbientLight);
			pLineRenderer->Render();
			mrRenderingStatistics.drawCalls++;
		}

		mpLineStripShader->Unbind();
	}

	if (mrPointsRenderer.size() > 0)
	{
		mpPointsShader->Bind();
		mpPointsShader->SetMat4("_View", rView);
		mpPointsShader->SetMat4("_Projection", rProjection);

		for (unsigned int i = 0; i < mrPointsRenderer.size(); i++)
		{
			PointsRenderer* pPointsRenderer = mrPointsRenderer[i];

			if (!pPointsRenderer->GetGameObject()->IsActive())
			{
				continue;
			}

			const glm::mat4& rModel = pPointsRenderer->GetGameObject()->GetTransform()->GetModel();
			glm::mat4 modelView = rView * rModel;
			mpPointsShader->SetMat4("_Model", rModel);
			mpPointsShader->SetMat4("_ModelView", modelView);
			mpPointsShader->SetMat3("_ModelViewInverseTranspose", glm::transpose(glm::inverse(glm::mat3(modelView))));
			mpPointsShader->SetMat4("_ModelViewProjection", rProjection * modelView);
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