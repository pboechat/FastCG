#ifndef FASTCG_OPENGL_RENDERING_SYSTEM_H
#define FASTCG_OPENGL_RENDERING_SYSTEM_H

#ifdef FASTCG_OPENGL

#include <FastCG/FastCG.h>
#include <FastCG/BaseRenderingSystem.h>
#include <FastCG/BaseApplication.h>

#include <memory>

#ifdef FASTCG_WINDOWS
#include <Windows.h>
#endif

namespace FastCG
{
    class RenderingPathStrategy;

    class OpenGLRenderingSystem : public BaseRenderingSystem
    {
        FASTCG_DECLARE_SYSTEM(OpenGLRenderingSystem, RenderingSystemArgs);

    public:
        inline RenderingPathStrategy *GetRenderingPathStrategy() const
        {
            return mpRenderingPathStrategy.get();
        }

        void Render(const Camera *pMainCamera) override;
        void DrawDebugTexts() override;

        friend class BaseApplication;

    protected:
        OpenGLRenderingSystem(const RenderingSystemArgs &rArgs);
        virtual ~OpenGLRenderingSystem() = default;

        void OnInitialize() override;
        void OnStart() override;
        void OnFinalize() override;

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