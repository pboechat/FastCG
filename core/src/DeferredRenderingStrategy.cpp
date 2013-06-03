#ifndef FIXED_FUNCTION_PIPELINE

#include <DeferredRenderingStrategy.h>
#include <ShaderRegistry.h>
#include <StandardGeometries.h>
#include <Random.h>
#include <MathT.h>
#include <OpenGLExceptions.h>

#include <glm/gtc/matrix_transform.hpp>

const unsigned int DeferredRenderingStrategy::NUMBER_OF_RANDOM_SAMPLES = 30;
const unsigned int DeferredRenderingStrategy::LIGHT_MESH_DETAIL = 20;
const float DeferredRenderingStrategy::RAY_LENGTH = 40.0f;
const float DeferredRenderingStrategy::OCCLUSION_EXPONENTIAL = 1.0f;
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
	mFinalOutputTextureId(0),
	mDepthTextureId(0),
	mAmbientTextureId(0),
	mBlurredAmbientTextureId(0),
	mpQuadMesh(0),
	mpSphereMesh(0)
{
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

	if (mTexturesIds[0] != 0)
	{
		glDeleteTextures(NUMBER_OF_GBUFFER_COLOR_ATTACHMENTS, mTexturesIds);
	}

	if (mDepthTextureId != 0)
	{
		glDeleteTextures(1, &mDepthTextureId);
	}

	if (mAmbientTextureId != 0)
	{
		glDeleteTextures(1, &mAmbientTextureId);
	}

	if (mBlurredAmbientTextureId != 0)
	{
		glDeleteTextures(1, &mBlurredAmbientTextureId);
	}

	if (mFinalOutputTextureId != 0)
	{
		glDeleteTextures(1, &mFinalOutputTextureId);
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
			pNoises[i * NOISE_TEXTURE_HEIGHT + j] = glm::normalize(noise);
		}
	}

	mpNoiseTexture = new Texture(NOISE_TEXTURE_WIDTH, NOISE_TEXTURE_HEIGHT, 3, IF_RGB, DF_FLOAT, FM_LINEAR_FILTER, WM_REPEAT, &pNoises[0][0]);
}

void DeferredRenderingStrategy::GenerateRandomSamplesInAHemisphere()
{
	Random::SeedWithTime();
	for (unsigned int i = 0; i < NUMBER_OF_RANDOM_SAMPLES; i++)
	{
		glm::vec3 randomSampleInAHemisphere(Random::NextFloat() * 2.0f - 1.0f, Random::NextFloat() * 2.0f - 1.0f, Random::NextFloat());
		randomSampleInAHemisphere = glm::normalize(randomSampleInAHemisphere);
		float scale = (float)i / (float)NUMBER_OF_RANDOM_SAMPLES;
		scale = MathF::Lerp(0.1f, 1.0f, MathF::Pow(scale, 2));
		randomSampleInAHemisphere *= scale;
		mRandomSamplesInAHemisphere.push_back(randomSampleInAHemisphere);
	}
}

void DeferredRenderingStrategy::AllocateTexturesAndFBOs()
{
	// create g-buffer fbo
	glGenFramebuffers(1, &mGBufferFBOId);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mGBufferFBOId);

	glGenTextures(4, mTexturesIds);
	glGenTextures(1, &mDepthTextureId);
	glGenTextures(1, &mFinalOutputTextureId);

	// create g-buffer textures
	for (unsigned int i = 0 ; i < 4; i++)
	{
		glBindTexture(GL_TEXTURE_2D, mTexturesIds[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, mrScreenWidth, mrScreenHeight, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, mTexturesIds[i], 0);
	}

	// create final output texture
	glBindTexture(GL_TEXTURE_2D, mFinalOutputTextureId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, mrScreenWidth, mrScreenHeight, 0, GL_RGB, GL_FLOAT, NULL);
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, GL_TEXTURE_2D, mFinalOutputTextureId, 0);

	// create depth texture
	glBindTexture(GL_TEXTURE_2D, mDepthTextureId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, mrScreenWidth, mrScreenHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, mDepthTextureId, 0);

	// create ssao fbo
	glGenFramebuffers(1, &mSSAOFBOId);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mSSAOFBOId);

	glGenTextures(1, &mAmbientTextureId);

	// create ambient texture
	glBindTexture(GL_TEXTURE_2D, mAmbientTextureId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, mrScreenWidth, mrScreenHeight, 0, GL_RGB, GL_FLOAT, NULL);
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mAmbientTextureId, 0);

	// create ssao blur fbo
	glGenFramebuffers(1, &mSSAOBlurFBOId);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mSSAOBlurFBOId);

	glGenTextures(1, &mBlurredAmbientTextureId);

	// create blurred ambient texture
	glBindTexture(GL_TEXTURE_2D, mBlurredAmbientTextureId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, mrScreenWidth, mrScreenHeight, 0, GL_RGB, GL_FLOAT, NULL);
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mBlurredAmbientTextureId, 0);

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
	static unsigned int pDrawBuffers[] = { 
		GL_COLOR_ATTACHMENT0,
		GL_COLOR_ATTACHMENT1,
		GL_COLOR_ATTACHMENT2,
		GL_COLOR_ATTACHMENT3
	};

	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);

	// geometry pass

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mGBufferFBOId);
	glDrawBuffers(4, pDrawBuffers);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::mat4& rView = pCamera->GetView();
	glm::mat4& rProjection = pCamera->GetProjection();
	glm::mat4 inverseProjection = glm::inverse(rProjection);

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
		unsigned int halfScreenWidth = mrScreenWidth / 2;
		unsigned int halfScreenHeight = mrScreenHeight / 2;

		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT);

		glBindFramebuffer(GL_READ_FRAMEBUFFER, mGBufferFBOId);

		// top left
		glReadBuffer(GL_COLOR_ATTACHMENT0);
		glBlitFramebuffer(0, 0, mrScreenWidth, mrScreenHeight, 0, halfScreenHeight, halfScreenWidth, mrScreenHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);

		// top right
		glReadBuffer(GL_COLOR_ATTACHMENT1);
		glBlitFramebuffer(0, 0, mrScreenWidth, mrScreenHeight, halfScreenWidth, halfScreenHeight, mrScreenWidth, mrScreenHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);

		// bottom left
		glReadBuffer(GL_COLOR_ATTACHMENT2);
		glBlitFramebuffer(0, 0, mrScreenWidth, mrScreenHeight, 0, 0, halfScreenWidth, halfScreenHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);

		// bottom right
		glReadBuffer(GL_COLOR_ATTACHMENT3);
		glBlitFramebuffer(0, 0, mrScreenWidth, mrScreenHeight, halfScreenWidth, 0, mrScreenWidth, halfScreenHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);
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
			glBindTexture(GL_TEXTURE_2D, mTexturesIds[2]);

			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, mDepthTextureId);

			mpSSAOHighFrequencyPassShader->Bind();
			mpSSAOHighFrequencyPassShader->SetMat4("_Projection", rProjection);
			mpSSAOHighFrequencyPassShader->SetMat4("_InverseProjection", inverseProjection);
			mpSSAOHighFrequencyPassShader->SetVec2("_ScreenSize", glm::vec2(mrScreenWidth, mrScreenHeight));
			mpSSAOHighFrequencyPassShader->SetTexture("_NormalMap", 0);
			mpSSAOHighFrequencyPassShader->SetTexture("_DepthMap", 1);
			mpSSAOHighFrequencyPassShader->SetTexture("_NoiseMap", mpNoiseTexture, 2);
			mpSSAOHighFrequencyPassShader->SetFloat("_RayLength", RAY_LENGTH);
			mpSSAOHighFrequencyPassShader->SetFloat("_OcclusionExponential", OCCLUSION_EXPONENTIAL);
			mpSSAOHighFrequencyPassShader->SetVec3Array("_RandomSamplesInAHemisphere", NUMBER_OF_RANDOM_SAMPLES, &mRandomSamplesInAHemisphere[0]);
			mpQuadMesh->DrawCall();
			mrRenderingStatistics.drawCalls++;
			mpSSAOHighFrequencyPassShader->Unbind();

			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mSSAOBlurFBOId);
			glDrawBuffer(GL_COLOR_ATTACHMENT0);
			glClear(GL_COLOR_BUFFER_BIT);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, mAmbientTextureId);
			
			mpSSAOBlurPassShader->Bind();
			mpSSAOBlurPassShader->SetVec2("_ScreenSize", glm::vec2(mrScreenWidth, mrScreenHeight));
			mpSSAOBlurPassShader->SetVec2("_TexelSize", glm::vec2(1.0f / mrScreenWidth, 1.0f / mrScreenHeight));
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
			glDrawBuffer(GL_COLOR_ATTACHMENT4);
			glClear(GL_COLOR_BUFFER_BIT);

			for (unsigned int i = 0 ; i < 4; i++) 
			{
				glActiveTexture(GL_TEXTURE0 + i);		
				glBindTexture(GL_TEXTURE_2D, mTexturesIds[i]);
			}

			glActiveTexture(GL_TEXTURE4);
			glBindTexture(GL_TEXTURE_2D, mBlurredAmbientTextureId);

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

				glClear(GL_STENCIL_BUFFER_BIT);

				glDrawBuffer(0);

				mpStencilPassShader->Bind();
				mpStencilPassShader->SetMat4("_ModelViewProjection", modelViewProjection);
				mpSphereMesh->DrawCall();
				mrRenderingStatistics.drawCalls++;
				mpStencilPassShader->Unbind();

				glDisable(GL_DEPTH_TEST); 
				glStencilFunc(GL_NOTEQUAL, 0, 0xFF);
				glEnable(GL_CULL_FACE);
				glCullFace(GL_FRONT);

				glEnable(GL_BLEND); 
				glBlendEquation(GL_FUNC_ADD);
				glBlendFunc(GL_ONE, GL_ONE);

				glm::vec3 lightPosition = glm::vec3(rView * glm::vec4(pPointLight->GetGameObject()->GetTransform()->GetPosition(), 1.0f));

				glDrawBuffer(GL_COLOR_ATTACHMENT4);

				mpPointLightPassShader->Bind();
				mpPointLightPassShader->SetMat4("_ModelViewProjection", modelViewProjection);
				mpPointLightPassShader->SetVec2("_ScreenSize", glm::vec2(mrScreenWidth, mrScreenHeight));
				mpPointLightPassShader->SetTexture("_PositionMap", 0);
				mpPointLightPassShader->SetTexture("_ColorMap", 1);
				mpPointLightPassShader->SetTexture("_NormalMap", 2);
				mpPointLightPassShader->SetTexture("_SpecularMap", 3);
				mpPointLightPassShader->SetTexture("_AmbientOcclusionMap", 4);
				mpPointLightPassShader->SetFloat("_AmbientOcclusionFlag", ambientOcclusionFlag);
				mpPointLightPassShader->SetVec4("_GlobalLightAmbientColor", mrGlobalAmbientLight);
				mpPointLightPassShader->SetVec3("_Light0Position", lightPosition);
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

			glDrawBuffer(GL_COLOR_ATTACHMENT4);

			mpDirectionalLightPassShader->Bind();
			mpDirectionalLightPassShader->SetVec2("_ScreenSize", glm::vec2(mrScreenWidth, mrScreenHeight));
			mpDirectionalLightPassShader->SetTexture("_PositionMap", 0);
			mpDirectionalLightPassShader->SetTexture("_ColorMap", 1);
			mpDirectionalLightPassShader->SetTexture("_NormalMap", 2);
			mpDirectionalLightPassShader->SetTexture("_SpecularMap", 3);
			mpDirectionalLightPassShader->SetTexture("_AmbientOcclusionMap", 4);
			mpDirectionalLightPassShader->SetFloat("_AmbientOcclusionFlag", ambientOcclusionFlag);
			mpDirectionalLightPassShader->SetVec4("_GlobalLightAmbientColor", mrGlobalAmbientLight);

			glm::quat viewRotation = glm::toQuat(rView);
			for (unsigned int i = 0; i < mrDirectionalLights.size(); i++)
			{
				DirectionalLight* pDirectionalLight = mrDirectionalLights[i];

				glm::vec3 lightDirection = glm::vec3(viewRotation * glm::vec4(pDirectionalLight->GetDirection(), 1.0f));

				mpDirectionalLightPassShader->SetVec3("_Light0Position", pDirectionalLight->GetDirection());
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
			glReadBuffer(GL_COLOR_ATTACHMENT4);

			glBlitFramebuffer(0, 0, mrScreenWidth, mrScreenHeight, 0, 0, mrScreenWidth, mrScreenHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);

			RenderUnlitGeometries(rView, rProjection);
		}

		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
	}
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

#endif
