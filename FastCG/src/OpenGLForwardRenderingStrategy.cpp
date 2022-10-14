#ifdef FASTCG_OPENGL

#include <FastCG/ShaderRegistry.h>
#include <FastCG/OpenGLExceptions.h>
#include <FastCG/OpenGLForwardRenderingStrategy.h>

#include <GL/glew.h>
#include <GL/gl.h>

namespace FastCG
{
	void OpenGLForwardRenderingStrategy::OnResourcesLoaded()
	{
		mpLineStripShader = ShaderRegistry::Find("LineStrip");
		mpPointsShader = ShaderRegistry::Find("Points");
	}

	void OpenGLForwardRenderingStrategy::Render(const Camera *pCamera)
	{
#ifdef _DEBUG
		glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, -1, "Forward Rendering");
#endif
		auto &rView = pCamera->GetView();
		auto &rProjection = pCamera->GetProjection();

		mArgs.rRenderingStatistics.Reset();

		glViewport(0, 0, mArgs.rScreenWidth, mArgs.rScreenHeight);

#ifdef _DEBUG
		glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, -1, "Clear Backbuffer");
#endif

		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

		glDepthMask(GL_TRUE);
		glClearColor(mArgs.rClearColor.x, mArgs.rClearColor.y, mArgs.rClearColor.z, mArgs.rClearColor.w);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

#ifdef _DEBUG
		glPopDebugGroup();
#endif

#ifdef _DEBUG
		glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, -1, "Geometry Passes");
#endif

		for (const auto &pRenderingGroup : mArgs.rRenderBatches)
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
				pShader->SetVec4("_AmbientColor", mArgs.rAmbientLight);
				if (pMaterial->IsUnlit() || (mArgs.rDirectionalLights.size() == 0 && mArgs.rPointLights.size() == 0))
				{
					pRenderer->Render();
					mArgs.rRenderingStatistics.drawCalls += pRenderer->GetNumberOfDrawCalls();
					mArgs.rRenderingStatistics.numberOfTriangles += pRenderer->GetNumberOfTriangles();
				}
				else
				{
					glDepthFunc(GL_LEQUAL);

					bool hasSetupMultiPass = false;
					for (size_t i = 0; i < mArgs.rDirectionalLights.size(); i++)
					{
						if (i > 0 && !hasSetupMultiPass)
						{
							glDepthFunc(GL_EQUAL);

							glEnable(GL_BLEND);
							glBlendEquation(GL_FUNC_ADD);
							glBlendFunc(GL_ONE, GL_ONE);

							hasSetupMultiPass = true;
						}

						auto *pDirectionalLight = mArgs.rDirectionalLights[i];
						pShader->SetVec4("_Light0DiffuseColor", pDirectionalLight->GetDiffuseColor());
						pShader->SetVec4("_Light0SpecularColor", pDirectionalLight->GetSpecularColor());
						pShader->SetFloat("_Light0Intensity", pDirectionalLight->GetIntensity());
						pShader->SetVec3("_Light0Position", pDirectionalLight->GetDirection());
						pShader->SetFloat("_Light0Type", -1);
						pRenderer->Render();
						mArgs.rRenderingStatistics.drawCalls += pRenderer->GetNumberOfDrawCalls();
					}

					for (size_t i = 0; i < mArgs.rPointLights.size(); i++)
					{
						if (i > 0 && !hasSetupMultiPass)
						{
							glDepthFunc(GL_EQUAL);
							glEnable(GL_BLEND);
							glBlendEquation(GL_FUNC_ADD);
							glBlendFunc(GL_ONE, GL_ONE);
							hasSetupMultiPass = true;
						}

						auto *pPointLight = mArgs.rPointLights[i];
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
						mArgs.rRenderingStatistics.drawCalls += pRenderer->GetNumberOfDrawCalls();
					}

					glDisable(GL_BLEND);
				}
				mArgs.rRenderingStatistics.numberOfTriangles += pRenderer->GetNumberOfTriangles();
			}

			pShader->Unbind();

			FASTCG_CHECK_OPENGL_ERROR();
		}

#ifdef _DEBUG
		glPopDebugGroup();
#endif

		if (!mArgs.rLineRenderers.empty())
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

			for (auto *pLineRenderer : mArgs.rLineRenderers)
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
				mArgs.rRenderingStatistics.drawCalls += pLineRenderer->GetNumberOfDrawCalls();
			}

			mpLineStripShader->Unbind();

#ifdef _DEBUG
			glPopDebugGroup();
#endif

			FASTCG_CHECK_OPENGL_ERROR();
		}

		if (!mArgs.rPointsRenderers.empty())
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

			for (auto *pPointsRenderer : mArgs.rPointsRenderers)
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
				mArgs.rRenderingStatistics.drawCalls += pPointsRenderer->GetNumberOfDrawCalls();
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

#endif