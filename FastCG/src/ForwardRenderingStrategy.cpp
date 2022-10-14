#include <FastCG/ShaderRegistry.h>
#include <FastCG/OpenGLExceptions.h>
#include <FastCG/ForwardRenderingStrategy.h>

namespace FastCG
{
	ForwardRenderingStrategy::ForwardRenderingStrategy(const uint32_t &rScreenWidth,
													   const uint32_t &rScreenHeight,
													   const glm::vec4 &rAmbientLight,
													   const std::vector<DirectionalLight *> &rDirectionalLights,
													   const std::vector<PointLight *> &rPointLights,
													   const std::vector<LineRenderer *> &rLineRenderers,
													   const std::vector<PointsRenderer *> &rPointsRenderer,
													   const std::vector<std::unique_ptr<RenderBatch>> &rRenderBatches,
													   RenderingStatistics &rRenderingStatistics) : RenderingPathStrategy(rScreenWidth, rScreenHeight, rAmbientLight, rDirectionalLights, rPointLights, rLineRenderers, rPointsRenderer, rRenderBatches, rRenderingStatistics)
	{
		mpLineStripShader = ShaderRegistry::Find("LineStrip");
		mpPointsShader = ShaderRegistry::Find("Points");
	}

	void ForwardRenderingStrategy::Render(const Camera *pCamera)
	{
#ifdef _DEBUG
		glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, -1, "Forward Rendering");
#endif
		auto &rView = pCamera->GetView();
		auto &rProjection = pCamera->GetProjection();

		mrRenderingStatistics.Reset();

		glViewport(0, 0, mrScreenWidth, mrScreenHeight);

#ifdef _DEBUG
		glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, -1, "Clear Backbuffer");
#endif

		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

		glDepthMask(GL_TRUE);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

#ifdef _DEBUG
		glPopDebugGroup();
#endif

#ifdef _DEBUG
		glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, -1, "Geometry Passes");
#endif

		for (const auto &pRenderingGroup : mrRenderBatches)
		{
			const auto &pMaterial = pRenderingGroup->pMaterial;

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
			glDisable(GL_STENCIL_TEST);
			glStencilMask(0);

			if (pMaterial->IsTwoSided())
			{
				glDisable(GL_CULL_FACE);
			}
			else
			{
				glEnable(GL_CULL_FACE);
				glCullFace(GL_BACK);
			}

			auto &rMeshFilters = pRenderingGroup->meshFilters;
			auto pShader = pMaterial->GetShader();
			pShader->Bind();
			pShader->SetMat4("_View", rView);
			pShader->SetMat4("_Projection", rProjection);
			pMaterial->SetUpParameters();

			for (auto *pMeshFilter : rMeshFilters)
			{
				if (!pMeshFilter->GetGameObject()->IsActive())
				{
					continue;
				}

				auto *pRenderer = pMeshFilter->GetGameObject()->GetRenderer();
				if (pRenderer == nullptr)
				{
					continue;
				}

				const auto &rModel = pRenderer->GetGameObject()->GetTransform()->GetModel();
				auto modelView = rView * rModel;
				pShader->SetMat4("_Model", rModel);
				pShader->SetMat4("_ModelView", modelView);
				pShader->SetMat3("_ModelViewInverseTranspose", glm::transpose(glm::inverse(glm::mat3(modelView))));
				pShader->SetMat4("_ModelViewProjection", rProjection * modelView);
				pShader->SetVec4("_AmbientColor", mrAmbientLight);
				if (pMaterial->IsUnlit() || (mrDirectionalLights.size() == 0 && mrPointLights.size() == 0))
				{
					pRenderer->Render();
					mrRenderingStatistics.drawCalls += pRenderer->GetNumberOfDrawCalls();
					mrRenderingStatistics.numberOfTriangles += pRenderer->GetNumberOfTriangles();
				}
				else
				{
					glDepthFunc(GL_LEQUAL);

					bool hasSetupMultiPass = false;
					for (size_t i = 0; i < mrDirectionalLights.size(); i++)
					{
						if (i > 0 && !hasSetupMultiPass)
						{
							glDepthFunc(GL_EQUAL);

							glEnable(GL_BLEND);
							glBlendEquation(GL_FUNC_ADD);
							glBlendFunc(GL_ONE, GL_ONE);

							hasSetupMultiPass = true;
						}

						auto *pDirectionalLight = mrDirectionalLights[i];
						pShader->SetVec4("_Light0DiffuseColor", pDirectionalLight->GetDiffuseColor());
						pShader->SetVec4("_Light0SpecularColor", pDirectionalLight->GetSpecularColor());
						pShader->SetFloat("_Light0Intensity", pDirectionalLight->GetIntensity());
						pShader->SetVec3("_Light0Position", pDirectionalLight->GetDirection());
						pShader->SetFloat("_Light0Type", -1);
						pRenderer->Render();
						mrRenderingStatistics.drawCalls += pRenderer->GetNumberOfDrawCalls();
					}

					for (size_t i = 0; i < mrPointLights.size(); i++)
					{
						if (i > 0 && !hasSetupMultiPass)
						{
							glDepthFunc(GL_EQUAL);
							glEnable(GL_BLEND);
							glBlendEquation(GL_FUNC_ADD);
							glBlendFunc(GL_ONE, GL_ONE);
							hasSetupMultiPass = true;
						}

						auto *pPointLight = mrPointLights[i];
						pShader->SetVec4("_Light0DiffuseColor", pPointLight->GetDiffuseColor());
						pShader->SetVec4("_Light0SpecularColor", pPointLight->GetSpecularColor());
						pShader->SetFloat("_Light0Intensity", pPointLight->GetIntensity());
						auto lightPosition = glm::vec3(rView * glm::vec4(pPointLight->GetGameObject()->GetTransform()->GetPosition(), 1));
						pShader->SetVec3("_Light0Position", lightPosition);
						pShader->SetFloat("_Light0ConstantAttenuation", pPointLight->GetConstantAttenuation());
						pShader->SetFloat("_Light0LinearAttenuation", pPointLight->GetLinearAttenuation());
						pShader->SetFloat("_Light0QuadraticAttenuation", pPointLight->GetQuadraticAttenuation());
						pShader->SetFloat("_Light0Type", 1);
						pRenderer->Render();
						mrRenderingStatistics.drawCalls += pRenderer->GetNumberOfDrawCalls();
					}

					glDisable(GL_BLEND);
				}
				mrRenderingStatistics.numberOfTriangles += pRenderer->GetNumberOfTriangles();
			}

			pShader->Unbind();

			FASTCG_CHECK_OPENGL_ERROR();
		}

#ifdef _DEBUG
		glPopDebugGroup();
#endif

		if (!mrLineRenderers.empty())
		{
#ifdef _DEBUG
			glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, -1, "Line Passes");
#endif

			glDisable(GL_DEPTH_TEST);
			glDepthMask(GL_FALSE);
			glDisable(GL_STENCIL_TEST);
			glStencilMask(0);
			glDisable(GL_CULL_FACE);

			mpLineStripShader->Bind();
			mpLineStripShader->SetMat4("_View", rView);
			mpLineStripShader->SetMat4("_Projection", rProjection);

			for (auto *pLineRenderer : mrLineRenderers)
			{
				if (!pLineRenderer->GetGameObject()->IsActive())
				{
					continue;
				}

				const auto &rModel = pLineRenderer->GetGameObject()->GetTransform()->GetModel();
				auto modelView = rView * rModel;
				mpLineStripShader->SetMat4("_Model", rModel);
				mpLineStripShader->SetMat4("_ModelView", modelView);
				mpLineStripShader->SetMat3("_ModelViewInverseTranspose", glm::transpose(glm::inverse(glm::mat3(modelView))));
				mpLineStripShader->SetMat4("_ModelViewProjection", rProjection * modelView);
				pLineRenderer->Render();
				mrRenderingStatistics.drawCalls += pLineRenderer->GetNumberOfDrawCalls();
			}

			mpLineStripShader->Unbind();

#ifdef _DEBUG
			glPopDebugGroup();
#endif

			FASTCG_CHECK_OPENGL_ERROR();
		}

		if (!mrPointsRenderer.empty())
		{
#ifdef _DEBUG
			glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, -1, "Point Passes");
#endif

			glDisable(GL_DEPTH_TEST);
			glDepthMask(GL_FALSE);
			glDisable(GL_STENCIL_TEST);
			glStencilMask(0);
			glDisable(GL_CULL_FACE);

			mpPointsShader->Bind();
			mpPointsShader->SetMat4("_View", rView);
			mpPointsShader->SetMat4("_Projection", rProjection);

			for (auto *pPointsRenderer : mrPointsRenderer)
			{
				if (!pPointsRenderer->GetGameObject()->IsActive())
				{
					continue;
				}

				const auto &rModel = pPointsRenderer->GetGameObject()->GetTransform()->GetModel();
				auto modelView = rView * rModel;
				mpPointsShader->SetMat4("_Model", rModel);
				mpPointsShader->SetMat4("_ModelView", modelView);
				mpPointsShader->SetMat3("_ModelViewInverseTranspose", glm::transpose(glm::inverse(glm::mat3(modelView))));
				mpPointsShader->SetMat4("_ModelViewProjection", rProjection * modelView);
				auto *pPoints = pPointsRenderer->GetPoints();

				if (pPoints != 0)
				{
					mpPointsShader->SetFloat("size", pPoints->GetSize());
				}

				pPointsRenderer->Render();
				mrRenderingStatistics.drawCalls += pPointsRenderer->GetNumberOfDrawCalls();
			}

			mpPointsShader->Unbind();

#ifdef _DEBUG
			glPopDebugGroup();
#endif

			FASTCG_CHECK_OPENGL_ERROR();
		}

#ifdef _DEBUG
		glPopDebugGroup();
#endif
	}

}