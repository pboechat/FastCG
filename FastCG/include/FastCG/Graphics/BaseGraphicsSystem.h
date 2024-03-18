#ifndef FASTCG_BASE_GRAPHICS_SYSTEM_H
#define FASTCG_BASE_GRAPHICS_SYSTEM_H

#include <FastCG/Graphics/BaseTexture.h>
#include <FastCG/Graphics/BaseShader.h>
#include <FastCG/Graphics/BaseGraphicsContext.h>
#include <FastCG/Graphics/BaseBuffer.h>
#include <FastCG/Core/Hash.h>

#include <glm/glm.hpp>

#include <unordered_map>
#include <vector>
#include <type_traits>
#include <string>

namespace FastCG
{
    class BaseApplication;

    struct GraphicsSystemArgs
    {
        const uint32_t &rScreenWidth;
        const uint32_t &rScreenHeight;
        uint32_t maxSimultaneousFrames;
        bool vsync;
    };

    template <class BufferT, class GraphicsContextT, class ShaderT, class TextureT>
    class BaseGraphicsSystem
    {
    public:
        using Buffer = BufferT;
        using GraphicsContext = GraphicsContextT;
        using Shader = ShaderT;
        using Texture = TextureT;

        static_assert(std::is_same<typename GraphicsContext::Shader, Shader>::value, "GraphicsContext::Shader type must be the same as Shader type");
        static_assert(std::is_same<typename GraphicsContext::Texture, Texture>::value, "GraphicsContext::Texture type must be the same as Texture type");
        static_assert(std::is_same<typename GraphicsContext::Buffer, Buffer>::value, "GraphicsContext::Buffer type must be the same as Buffer type");

        inline bool IsInitialized() const
        {
            return mInitialized;
        }
        inline const Texture *GetBackbuffer() const
        {
            return mpBackbuffer;
        }
        inline uint32_t GetScreenWidth() const
        {
            return mArgs.rScreenWidth;
        }
        inline uint32_t GetScreenHeight() const
        {
            return mArgs.rScreenHeight;
        }
        inline const auto &GetBuffers() const
        {
            return mBuffers;
        }
        inline const auto &GetGraphicsContexts() const
        {
            return mGraphicsContexts;
        }
        inline const auto &GetShaders() const
        {
            return mShaders;
        }
        inline const auto &GetTextures() const
        {
            return mTextures;
        }
#if _DEBUG
        inline void SetSelectedTexture(const Texture *pTexture)
        {
            mShowTextureBrowser = true;
            mpSelectedTexture = pTexture;
        }
#endif
        // Template interface methods
        inline uint32_t GetMaxSimultaneousFrames() const;
        inline uint32_t GetCurrentFrame() const;
        inline Buffer *CreateBuffer(const typename Buffer::Args &rArgs);
        inline GraphicsContext *CreateGraphicsContext(const typename GraphicsContext::Args &rArgs);
        inline Shader *CreateShader(const typename Shader::Args &rArgs);
        inline Texture *CreateTexture(const typename Texture::Args &rArgs);
        inline virtual void DestroyBuffer(const Buffer *pBuffer);
        inline virtual void DestroyGraphicsContext(const GraphicsContext *pGraphicsContext);
        inline virtual void DestroyShader(const Shader *pShader);
        inline virtual void DestroyTexture(const Texture *pTexture);
        inline const Shader *FindShader(const std::string &rName) const;
        inline const Texture *GetMissingTexture(TextureType textureType) const;
        inline bool IsHeadless() const;
        inline void Synchronize();
#if defined FASTCG_ANDROID
        inline void OnWindowInitialized();
        inline void OnWindowTerminated();
#endif

    protected:
        const GraphicsSystemArgs mArgs;

        BaseGraphicsSystem(const GraphicsSystemArgs &rArgs) : mArgs(rArgs)
        {
        }
        virtual ~BaseGraphicsSystem() = default;

        // Non-interface methods
        virtual void OnInitialize() {}
        virtual void OnPreFinalize() {}
        virtual void OnPostFinalize() {}
#if _DEBUG
        inline void DebugMenuCallback(int result);
        inline void DebugMenuItemCallback(int &result);
#endif
        // Template interface methods
        void Resize();
        void Present();
        double GetGpuElapsedTime() const;

    private:
        bool mInitialized{false};
        const Texture *mpBackbuffer;
        std::vector<Buffer *> mBuffers;
        std::vector<GraphicsContext *> mGraphicsContexts;
        std::vector<Shader *> mShaders;
        std::vector<Texture *> mTextures;
        std::unordered_map<TextureType, const Texture *, IdentityHasher<TextureType>> mMissingTextures;
#if _DEBUG
        const Texture *mpSelectedTexture{nullptr};
        bool mShowTextureBrowser{false};
#endif

        void Initialize();
        void Finalize();
        void CreateDebugObjects();

        friend class BaseApplication;
    };

}

#include <FastCG/Graphics/BaseGraphicsSystem.inc>

#endif