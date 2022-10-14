#ifndef FASTCG_OPENGL_RENDERING_SYSTEM_H
#define FASTCG_OPENGL_RENDERING_SYSTEM_H

#ifdef FASTCG_OPENGL

#include <FastCG/FastCG.h>
#include <FastCG/BaseRenderingSystem.h>
#include <FastCG/BaseApplication.h>

#include <memory>

#include <GL/gl.h>
#include <GL/glew.h>
#ifdef FASTCG_WINDOWS
#include <GL/wglew.h>
#include <Windows.h>
#endif

namespace FastCG
{
    class RenderingPathStrategy;

    class OpenGLRenderingSystem : public BaseRenderingSystem
    {
    public:
        inline static OpenGLRenderingSystem *GetInstance()
        {
            return static_cast<OpenGLRenderingSystem *>(BaseRenderingSystem::GetInstance());
        }

        inline RenderingPathStrategy *GetRenderingPathStrategy() const
        {
            return mpRenderingPathStrategy.get();
        }

        friend class BaseApplication;

    protected:
        OpenGLRenderingSystem(const RenderingSystemArgs &rArgs);
        virtual ~OpenGLRenderingSystem() = default;

        void OnRender(const Camera *pMainCamera) override;
        void OnInitialize() override;
        void OnStart() override;
        void OnFinalize() override;
        void BeforeDrawDebugTexts() override;
        void AfterDrawDebugTexts() override;

    private:
#ifdef FASTCG_WINDOWS
        HGLRC mHGLRC{0};
#endif
        std::unique_ptr<RenderingPathStrategy> mpRenderingPathStrategy;

        void CreateOpenGLContext();
        void DestroyOpenGLContext();
    };

}

#endif

#endif