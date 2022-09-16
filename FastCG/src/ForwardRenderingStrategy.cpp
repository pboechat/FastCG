#include <FastCG/ShaderRegistry.h>
#include <FastCG/ForwardRenderingStrategy.h>

namespace FastCG
{
	ForwardRenderingStrategy::ForwardRenderingStrategy(const uint32_t& rScreenWidth,
		const uint32_t& rScreenHeight,
		const std::vector<Light*>& rLights,
		const std::vector<DirectionalLight*>& rDirectionalLights,
		const std::vector<PointLight*>& rPointLights,
		const glm::vec4& rGlobalAmbientLight,
		const std::vector<std::unique_ptr<RenderBatch>>& rRenderingGroups,
		const std::vector<LineRenderer*>& rLineRenderers,
		const std::vector<PointsRenderer*>& rPointsRenderer,
		RenderingStatistics& rRenderingStatistics) :
		RenderingStrategy(rScreenWidth, rScreenHeight, rLights, rDirectionalLights, rPointLights, rGlobalAmbientLight, rRenderingGroups, rLineRenderers, rPointsRenderer, rRenderingStatistics)
	{
		mpLineStripShader = ShaderRegistry::Find("LineStrip");
		mpPointsShader = ShaderRegistry::Find("Points");
	}

	void ForwardRenderingStrategy::Render(const Camera* pCamera)
	{
		glViewport(0, 0, mrScreenWidth, mrScreenHeight);

		glDepthMask(GL_TRUE);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		auto& rView = pCamera->GetView();
		auto& rProjection = pCamera->GetProjection();

		mrRenderingStatistics.Reset();
		for (const auto& pRenderingGroup : mrRenderBatches)
		{
			const auto& pMaterial = pRenderingGroup->pMaterial;

			if (pMaterial->HasDepth())
			{
				glEnable(GL_DEPTH_TEST);
				glDepthMask(GL_TRUE);
			}
			else
			{
				glDisable(GL_DEPTH_TEST);
				glDepthMask(GL_FALSE);
			}

			if (pMaterial->IsTwoSided())
			{
				glDisable(GL_CULL_FACE);
			}
			else
			{
				glEnable(GL_CULL_FACE);
				glCullFace(GL_BACK);
			}

			auto& rMeshFilters = pRenderingGroup->meshFilters;
			auto pShader = pMaterial->GetShader();
			pShader->Bind();
			pShader->SetMat4("_View", rView);
			pShader->SetMat4("_Projection", rProjection);
			pMaterial->SetUpParameters();

			for (auto* pMeshFilter : rMeshFilters)
			{
				if (!pMeshFilter->GetGameObject()->IsActive())
				{
					continue;
				}

				auto* pRenderer = pMeshFilter->GetGameObject()->GetRenderer();
				if (pRenderer == nullptr)
				{
					continue;
				}

				const auto& rModel = pRenderer->GetGameObject()->GetTransform()->GetModel();
				auto modelView = rView * rModel;
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
					glDepthFunc(GL_LEQUAL);

					for (size_t i = 0; i < mrLights.size(); i++)
					{
						if (i == 1)
						{
							glDepthFunc(GL_EQUAL);
							glEnable(GL_BLEND);
							glBlendEquation(GL_FUNC_ADD);
							glBlendFunc(GL_ONE, GL_ONE);
						}

						auto* pLight = mrLights[i];
						pShader->SetVec4("_Light0AmbientColor", pLight->GetAmbientColor());
						pShader->SetVec4("_Light0DiffuseColor", pLight->GetDiffuseColor());
						pShader->SetVec4("_Light0SpecularColor", pLight->GetSpecularColor());
						pShader->SetFloat("_Light0Intensity", pLight->GetIntensity());

						if (pLight->GetType().IsExactly(DirectionalLight::TYPE))
						{
							pShader->SetVec3("_Light0Position", pLight->GetGameObject()->GetTransform()->GetPosition());
							pShader->SetFloat("_Light0Type", -1.0);
						}
						else if (pLight->GetType().IsExactly(PointLight::TYPE))
						{
							auto lightPosition = glm::vec3(rView * glm::vec4(pLight->GetGameObject()->GetTransform()->GetPosition(), 1.0f));
							pShader->SetVec3("_Light0Position", lightPosition);

							auto* pPointLight = dynamic_cast<PointLight*>(pLight);
							pShader->SetFloat("_Light0ConstantAttenuation", pPointLight->GetConstantAttenuation());
							pShader->SetFloat("_Light0LinearAttenuation", pPointLight->GetLinearAttenuation());
							pShader->SetFloat("_Light0QuadraticAttenuation", pPointLight->GetQuadraticAttenuation());
							pShader->SetFloat("_Light0Type", 1.0);
						}
						else
						{
							THROW_EXCEPTION(Exception, "Unhandled light type: %s", pLight->GetType().GetName().c_str());
						}

						pRenderer->Render();
						mrRenderingStatistics.drawCalls += pRenderer->GetNumberOfDrawCalls();
					}

					glDisable(GL_BLEND);
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

			for (auto* pLineRenderer : mrLineRenderers)
			{
				if (!pLineRenderer->GetGameObject()->IsActive())
				{
					continue;
				}

				const auto& rModel = pLineRenderer->GetGameObject()->GetTransform()->GetModel();
				auto modelView = rView * rModel;
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

			for (auto* pPointsRenderer : mrPointsRenderer)
			{
				if (!pPointsRenderer->GetGameObject()->IsActive())
				{
					continue;
				}

				const auto& rModel = pPointsRenderer->GetGameObject()->GetTransform()->GetModel();
				auto modelView = rView * rModel;
				mpPointsShader->SetMat4("_Model", rModel);
				mpPointsShader->SetMat4("_ModelView", modelView);
				mpPointsShader->SetMat3("_ModelViewInverseTranspose", glm::transpose(glm::inverse(glm::mat3(modelView))));
				mpPointsShader->SetMat4("_ModelViewProjection", rProjection * modelView);
				mpPointsShader->SetVec4("_GlobalLightAmbientColor", mrGlobalAmbientLight);
				auto* pPoints = pPointsRenderer->GetPoints();

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

}