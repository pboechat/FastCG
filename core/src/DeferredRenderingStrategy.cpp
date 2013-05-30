#ifndef FIXED_FUNCTION_PIPELINE

#include <DeferredRenderingStrategy.h>
#include <ShaderRegistry.h>
#include <StandardGeometries.h>
#include <MathT.h>

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
	mpQuadMesh(0)
{
	mpGBuffer = new GBuffer(rScreenWidth, rScreenHeight);
	mpStencilPassShader = ShaderRegistry::Find("StencilPass");
	mpDirectionalLightPassShader = ShaderRegistry::Find("DirectionalLightPass");
	mpPointLightPassShader = ShaderRegistry::Find("PointLightPass");
	mpSSAOHighFrequencyPassShader = ShaderRegistry::Find("SSAOHighFrequencyPass");
	mpLineStripShader = ShaderRegistry::Find("LineStrip");
	mpPointsShader = ShaderRegistry::Find("Points");
	mpQuadMesh = StandardGeometries::CreateXYPlane(2, 2, 1, 1, glm::vec3(0.0f, 0.0f, 0.0f));
	mpSphereMesh = StandardGeometries::CreateSphere(1.0f, 10);
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
			pShader->SetMat4("_Model", rModel);
			glm::mat4 modelViewProjection = rProjection * (rView * rModel);
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

		mpGBuffer->SetReadBuffer(GBuffer::GTT_POSITION_TEXTURE); 
		glBlitFramebuffer(0, 0, mrScreenWidth, mrScreenHeight, 0, 0, halfScreenWidth, halfScreenHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);

		mpGBuffer->SetReadBuffer(GBuffer::GTT_DIFFUSE_TEXTURE); 
		glBlitFramebuffer(0, 0, mrScreenWidth, mrScreenHeight, 0, halfScreenHeight, halfScreenWidth, mrScreenHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);

		mpGBuffer->SetReadBuffer(GBuffer::GTT_NORMAL_TEXTURE); 
		glBlitFramebuffer(0, 0, mrScreenWidth, mrScreenHeight, halfScreenWidth, 0, mrScreenWidth, halfScreenHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);

		mpGBuffer->SetReadBuffer(GBuffer::GTT_SPECULAR_TEXTURE); 
		glBlitFramebuffer(0, 0, mrScreenWidth, mrScreenHeight, halfScreenWidth, halfScreenHeight, mrScreenWidth, mrScreenHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);
	}
	else
	{
		if (pCamera->IsSSAOEnabled())
		{
			// ssao high frequency pass
			glDisable(GL_DEPTH_TEST);

			mpGBuffer->BindForSSAOHighFrequencyPass();
			mpSSAOHighFrequencyPassShader->Bind();
			mpSSAOHighFrequencyPassShader->SetTexture("normalMap", 0);
			mpSSAOHighFrequencyPassShader->SetTexture("depthMap", 1);
			mpSSAOHighFrequencyPassShader->SetVec2("screenSize", glm::vec2(mrScreenWidth, mrScreenHeight));
			mpQuadMesh->DrawCall();
			mrRenderingStatistics.drawCalls++;
			mrRenderingStatistics.numberOfTriangles += mpQuadMesh->GetNumberOfTriangles();
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
				rModel = glm::scale(rModel, glm::vec3(lightBoundingSphereScale, lightBoundingSphereScale, lightBoundingSphereScale));
				glm::mat4 modelViewProjection = rProjection * (rView * rModel);

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

				mpGBuffer->BindForLightPass();
				mpPointLightPassShader->Bind();
				mpPointLightPassShader->SetTexture("positionMap", GBuffer::GTT_POSITION_TEXTURE);
				mpPointLightPassShader->SetTexture("colorMap", GBuffer::GTT_DIFFUSE_TEXTURE);
				mpPointLightPassShader->SetTexture("normalMap", GBuffer::GTT_NORMAL_TEXTURE);
				mpPointLightPassShader->SetTexture("specularMap", GBuffer::GTT_SPECULAR_TEXTURE);
				mpPointLightPassShader->SetVec4("_GlobalLightAmbientColor", mrGlobalAmbientLight);
				mpPointLightPassShader->SetVec2("screenSize", glm::vec2(mrScreenWidth, mrScreenHeight));
				mpPointLightPassShader->SetVec3("viewerPosition", pCamera->GetGameObject()->GetTransform()->GetPosition());
				mpPointLightPassShader->SetMat4("_View", rView);
				mpPointLightPassShader->SetMat4("_ModelViewProjection", modelViewProjection);
				mpPointLightPassShader->SetVec3("lightPosition", pPointLight->GetGameObject()->GetTransform()->GetPosition());
				mpPointLightPassShader->SetVec4("lightAmbientColor", pPointLight->GetAmbientColor());
				mpPointLightPassShader->SetVec4("lightDiffuseColor", pPointLight->GetDiffuseColor());
				mpPointLightPassShader->SetVec4("lightSpecularColor", pPointLight->GetSpecularColor());
				mpPointLightPassShader->SetFloat("lightIntensity", pPointLight->GetIntensity());
				mpPointLightPassShader->SetFloat("lightConstantAttenuation", pPointLight->GetConstantAttenuation());
				mpPointLightPassShader->SetFloat("lightLinearAttenuation", pPointLight->GetLinearAttenuation());
				mpPointLightPassShader->SetFloat("lightQuadraticAttenuation", pPointLight->GetQuadraticAttenuation());
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
				mrRenderingStatistics.numberOfTriangles += mpSphereMesh->GetNumberOfTriangles();
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
			mpDirectionalLightPassShader->SetTexture("positionMap", GBuffer::GTT_POSITION_TEXTURE);
			mpDirectionalLightPassShader->SetTexture("colorMap", GBuffer::GTT_DIFFUSE_TEXTURE);
			mpDirectionalLightPassShader->SetTexture("normalMap", GBuffer::GTT_NORMAL_TEXTURE);
			mpDirectionalLightPassShader->SetTexture("specularMap", GBuffer::GTT_SPECULAR_TEXTURE);
			mpDirectionalLightPassShader->SetVec4("_GlobalLightAmbientColor", mrGlobalAmbientLight);
			mpDirectionalLightPassShader->SetVec2("screenSize", glm::vec2(mrScreenWidth, mrScreenHeight));
			mpDirectionalLightPassShader->SetVec3("viewerPosition", pCamera->GetGameObject()->GetTransform()->GetPosition());
			mpDirectionalLightPassShader->SetMat4("_View", rView);
			for (unsigned int i = 0; i < mrDirectionalLights.size(); i++)
			{
				DirectionalLight* pDirectionalLight = mrDirectionalLights[i];
				mpDirectionalLightPassShader->SetVec3("lightPosition", pDirectionalLight->GetDirection());
				mpDirectionalLightPassShader->SetVec4("lightAmbientColor", pDirectionalLight->GetAmbientColor());
				mpDirectionalLightPassShader->SetVec4("lightDiffuseColor", pDirectionalLight->GetDiffuseColor());
				mpDirectionalLightPassShader->SetVec4("lightSpecularColor", pDirectionalLight->GetSpecularColor());
				mpDirectionalLightPassShader->SetFloat("lightIntensity", pDirectionalLight->GetIntensity());
				mpQuadMesh->DrawCall();
				mrRenderingStatistics.drawCalls++;
				mrRenderingStatistics.numberOfTriangles += mpQuadMesh->GetNumberOfTriangles();
			}
			mpDirectionalLightPassShader->Unbind();

			glDisable(GL_BLEND);

			mpGBuffer->BindForFinalPass();
			mpGBuffer->SetReadBuffer(GBuffer::GTT_FINAL_TEXTURE);
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
	glm::vec4 diffuseColor = pPointLight->GetDiffuseColor() * pPointLight->GetIntensity();
	float distance = MathF::Max(MathF::Max(diffuseColor.r, diffuseColor.g), diffuseColor.b);
	return 8.0f * MathF::Sqrt(distance) + 1.0f;
}

#endif
