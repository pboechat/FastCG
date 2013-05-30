#ifdef FIXED_FUNCTION_PIPELINE

#include <FixedFunctionRenderingStrategy.h>

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/freeglut.h>

FixedFunctionRenderingStrategy::FixedFunctionRenderingStrategy(std::vector<Light*>& rLights,
															   std::vector<DirectionalLight*>& rDirectionalLights,
															   std::vector<PointLight*>& rPointLights,
															   glm::vec4& rGlobalAmbientLight,
															   std::vector<RenderBatch*>& rRenderBatches,
															   std::vector<LineRenderer*>& rLineRenderers,
															   std::vector<PointsRenderer*>& rPointsRenderer,
															   RenderingStatistics& rRenderingStatistics) :
					RenderingStrategy(rLights, rDirectionalLights, rPointLights, rGlobalAmbientLight, rRenderBatches, rLineRenderers, rPointsRenderer, rRenderingStatistics)
{
}

void FixedFunctionRenderingStrategy::Render(const Camera* pCamera)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::mat4& rView = pCamera->GetView();
	glm::mat4& rProjection = pCamera->GetProjection();

	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(&rProjection[0][0]);
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(&rView[0][0]);
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, &mrGlobalAmbientLight[0]);

	for (unsigned int i = 0; i < mrLights.size(); i++)
	{
		glEnable(GL_LIGHT0 + i);
		Light* pLight = mrLights[i];
		glm::vec4& rLightPosition = glm::vec4(pLight->GetGameObject()->GetTransform()->GetWorldPosition(), 1.0f);
		float intensity = pLight->GetIntensity();
		glm::vec4 lightAmbientColor = pLight->GetAmbientColor() * intensity;
		glm::vec4 lightDiffuseColor = pLight->GetDiffuseColor() * intensity;
		glm::vec4 lightSpecularColor = pLight->GetSpecularColor() * intensity;
		glLightfv(GL_LIGHT0 + i, GL_POSITION, &rLightPosition[0]);
		glLightfv(GL_LIGHT0 + i, GL_AMBIENT, &lightAmbientColor[0]);
		glLightfv(GL_LIGHT0 + i, GL_DIFFUSE, &lightDiffuseColor[0]);
		glLightfv(GL_LIGHT0 + i, GL_SPECULAR, &lightSpecularColor[0]);
		if (pLight->GetType().IsExactly(PointLight::TYPE))
		{
			PointLight* pPointLight = dynamic_cast<PointLight*>(pLight);
			glLightf(GL_LIGHT0 + i, GL_CONSTANT_ATTENUATION, pPointLight->GetConstantAttenuation());
			glLightf(GL_LIGHT0 + i, GL_LINEAR_ATTENUATION, pPointLight->GetLinearAttenuation());
			glLightf(GL_LIGHT0 + i, GL_QUADRATIC_ATTENUATION, pPointLight->GetQuadraticAttenuation());
		}
		/*else if (pLight->GetType().IsExactly(SpotLight::TYPE))
		{
			glLightfv(GL_LIGHT0 + i, GL_SPOT_DIRECTION, &pLight->GetSpotDirection()[0]);
			glLightf(GL_LIGHT0 + i, GL_SPOT_CUTOFF, pLight->GetSpotCutoff());
			glLightf(GL_LIGHT0 + i, GL_SPOT_EXPONENT, pLight->GetSpotExponent());
		}*/
		else if (!pLight->GetType().IsExactly(DirectionalLight::TYPE))
		{
			// FIXME: checking invariants
			THROW_EXCEPTION(Exception, "Unknown light type: %s", pLight->GetType().GetName().c_str());
		}
	}

	mrRenderingStatistics.Reset();
	for (unsigned int i = 0; i < mrRenderBatches.size(); i++)
	{
		RenderBatch* pRenderBatch = mrRenderBatches[i];
		Material* pMaterial = pRenderBatch->pMaterial;
		std::vector<MeshFilter*>& rMeshFilters = pRenderBatch->meshFilters;
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

			const glm::mat4& rModel = pRenderer->GetGameObject()->GetTransform()->GetModel();
			glPushMatrix();
			glMultMatrixf(&rModel[0][0]);
			pRenderer->Render();
			mrRenderingStatistics.drawCalls += pRenderer->GetNumberOfDrawCalls();
			mrRenderingStatistics.numberOfTriangles += pRenderer->GetNumberOfTriangles();
			glPopMatrix();
		}
		// FIXME: shouldn't be necessary if we could guarantee that all textures are unbound after use!
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	if (mrLineRenderers.size() > 0)
	{
		for (unsigned int i = 0; i < mrLineRenderers.size(); i++)
		{
			LineRenderer* pLineRenderer = mrLineRenderers[i];

			if (!pLineRenderer->GetGameObject()->IsActive())
			{
				continue;
			}

			const glm::mat4& rModel = pLineRenderer->GetGameObject()->GetTransform()->GetModel();
			glPushMatrix();
			glMultMatrixf(&rModel[0][0]);
			pLineRenderer->Render();
			mrRenderingStatistics.drawCalls += pLineRenderer->GetNumberOfDrawCalls();
			glPopMatrix();
		}
	}

	if (mrPointsRenderer.size() > 0)
	{
		for (unsigned int i = 0; i < mrPointsRenderer.size(); i++)
		{
			PointsRenderer* pPointsRenderer = mrPointsRenderer[i];

			if (!pPointsRenderer->GetGameObject()->IsActive())
			{
				continue;
			}

			const glm::mat4& rModel = pPointsRenderer->GetGameObject()->GetTransform()->GetModel();
			glPushMatrix();
			glMultMatrixf(&rModel[0][0]);
			pPointsRenderer->Render();
			mrRenderingStatistics.drawCalls += pPointsRenderer->GetNumberOfDrawCalls();
			glPopMatrix();
		}
	}
}

#endif