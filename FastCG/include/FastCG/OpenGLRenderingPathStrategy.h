#ifndef FASTCG_OPENGL_RENDERING_PATH_STRATEGY_H
#define FASTCG_OPENGL_RENDERING_PATH_STRATEGY_H

#ifdef FASTCG_OPENGL

#include <FastCG/RenderingPathStrategy.h>
#include <FastCG/PointLight.h>
#include <FastCG/DirectionalLight.h>
#include <FastCG/Camera.h>

#include <glm/glm.hpp>

#include <GL/glew.h>
#include <GL/gl.h>

namespace FastCG
{
    template <typename SceneConstantsT, typename InstanceConstantsT, typename LightingConstantsT>
    class OpenGLRenderingPathStrategy : public RenderingPathStrategy
    {
    public:
        using SceneConstants = SceneConstantsT;
        using InstanceConstants = InstanceConstantsT;
        using LightingConstants = LightingConstantsT;

        OpenGLRenderingPathStrategy(const RenderingPathStrategyArgs &rArgs) : RenderingPathStrategy(rArgs) {}
        virtual ~OpenGLRenderingPathStrategy() = default;

        inline void Initialize() override;
        inline void Finalize() override;

    protected:
        GLuint mSceneConstantsBufferId{~0u};
        GLuint mInstanceConstantsBufferId{~0u};
        GLuint mLightingConstantsBufferId{~0u};
        SceneConstants mSceneConstants{};
        InstanceConstants mInstanceConstants{};
        LightingConstants mLightingConstants{};

        inline virtual void CreateUniformBuffers();
        inline virtual void DestroyUniformBuffers();
        inline virtual void UpdateSceneConstantsBuffer(const Camera *pMainCamera);
        inline virtual void UpdateInstanceConstantsBuffer(const glm::mat4 &model);
        inline virtual void UpdateLightingConstantsBuffer(const PointLight *pPointLight);
        inline virtual void UpdateLightingConstantsBuffer(const DirectionalLight *pDirectionalLight, const glm::vec3 &direction);
    };

}

#include <FastCG/OpenGLRenderingPathStrategy.inc>

#endif

#endif