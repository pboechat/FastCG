#ifndef FASTCG_OPENGL_GRAPHICS_SYSTEM_H
#define FASTCG_OPENGL_GRAPHICS_SYSTEM_H

#ifdef FASTCG_OPENGL

#include <FastCG/System.h>
#include <FastCG/ShaderConstants.h>
#include <FastCG/Graphics/OpenGL/OpenGLTexture.h>
#include <FastCG/Graphics/OpenGL/OpenGLShader.h>
#include <FastCG/Graphics/OpenGL/OpenGLGraphicsContext.h>
#include <FastCG/Graphics/OpenGL/OpenGLMesh.h>
#include <FastCG/Graphics/OpenGL/OpenGLMaterialDefinition.h>
#include <FastCG/Graphics/OpenGL/OpenGLMaterial.h>
#include <FastCG/Graphics/OpenGL/OpenGLBuffer.h>
#include <FastCG/Graphics/BaseGraphicsSystem.h>

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
    class OpenGLGraphicsSystem : public BaseGraphicsSystem<OpenGLBuffer, OpenGLMaterial, OpenGLMesh, OpenGLGraphicsContext, OpenGLShader, OpenGLTexture>
    {
        FASTCG_DECLARE_SYSTEM(OpenGLGraphicsSystem, GraphicsSystemArgs);

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
        OpenGLGraphicsContext *CreateGraphicsContext(const GraphicsContextArgs &rArgs);
        OpenGLShader *CreateShader(const ShaderArgs &rArgs);
        OpenGLTexture *CreateTexture(const TextureArgs &rArgs);
        void DestroyBuffer(const OpenGLBuffer *pBuffer);
        void DestroyMaterial(const OpenGLMaterial *pMaterial);
        void DestroyMaterialDefinition(const OpenGLMaterialDefinition *pMaterialDefinition);
        void DestroyMesh(const OpenGLMesh *pMesh);
        void DestroyGraphicsContext(const OpenGLGraphicsContext *pGraphicsContext);
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
        OpenGLGraphicsSystem(const GraphicsSystemArgs &rArgs);
        virtual ~OpenGLGraphicsSystem();

    private:
#if defined FASTCG_WINDOWS
        HDC mHDC{0};
        HGLRC mHGLRC{0};
#elif defined FASTCG_LINUX
        GLXContext mpRenderContext{nullptr};
#endif
        std::vector<OpenGLBuffer *> mBuffers;
        std::vector<OpenGLMaterial *> mMaterials;
        std::vector<OpenGLMaterialDefinition *> mMaterialDefinitions;
        std::vector<OpenGLMesh *> mMeshes;
        std::vector<OpenGLGraphicsContext *> mGraphicsContexts;
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
        double GetPresentElapsedTime() const;
        double GetGpuElapsedTime() const;
        void InitializeGlew();
        void CreateOpenGLContext();
        void InitializeDeviceProperties();
        void DestroyOpenGLContext();
    };

}

#endif

#endif