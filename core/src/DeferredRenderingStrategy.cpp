#ifndef FIXED_FUNCTION_PIPELINE

#include <DeferredRenderingStrategy.h>
#include <ShaderRegistry.h>
#include <StandardGeometries.h>
#include <Random.h>
#include <MathT.h>

#include <glm/gtc/matrix_transform.hpp>

const unsigned int DeferredRenderingStrategy::NUMBER_OF_RANDOM_SAMPLES = 30;
const float DeferredRenderingStrategy::RAY_LENGTH = 40.0f;
const unsigned int DeferredRenderingStrategy::LIGHT_MESH_DETAIL = 20;

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
	mpGBuffer(0),
	mpStencilPassShader(0),
	mpDirectionalLightPassShader(0),
	mpPointLightPassShader(0),
	mpLineStripShader(0),
	mpPointsShader(0),
	mpNoiseTexture(0),
	mpQuadMesh(0),
	mpSphereMesh(0)
{
	mpGBuffer = new GBuffer(rScreenWidth, rScreenHeight);

	FindShaders();
	BuildAuxiliaryMeshes();
	GenerateNoiseTexture();
	GenerateRandomSamplesInAHemisphere();
}

DeferredRenderingStrategy::~DeferredRenderingStrategy()
{
	if (mpGBuffer != 0)
	{
		delete mpGBuffer;
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

void DeferredRenderingStrategy::Render(const Camera* pCamera)
{
	mpGBuffer->StartFrame();

	// geometry pass
	mpGBuffer->BindForGeometryPass();

	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);

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

		mpGBuffer->BindForDebugging();

		// top left
		mpGBuffer->SetReadBuffer(GBuffer::GTT_COLOR_TEXTURE); 
		glBlitFramebuffer(0, 0, mrScreenWidth, mrScreenHeight, 0, halfScreenHeight, halfScreenWidth, mrScreenHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);

		// top right
		mpGBuffer->SetReadBuffer(GBuffer::GTT_SPECULAR_TEXTURE); 
		glBlitFramebuffer(0, 0, mrScreenWidth, mrScreenHeight, halfScreenWidth, halfScreenHeight, mrScreenWidth, mrScreenHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);

		// bottom left
		mpGBuffer->SetReadBuffer(GBuffer::GTT_POSITION_TEXTURE); 
		glBlitFramebuffer(0, 0, mrScreenWidth, mrScreenHeight, 0, 0, halfScreenWidth, halfScreenHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);

		// bottom right
		mpGBuffer->SetReadBuffer(GBuffer::GTT_NORMAL_TEXTURE); 
		glBlitFramebuffer(0, 0, mrScreenWidth, mrScreenHeight, halfScreenWidth, 0, mrScreenWidth, halfScreenHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);
	}
	else
	{
		if (pCamera->IsSSAOEnabled())
		{
			// ssao high frequency pass
			glDisable(GL_DEPTH_TEST);

			mpGBuffer->BindForSSAOHighFrequencyPass();
			mpSSAOHighFrequencyPassShader->Bind();
			mpSSAOHighFrequencyPassShader->SetMat4("_Projection", rProjection);
			mpSSAOHighFrequencyPassShader->SetMat4("_InverseProjection", inverseProjection);
			mpSSAOHighFrequencyPassShader->SetVec2("_ScreenSize", glm::vec2(mrScreenWidth, mrScreenHeight));
			mpSSAOHighFrequencyPassShader->SetTexture("_NormalMap", 0);
			mpSSAOHighFrequencyPassShader->SetTexture("_DepthMap", 1);
			mpSSAOHighFrequencyPassShader->SetTexture("_NoiseMap", mpNoiseTexture, 2);
			mpSSAOHighFrequencyPassShader->SetFloat("_RayLength", RAY_LENGTH);
			mpSSAOHighFrequencyPassShader->SetVec3Array("_RandomSamplesInAHemisphere", NUMBER_OF_RANDOM_SAMPLES, &mRandomSamplesInAHemisphere[0]);
			mpQuadMesh->DrawCall();
			mrRenderingStatistics.drawCalls++;
			mpSSAOHighFrequencyPassShader->Unbind();
		}

		if (mDisplaySSAOTextureEnabled)
		{
			mpGBuffer->BindForDebugging();
			mpGBuffer->SetReadBuffer(GBuffer::GTT_SSAO_TEXTURE); 
			glBlitFramebuffer(0, 0, mrScreenWidth, mrScreenHeight, 0, 0, mrScreenWidth, mrScreenHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);
		}
		else
		{
			// light pass
			glEnable(GL_STENCIL_TEST);

			for (unsigned int i = 0; i < mrPointLights.size(); i++)
			{
				PointLight* pPointLight = mrPointLights[i];
				glm::mat4& rModel = pPointLight->GetGameObject()->GetTransform()->GetModel();
				float lightBoundingSphereScale = CalculateLightBoundingBoxScale(pPointLight);
				glm::vec3 scaleVector(lightBoundingSphereScale, lightBoundingSphereScale, lightBoundingSphereScale);
				glm::mat4 modelViewProjection = rProjection * (rView * glm::scale(rModel, scaleVector));

				glClear(GL_STENCIL_BUFFER_BIT);

				glEnable(GL_DEPTH_TEST);
				glStencilFunc(GL_ALWAYS, 0, 0);
				glStencilOpSeparate(GL_BACK, GL_KEEP, GL_INCR, GL_KEEP);
				glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_DECR, GL_KEEP);
				glDisable(GL_CULL_FACE);

				mpGBuffer->BindForStencilPass();
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

				mpGBuffer->BindForLightPass();
				mpPointLightPassShader->Bind();
				mpPointLightPassShader->SetMat4("_ModelViewProjection", modelViewProjection);
				mpPointLightPassShader->SetVec2("_ScreenSize", glm::vec2(mrScreenWidth, mrScreenHeight));
				mpPointLightPassShader->SetTexture("_PositionMap", GBuffer::GTT_POSITION_TEXTURE);
				mpPointLightPassShader->SetTexture("_ColorMap", GBuffer::GTT_COLOR_TEXTURE);
				mpPointLightPassShader->SetTexture("_NormalMap", GBuffer::GTT_NORMAL_TEXTURE);
				mpPointLightPassShader->SetTexture("_SpecularMap", GBuffer::GTT_SPECULAR_TEXTURE);
				mpPointLightPassShader->SetTexture("_AmbientMap", GBuffer::GTT_SSAO_TEXTURE);
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
					mpPointLightPassShader->SetFloat("debug", 0.25f);
				}
				else
				{
					mpPointLightPassShader->SetFloat("debug", 0.0f);
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

			mpGBuffer->BindForLightPass();
			mpDirectionalLightPassShader->Bind();
			mpDirectionalLightPassShader->SetVec2("_ScreenSize", glm::vec2(mrScreenWidth, mrScreenHeight));
			mpDirectionalLightPassShader->SetTexture("_PositionMap", GBuffer::GTT_POSITION_TEXTURE);
			mpDirectionalLightPassShader->SetTexture("_ColorMap", GBuffer::GTT_COLOR_TEXTURE);
			mpDirectionalLightPassShader->SetTexture("_NormalMap", GBuffer::GTT_NORMAL_TEXTURE);
			mpDirectionalLightPassShader->SetTexture("_SpecularMap", GBuffer::GTT_SPECULAR_TEXTURE);
			mpDirectionalLightPassShader->SetTexture("_AmbientMap", GBuffer::GTT_SSAO_TEXTURE);
			mpDirectionalLightPassShader->SetVec4("_GlobalLightAmbientColor", mrGlobalAmbientLight);
			for (unsigned int i = 0; i < mrDirectionalLights.size(); i++)
			{
				DirectionalLight* pDirectionalLight = mrDirectionalLights[i];

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

			mpGBuffer->BindForFinalPass();
			mpGBuffer->SetReadBuffer(GBuffer::GTT_FINAL_OUTPUT_TEXTURE);
			glBlitFramebuffer(0, 0, mrScreenWidth, mrScreenHeight, 0, 0, mrScreenWidth, mrScreenHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);

			RenderUnlitGeometries(rView, rProjection);
		}

		// FIXME: find out why glut crashes when READ framebuffer is not set back to 0
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

void DeferredRenderingStrategy::FindShaders()
{
	mpStencilPassShader = ShaderRegistry::Find("StencilPass");
	mpDirectionalLightPassShader = ShaderRegistry::Find("DirectionalLightPass");
	mpPointLightPassShader = ShaderRegistry::Find("PointLightPass");
	mpSSAOHighFrequencyPassShader = ShaderRegistry::Find("SSAOHighFrequencyPass");
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
	glm::vec3* pNoises = new glm::vec3[16];
	for (unsigned int i = 0; i < 16; i++)
	{
		glm::vec3 noise(Random::NextFloat() * 2.0f - 1.0f, Random::NextFloat() * 2.0f - 1.0f, 0.0f);
		pNoises[i] = glm::normalize(noise);
	}

	mpNoiseTexture = new Texture(4, 4, 3, IF_RGB, DF_FLOAT, FM_LINEAR_FILTER, WM_REPEAT, &pNoises[0][0]);
}

void DeferredRenderingStrategy::GenerateRandomSamplesInAHemisphere()
{
	Random::SeedWithTime();
	for (unsigned int i = 0; i < NUMBER_OF_RANDOM_SAMPLES; i++)
	{
		glm::vec3 randomSampleInAHemisphere(Random::NextFloat() * 2.0f - 1.0f, Random::NextFloat() * 2.0f - 1.0f, Random::NextFloat());
		randomSampleInAHemisphere = glm::normalize(randomSampleInAHemisphere);
		float scale = i / (float)NUMBER_OF_RANDOM_SAMPLES;
		scale = MathF::Lerp(0.1f, 1.0f, scale * scale);
		randomSampleInAHemisphere *= scale;
		mRandomSamplesInAHemisphere.push_back(randomSampleInAHemisphere);
	}
}

#endif
