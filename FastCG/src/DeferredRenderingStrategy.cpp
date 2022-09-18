#include <FastCG/ShaderRegistry.h>
#include <FastCG/StandardGeometries.h>
#include <FastCG/Random.h>
#include <FastCG/OpenGLExceptions.h>
#include <FastCG/MathT.h>
#include <FastCG/DeferredRenderingStrategy.h>

#include <GL/glew.h>
#include <GL/gl.h>

#include <glm/gtc/matrix_transform.hpp>

float CalculateLightBoundingSphereScale(const FastCG::PointLight* pPointLight)
{
	auto diffuseColor = pPointLight->GetDiffuseColor();
	auto maxChannel = FastCG::MathF::Max(FastCG::MathF::Max(diffuseColor.r, diffuseColor.g), diffuseColor.b);
	auto c = maxChannel * pPointLight->GetIntensity();
	return (8.0f * FastCG::MathF::Sqrt(c) + 1.0f);
}

namespace FastCG
{
	const uint32_t DeferredRenderingStrategy::NUMBER_OF_RANDOM_SAMPLES = 30;
	const uint32_t DeferredRenderingStrategy::LIGHT_MESH_DETAIL = 20;
	const float DeferredRenderingStrategy::DEFAULT_SSAO_RADIUS = 0.5f;
	const float DeferredRenderingStrategy::DEFAULT_SSAO_DISTANCE_SCALE = 5.0f;
	const uint32_t DeferredRenderingStrategy::NOISE_TEXTURE_WIDTH = 4;
	const uint32_t DeferredRenderingStrategy::NOISE_TEXTURE_HEIGHT = 4;
	const uint32_t DeferredRenderingStrategy::NOISE_TEXTURE_SIZE = NOISE_TEXTURE_WIDTH * NOISE_TEXTURE_HEIGHT;

	DeferredRenderingStrategy::DeferredRenderingStrategy(const uint32_t& rScreenWidth,
		const uint32_t& rScreenHeight,
		const glm::vec4& rAmbientLight,
		const std::vector<DirectionalLight*>& rDirectionalLights,
		const std::vector<PointLight*>& rPointLights,
		const std::vector<LineRenderer*>& rLineRenderers,
		const std::vector<PointsRenderer*>& rPointsRenderer,
		const std::vector<std::unique_ptr<RenderBatch>>& rRenderBatches,
		RenderingStatistics& rRenderingStatistics) :
		RenderingPathStrategy(rScreenWidth, rScreenHeight, rAmbientLight, rDirectionalLights, rPointLights, rLineRenderers, rPointsRenderer, rRenderBatches, rRenderingStatistics),
		mSSAORadius(DEFAULT_SSAO_RADIUS),
		mSSAODistanceScale(DEFAULT_SSAO_DISTANCE_SCALE)
	{
		mHalfScreenWidth = mrScreenWidth / 2;
		mHalfScreenHeight = mrScreenHeight / 2;
		mAspectRatio = mrScreenWidth / (float)mrScreenHeight;

		FindShaders();
		BuildAuxiliaryMeshes();
		GenerateNoiseTexture();
		GenerateRandomSamplesInAHemisphere();
		AllocateTexturesAndFBOs();
	}

	DeferredRenderingStrategy::~DeferredRenderingStrategy()
	{
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

		if (mDiffuseTextureId != ~0u)
		{
			glDeleteTextures(1, &mDiffuseTextureId);
		}

		if (mNormalTextureId != ~0u)
		{
			glDeleteTextures(1, &mNormalTextureId);
		}

		if (mSpecularTextureId != ~0u)
		{
			glDeleteTextures(1, &mSpecularTextureId);
		}

		if (mFinalOutputTextureId != ~0u)
		{
			glDeleteTextures(1, &mFinalOutputTextureId);
		}

		if (mDepthTextureId != ~0u)
		{
			glDeleteTextures(1, &mDepthTextureId);
		}

		if (mSSAOTextureId != ~0u)
		{
			glDeleteTextures(1, &mSSAOTextureId);
		}

		if (mBlurredSSAOTextureId != ~0u)
		{
			glDeleteTextures(1, &mBlurredSSAOTextureId);
		}
	}

	void DeferredRenderingStrategy::FindShaders()
	{
		mpStencilPassShader = ShaderRegistry::Find("StencilPass");
		mpDirectionalLightPassShader = ShaderRegistry::Find("DirectionalLightPass");
		mpPointLightPassShader = ShaderRegistry::Find("PointLightPass");
		mpSSAOHighFrequencyPassShader = ShaderRegistry::Find("SSAOHighFrequencyPass");
		mpSSAOBlurPassShader = ShaderRegistry::Find("SSAOBlurPass");
		mpLineStripShader = ShaderRegistry::Find("LineStrip");
		mpPointsShader = ShaderRegistry::Find("Points");
		mpDepthToScreenShader = ShaderRegistry::Find("DepthToScreen");
	}

	void DeferredRenderingStrategy::BuildAuxiliaryMeshes()
	{
		mpQuadMesh = StandardGeometries::CreateXYPlane(2, 2, 1, 1, glm::vec3(0, 0, 0));
		mpSphereMesh = StandardGeometries::CreateSphere(1, LIGHT_MESH_DETAIL);
	}

	void DeferredRenderingStrategy::GenerateNoiseTexture()
	{
		Random::SeedWithTime();
		auto pNoises = std::make_unique<glm::vec3[]>(NOISE_TEXTURE_SIZE);
		for (uint32_t i = 0; i < NOISE_TEXTURE_HEIGHT; i++)
		{
			for (uint32_t j = 0; j < NOISE_TEXTURE_WIDTH; j++)
			{
				glm::vec3 noise(Random::NextFloat() * 2 - 1, Random::NextFloat() * 2 - 1, 0);
				pNoises[i * NOISE_TEXTURE_WIDTH + j] = glm::normalize(noise);
			}
		}
		mpNoiseTexture = std::make_unique<Texture>(NOISE_TEXTURE_WIDTH, NOISE_TEXTURE_HEIGHT, TextureFormat::TF_RGB, TextureDataType::DT_FLOAT, TextureFilter::TF_POINT_FILTER, TextureWrapMode::TW_REPEAT, false, &pNoises[0][0]);
	}

	void DeferredRenderingStrategy::GenerateRandomSamplesInAHemisphere()
	{
		Random::SeedWithTime();
		for (uint32_t i = 0; i < NUMBER_OF_RANDOM_SAMPLES; i++)
		{
			glm::vec3 randomSample(Random::NextFloat() * 2 - 1, Random::NextFloat() * 2 - 1, Random::NextFloat());
			randomSample = glm::normalize(randomSample);
			float scale = i / (float)NUMBER_OF_RANDOM_SAMPLES;
			scale = MathF::Lerp(0.1f, 1.0f, MathF::Pow(scale, 2));
			randomSample *= scale;
			mRandomSamples.emplace_back(randomSample);
		}
	}

	void DeferredRenderingStrategy::AllocateTexturesAndFBOs()
	{
		// create g-buffer fbo
		glGenFramebuffers(1, &mGBufferFBOId);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mGBufferFBOId);

		CHECK_FOR_OPENGL_ERRORS();

		// create g-buffer attachments

		glGenTextures(1, &mDiffuseTextureId);
		glGenTextures(1, &mNormalTextureId);
		glGenTextures(1, &mSpecularTextureId);
		glGenTextures(1, &mFinalOutputTextureId);
		glGenTextures(1, &mDepthTextureId);

		glBindTexture(GL_TEXTURE_2D, mDiffuseTextureId);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, mrScreenWidth, mrScreenHeight, 0, GL_RGBA, GL_FLOAT, 0);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mDiffuseTextureId, 0);

		CHECK_FOR_OPENGL_ERRORS();

		glBindTexture(GL_TEXTURE_2D, mNormalTextureId);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, mrScreenWidth, mrScreenHeight, 0, GL_RGBA, GL_FLOAT, 0);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, mNormalTextureId, 0);

		CHECK_FOR_OPENGL_ERRORS();

		glBindTexture(GL_TEXTURE_2D, mSpecularTextureId);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, mrScreenWidth, mrScreenHeight, 0, GL_RGBA, GL_FLOAT, 0);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, mSpecularTextureId, 0);

		CHECK_FOR_OPENGL_ERRORS();

		glBindTexture(GL_TEXTURE_2D, mFinalOutputTextureId);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, mrScreenWidth, mrScreenHeight, 0, GL_RGBA, GL_FLOAT, 0);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, mFinalOutputTextureId, 0);

		CHECK_FOR_OPENGL_ERRORS();

		glBindTexture(GL_TEXTURE_2D, mDepthTextureId);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, mrScreenWidth, mrScreenHeight, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);
		glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, mDepthTextureId, 0);

		CHECK_FOR_OPENGL_ERRORS();

		auto status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if (status != GL_FRAMEBUFFER_COMPLETE)
		{
			THROW_EXCEPTION(OpenGLException, "Error creating G-Buffer FBO: 0x%x\n", status);
		}

		// create ssao fbo
		glGenFramebuffers(1, &mSSAOFBOId);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mSSAOFBOId);

		CHECK_FOR_OPENGL_ERRORS();

		// create ssao attachments
		glGenTextures(1, &mSSAOTextureId);
		glBindTexture(GL_TEXTURE_2D, mSSAOTextureId);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, mrScreenWidth, mrScreenHeight, 0, GL_RGBA, GL_FLOAT, 0);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mSSAOTextureId, 0);

		CHECK_FOR_OPENGL_ERRORS();

		status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if (status != GL_FRAMEBUFFER_COMPLETE)
		{
			THROW_EXCEPTION(OpenGLException, "Error creating SSAO FBO: 0x%x\n", status);
		}

		// create ssao blur fbo
		glGenFramebuffers(1, &mSSAOBlurFBOId);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mSSAOBlurFBOId);

		CHECK_FOR_OPENGL_ERRORS();

		glGenTextures(1, &mBlurredSSAOTextureId);

		// create sso blur attachments
		glBindTexture(GL_TEXTURE_2D, mBlurredSSAOTextureId);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, mrScreenWidth, mrScreenHeight, 0, GL_RGBA, GL_FLOAT, 0);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mBlurredSSAOTextureId, 0);

		CHECK_FOR_OPENGL_ERRORS();

		status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if (status != GL_FRAMEBUFFER_COMPLETE)
		{
			THROW_EXCEPTION(OpenGLException, "Error creating SSAO Blur FBO: 0x%x\n", status);
		}

		// restore to "default" fbo
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	}

	void DeferredRenderingStrategy::Render(const Camera* pCamera)
	{
		static const GLenum pGeometryPassDrawBuffers[] = {
			GL_COLOR_ATTACHMENT0,
			GL_COLOR_ATTACHMENT1,
			GL_COLOR_ATTACHMENT2,
		};

		glViewport(0, 0, mrScreenWidth, mrScreenHeight);

		// geometry pass

		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mGBufferFBOId);
		glDrawBuffers(sizeof(pGeometryPassDrawBuffers) / sizeof(GLenum), pGeometryPassDrawBuffers);

		glClearBufferfv(GL_COLOR, 0, &Colors::BLACK[0]);
		glClearBufferfv(GL_COLOR, 1, &Colors::BLACK[0]);
		glClearBufferfv(GL_COLOR, 2, &Colors::BLACK[0]);
		glDepthMask(GL_TRUE);
		glStencilMask(0xFF);
		glClearBufferfi(GL_DEPTH_STENCIL, 0, 1, 0);
		glDepthMask(GL_FALSE);
		glStencilMask(0);

		// no transparency in this deferred rendering implementation
		glDisable(GL_BLEND);

		auto& rView = pCamera->GetView();
		auto& rProjection = pCamera->GetProjection();
		auto& rInverseProjection = glm::inverse(rProjection);
		float tanHalfFov = MathF::Tan((MathF::DEGREES_TO_RADIANS * pCamera->GetFieldOfView()) / 2.0f);

		mrRenderingStatistics.Reset();
		for (const auto& pRenderBatch : mrRenderBatches)
		{
			auto pMaterial = pRenderBatch->pMaterial;

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

			auto& rMeshFilters = pRenderBatch->meshFilters;
			const auto& pShader = pMaterial->GetShader();
			pShader->Bind();
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

				auto& rModel = pRenderer->GetGameObject()->GetTransform()->GetModel();
				auto modelView = rView * rModel;
				auto modelViewProjection = rProjection * modelView;
				pShader->SetMat4("_ModelView", modelView);
				pShader->SetMat3("_ModelViewInverseTranspose", glm::transpose(glm::inverse(glm::mat3(modelView))));
				pShader->SetMat4("_ModelViewProjection", modelViewProjection);
				pRenderer->Render();
				mrRenderingStatistics.drawCalls += pRenderer->GetNumberOfDrawCalls();
				mrRenderingStatistics.numberOfTriangles += pRenderer->GetNumberOfTriangles();
			}

			pShader->Unbind();

			CHECK_FOR_OPENGL_ERRORS();
		}

		if (mDisplayGBufferEnabled)
		{
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

			glClear(GL_COLOR_BUFFER_BIT);

			glBindFramebuffer(GL_READ_FRAMEBUFFER, mGBufferFBOId);

			// top left
			glReadBuffer(GL_COLOR_ATTACHMENT0);
			glBlitFramebuffer(0, 0, mrScreenWidth, mrScreenHeight, 0, mHalfScreenHeight, mHalfScreenWidth, mrScreenHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);

			// top right
			glViewport(mHalfScreenWidth, mHalfScreenHeight, mHalfScreenWidth, mHalfScreenHeight);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, mDepthTextureId);

			glDisable(GL_DEPTH_TEST);
			glDepthMask(GL_FALSE);
			glDisable(GL_STENCIL_TEST);
			glStencilMask(0);

			mpDepthToScreenShader->Bind();
			mpDepthToScreenShader->SetTexture("_Depth", 0);
			mpQuadMesh->Draw();
			mrRenderingStatistics.drawCalls++;
			mpDepthToScreenShader->Unbind();

			// bottom left
			glReadBuffer(GL_COLOR_ATTACHMENT1);
			glBlitFramebuffer(0, 0, mrScreenWidth, mrScreenHeight, 0, 0, mHalfScreenWidth, mHalfScreenHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);

			// bottom right
			glReadBuffer(GL_COLOR_ATTACHMENT2);
			glBlitFramebuffer(0, 0, mrScreenWidth, mrScreenHeight, mHalfScreenWidth, 0, mrScreenWidth, mHalfScreenHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);

			CHECK_FOR_OPENGL_ERRORS();
		}
		else
		{
			auto isSSAOEnabled = pCamera->IsSSAOEnabled();
			auto ambientOcclusionFlag = (isSSAOEnabled) ? 1.0f : 0.0f;

			if (isSSAOEnabled)
			{
				// ssao high frequency pass

				glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mSSAOFBOId);
				glDrawBuffer(GL_COLOR_ATTACHMENT0);

				glClearBufferfv(GL_COLOR, 0, &Colors::BLACK[0]);

				glDisable(GL_DEPTH_TEST);
				glDepthMask(GL_FALSE);

				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, mNormalTextureId);

				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, mDepthTextureId);

				mpSSAOHighFrequencyPassShader->Bind();
				mpSSAOHighFrequencyPassShader->SetMat4("_Projection", rProjection);
				mpSSAOHighFrequencyPassShader->SetMat4("_InverseProjection", rInverseProjection);
				mpSSAOHighFrequencyPassShader->SetFloat("_AspectRatio", mAspectRatio);
				mpSSAOHighFrequencyPassShader->SetFloat("_TanHalfFov", tanHalfFov);
				mpSSAOHighFrequencyPassShader->SetVec2("_ScreenSize", glm::vec2(mrScreenWidth, mrScreenHeight));
				mpSSAOHighFrequencyPassShader->SetTexture("_NormalMap", 0);
				mpSSAOHighFrequencyPassShader->SetTexture("_DepthMap", 1);
				mpSSAOHighFrequencyPassShader->SetTexture("_NoiseMap", mpNoiseTexture, 2);
				mpSSAOHighFrequencyPassShader->SetFloat("_Radius", mSSAORadius);
				mpSSAOHighFrequencyPassShader->SetFloat("_DistanceScale", mSSAODistanceScale);
				mpSSAOHighFrequencyPassShader->SetVec3Array("_RandomSamples", NUMBER_OF_RANDOM_SAMPLES, &mRandomSamples[0]);
				mpQuadMesh->Draw();
				mrRenderingStatistics.drawCalls++;
				mpSSAOHighFrequencyPassShader->Unbind();

				CHECK_FOR_OPENGL_ERRORS();

				if (mSSAOBlurEnabled)
				{
					glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mSSAOBlurFBOId);
					glDrawBuffer(GL_COLOR_ATTACHMENT0);

					CHECK_FOR_OPENGL_ERRORS();

					glClearBufferfv(GL_COLOR, 0, &Colors::BLACK[0]);

					glActiveTexture(GL_TEXTURE0);
					glBindTexture(GL_TEXTURE_2D, mSSAOTextureId);

					mpSSAOBlurPassShader->Bind();
					mpSSAOBlurPassShader->SetTexture("_AmbientOcclusionMap", 0);
					mpQuadMesh->Draw();
					mrRenderingStatistics.drawCalls++;
					mpSSAOBlurPassShader->Unbind();

					CHECK_FOR_OPENGL_ERRORS();
				}
			}

			if (mDisplaySSAOTextureEnabled)
			{
				glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

				glClear(GL_COLOR_BUFFER_BIT);

				if (mSSAOBlurEnabled)
				{
					glBindFramebuffer(GL_READ_FRAMEBUFFER, mSSAOBlurFBOId);
					glReadBuffer(GL_COLOR_ATTACHMENT0);

					CHECK_FOR_OPENGL_ERRORS();
				}
				else
				{
					glBindFramebuffer(GL_READ_FRAMEBUFFER, mSSAOFBOId);
					glReadBuffer(GL_COLOR_ATTACHMENT0);

					CHECK_FOR_OPENGL_ERRORS();
				}

				glBlitFramebuffer(0, 0, mrScreenWidth, mrScreenHeight, 0, 0, mrScreenWidth, mrScreenHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);

				CHECK_FOR_OPENGL_ERRORS();
			}
			else
			{
				// light pass

				glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mGBufferFBOId);
				glDrawBuffer(GL_COLOR_ATTACHMENT3);

				glClearBufferfv(GL_COLOR, 0, &Colors::BLACK[0]);

				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, mDiffuseTextureId);

				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, mNormalTextureId);

				glActiveTexture(GL_TEXTURE2);
				glBindTexture(GL_TEXTURE_2D, mSpecularTextureId);

				glActiveTexture(GL_TEXTURE3);
				glBindTexture(GL_TEXTURE_2D, mDepthTextureId);

				glActiveTexture(GL_TEXTURE4);
				if (mSSAOBlurEnabled)
				{
					glBindTexture(GL_TEXTURE_2D, mBlurredSSAOTextureId);
				}
				else
				{
					glBindTexture(GL_TEXTURE_2D, mSSAOTextureId);
				}

				for (auto* pPointLight : mrPointLights)
				{
					glStencilMask(0xFF);
					const GLint emptyStencil = 0;
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

					auto& rModel = pPointLight->GetGameObject()->GetTransform()->GetModel();
					auto lightBoundingSphereScale = CalculateLightBoundingSphereScale(pPointLight);
					glm::vec3 scaleVector(lightBoundingSphereScale, lightBoundingSphereScale, lightBoundingSphereScale);
					auto modelViewProjection = rProjection * (rView * glm::scale(rModel, scaleVector));

					mpStencilPassShader->Bind();
					mpStencilPassShader->SetMat4("_ModelViewProjection", modelViewProjection);
					mpSphereMesh->Draw();
					mrRenderingStatistics.drawCalls++;
					mpStencilPassShader->Unbind();

					CHECK_FOR_OPENGL_ERRORS();

					glDrawBuffer(GL_COLOR_ATTACHMENT3);

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
					mpPointLightPassShader->SetMat4("_Projection", rProjection);
					mpPointLightPassShader->SetMat4("_InverseProjection", rInverseProjection);
					mpPointLightPassShader->SetMat4("_ModelViewProjection", modelViewProjection);
					mpPointLightPassShader->SetVec2("_ScreenSize", glm::vec2(mrScreenWidth, mrScreenHeight));
					mpPointLightPassShader->SetTexture("_DiffuseMap", 0);
					mpPointLightPassShader->SetTexture("_NormalMap", 1);
					mpPointLightPassShader->SetTexture("_SpecularMap", 2);
					mpPointLightPassShader->SetTexture("_DepthMap", 3);
					mpPointLightPassShader->SetTexture("_AmbientOcclusionMap", 4);
					mpPointLightPassShader->SetFloat("_AmbientOcclusionFlag", ambientOcclusionFlag);
					mpPointLightPassShader->SetVec4("_GlobalLightAmbientColor", mrAmbientLight);
					mpPointLightPassShader->SetVec3("_Light0Position", glm::vec3(rView * glm::vec4(pPointLight->GetGameObject()->GetTransform()->GetPosition(), 1.0f)));
					mpPointLightPassShader->SetVec4("_Light0AmbientColor", pPointLight->GetAmbientColor());
					mpPointLightPassShader->SetVec4("_Light0DiffuseColor", pPointLight->GetDiffuseColor());
					mpPointLightPassShader->SetVec4("_Light0SpecularColor", pPointLight->GetSpecularColor());
					mpPointLightPassShader->SetFloat("_Light0Intensity", pPointLight->GetIntensity());
					mpPointLightPassShader->SetFloat("_Light0ConstantAttenuation", pPointLight->GetConstantAttenuation());
					mpPointLightPassShader->SetFloat("_Light0LinearAttenuation", pPointLight->GetLinearAttenuation());
					mpPointLightPassShader->SetFloat("_Light0QuadraticAttenuation", pPointLight->GetQuadraticAttenuation());
					if (mShowPointLightsEnabled)
					{
						mpPointLightPassShader->SetFloat("_Debug", 0.25f);
					}
					else
					{
						mpPointLightPassShader->SetFloat("_Debug", 0.0f);
					}
					mpSphereMesh->Draw();
					mrRenderingStatistics.drawCalls++;
					mpPointLightPassShader->Unbind();

					CHECK_FOR_OPENGL_ERRORS();
				}

				glDrawBuffer(GL_COLOR_ATTACHMENT3);

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
				mpDirectionalLightPassShader->SetMat4("_Projection", rProjection);
				mpDirectionalLightPassShader->SetMat4("_InverseProjection", rInverseProjection);
				mpDirectionalLightPassShader->SetVec2("_ScreenSize", glm::vec2(mrScreenWidth, mrScreenHeight));
				mpDirectionalLightPassShader->SetTexture("_DiffuseMap", 0);
				mpDirectionalLightPassShader->SetTexture("_NormalMap", 1);
				mpDirectionalLightPassShader->SetTexture("_SpecularMap", 2);
				mpDirectionalLightPassShader->SetTexture("_DepthMap", 3);
				mpDirectionalLightPassShader->SetTexture("_AmbientOcclusionMap", 4);
				mpDirectionalLightPassShader->SetFloat("_AmbientOcclusionFlag", ambientOcclusionFlag);
				mpDirectionalLightPassShader->SetVec4("_GlobalLightAmbientColor", mrAmbientLight);

				auto inverseCameraRotation = glm::inverse(pCamera->GetGameObject()->GetTransform()->GetRotation());
				for (auto* pDirectionalLight : mrDirectionalLights)
				{
					mpDirectionalLightPassShader->SetVec3("_Light0Position", glm::vec3(glm::normalize(inverseCameraRotation * glm::vec4(pDirectionalLight->GetDirection(), 1.0f))));
					mpDirectionalLightPassShader->SetVec4("_Light0AmbientColor", pDirectionalLight->GetAmbientColor());
					mpDirectionalLightPassShader->SetVec4("_Light0DiffuseColor", pDirectionalLight->GetDiffuseColor());
					mpDirectionalLightPassShader->SetVec4("_Light0SpecularColor", pDirectionalLight->GetSpecularColor());
					mpDirectionalLightPassShader->SetFloat("_Light0Intensity", pDirectionalLight->GetIntensity());
					mpQuadMesh->Draw();
					mrRenderingStatistics.drawCalls++;
				}
				mpDirectionalLightPassShader->Unbind();

				CHECK_FOR_OPENGL_ERRORS();

				glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
				glBindFramebuffer(GL_READ_FRAMEBUFFER, mGBufferFBOId);
				glReadBuffer(GL_COLOR_ATTACHMENT3);
				glBlitFramebuffer(0, 0, mrScreenWidth, mrScreenHeight, 0, 0, mrScreenWidth, mrScreenHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);

				CHECK_FOR_OPENGL_ERRORS();

				RenderUnlitGeometries(rView, rProjection);
			}
		}

		glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
	}

	void DeferredRenderingStrategy::RenderUnlitGeometries(const glm::mat4& view, const glm::mat4& projection)
	{
		if (mrLineRenderers.size() > 0)
		{
			mpLineStripShader->Bind();
			mpLineStripShader->SetMat4("_View", view);
			mpLineStripShader->SetMat4("_Projection", projection);

			for (auto* pLineRenderer : mrLineRenderers)
			{
				if (!pLineRenderer->GetGameObject()->IsActive())
				{
					continue;
				}

				const auto& rModel = pLineRenderer->GetGameObject()->GetTransform()->GetModel();
				auto modelView = view * rModel;
				mpLineStripShader->SetMat4("_Model", rModel);
				mpLineStripShader->SetMat4("_ModelView", modelView);
				mpLineStripShader->SetMat3("_ModelViewInverseTranspose", glm::transpose(glm::inverse(glm::mat3(modelView))));
				mpLineStripShader->SetMat4("_ModelViewProjection", projection * modelView);
				mpLineStripShader->SetVec4("_GlobalLightAmbientColor", mrAmbientLight);
				pLineRenderer->Render();
				mrRenderingStatistics.drawCalls += pLineRenderer->GetNumberOfDrawCalls();
			}

			mpLineStripShader->Unbind();

			CHECK_FOR_OPENGL_ERRORS();
		}

		if (mrPointsRenderer.size() > 0)
		{
			mpPointsShader->Bind();
			mpPointsShader->SetMat4("_View", view);
			mpPointsShader->SetMat4("_Projection", projection);

			for (auto* pPointsRenderer : mrPointsRenderer)
			{
				if (!pPointsRenderer->GetGameObject()->IsActive())
				{
					continue;
				}

				const auto& rModel = pPointsRenderer->GetGameObject()->GetTransform()->GetModel();
				auto modelView = view * rModel;
				mpPointsShader->SetMat4("_Model", rModel);
				mpPointsShader->SetMat4("_ModelView", modelView);
				mpPointsShader->SetMat3("_ModelViewInverseTranspose", glm::transpose(glm::inverse(glm::mat3(modelView))));
				mpPointsShader->SetMat4("_ModelViewProjection", projection * modelView);
				mpPointsShader->SetVec4("_GlobalLightAmbientColor", mrAmbientLight);
				auto* pPoints = pPointsRenderer->GetPoints();

				if (pPoints != 0)
				{
					mpPointsShader->SetFloat("size", pPoints->GetSize());
				}

				pPointsRenderer->Render();
				mrRenderingStatistics.drawCalls += pPointsRenderer->GetNumberOfDrawCalls();
			}

			mpPointsShader->Unbind();

			CHECK_FOR_OPENGL_ERRORS();
		}
	}

}
