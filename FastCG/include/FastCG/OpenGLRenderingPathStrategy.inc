#include <FastCG/OpenGLExceptions.h>

namespace FastCG
{
    template <typename SceneConstantsT, typename InstanceConstantsT, typename LightingConstantsT>
    void OpenGLRenderingPathStrategy<SceneConstantsT, InstanceConstantsT, LightingConstantsT>::Initialize()
    {
        CreateUniformBuffers();
    }

    template <typename SceneConstantsT, typename InstanceConstantsT, typename LightingConstantsT>
    void OpenGLRenderingPathStrategy<SceneConstantsT, InstanceConstantsT, LightingConstantsT>::CreateUniformBuffers()
    {
        glGenBuffers(1, &mSceneConstantsBufferId);
        glBindBuffer(GL_UNIFORM_BUFFER, mSceneConstantsBufferId);
#ifdef _DEBUG
        {
            const char bufferLabel[] = "Scene Constants (GL_BUFFER)";
            glObjectLabel(GL_BUFFER, mSceneConstantsBufferId, FASTCG_ARRAYSIZE(bufferLabel), bufferLabel);
        }
#endif
        glBufferData(GL_UNIFORM_BUFFER, sizeof(SceneConstants), &mSceneConstants, GL_STATIC_DRAW);

        FASTCG_CHECK_OPENGL_ERROR();

        glGenBuffers(1, &mInstanceConstantsBufferId);
        glBindBuffer(GL_UNIFORM_BUFFER, mInstanceConstantsBufferId);
#ifdef _DEBUG
        {
            const char bufferLabel[] = "Instance Constants (GL_BUFFER)";
            glObjectLabel(GL_BUFFER, mInstanceConstantsBufferId, FASTCG_ARRAYSIZE(bufferLabel), bufferLabel);
        }
#endif
        glBufferData(GL_UNIFORM_BUFFER, sizeof(InstanceConstants), &mInstanceConstants, GL_STATIC_DRAW);

        FASTCG_CHECK_OPENGL_ERROR();

        glGenBuffers(1, &mLightingConstantsBufferId);
        glBindBuffer(GL_UNIFORM_BUFFER, mLightingConstantsBufferId);
#ifdef _DEBUG
        {
            const char bufferLabel[] = "Lighting Constants (GL_BUFFER)";
            glObjectLabel(GL_BUFFER, mLightingConstantsBufferId, FASTCG_ARRAYSIZE(bufferLabel), bufferLabel);
        }
#endif
        glBufferData(GL_UNIFORM_BUFFER, sizeof(LightingConstants), &mLightingConstants, GL_STATIC_DRAW);

        FASTCG_CHECK_OPENGL_ERROR();
    }

    template <typename SceneConstantsT, typename InstanceConstantsT, typename LightingConstantsT>
    void OpenGLRenderingPathStrategy<SceneConstantsT, InstanceConstantsT, LightingConstantsT>::Finalize()
    {
        DestroyUniformBuffers();
    }

    template <typename SceneConstantsT, typename InstanceConstantsT, typename LightingConstantsT>
    void OpenGLRenderingPathStrategy<SceneConstantsT, InstanceConstantsT, LightingConstantsT>::DestroyUniformBuffers()
    {
        if (mSceneConstantsBufferId != ~0u)
        {
            glDeleteBuffers(1, &mSceneConstantsBufferId);
            mSceneConstantsBufferId = ~0u;
        }

        if (mInstanceConstantsBufferId != ~0u)
        {
            glDeleteBuffers(1, &mInstanceConstantsBufferId);
            mInstanceConstantsBufferId = ~0u;
        }

        if (mLightingConstantsBufferId != ~0u)
        {
            glDeleteBuffers(1, &mLightingConstantsBufferId);
            mLightingConstantsBufferId = ~0u;
        }
    }

    template <typename SceneConstantsT, typename InstanceConstantsT, typename LightingConstantsT>
    void OpenGLRenderingPathStrategy<SceneConstantsT, InstanceConstantsT, LightingConstantsT>::UpdateSceneConstantsBuffer(const Camera *pMainCamera)
    {
        mSceneConstants.view = pMainCamera->GetView();
        mSceneConstants.projection = pMainCamera->GetProjection();
        mSceneConstants.inverseProjection = glm::inverse(mSceneConstants.projection);
        mSceneConstants.pointSize = 1.0f; // TODO:

        glBindBuffer(GL_UNIFORM_BUFFER, mSceneConstantsBufferId);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(SceneConstants), &mSceneConstants);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }

    template <typename SceneConstantsT, typename InstanceConstantsT, typename LightingConstantsT>
    void OpenGLRenderingPathStrategy<SceneConstantsT, InstanceConstantsT, LightingConstantsT>::UpdateInstanceConstantsBuffer(const glm::mat4 &model)
    {
        auto modelView = mSceneConstants.view * model;

        mInstanceConstants.model = model;
        mInstanceConstants.modelView = modelView;
        mInstanceConstants.modelViewInverseTranspose = glm::transpose(glm::inverse(modelView));
        mInstanceConstants.modelViewProjection = mSceneConstants.projection * modelView;

        glBindBuffer(GL_UNIFORM_BUFFER, mInstanceConstantsBufferId);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(InstanceConstants), &mInstanceConstants);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }

    template <typename SceneConstantsT, typename InstanceConstantsT, typename LightingConstantsT>
    void OpenGLRenderingPathStrategy<SceneConstantsT, InstanceConstantsT, LightingConstantsT>::UpdateLightingConstantsBuffer(const PointLight *pPointLight)
    {
        mLightingConstants.light0Position = mSceneConstants.view * glm::vec4(pPointLight->GetGameObject()->GetTransform()->GetPosition(), 1);
        mLightingConstants.light0Position.w = 1;
        mLightingConstants.light0DiffuseColor = pPointLight->GetDiffuseColor();
        mLightingConstants.light0SpecularColor = pPointLight->GetSpecularColor();
        mLightingConstants.light0Intensity = pPointLight->GetIntensity();
        mLightingConstants.light0ConstantAttenuation = pPointLight->GetConstantAttenuation();
        mLightingConstants.light0LinearAttenuation = pPointLight->GetLinearAttenuation();
        mLightingConstants.light0QuadraticAttenuation = pPointLight->GetQuadraticAttenuation();
        mLightingConstants.ambientColor = mArgs.rAmbientLight;

        glBindBuffer(GL_UNIFORM_BUFFER, mLightingConstantsBufferId);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(LightingConstants), &mLightingConstants);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }

    template <typename SceneConstantsT, typename InstanceConstantsT, typename LightingConstantsT>
    void OpenGLRenderingPathStrategy<SceneConstantsT, InstanceConstantsT, LightingConstantsT>::UpdateLightingConstantsBuffer(const DirectionalLight *pDirectionalLight, const glm::vec3 &direction)
    {
        mLightingConstants.light0Position = glm::vec4(direction, -1);
        mLightingConstants.light0DiffuseColor = pDirectionalLight->GetDiffuseColor();
        mLightingConstants.light0SpecularColor = pDirectionalLight->GetSpecularColor();
        mLightingConstants.light0Intensity = pDirectionalLight->GetIntensity();
        mLightingConstants.ambientColor = mArgs.rAmbientLight;

        glBindBuffer(GL_UNIFORM_BUFFER, mLightingConstantsBufferId);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(LightingConstants), &mLightingConstants);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }
}