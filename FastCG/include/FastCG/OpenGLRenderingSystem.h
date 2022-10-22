#ifndef FASTCG_OPENGL_RENDERING_SYSTEM_H
#define FASTCG_OPENGL_RENDERING_SYSTEM_H

#ifdef FASTCG_OPENGL

#include <FastCG/OpenGLTexture.h>
#include <FastCG/OpenGLShader.h>
#include <FastCG/OpenGLMesh.h>
#include <FastCG/OpenGLMaterial.h>
#include <FastCG/FastCG.h>
#include <FastCG/BaseRenderingSystem.h>
#include <FastCG/BaseApplication.h>

#ifdef FASTCG_WINDOWS
#include <Windows.h>
#endif

#include <vector>
#include <memory>
#include <cstring>

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
        void Initialize();
        void PostInitialize();
        void Finalize();
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
#ifdef FASTCG_WINDOWS
        HGLRC mHGLRC{0};
#endif
        std::unique_ptr<RenderingPathStrategy> mpRenderingPathStrategy;
        std::vector<OpenGLMaterial *> mMaterials;
        std::vector<OpenGLMesh *> mMeshes;
        std::vector<OpenGLShader *> mShaders;
        std::vector<OpenGLTexture *> mTextures;

        void CreateOpenGLContext();
        void DestroyOpenGLContext();
    };

}

#endif

#endif