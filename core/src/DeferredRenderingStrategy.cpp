#include <DeferredRenderingStrategy.h>
#include <ShaderRegistry.h>
#include <StandardGeometries.h>
#include <Random.h>
#include <MathT.h>
#include <OpenGLExceptions.h>

#include <glm/gtc/matrix_transform.hpp>

const unsigned int DeferredRenderingStrategy::NUMBER_OF_RANDOM_SAMPLES = 30;
const unsigned int DeferredRenderingStrategy::LIGHT_MESH_DETAIL = 20;
const float DeferredRenderingStrategy::DEFAULT_SSAO_RADIUS = 0.3f;
const unsigned int DeferredRenderingStrategy::NOISE_TEXTURE_WIDTH = 4;
const unsigned int DeferredRenderingStrategy::NOISE_TEXTURE_HEIGHT = 4;
const unsigned int DeferredRenderingStrategy::NOISE_TEXTURE_SIZE = NOISE_TEXTURE_WIDTH * NOISE_TEXTURE_HEIGHT;

DeferredRenderingStrategy::DeferredRenderingStrategy(std::vector<Light*>& rLights,
													 std::vector<DirectionalLight*>& rDirectionalLights,
													 std::vector<PointLight*>& rPointLights,
													 glm::vec4& rGlobalAmbientLight,
													 std::vector<RenderBatch*>& rRenderingGroups,
													 std::vector<LineRenderer*>& rLineRenderers,
													 std::vector<PointsRenderer*>& rPointsRenderer,
													 RenderingStatistics& rRenderingStatistics,
													 unsigned int& rScreenWidth,
													 unsigned int& rScreenHeight) :
	RenderingStrategy(rLights, rDirectionalLights, rPointLights, rGlobalAmbientLight, rRenderingGroups, rLineRenderers, rPointsRenderer, rRenderingStatistics),
	mrScreenWidth(rScreenWidth),
	mrScreenHeight(rScreenHeight),
	mDisplayGBufferEnabled(false),
	mDisplaySSAOTextureEnabled(false),
	mShowPointLightsEnabled(false),
	mpStencilPassShader(0),
	mpDirectionalLightPassShader(0),
	mpPointLightPassShader(0),
	mpSSAOHighFrequencyPassShader(0),
	mpSSAOBlurPassShader(0),
	mpLineStripShader(0),
	mpPointsShader(0),
	mpNoiseTexture(0),
	mGBufferFBOId(0),
	mSSAOFBOId(0),
	mSSAOBlurFBOId(0),
	mDiffuseTextureId(0),
	mNormalTextureId(0),
	mSpecularTextureId(0),
	mFinalOutputTextureId(0),
	mDepthTextureId(0),
	mAmbientOcclusionTextureId(0),
	mBlurredAmbientOcclusionTextureId(0),
	mSSAORadius(DEFAULT_SSAO_RADIUS),
	mpQuadMesh(0),
	mpSphereMesh(0)
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
	if (mGBufferFBOId != 0)
	{
		glDeleteFramebuffers(1, &mGBufferFBOId);
	}

	if (mSSAOFBOId != 0)
	{
		glDeleteFramebuffers(1, &mSSAOFBOId);
	}

	if (mSSAOBlurFBOId != 0)
	{
		glDeleteFramebuffers(1, &mSSAOBlurFBOId);
	}

	if (mDiffuseTextureId != 0)
	{
		glDeleteTextures(1, &mDiffuseTextureId);
	}

	if (mNormalTextureId != 0)
	{
		glDeleteTextures(1, &mNormalTextureId);
	}

	if (mSpecularTextureId != 0)
	{
		glDeleteTextures(1, &mSpecularTextureId);
	}

	if (mFinalOutputTextureId != 0)
	{
		glDeleteTextures(1, &mFinalOutputTextureId);
	}

	if (mDepthTextureId != 0)
	{
		glDeleteTextures(1, &mDepthTextureId);
	}

	if (mAmbientOcclusionTextureId != 0)
	{
		glDeleteTextures(1, &mAmbientOcclusionTextureId);
	}

	if (mBlurredAmbientOcclusionTextureId != 0)
	{
		glDeleteTextures(1, &mBlurredAmbientOcclusionTextureId);
	}
	
	if (mpQuadMesh != 0)
	{
		delete mpQuadMesh;
	}

	if (mpSphereMesh != 0)
	{
		delete mpSphereMesh;
	}

	if (mpNoiseTexture != 0)
	{
		delete mpNoiseTexture;
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
}

void DeferredRenderingStrategy::BuildAuxiliaryMeshes()
{
	mpQuadMesh = StandardGeometries::CreateXYPlane(2, 2, 1, 1, glm::vec3(0.0f, 0.0f, 0.0f));
	mpSphereMesh = StandardGeometries::CreateSphere(1.0f, LIGHT_MESH_DETAIL);
}

void DeferredRenderingStrategy::GenerateNoiseTexture()
{
	Random::SeedWithTime();
	glm::vec3* pNoises = new glm::vec3[NOISE_TEXTURE_SIZE];
	for (unsigned int i = 0; i < NOISE_TEXTURE_HEIGHT; i++)
	{
		for (unsigned int j = 0; j < NOISE_TEXTURE_WIDTH; j++)
		{
			glm::vec3 noise(Random::NextFloat() * 2.0f - 1.0f, Random::NextFloat() * 2.0f - 1.0f, 0.0f);
			pNoises[i * NOISE_TEXTURE_WIDTH + j] = glm::normalize(noise);
		}
	}

	mpNoiseTexture = new Texture(NOISE_TEXTURE_WIDTH, NOISE_TEXTURE_HEIGHT, 3, IF_RGB, DF_FLOAT, FM_POINT_FILTER, WM_REPEAT, &pNoises[0][0]);
}

void DeferredRenderingStrategy::GenerateRandomSamplesInAHemisphere()
{
	Random::SeedWithTime();
	for (unsigned int i = 0; i < NUMBER_OF_RANDOM_SAMPLES; i++)
	{
		glm::vec3 randomSample(Random::NextFloat() * 2.0f - 1.0f, Random::NextFloat() * 2.0f - 1.0f, Random::NextFloat());
		randomSample = glm::normalize(randomSample);
		float scale = i / (float)NUMBER_OF_RANDOM_SAMPLES;
		scale = MathF::Lerp(0.1f, 1.0f, MathF::Pow(scale, 2));
		randomSample *= scale;
		mRandomSamples.push_back(randomSample);
	}
}

void DeferredRenderingStrategy::AllocateTexturesAndFBOs()
{
	// create g-buffer fbo
	glGenFramebuffers(1, &mGBufferFBOId);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mGBufferFBOId);

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

	glBindTexture(GL_TEXTURE_2D, mNormalTextureId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, mrScreenWidth, mrScreenHeight, 0, GL_RGBA, GL_FLOAT, 0);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, mNormalTextureId, 0);

	glBindTexture(GL_TEXTURE_2D, mSpecularTextureId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, mrScreenWidth, mrScreenHeight, 0, GL_RGBA, GL_FLOAT, 0);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, mSpecularTextureId, 0);

	glBindTexture(GL_TEXTURE_2D, mFinalOutputTextureId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, mrScreenWidth, mrScreenHeight, 0, GL_RGBA, GL_FLOAT, 0);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, mFinalOutputTextureId, 0);

	glBindTexture(GL_TEXTURE_2D, mDepthTextureId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, mrScreenWidth, mrScreenHeight, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, mDepthTextureId, 0);

	// create ssao fbo
	glGenFramebuffers(1, &mSSAOFBOId);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mSSAOFBOId);

	glGenTextures(1, &mAmbientOcclusionTextureId);

	glBindTexture(GL_TEXTURE_2D, mAmbientOcclusionTextureId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, mrScreenWidth, mrScreenHeight, 0, GL_RGBA, GL_FLOAT, 0);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mAmbientOcclusionTextureId, 0);

	// create ssao blur fbo
	glGenFramebuffers(1, &mSSAOBlurFBOId);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mSSAOBlurFBOId);

	glGenTextures(1, &mBlurredAmbientOcclusionTextureId);

	glBindTexture(GL_TEXTURE_2D, mBlurredAmbientOcclusionTextureId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, mrScreenWidth, mrScreenHeight, 0, GL_RGBA, GL_FLOAT, 0);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mBlurredAmbientOcclusionTextureId, 0);

	unsigned int status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	if (status != GL_FRAMEBUFFER_COMPLETE)
	{
		THROW_EXCEPTION(OpenGLException, "FBO error: 0x%x\n", status);
	}

	// restore default FBO
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}

void DeferredRenderingStrategy::Render(const Camera* pCamera)
{
	static unsigned int pGeometryPassDrawBuffers[] = { 
		GL_COLOR_ATTACHMENT0,
		GL_COLOR_ATTACHMENT1,
		GL_COLOR_ATTACHMENT2
	};

	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);

	// geometry pass

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mGBufferFBOId);
	glDrawBuffers(3, pGeometryPassDrawBuffers);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::mat4& rView = pCamera->GetView();
	glm::mat4& rProjection = pCamera->GetProjection();
	glm::mat4& rInverseProjection = glm::inverse(rProjection);

	mrRenderingStatistics.Reset();
	for (unsigned int i = 0; i < mrRenderBatches.size(); i++)
	{
		RenderBatch* pRenderBatch = mrRenderBatches[i];
		Material* pMaterial = pRenderBatch->pMaterial;
		std::vector<MeshFilter*>& rMeshFilters = pRenderBatch->meshFilters;
		Shader* pShader = pMaterial->GetShader();
		pShader->Bind();
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

			glm::mat4& rModel = pRenderer->GetGameObject()->GetTransform()->GetModel();
			glm::mat4 modelView = rView * rModel;
			glm::mat4 modelViewProjection = rProjection * modelView;
			pShader->SetMat4("_ModelView", modelView);
			pShader->SetMat3("_ModelViewInverseTranspose", glm::transpose(glm::inverse(glm::mat3(modelView))));
			pShader->SetMat4("_ModelViewProjection", modelViewProjection);
			pRenderer->Render();
			mrRenderingStatistics.drawCalls += pRenderer->GetNumberOfDrawCalls();
			mrRenderingStatistics.numberOfTriangles += pRenderer->GetNumberOfTriangles();
		}

		pShader->Unbind();

		// FIXME: shouldn't be necessary if we could guarantee that all textures are unbound after use!
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	glDepthMask(GL_FALSE);

	if (mDisplayGBufferEnabled)
	{
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT);

		glBindFramebuffer(GL_READ_FRAMEBUFFER, mGBufferFBOId);

		// top left
		glReadBuffer(GL_COLOR_ATTACHMENT0);
		glBlitFramebuffer(0, 0, mrScreenWidth, mrScreenHeight, 0, mHalfScreenHeight, mHalfScreenWidth, mrScreenHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);

		// top right
		glReadBuffer(GL_COLOR_ATTACHMENT1);
		glBlitFramebuffer(0, 0, mrScreenWidth, mrScreenHeight, mHalfScreenWidth, mHalfScreenHeight, mrScreenWidth, mrScreenHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);

		// bottom left
		glReadBuffer(GL_COLOR_ATTACHMENT2);
		glBlitFramebuffer(0, 0, mrScreenWidth, mrScreenHeight, 0, 0, mHalfScreenWidth, mHalfScreenHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);

		// bottom right
		glReadBuffer(GL_DEPTH_ATTACHMENT);
		glBlitFramebuffer(0, 0, mrScreenWidth, mrScreenHeight, mHalfScreenWidth, 0, mrScreenWidth, mHalfScreenHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);
	}
	else
	{
		bool isSSAOEnabled = pCamera->IsSSAOEnabled();
		float ambientOcclusionFlag = (isSSAOEnabled) ? 1.0f : 0.0f;

		if (isSSAOEnabled)
		{
			// ssao high frequency pass
			glDisable(GL_DEPTH_TEST);

			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mSSAOFBOId);
			glDrawBuffer(GL_COLOR_ATTACHMENT0);

			glClear(GL_COLOR_BUFFER_BIT);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, mNormalTextureId);

			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, mDepthTextureId);

			mpSSAOHighFrequencyPassShader->Bind();
			mpSSAOHighFrequencyPassShader->SetMat4("_Projection", rProjection);
			mpSSAOHighFrequencyPassShader->SetMat4("_InverseProjection", rInverseProjection);
			mpSSAOHighFrequencyPassShader->SetVec2("_ScreenSize", glm::vec2(mrScreenWidth, mrScreenHeight));
			mpSSAOHighFrequencyPassShader->SetTexture("_NormalMap", 0);
			mpSSAOHighFrequencyPassShader->SetTexture("_DepthMap", 1);
			mpSSAOHighFrequencyPassShader->SetTexture("_NoiseMap", mpNoiseTexture, 2);
			mpSSAOHighFrequencyPassShader->SetFloat("_Radius", mSSAORadius);
			mpSSAOHighFrequencyPassShader->SetVec3Array("_RandomSamples", NUMBER_OF_RANDOM_SAMPLES, &mRandomSamples[0]);
			mpQuadMesh->DrawCall();
			mrRenderingStatistics.drawCalls++;
			mpSSAOHighFrequencyPassShader->Unbind();

			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mSSAOBlurFBOId);
			glDrawBuffer(GL_COLOR_ATTACHMENT0);
			glClear(GL_COLOR_BUFFER_BIT);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, mAmbientOcclusionTextureId);
			
			mpSSAOBlurPassShader->Bind();
			mpSSAOBlurPassShader->SetTexture("_AmbientOcclusionMap", 0);
			mpQuadMesh->DrawCall();
			mrRenderingStatistics.drawCalls++;
			mpSSAOBlurPassShader->Unbind();
		}

		if (mDisplaySSAOTextureEnabled)
		{
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
			glClear(GL_COLOR_BUFFER_BIT);

			glBindFramebuffer(GL_READ_FRAMEBUFFER, mSSAOBlurFBOId);
			glReadBuffer(GL_COLOR_ATTACHMENT0);

			glBlitFramebuffer(0, 0, mrScreenWidth, mrScreenHeight, 0, 0, mrScreenWidth, mrScreenHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);
		}
		else
		{
			// light pass
			glEnable(GL_STENCIL_TEST);

			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mGBufferFBOId);
			glDrawBuffer(GL_COLOR_ATTACHMENT3);
			glClear(GL_COLOR_BUFFER_BIT);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, mDiffuseTextureId);

			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, mNormalTextureId);

			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, mSpecularTextureId);

			glActiveTexture(GL_TEXTURE3);
			glBindTexture(GL_TEXTURE_2D, mDepthTextureId);

			glActiveTexture(GL_TEXTURE4);
			glBindTexture(GL_TEXTURE_2D, mBlurredAmbientOcclusionTextureId);

			for (unsigned int i = 0; i < mrPointLights.size(); i++)
			{
				PointLight* pPointLight = mrPointLights[i];
				glm::mat4& rModel = pPointLight->GetGameObject()->GetTransform()->GetModel();
				float lightBoundingSphereScale = CalculateLightBoundingBoxScale(pPointLight);
				glm::vec3 scaleVector(lightBoundingSphereScale, lightBoundingSphereScale, lightBoundingSphereScale);
				glm::mat4 modelViewProjection = rProjection * (rView * glm::scale(rModel, scaleVector));

				glEnable(GL_DEPTH_TEST);

				glStencilFunc(GL_ALWAYS, 0, 0);
				glStencilOpSeparate(GL_BACK, GL_KEEP, GL_INCR, GL_KEEP);
				glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_DECR, GL_KEEP);
				glDisable(GL_CULL_FACE);

				glDrawBuffer(0);
				glClear(GL_STENCIL_BUFFER_BIT);

				mpStencilPassShader->Bind();
				mpStencilPassShader->SetMat4("_ModelViewProjection", modelViewProjection);
				mpSphereMesh->DrawCall();
				mrRenderingStatistics.drawCalls++;
				mpStencilPassShader->Unbind();

				glDisable(GL_DEPTH_TEST); 
				glStencilFunc(GL_NOTEQUAL, 0, 0xff);
				glEnable(GL_CULL_FACE);
				glCullFace(GL_FRONT);

				glEnable(GL_BLEND); 
				glBlendEquation(GL_FUNC_ADD);
				glBlendFunc(GL_ONE, GL_ONE);

				glDrawBuffer(GL_COLOR_ATTACHMENT3);

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
				mpPointLightPassShader->SetVec4("_GlobalLightAmbientColor", mrGlobalAmbientLight);
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
				mpSphereMesh->DrawCall();
				mrRenderingStatistics.drawCalls++;
				mpPointLightPassShader->Unbind();

				glCullFace(GL_BACK);
				glDisable(GL_BLEND);
			}

			glDisable(GL_STENCIL_TEST);
			glDisable(GL_DEPTH_TEST); 

			glEnable(GL_BLEND); 
			glBlendEquation(GL_FUNC_ADD);
			glBlendFunc(GL_ONE, GL_ONE);

			glDrawBuffer(GL_COLOR_ATTACHMENT3);

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
			mpDirectionalLightPassShader->SetVec4("_GlobalLightAmbientColor", mrGlobalAmbientLight);

			glm::quat inverseCameraRotation = glm::inverse(pCamera->GetGameObject()->GetTransform()->GetRotation());
			for (unsigned int i = 0; i < mrDirectionalLights.size(); i++)
			{
				DirectionalLight* pDirectionalLight = mrDirectionalLights[i];
				mpDirectionalLightPassShader->SetVec3("_Light0Position", glm::vec3(glm::normalize(inverseCameraRotation * glm::vec4(pDirectionalLight->GetDirection(), 1.0f))));
				mpDirectionalLightPassShader->SetVec4("_Light0AmbientColor", pDirectionalLight->GetAmbientColor());
				mpDirectionalLightPassShader->SetVec4("_Light0DiffuseColor", pDirectionalLight->GetDiffuseColor());
				mpDirectionalLightPassShader->SetVec4("_Light0SpecularColor", pDirectionalLight->GetSpecularColor());
				mpDirectionalLightPassShader->SetFloat("_Light0Intensity", pDirectionalLight->GetIntensity());
				mpQuadMesh->DrawCall();
				mrRenderingStatistics.drawCalls++;
			}
			mpDirectionalLightPassShader->Unbind();

			glDisable(GL_BLEND);

			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

			glBindFramebuffer(GL_READ_FRAMEBUFFER, mGBufferFBOId);
			glReadBuffer(GL_COLOR_ATTACHMENT3);

			glBlitFramebuffer(0, 0, mrScreenWidth, mrScreenHeight, 0, 0, mrScreenWidth, mrScreenHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);

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
			mrRenderingStatistics.drawCalls += pLineRenderer->GetNumberOfDrawCalls();
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
			mrRenderingStatistics.drawCalls += pPointsRenderer->GetNumberOfDrawCalls();
		}

		mpPointsShader->Unbind();
	}
}

float DeferredRenderingStrategy::CalculateLightBoundingBoxScale(PointLight* pPointLight)
{
	glm::vec4 diffuseColor = pPointLight->GetDiffuseColor();
	float maxChannel = MathF::Max(MathF::Max(diffuseColor.r, diffuseColor.g), diffuseColor.b);
	float c = maxChannel * pPointLight->GetIntensity();
	return (8.0f * MathF::Sqrt(c) + 1.0f);
}
