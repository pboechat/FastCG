#ifndef FIXED_FUNCTION_PIPELINE

#include <ForwardRenderingStrategy.h>
#include <ShaderRegistry.h>

ForwardRenderingStrategy::ForwardRenderingStrategy(std::vector<Light*>& rLights,
												   std::vector<DirectionalLight*>& rDirectionalLights,
												   std::vector<PointLight*>& rPointLights,
												   glm::vec4& rGlobalAmbientLight,
												   std::vector<RenderBatch*>& rRenderingGroups,
												   std::vector<LineRenderer*>& rLineRenderers,
												   std::vector<PointsRenderer*>& rPointsRenderer,
												   RenderingStatistics& rRenderingStatistics) :
	RenderingStrategy(rLights, rDirectionalLights, rPointLights, rGlobalAmbientLight, rRenderingGroups, rLineRenderers, rPointsRenderer, rRenderingStatistics)
{
	mpLineStripShader = ShaderRegistry::Find("LineStrip");
	mpPointsShader = ShaderRegistry::Find("Points");
}

void ForwardRenderingStrategy::Render(const Camera* pCamera)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::mat4& rView = pCamera->GetView();
	glm::mat4& rProjection = pCamera->GetProjection();

	mrRenderingStatistics.Reset();
	for (unsigned int i = 0; i < mrRenderBatches.size(); i++)
	{
		RenderBatch* pRenderingGroup = mrRenderBatches[i];
		Material* pMaterial = pRenderingGroup->pMaterial;
		std::vector<MeshFilter*>& rMeshFilters = pRenderingGroup->meshFilters;
		Shader* pShader = pMaterial->GetShader();
		pShader->Bind();
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
			if (pMaterial->IsUnlit() || mrLights.size() == 0)
			{
				pRenderer->Render();
				mrRenderingStatistics.drawCalls += pRenderer->GetNumberOfDrawCalls();
				mrRenderingStatistics.numberOfTriangles += pRenderer->GetNumberOfTriangles();
			}
			else
			{
				for (unsigned int i = 0; i < mrLights.size(); i++)
				{
					if (i == 1)
					{
						glDepthFunc(GL_EQUAL);
						glEnable(GL_BLEND);
						glBlendEquation(GL_FUNC_ADD);
						glBlendFunc(GL_ONE, GL_ONE);
					}

					Light* pLight = mrLights[i];
					pShader->SetVec3("_Light0Position", pLight->GetGameObject()->GetTransform()->GetPosition());
					pShader->SetVec4("_Light0AmbientColor", pLight->GetAmbientColor());
					pShader->SetVec4("_Light0DiffuseColor", pLight->GetDiffuseColor());
					pShader->SetVec4("_Light0SpecularColor", pLight->GetSpecularColor());
					pShader->SetFloat("_Light0Intensity", pLight->GetIntensity());

					if (pLight->GetType().IsExactly(DirectionalLight::TYPE))
					{
						pShader->SetFloat("_Light0Type", 0.0);
					}
					else if (pLight->GetType().IsExactly(PointLight::TYPE))
					{
						PointLight* pPointLight = dynamic_cast<PointLight*>(pLight);
						pShader->SetFloat("_Light0ConstantAttenuation", pPointLight->GetConstantAttenuation());
						pShader->SetFloat("_Light0LinearAttenuation", pPointLight->GetLinearAttenuation());
						pShader->SetFloat("_Light0QuadraticAttenuation", pPointLight->GetQuadraticAttenuation());
						pShader->SetFloat("_Light0Type", 1.0);
					}
					/*else if (pLight->GetType().IsExactly(SpotLight::TYPE))
					{
						pShader->SetVec3("_Light0SpotDirection", pLight->GetSpotDirection());
						pShader->SetFloat("_Light0SpotCutoff", pLight->GetSpotCutoff());
						pShader->SetFloat("_Light0SpotExponent", pLight->GetSpotExponent());
					}*/
					else 
					{
						// FIXME: checking invariants
						THROW_EXCEPTION(Exception, "Unknown light type: %s", pLight->GetType().GetName().c_str());
					}

					pRenderer->Render();
					mrRenderingStatistics.drawCalls += pRenderer->GetNumberOfDrawCalls();
				}
				glDisable(GL_BLEND);
				glDepthFunc(GL_LESS);
			}
			mrRenderingStatistics.numberOfTriangles += pRenderer->GetNumberOfTriangles();
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
			mrRenderingStatistics.drawCalls += pLineRenderer->GetNumberOfDrawCalls();
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
			mrRenderingStatistics.drawCalls += pPointsRenderer->GetNumberOfDrawCalls();
		}

		mpPointsShader->Unbind();
	}
}

#endif