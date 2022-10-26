#ifdef FASTCG_OPENGL

#include <FastCG/StandardGeometries.h>
#include <FastCG/Random.h>
#include <FastCG/OpenGLTexture.h>
#include <FastCG/OpenGLShader.h>
#include <FastCG/OpenGLRenderingSystem.h>
#include <FastCG/OpenGLMesh.h>
#include <FastCG/OpenGLDeferredRenderingPathStrategy.h>
#include <FastCG/OpenGLExceptions.h>
#include <FastCG/MathT.h>

#include <GL/glew.h>
#include <GL/gl.h>

#include <glm/gtc/matrix_transform.hpp>

namespace
{
	template <size_t N>
	void GenerateRandomSamplesInAHemisphere(glm::vec4 randomSamples[N])
	{
		FastCG::Random::SeedWithTime();
		for (uint32_t i = 0; i < N; i++)
		{
			glm::vec3 randomSample{FastCG::Random::NextFloat() * 2 - 1, FastCG::Random::NextFloat() * 2 - 1, FastCG::Random::NextFloat()};
			randomSample = glm::normalize(randomSample);
			auto scale = i / (float)N;
			scale = FastCG::MathF::Lerp(0.1f, 1.0f, FastCG::MathF::Pow(scale, 2));
			randomSample *= scale;
			randomSamples[i] = glm::vec4{randomSample, 0};
		}
	}

	float CalculateLightBoundingSphereScale(const FastCG::PointLight *pPointLight)
	{
		auto uDiffuseColor = pPointLight->GetDiffuseColor();
		auto maxChannel = FastCG::MathF::Max(FastCG::MathF::Max(uDiffuseColor.r, uDiffuseColor.g), uDiffuseColor.b);
		auto c = maxChannel * pPointLight->GetIntensity();
		return (8.0f * FastCG::MathF::Sqrt(c) + 1.0f);
	}

}

namespace FastCG
{
	const uint32_t OpenGLDeferredRenderingPathStrategy::LIGHT_MESH_DETAIL = 20;

	OpenGLDeferredRenderingPathStrategy::OpenGLDeferredRenderingPathStrategy(const RenderingPathStrategyArgs &rArgs) : OpenGLRenderingPathStrategy(rArgs),
																													   mAspectRatio(mArgs.rScreenWidth / (float)mArgs.rScreenHeight)
	{
		GenerateRandomSamplesInAHemisphere<NUMBER_OF_RANDOM_SAMPLES>(mSSAOHighFrequencyPassConstants.randomSamples);
	}

	void OpenGLDeferredRenderingPathStrategy::Initialize()
	{
		OpenGLRenderingPathStrategy::Initialize();

		AllocateRenderTargetsAndFBOs();
		CreateAuxiliaryMeshes();
		CreateAuxiliaryTextures();
	}

	void OpenGLDeferredRenderingPathStrategy::PostInitialize()
	{
		FindShaders();
	}

	void OpenGLDeferredRenderingPathStrategy::Finalize()
	{
		OpenGLRenderingPathStrategy::Finalize();

		if (mGBufferFBOId != ~0u)
		{
			glDeleteFramebuffers(1, &mGBufferFBOId);
		}

		if (mSSAOFBOId != ~0u)
		{
			glDeleteFramebuffers(1, &mSSAOFBOId);
		}

		if (mSSAOBlurFBOId != ~0u)
		{
			glDeleteFramebuffers(1, &mSSAOBlurFBOId);
		}

		if (mpDiffuseTexture != nullptr)
		{
			OpenGLRenderingSystem::GetInstance()->DestroyTexture(mpDiffuseTexture);
		}

		if (mpNormalTexture != nullptr)
		{
			OpenGLRenderingSystem::GetInstance()->DestroyTexture(mpNormalTexture);
		}

		if (mpSpecularTexture != nullptr)
		{
			OpenGLRenderingSystem::GetInstance()->DestroyTexture(mpSpecularTexture);
		}

		if (mpTangentTexture != nullptr)
		{
			OpenGLRenderingSystem::GetInstance()->DestroyTexture(mpTangentTexture);
		}

		if (mpExtraDataTexture != nullptr)
		{
			OpenGLRenderingSystem::GetInstance()->DestroyTexture(mpExtraDataTexture);
		}

		if (mpFinalTexture != nullptr)
		{
			OpenGLRenderingSystem::GetInstance()->DestroyTexture(mpFinalTexture);
		}

		if (mpDepthTexture != nullptr)
		{
			OpenGLRenderingSystem::GetInstance()->DestroyTexture(mpDepthTexture);
		}

		if (mpNoiseTexture != nullptr)
		{
			OpenGLRenderingSystem::GetInstance()->DestroyTexture(mpNoiseTexture);
		}

		if (mpEmptySSAOTexture != nullptr)
		{
			OpenGLRenderingSystem::GetInstance()->DestroyTexture(mpEmptySSAOTexture);
		}

		if (mpSSAOTexture != nullptr)
		{
			OpenGLRenderingSystem::GetInstance()->DestroyTexture(mpSSAOTexture);
		}

		if (mpSSAOBlurTexture != nullptr)
		{
			OpenGLRenderingSystem::GetInstance()->DestroyTexture(mpSSAOBlurTexture);
		}
	}

	void OpenGLDeferredRenderingPathStrategy::FindShaders()
	{
		mpStencilPassShader = OpenGLRenderingSystem::GetInstance()->FindShader("StencilPass");
		mpDirectionalLightPassShader = OpenGLRenderingSystem::GetInstance()->FindShader("DirectionalLightPass");
		mpPointLightPassShader = OpenGLRenderingSystem::GetInstance()->FindShader("PointLightPass");
		mpSSAOHighFrequencyPassShader = OpenGLRenderingSystem::GetInstance()->FindShader("SSAOHighFrequencyPass");
		mpSSAOBlurPassShader = OpenGLRenderingSystem::GetInstance()->FindShader("SSAOBlurPass");
		mpDepthToScreenShader = OpenGLRenderingSystem::GetInstance()->FindShader("DepthToScreen");

		auto GetLightPassSamplerLocations = [](const OpenGLShader *pShader, LightPassSamplerLocations &locations)
		{
			locations.diffuseMap = pShader->GetBindingLocation("uDiffuseMap");
			locations.normalMap = pShader->GetBindingLocation("uNormalMap");
			locations.specularMap = pShader->GetBindingLocation("uSpecularMap");
			locations.tangentMap = pShader->GetBindingLocation("uTangentMap");
			locations.extraData = pShader->GetBindingLocation("uExtraData");
			locations.depth = pShader->GetBindingLocation("uDepth");
			locations.ambientOcclusionMap = pShader->GetBindingLocation("uAmbientOcclusionMap");
		};

		GetLightPassSamplerLocations(mpPointLightPassShader, mPointLightPassSamplerLocations);
		GetLightPassSamplerLocations(mpDirectionalLightPassShader, mDirectionalLightPassSamplerLocations);

		mSSAOHighFrequencyPassNoiseMapLocation = mpSSAOHighFrequencyPassShader->GetBindingLocation("uNoiseMap");
		mSSAOHighFrequencyPassNormalMapLocation = mpSSAOHighFrequencyPassShader->GetBindingLocation("uNormalMap");
		mSSAOHighFrequencyPassDepthLocation = mpSSAOHighFrequencyPassShader->GetBindingLocation("uDepth");

		mSSAOBlurPassAmbientOcclusionMapLocation = mpSSAOBlurPassShader->GetBindingLocation("uAmbientOcclusionMap");

#ifdef _DEBUG
		mpDepthToScreenDepthLocation = mpDepthToScreenShader->GetBindingLocation("uDepth");
#endif
	}

	void OpenGLDeferredRenderingPathStrategy::CreateAuxiliaryMeshes()
	{
		mpQuadMesh = StandardGeometries::CreateXYPlane("Deferred Rendering Fullscreen Quad", 2, 2, 1, 1, glm::vec3(0, 0, 0));
		mpSphereMesh = StandardGeometries::CreateSphere("Deferred Rendering PointLight Sphere", 1, LIGHT_MESH_DETAIL);
	}

	void OpenGLDeferredRenderingPathStrategy::CreateAuxiliaryTextures()
	{
		{
			Random::SeedWithTime();
			auto pNoise = std::make_unique<glm::vec3[]>(NOISE_TEXTURE_SIZE);
			for (uint32_t i = 0; i < NOISE_TEXTURE_HEIGHT; i++)
			{
				for (uint32_t j = 0; j < NOISE_TEXTURE_WIDTH; j++)
				{
					glm::vec3 noise(Random::NextFloat() * 2 - 1, Random::NextFloat() * 2 - 1, 0);
					pNoise[i * NOISE_TEXTURE_WIDTH + j] = glm::normalize(noise);
				}
			}
			mpNoiseTexture = OpenGLRenderingSystem::GetInstance()->CreateTexture({"Noise",
																				  NOISE_TEXTURE_WIDTH,
																				  NOISE_TEXTURE_HEIGHT,
																				  TextureType::TEXTURE_2D,
																				  TextureFormat::RGB,
																				  {8, 8, 8},
																				  TextureDataType::FLOAT,
																				  TextureFilter::POINT_FILTER,
																				  TextureWrapMode::REPEAT,
																				  false,
																				  &pNoise[0][0]});
		}

		{
			glm::vec4 data[] = {glm::vec4{1, 1, 1, 1}};
			mpEmptySSAOTexture = OpenGLRenderingSystem::GetInstance()->CreateTexture({"Empty SSAO",
																					  1,
																					  1,
																					  TextureType::TEXTURE_2D,
																					  TextureFormat::RGBA,
																					  {8, 8, 8, 8},
																					  TextureDataType::FLOAT,
																					  TextureFilter::POINT_FILTER,
																					  TextureWrapMode::REPEAT,
																					  false,
																					  data});
		}
	}

	void OpenGLDeferredRenderingPathStrategy::AllocateRenderTargetsAndFBOs()
	{
		// create g-buffer fbo
		glGenFramebuffers(1, &mGBufferFBOId);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mGBufferFBOId);
#ifdef _DEBUG
		{
			const char framebufferLabel[] = "G-Buffer (GL_FRAMEBUFFER)";
			glObjectLabel(GL_FRAMEBUFFER, mGBufferFBOId, sizeof(framebufferLabel) / sizeof(char), framebufferLabel);
		}
#endif

		FASTCG_CHECK_OPENGL_ERROR();

		// create g-buffer attachments

		mpDiffuseTexture = OpenGLRenderingSystem::GetInstance()->CreateTexture({"G-Buffer Diffuse",
																				mArgs.rScreenWidth,
																				mArgs.rScreenHeight,
																				TextureType::TEXTURE_2D,
																				TextureFormat::RGBA,
																				{32, 32, 32, 32},
																				TextureDataType::FLOAT,
																				TextureFilter::LINEAR_FILTER,
																				TextureWrapMode::CLAMP,
																				false});
		glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, *mpDiffuseTexture, 0);

		FASTCG_CHECK_OPENGL_ERROR();

		mpNormalTexture = OpenGLRenderingSystem::GetInstance()->CreateTexture({"G-Buffer Normal",
																			   mArgs.rScreenWidth,
																			   mArgs.rScreenHeight,
																			   TextureType::TEXTURE_2D,
																			   TextureFormat::RGBA,
																			   {32, 32, 32, 32},
																			   TextureDataType::FLOAT,
																			   TextureFilter::LINEAR_FILTER,
																			   TextureWrapMode::CLAMP,
																			   false});
		glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, *mpNormalTexture, 0);

		FASTCG_CHECK_OPENGL_ERROR();

		mpSpecularTexture = OpenGLRenderingSystem::GetInstance()->CreateTexture({"G-Buffer Specular",
																				 mArgs.rScreenWidth,
																				 mArgs.rScreenHeight,
																				 TextureType::TEXTURE_2D,
																				 TextureFormat::RGBA,
																				 {32, 32, 32, 32},
																				 TextureDataType::FLOAT,
																				 TextureFilter::LINEAR_FILTER,
																				 TextureWrapMode::CLAMP,
																				 false});
		glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, *mpSpecularTexture, 0);

		FASTCG_CHECK_OPENGL_ERROR();

		mpTangentTexture = OpenGLRenderingSystem::GetInstance()->CreateTexture({"G-Buffer Tangent",
																				mArgs.rScreenWidth,
																				mArgs.rScreenHeight,
																				TextureType::TEXTURE_2D,
																				TextureFormat::RGBA,
																				{32, 32, 32, 32},
																				TextureDataType::FLOAT,
																				TextureFilter::LINEAR_FILTER,
																				TextureWrapMode::CLAMP,
																				false});
		glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, *mpTangentTexture, 0);

		FASTCG_CHECK_OPENGL_ERROR();

		mpExtraDataTexture = OpenGLRenderingSystem::GetInstance()->CreateTexture({"G-Buffer Extra Data",
																				  mArgs.rScreenWidth,
																				  mArgs.rScreenHeight,
																				  TextureType::TEXTURE_2D,
																				  TextureFormat::RGBA,
																				  {32, 32, 32, 32},
																				  TextureDataType::FLOAT,
																				  TextureFilter::LINEAR_FILTER,
																				  TextureWrapMode::CLAMP,
																				  false});
		glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, GL_TEXTURE_2D, *mpExtraDataTexture, 0);

		FASTCG_CHECK_OPENGL_ERROR();

		mpFinalTexture = OpenGLRenderingSystem::GetInstance()->CreateTexture({"G-Buffer Final",
																			  mArgs.rScreenWidth,
																			  mArgs.rScreenHeight,
																			  TextureType::TEXTURE_2D,
																			  TextureFormat::RGBA,
																			  {32, 32, 32, 32},
																			  TextureDataType::FLOAT,
																			  TextureFilter::LINEAR_FILTER,
																			  TextureWrapMode::CLAMP,
																			  false});
		glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT7, GL_TEXTURE_2D, *mpFinalTexture, 0);

		FASTCG_CHECK_OPENGL_ERROR();

		mpDepthTexture = OpenGLRenderingSystem::GetInstance()->CreateTexture({"G-Buffer Depth",
																			  mArgs.rScreenWidth,
																			  mArgs.rScreenHeight,
																			  TextureType::TEXTURE_2D,
																			  TextureFormat::DEPTH_STENCIL,
																			  {24, 8},
																			  TextureDataType::UNSIGNED_INT,
																			  TextureFilter::POINT_FILTER,
																			  TextureWrapMode::CLAMP,
																			  false});
		glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, *mpDepthTexture, 0);

		FASTCG_CHECK_OPENGL_ERROR();

		auto status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if (status != GL_FRAMEBUFFER_COMPLETE)
		{
			FASTCG_THROW_EXCEPTION(OpenGLException, "Error creating G-Buffer FBO: 0x%x\n", status);
		}

		// create ssao fbo
		glGenFramebuffers(1, &mSSAOFBOId);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mSSAOFBOId);
#ifdef _DEBUG
		{
			const char framebufferLabel[] = "SSAO (GL_FRAMEBUFFER)";
			glObjectLabel(GL_FRAMEBUFFER, mSSAOFBOId, sizeof(framebufferLabel) / sizeof(char), framebufferLabel);
		}
#endif

		FASTCG_CHECK_OPENGL_ERROR();

		// create ssao attachments

		mpSSAOTexture = OpenGLRenderingSystem::GetInstance()->CreateTexture({"SSAO",
																			 mArgs.rScreenWidth,
																			 mArgs.rScreenHeight,
																			 TextureType::TEXTURE_2D,
																			 TextureFormat::RGBA,
																			 {32, 32, 32, 32},
																			 TextureDataType::FLOAT,
																			 TextureFilter::LINEAR_FILTER,
																			 TextureWrapMode::CLAMP,
																			 false});
		glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, *mpSSAOTexture, 0);

		FASTCG_CHECK_OPENGL_ERROR();

		status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if (status != GL_FRAMEBUFFER_COMPLETE)
		{
			FASTCG_THROW_EXCEPTION(OpenGLException, "Error creating SSAO FBO: 0x%x\n", status);
		}

		// create ssao blur fbo
		glGenFramebuffers(1, &mSSAOBlurFBOId);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mSSAOBlurFBOId);
#ifdef _DEBUG
		{
			const char framebufferLabel[] = "SSAO Blur (GL_FRAMEBUFFER)";
			glObjectLabel(GL_FRAMEBUFFER, mSSAOBlurFBOId, sizeof(framebufferLabel) / sizeof(char), framebufferLabel);
		}
#endif

		FASTCG_CHECK_OPENGL_ERROR();

		// create sso blur attachments
		mpSSAOBlurTexture = OpenGLRenderingSystem::GetInstance()->CreateTexture({"SSAO Blur",
																				 mArgs.rScreenWidth,
																				 mArgs.rScreenHeight,
																				 TextureType::TEXTURE_2D,
																				 TextureFormat::RGBA,
																				 {32, 32, 32, 32},
																				 TextureDataType::FLOAT,
																				 TextureFilter::LINEAR_FILTER,
																				 TextureWrapMode::CLAMP,
																				 false});
		glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, *mpSSAOBlurTexture, 0);

		FASTCG_CHECK_OPENGL_ERROR();

		status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if (status != GL_FRAMEBUFFER_COMPLETE)
		{
			FASTCG_THROW_EXCEPTION(OpenGLException, "Error creating SSAO Blur FBO: 0x%x\n", status);
		}

		// restore to "default" fbo
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	}

	void OpenGLDeferredRenderingPathStrategy::CreateUniformBuffers()
	{
		OpenGLRenderingPathStrategy::CreateUniformBuffers();

		glGenBuffers(1, &mSSAOHighFrequencyPassBufferId);
		glBindBuffer(GL_UNIFORM_BUFFER, mSSAOHighFrequencyPassBufferId);
#ifdef _DEBUG
		{
			const char bufferLabel[] = "SSAO High Frequency Pass Constants (GL_BUFFER)";
			glObjectLabel(GL_BUFFER, mSSAOHighFrequencyPassBufferId, FASTCG_ARRAYSIZE(bufferLabel), bufferLabel);
		}
#endif
		glBufferData(GL_UNIFORM_BUFFER, sizeof(SSAOHighFrequencyPassConstants), &mSSAOHighFrequencyPassBufferId, GL_DYNAMIC_DRAW);

		FASTCG_CHECK_OPENGL_ERROR();
	}

	void OpenGLDeferredRenderingPathStrategy::DestroyUniformBuffers()
	{
		if (mSSAOHighFrequencyPassBufferId != ~0u)
		{
			glDeleteBuffers(1, &mSSAOHighFrequencyPassBufferId);
			mSSAOHighFrequencyPassBufferId = ~0u;
		}

		OpenGLRenderingPathStrategy::DestroyUniformBuffers();
	}

	void OpenGLDeferredRenderingPathStrategy::UpdateSceneConstantsBuffer(const Camera *pMainCamera)
	{
		mSceneConstants.screenSize = glm::vec2{mArgs.rScreenWidth, mArgs.rScreenHeight};
		mSceneConstants.aspectRatio = mAspectRatio;
		mSceneConstants.tanHalfFov = MathF::Tan((MathF::DEGREES_TO_RADIANS * pMainCamera->GetFieldOfView()) / 2.0f);
		OpenGLRenderingPathStrategy::UpdateSceneConstantsBuffer(pMainCamera);
	}

	void OpenGLDeferredRenderingPathStrategy::UpdateSSAOHighFrequencyPassConstantsBuffer()
	{
		glBindBuffer(GL_UNIFORM_BUFFER, mSSAOHighFrequencyPassBufferId);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(SSAOHighFrequencyPassConstants), &mSSAOHighFrequencyPassConstants);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

	void OpenGLDeferredRenderingPathStrategy::Render(const Camera *pMainCamera)
	{
#ifdef _DEBUG
		glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, -1, "Deferred Rendering Path");
#endif

		mArgs.rRenderingStatistics.Reset();

		constexpr GLenum pMeshPassDrawBuffers[] = {
			GL_COLOR_ATTACHMENT0,
			GL_COLOR_ATTACHMENT1,
			GL_COLOR_ATTACHMENT2,
			GL_COLOR_ATTACHMENT3,
			GL_COLOR_ATTACHMENT4,
		};
		constexpr GLint numMeshPassDrawBuffers = sizeof(pMeshPassDrawBuffers) / sizeof(GLenum);

		glViewport(0, 0, mArgs.rScreenWidth, mArgs.rScreenHeight);
		glDisable(GL_SCISSOR_TEST);

		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mGBufferFBOId);
		glDrawBuffers(numMeshPassDrawBuffers, pMeshPassDrawBuffers);

		{
#ifdef _DEBUG
			glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, -1, "Clear G-Buffer Intermediate Render Targets");
#endif

			for (GLint bufferIdx = 0; bufferIdx < numMeshPassDrawBuffers; bufferIdx++)
			{
				glClearBufferfv(GL_COLOR, bufferIdx, &Colors::NONE[0]);
			}

			glDepthMask(GL_TRUE);
			glStencilMask(0xFF);
			glClearBufferfi(GL_DEPTH_STENCIL, 0, 1, 0);
			glDepthMask(GL_FALSE);
			glStencilMask(0);

#ifdef _DEBUG
			glPopDebugGroup();
#endif
		}

		{
#ifdef _DEBUG
			glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, -1, "G-Buffer Intermediate Render Targets Filling Passes");
#endif

			UpdateSceneConstantsBuffer(pMainCamera);

			for (const auto *pRenderBatch : mArgs.rRenderBatches)
			{
				const auto *pMaterial = pRenderBatch->pMaterial;

				{
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

						auto *pMesh = pRenderable->GetMesh();

						pMesh->Draw();

						mArgs.rRenderingStatistics.drawCalls++;
						mArgs.rRenderingStatistics.numberOfTriangles += pMesh->GetNumberOfTriangles();
					}

					pMaterial->Unbind();

					FASTCG_CHECK_OPENGL_ERROR();

#ifdef _DEBUG
					glPopDebugGroup();
#endif
				}
			}

#ifdef _DEBUG
			glPopDebugGroup();
#endif
		}

#ifdef _DEBUG
		if (mDisplayGBufferEnabled)
		{
			glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, -1, "Display G-Buffer Passes");

			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

			glClearColor(mArgs.rClearColor.x, mArgs.rClearColor.y, mArgs.rClearColor.z, mArgs.rClearColor.w);
			glClear(GL_COLOR_BUFFER_BIT);

			glBindFramebuffer(GL_READ_FRAMEBUFFER, mGBufferFBOId);

			auto tileWidth = mArgs.rScreenWidth / 3;
			auto tileHeight = mArgs.rScreenHeight / 2;

			// (0, 1)
			glReadBuffer(GL_COLOR_ATTACHMENT0);
			glBlitFramebuffer(0, 0, mArgs.rScreenWidth, mArgs.rScreenHeight, 0, tileHeight, tileWidth, mArgs.rScreenHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);

			// (1, 1)
			glReadBuffer(GL_COLOR_ATTACHMENT1);
			glBlitFramebuffer(0, 0, mArgs.rScreenWidth, mArgs.rScreenHeight, tileWidth, tileHeight, tileWidth * 2, mArgs.rScreenHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);

			// (2, 1)
			glReadBuffer(GL_COLOR_ATTACHMENT2);
			glBlitFramebuffer(0, 0, mArgs.rScreenWidth, mArgs.rScreenHeight, tileWidth * 2, tileHeight, mArgs.rScreenWidth, mArgs.rScreenHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);

			// (0, 0)
			glReadBuffer(GL_COLOR_ATTACHMENT3);
			glBlitFramebuffer(0, 0, mArgs.rScreenWidth, mArgs.rScreenHeight, 0, 0, tileWidth, tileHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);

			// (1, 0)
			glReadBuffer(GL_COLOR_ATTACHMENT4);
			glBlitFramebuffer(0, 0, mArgs.rScreenWidth, mArgs.rScreenHeight, tileWidth, 0, tileWidth * 2, tileHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);

			// (2, 0)
			glViewport(tileWidth * 2, 0, tileWidth, tileHeight);

			glDisable(GL_DEPTH_TEST);
			glDepthMask(GL_FALSE);
			glDisable(GL_STENCIL_TEST);
			glStencilMask(0);

			mpDepthToScreenShader->Bind();

			mpDepthToScreenShader->BindTexture(mpDepthToScreenDepthLocation, *mpDepthTexture, 0);

			mpQuadMesh->Draw();

			mArgs.rRenderingStatistics.drawCalls++;

			mpDepthToScreenShader->Unbind();

			FASTCG_CHECK_OPENGL_ERROR();

			glPopDebugGroup();
		}
		else
#endif
		{
			auto isSSAOEnabled = pMainCamera->IsSSAOEnabled();
			auto ambientOcclusionFlag = (isSSAOEnabled) ? 1.0f : 0.0f;

			if (isSSAOEnabled)
			{
#ifdef _DEBUG
				glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, -1, "SSAO High Frequency Pass");
#endif

				glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mSSAOFBOId);
				glDrawBuffer(GL_COLOR_ATTACHMENT0);

				glClearBufferfv(GL_COLOR, 0, &Colors::NONE[0]);

				glDisable(GL_DEPTH_TEST);
				glDepthMask(GL_FALSE);

				mpSSAOHighFrequencyPassShader->Bind();

				glBindBufferBase(GL_UNIFORM_BUFFER, OpenGLShader::SCENE_CONSTANTS_BINDING_INDEX, mSceneConstantsBufferId);

				UpdateSSAOHighFrequencyPassConstantsBuffer();

				glBindBufferBase(GL_UNIFORM_BUFFER, OpenGLShader::SSAO_HIGH_FREQUENCY_PASS_CONSTANTS_BINDING_INDEX, mSSAOHighFrequencyPassBufferId);

				mpSSAOHighFrequencyPassShader->BindTexture(mSSAOHighFrequencyPassNoiseMapLocation, *mpNoiseTexture, 0);
				mpSSAOHighFrequencyPassShader->BindTexture(mSSAOHighFrequencyPassNormalMapLocation, *mpNormalTexture, 1);
				mpSSAOHighFrequencyPassShader->BindTexture(mSSAOHighFrequencyPassDepthLocation, *mpDepthTexture, 2);

				mpQuadMesh->Draw();

				mArgs.rRenderingStatistics.drawCalls++;

				mpSSAOHighFrequencyPassShader->Unbind();

				FASTCG_CHECK_OPENGL_ERROR();

#ifdef _DEBUG
				glPopDebugGroup();
#endif

				if (mSSAOBlurEnabled)
				{
#ifdef _DEBUG
					glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, -1, "SSAO Blur Pass");
#endif

					glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mSSAOBlurFBOId);
					glDrawBuffer(GL_COLOR_ATTACHMENT0);

					glClearBufferfv(GL_COLOR, 0, &Colors::NONE[0]);

					mpSSAOBlurPassShader->Bind();

					mpSSAOBlurPassShader->BindTexture(mSSAOBlurPassAmbientOcclusionMapLocation, *mpSSAOTexture, 0);

					mpQuadMesh->Draw();

					mArgs.rRenderingStatistics.drawCalls++;

					mpSSAOBlurPassShader->Unbind();

					FASTCG_CHECK_OPENGL_ERROR();

#ifdef _DEBUG
					glPopDebugGroup();
#endif
				}
			}

#ifdef _DEBUG
			if (mDisplaySSAOTextureEnabled)
			{
				glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, -1, "Display SSAO Pass");

				glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

				glClearColor(mArgs.rClearColor.x, mArgs.rClearColor.y, mArgs.rClearColor.z, mArgs.rClearColor.w);
				glClear(GL_COLOR_BUFFER_BIT);

				if (mSSAOBlurEnabled)
				{
					glBindFramebuffer(GL_READ_FRAMEBUFFER, mSSAOBlurFBOId);
					glReadBuffer(GL_COLOR_ATTACHMENT0);

					FASTCG_CHECK_OPENGL_ERROR();
				}
				else
				{
					glBindFramebuffer(GL_READ_FRAMEBUFFER, mSSAOFBOId);
					glReadBuffer(GL_COLOR_ATTACHMENT0);

					FASTCG_CHECK_OPENGL_ERROR();
				}

				glBlitFramebuffer(0, 0, mArgs.rScreenWidth, mArgs.rScreenHeight, 0, 0, mArgs.rScreenWidth, mArgs.rScreenHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);

				FASTCG_CHECK_OPENGL_ERROR();

				glPopDebugGroup();
			}
			else
#endif
			{
				const auto BindLightPassSamplers = [&](const OpenGLShader *pShader, const LightPassSamplerLocations &samplerLocations)
				{
					pShader->BindTexture(samplerLocations.diffuseMap, *mpDiffuseTexture, 0);
					pShader->BindTexture(samplerLocations.normalMap, *mpNormalTexture, 1);
					pShader->BindTexture(samplerLocations.specularMap, *mpSpecularTexture, 2);
					pShader->BindTexture(samplerLocations.tangentMap, *mpTangentTexture, 3);
					pShader->BindTexture(samplerLocations.extraData, *mpExtraDataTexture, 4);
					pShader->BindTexture(samplerLocations.depth, *mpDepthTexture, 5);

					if (pMainCamera->IsSSAOEnabled())
					{
						if (mSSAOBlurEnabled)
						{
							pShader->BindTexture(samplerLocations.ambientOcclusionMap, *mpSSAOBlurTexture, 6);
						}
						else
						{
							pShader->BindTexture(samplerLocations.ambientOcclusionMap, *mpSSAOTexture, 6);
						}
					}
					else
					{
						pShader->BindTexture(samplerLocations.ambientOcclusionMap, *mpEmptySSAOTexture, 6);
					}
				};

				{
#ifdef _DEBUG
					glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, -1, "Clear G-Buffer Final Render Target");
#endif

					glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mGBufferFBOId);
					glDrawBuffer(GL_COLOR_ATTACHMENT7);

					glClearBufferfv(GL_COLOR, 0, &Colors::NONE[0]);

#ifdef _DEBUG
					glPopDebugGroup();
#endif
				}

				{
#ifdef _DEBUG
					glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, -1, "Point Light Passes");
#endif

					for (size_t i = 0; i < mArgs.rPointLights.size(); i++)
					{
						const auto *pPointLight = mArgs.rPointLights[i];

						{
#ifdef _DEBUG
							glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, -1, (std::string("Point Light Pass (") + std::to_string(i) + ")").c_str());
#endif

							auto model = glm::scale(pPointLight->GetGameObject()->GetTransform()->GetModel(), glm::vec3(CalculateLightBoundingSphereScale(pPointLight)));

							UpdateInstanceConstantsBuffer(model);

							{
#ifdef _DEBUG
								glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, -1, (std::string("Point Light (") + std::to_string(i) + ") Stencil Sub-Pass").c_str());
#endif

								glStencilMask(0xFF);
								constexpr GLint emptyStencil = 0;
								glClearBufferiv(GL_STENCIL, 0, &emptyStencil);

								glDrawBuffer(GL_NONE);

								glEnable(GL_DEPTH_TEST);
								glDepthMask(GL_FALSE);

								glEnable(GL_STENCIL_TEST);
								glStencilFunc(GL_ALWAYS, 0, 0);
								glStencilOpSeparate(GL_BACK, GL_KEEP, GL_INCR, GL_KEEP);
								glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_DECR, GL_KEEP);

								glDisable(GL_BLEND);

								glDisable(GL_CULL_FACE);

								mpStencilPassShader->Bind();

								glBindBufferBase(GL_UNIFORM_BUFFER, OpenGLShader::INSTANCE_CONTANTS_BINDING_INDEX, mInstanceConstantsBufferId);

								mpSphereMesh->Draw();

								mArgs.rRenderingStatistics.drawCalls++;

								mpStencilPassShader->Unbind();

								FASTCG_CHECK_OPENGL_ERROR();

#ifdef _DEBUG
								glPopDebugGroup();
#endif
							}

							{
#ifdef _DEBUG
								glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, -1, (std::string("Point Light (") + std::to_string(i) + ") Color Sub-Pass").c_str());
#endif
								glDrawBuffer(GL_COLOR_ATTACHMENT7);

								glDisable(GL_DEPTH_TEST);
								glDepthMask(GL_FALSE);

								glEnable(GL_STENCIL_TEST);
								glStencilMask(0);
								glStencilFunc(GL_NOTEQUAL, 0, 0xFF);

								glEnable(GL_BLEND);
								glBlendEquation(GL_FUNC_ADD);
								glBlendFunc(GL_ONE, GL_ONE);

								glEnable(GL_CULL_FACE);
								glCullFace(GL_FRONT);

								mpPointLightPassShader->Bind();

								glBindBufferBase(GL_UNIFORM_BUFFER, OpenGLShader::SCENE_CONSTANTS_BINDING_INDEX, mSceneConstantsBufferId);

								UpdateInstanceConstantsBuffer(model);

								glBindBufferBase(GL_UNIFORM_BUFFER, OpenGLShader::INSTANCE_CONTANTS_BINDING_INDEX, mInstanceConstantsBufferId);

								UpdateLightingConstantsBuffer(pPointLight);

								glBindBufferBase(GL_UNIFORM_BUFFER, OpenGLShader::LIGHTING_CONSTANTS_BINDING_INDEX, mLightingConstantsBufferId);

								BindLightPassSamplers(mpPointLightPassShader, mPointLightPassSamplerLocations);

								mpSphereMesh->Draw();

								mArgs.rRenderingStatistics.drawCalls++;

								mpPointLightPassShader->Unbind();

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

				{
#ifdef _DEBUG
					glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, -1, "Directional Light Passes");
#endif

					glDrawBuffer(GL_COLOR_ATTACHMENT7);

					glDisable(GL_DEPTH_TEST);
					glDepthMask(GL_FALSE);
					glDisable(GL_STENCIL_TEST);
					glStencilMask(0);

					glEnable(GL_BLEND);
					glBlendEquation(GL_FUNC_ADD);
					glBlendFunc(GL_ONE, GL_ONE);

					glEnable(GL_CULL_FACE);
					glCullFace(GL_BACK);

					mpDirectionalLightPassShader->Bind();

					BindLightPassSamplers(mpDirectionalLightPassShader, mDirectionalLightPassSamplerLocations);

					auto inverseCameraRotation = glm::inverse(pMainCamera->GetGameObject()->GetTransform()->GetRotation());
					for (size_t i = 0; i < mArgs.rDirectionalLights.size(); i++)
					{
						const auto *pDirectionalLight = mArgs.rDirectionalLights[i];
						{
#ifdef _DEBUG
							glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, -1, (std::string("Directional Light Pass (") + std::to_string(i) + ")").c_str());
#endif

							UpdateLightingConstantsBuffer(pDirectionalLight, glm::vec3(glm::normalize(inverseCameraRotation * glm::vec4(pDirectionalLight->GetDirection(), 1))));

							glBindBufferBase(GL_UNIFORM_BUFFER, OpenGLShader::LIGHTING_CONSTANTS_BINDING_INDEX, mLightingConstantsBufferId);

							mpQuadMesh->Draw();

							mArgs.rRenderingStatistics.drawCalls++;

#ifdef _DEBUG
							glPopDebugGroup();
#endif
						}
					}
					mpDirectionalLightPassShader->Unbind();

					FASTCG_CHECK_OPENGL_ERROR();

#ifdef _DEBUG
					glPopDebugGroup();
#endif
				}

				{
#ifdef _DEBUG
					glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, -1, "Copy G-Buffer Final Render Target into Color Backbuffer");
#endif

					glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
					glBindFramebuffer(GL_READ_FRAMEBUFFER, mGBufferFBOId);
					glReadBuffer(GL_COLOR_ATTACHMENT7);
					glBlitFramebuffer(0, 0, mArgs.rScreenWidth, mArgs.rScreenHeight, 0, 0, mArgs.rScreenWidth, mArgs.rScreenHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);
					glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);

#ifdef _DEBUG
					glPopDebugGroup();
#endif
				}
			}
		}

#ifdef _DEBUG
		glPopDebugGroup();
#endif
	}

}

#endif