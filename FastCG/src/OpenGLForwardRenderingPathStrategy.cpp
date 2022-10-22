#ifdef FASTCG_OPENGL

#include <FastCG/Renderable.h>
#include <FastCG/OpenGLShader.h>
#include <FastCG/OpenGLForwardRenderingPathStrategy.h>
#include <FastCG/OpenGLExceptions.h>

#include <glm/glm.hpp>

namespace FastCG
{
	void SetupMeshMainSubPass()
	{
		glDepthFunc(GL_LEQUAL);
		glDisable(GL_BLEND);
	}

	void SetupMeshSecondarySubPasses()
	{
		glDepthFunc(GL_EQUAL);
		glEnable(GL_BLEND);
		glBlendEquation(GL_FUNC_ADD);
		glBlendFunc(GL_ONE, GL_ONE);
	}

	void OpenGLForwardRenderingPathStrategy::Render(const Camera *pMainCamera)
	{
		mArgs.rRenderingStatistics.Reset();

		{
#ifdef _DEBUG
			glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, -1, "Forward Rendering Path");
#endif
			{

				glViewport(0, 0, mArgs.rScreenWidth, mArgs.rScreenHeight);

				{
#ifdef _DEBUG
					glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, -1, "Clear Backbuffers");
#endif

					glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

					glDepthMask(GL_TRUE);
					glClearColor(mArgs.rClearColor.x, mArgs.rClearColor.y, mArgs.rClearColor.z, mArgs.rClearColor.w);
					glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

#ifdef _DEBUG
					glPopDebugGroup();
#endif
				}

				{
#ifdef _DEBUG
					glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, -1, "Mesh Passes");
#endif

					UpdateSceneConstantsBuffer(pMainCamera);

					for (const auto *pRenderBatch : mArgs.rRenderBatches)
					{
						const auto *pMaterial = pRenderBatch->pMaterial;

#ifdef _DEBUG
						glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, -1, (pMaterial->GetName() + " Pass").c_str());
#endif

						pMaterial->Bind();

						glBindBufferBase(GL_UNIFORM_BUFFER, OpenGLShader::SCENE_CONSTANTS_BINDING_INDEX, mSceneConstantsBufferId);

						for (const auto *pRenderable : pRenderBatch->renderables)
						{
							if (!pRenderable->GetGameObject()->IsActive())
							{
								continue;
							}

							UpdateInstanceConstantsBuffer(pRenderable->GetGameObject()->GetTransform()->GetModel());

							glBindBufferBase(GL_UNIFORM_BUFFER, OpenGLShader::INSTANCE_CONTANTS_BINDING_INDEX, mInstanceConstantsBufferId);

							const auto *pMesh = pRenderable->GetMesh();

							if (mArgs.rDirectionalLights.size() == 0 && mArgs.rPointLights.size() == 0)
							{
								pMesh->Draw();

								mArgs.rRenderingStatistics.drawCalls++;
								mArgs.rRenderingStatistics.numberOfTriangles += pMesh->GetNumberOfTriangles();
							}
							else
							{
								enum class SubpassType : uint8_t
								{
									ST_NONE = 0,
									ST_MAIN,
									ST_SECONDARY
								};

								SubpassType lastSubpassType{SubpassType::ST_NONE};

								for (size_t i = 0; i < mArgs.rDirectionalLights.size(); i++)
								{
#ifdef _DEBUG
									glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, -1, (pMaterial->GetName() + " Directional Light Sub-Pass (" + std::to_string(i) + ")").c_str());
#endif
									switch (lastSubpassType)
									{
									case SubpassType::ST_NONE:
										SetupMeshMainSubPass();
										break;
									case SubpassType::ST_MAIN:
										SetupMeshSecondarySubPasses();
										break;
									default:
										break;
									}

									FASTCG_CHECK_OPENGL_ERROR();

									const auto *pDirectionalLight = mArgs.rDirectionalLights[i];

									UpdateLightingConstantsBuffer(pDirectionalLight, pDirectionalLight->GetDirection());

									glBindBufferBase(GL_UNIFORM_BUFFER, OpenGLShader::LIGHTING_CONSTANTS_BINDING_INDEX, mLightingConstantsBufferId);

									pMesh->Draw();

									mArgs.rRenderingStatistics.drawCalls++;

									switch (lastSubpassType)
									{
									case SubpassType::ST_NONE:
										lastSubpassType = SubpassType::ST_MAIN;
										break;
									case SubpassType::ST_MAIN:
										lastSubpassType = SubpassType::ST_SECONDARY;
										break;
									default:
										break;
									}

#ifdef _DEBUG
									glPopDebugGroup();
#endif
								}

								for (size_t i = 0; i < mArgs.rPointLights.size(); i++)
								{
#ifdef _DEBUG
									glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, -1, (pMaterial->GetName() + " Point Light Sub-Pass (" + std::to_string(i) + ")").c_str());
#endif

									switch (lastSubpassType)
									{
									case SubpassType::ST_NONE:
										SetupMeshMainSubPass();
										break;
									case SubpassType::ST_MAIN:
										SetupMeshSecondarySubPasses();
										break;
									default:
										break;
									}

									UpdateLightingConstantsBuffer(mArgs.rPointLights[i]);

									glBindBufferBase(GL_UNIFORM_BUFFER, OpenGLShader::LIGHTING_CONSTANTS_BINDING_INDEX, mLightingConstantsBufferId);

									pMesh->Draw();

									mArgs.rRenderingStatistics.drawCalls++;

									switch (lastSubpassType)
									{
									case SubpassType::ST_NONE:
										lastSubpassType = SubpassType::ST_MAIN;
										break;
									case SubpassType::ST_MAIN:
										lastSubpassType = SubpassType::ST_SECONDARY;
										break;
									default:
										break;
									}

#ifdef _DEBUG
									glPopDebugGroup();
#endif
								}

								mArgs.rRenderingStatistics.numberOfTriangles += pMesh->GetNumberOfTriangles();
							}
						}

						pMaterial->Unbind();

						FASTCG_CHECK_OPENGL_ERROR();

#ifdef _DEBUG
						glPopDebugGroup();
#endif
					}

#ifdef _DEBUG
					glPopDebugGroup();
#endif
				}
			}
#ifdef _DEBUG
			glPopDebugGroup();
#endif
		}
	}

}

#endif