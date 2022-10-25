#ifndef FASTCG_OPENGL_RENDERING_SYSTEM_H
#define FASTCG_OPENGL_RENDERING_SYSTEM_H

#ifdef FASTCG_OPENGL

#include <FastCG/ShaderConstants.h>
#include <FastCG/OpenGLTexture.h>
#include <FastCG/OpenGLShader.h>
#include <FastCG/OpenGLMesh.h>
#include <FastCG/OpenGLMaterial.h>
#include <FastCG/FastCG.h>
#include <FastCG/BaseRenderingSystem.h>
#include <FastCG/BaseApplication.h>

#if defined FASTCG_WINDOWS
#include <Windows.h>
#elif defined FASTCG_LINUX
#include <GL/glxew.h>
#endif

#include <vector>
#include <memory>
#include <cstring>
#include <algorithm>

namespace FastCG
{
    class RenderingPathStrategy;

    class OpenGLRenderingSystem : public BaseRenderingSystem<OpenGLMaterial, OpenGLMesh, OpenGLShader, OpenGLTexture>
    {
        FASTCG_DECLARE_SYSTEM(OpenGLRenderingSystem, RenderingSystemArgs);

    public:
        inline RenderingPathStrategy *GetRenderingPathStrategy() const
        {
            return mpRenderingPathStrategy.get();
        }

        void Render(const Camera *pMainCamera);
        void RenderImGui(const ImDrawData *pImDrawData);
        void Initialize();
        void PostInitialize();
        void Present();
        void Resize() {}
        void Finalize();
#ifdef FASTCG_LINUX
        XVisualInfo *GetVisualInfo();
#endif
        OpenGLMaterial *CreateMaterial(const OpenGLMaterial::MaterialArgs &rArgs);
        OpenGLMesh *CreateMesh(const MeshArgs &rArgs);
        OpenGLShader *CreateShader(const ShaderArgs &rArgs);
        OpenGLTexture *CreateTexture(const TextureArgs &rArgs);
        inline const OpenGLShader *FindShader(const std::string &rName) const
        {
            auto it = std::find_if(mShaders.cbegin(), mShaders.cend(), [&rName](const auto &pShader)
                                   { return strcmp(pShader->GetName().c_str(), rName.c_str()) == 0; });
            if (it == mShaders.cend())
            {
                return nullptr;
            }
            return *it;
        }

        friend class BaseApplication;

    protected:
        OpenGLRenderingSystem(const RenderingSystemArgs &rArgs);
        virtual ~OpenGLRenderingSystem();

    private:
#if defined FASTCG_WINDOWS
        HGLRC mHGLRC{0};
#elif defined FASTCG_LINUX
        XVisualInfo *mpVisualInfo{nullptr};
        GLXFBConfig mpFbConfig{nullptr};
        GLXContext mpRenderContext{nullptr};
#endif
        std::unique_ptr<RenderingPathStrategy> mpRenderingPathStrategy;
        std::vector<OpenGLMaterial *> mMaterials;
        std::vector<OpenGLMesh *> mMeshes;
        std::vector<OpenGLShader *> mShaders;
        std::vector<OpenGLTexture *> mTextures;
        ImGuiConstants mImGuiConstants{};
        GLint mImGuiColorMapLocation{-1};
        const OpenGLShader *mpImGuiShader{nullptr};
        GLuint mImGuiConstantsBufferId{~0u};
        GLuint mImGuiVertexArrayId{~0u};
        GLuint mImGuiVerticesBufferId{~0u};
        GLuint mImGuiIndicesBufferId{~0u};

#ifdef FASTCG_LINUX
        void AcquireVisualInfoAndFbConfig();
#endif
        void CreateOpenGLContext(bool temporary = false);
        void DestroyOpenGLContext();
    };

}

#endif

#endif