#ifndef FASTCG_OPENGL_RENDERING_SYSTEM_H
#define FASTCG_OPENGL_RENDERING_SYSTEM_H

#ifdef FASTCG_OPENGL

#include <FastCG/System.h>
#include <FastCG/ShaderConstants.h>
#include <FastCG/OpenGLTexture.h>
#include <FastCG/OpenGLShader.h>
#include <FastCG/OpenGLRenderingContext.h>
#include <FastCG/OpenGLMesh.h>
#include <FastCG/OpenGLMaterialDefinition.h>
#include <FastCG/OpenGLMaterial.h>
#include <FastCG/OpenGLBuffer.h>
#include <FastCG/BaseRenderingSystem.h>

#if defined FASTCG_WINDOWS
#include <Windows.h>
#elif defined FASTCG_LINUX
#include <GL/glxew.h>
#endif
#include <GL/glew.h>
#include <GL/gl.h>

#include <vector>
#include <unordered_map>
#include <memory>
#include <cstring>
#include <algorithm>

namespace FastCG
{
    class OpenGLRenderingSystem : public BaseRenderingSystem<OpenGLBuffer, OpenGLMaterial, OpenGLMesh, OpenGLRenderingContext, OpenGLShader, OpenGLTexture>
    {
        FASTCG_DECLARE_SYSTEM(OpenGLRenderingSystem, RenderingSystemArgs);

    public:
        struct DeviceProperties
        {
            GLint maxColorAttachments;
            GLint maxDrawBuffers;
        };

        inline bool IsInitialized() const
        {
            return mInitialized;
        }
        inline const OpenGLTexture *GetBackbuffer() const
        {
            return mpBackbuffer;
        }
        OpenGLBuffer *CreateBuffer(const BufferArgs &rArgs);
        OpenGLMaterial *CreateMaterial(const OpenGLMaterial::MaterialArgs &rArgs);
        OpenGLMaterialDefinition *CreateMaterialDefinition(const OpenGLMaterialDefinition::MaterialDefinitionArgs &rArgs);
        OpenGLMesh *CreateMesh(const MeshArgs &rArgs);
        OpenGLRenderingContext *CreateRenderingContext();
        OpenGLShader *CreateShader(const ShaderArgs &rArgs);
        OpenGLTexture *CreateTexture(const TextureArgs &rArgs);
        void DestroyBuffer(const OpenGLBuffer *pBuffer);
        void DestroyMaterial(const OpenGLMaterial *pMaterial);
        void DestroyMaterialDefinition(const OpenGLMaterialDefinition *pMaterialDefinition);
        void DestroyMesh(const OpenGLMesh *pMesh);
        void DestroyRenderingContext(const OpenGLRenderingContext *pRenderingContext);
        void DestroyShader(const OpenGLShader *pShader);
        void DestroyTexture(const OpenGLTexture *pTexture);
        inline const OpenGLMaterialDefinition *FindMaterialDefinition(const std::string &rName) const
        {
            auto it = std::find_if(mMaterialDefinitions.cbegin(), mMaterialDefinitions.cend(), [&rName](const auto &pMaterialDefinition)
                                   { return strcmp(pMaterialDefinition->GetName().c_str(), rName.c_str()) == 0; });
            if (it == mMaterialDefinitions.cend())
            {
                return nullptr;
            }
            return *it;
        }
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
#if defined FASTCG_WINDOWS
        void SetupPixelFormat() const;
#elif defined FASTCG_LINUX
        XVisualInfo *GetVisualInfo();
#endif
        inline size_t GetTextureCount() const override
        {
            return mTextures.size();
        }
        inline const OpenGLTexture *GetTextureAt(size_t i) const override
        {
            assert(i < mTextures.size());
            return mTextures[i];
        }
        inline size_t GetMaterialCount() const override
        {
            return mMaterials.size();
        }
        inline const OpenGLMaterial *GetMaterialAt(size_t i) const override
        {
            assert(i < mMaterials.size());
            return mMaterials[i];
        }
        GLuint GetOrCreateFramebuffer(const OpenGLTexture *const *pTextures, size_t textureCount);
        GLuint GetOrCreateVertexArray(const OpenGLBuffer *const *pBuffers, size_t bufferCount);
        inline const DeviceProperties &GetDeviceProperties() const
        {
            return mDeviceProperties;
        }

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
        std::vector<OpenGLBuffer *> mBuffers;
        std::vector<OpenGLMaterial *> mMaterials;
        std::vector<OpenGLMaterialDefinition *> mMaterialDefinitions;
        std::vector<OpenGLMesh *> mMeshes;
        std::vector<OpenGLRenderingContext *> mRenderingContexts;
        std::vector<OpenGLShader *> mShaders;
        std::vector<OpenGLTexture *> mTextures;
        bool mInitialized{false};
        const OpenGLTexture *mpBackbuffer;
        std::unordered_map<uint32_t, GLuint> mFboIds;
        std::unordered_map<GLint, std::vector<GLuint>> mTextureToFboHashes;
        std::unordered_map<uint32_t, GLuint> mVaoIds;
#ifdef _DEBUG
        GLuint mPresentTimestampQuery{~0u};
        double mPresentElapsedTime{0};
#endif
        DeviceProperties mDeviceProperties{};

        void Initialize() override;
        void Resize() {}
        void Present();
        void Finalize();
        double GetLastPresentElapsedTime();
#ifdef FASTCG_LINUX
        void AcquireVisualInfoAndFbConfig();
#endif
        void CreateOpenGLContext(bool temporary = false);
        void InitializeDeviceProperties();
        void DestroyOpenGLContext();
    };

}

#endif

#endif