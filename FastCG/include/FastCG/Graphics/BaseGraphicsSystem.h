#ifndef FASTCG_BASE_GRAPHICS_SYSTEM_H
#define FASTCG_BASE_GRAPHICS_SYSTEM_H

#include <FastCG/Graphics/BaseTexture.h>
#include <FastCG/Graphics/BaseShader.h>
#include <FastCG/Graphics/BaseGraphicsContext.h>
#include <FastCG/Graphics/BaseBuffer.h>

#include <glm/glm.hpp>

#include <vector>
#include <type_traits>
#include <string>
#include <cassert>
#include <algorithm>

namespace FastCG
{
    class BaseApplication;

    struct GraphicsSystemArgs
    {
        const uint32_t &rScreenWidth;
        const uint32_t &rScreenHeight;
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

        // Template interface methods
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
        inline Buffer *CreateBuffer(const BufferArgs &rArgs);
        inline GraphicsContext *CreateGraphicsContext(const GraphicsContextArgs &rArgs);
        inline Shader *CreateShader(const ShaderArgs &rArgs);
        inline Texture *CreateTexture(const TextureArgs &rArgs);
        inline void DestroyBuffer(const Buffer *pBuffer);
        inline void DestroyGraphicsContext(const GraphicsContext *pGraphicsContext);
        inline void DestroyShader(const Shader *pShader);
        inline void DestroyTexture(const Texture *pTexture);
        inline const Shader *FindShader(const std::string &rName) const;
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
#ifdef _DEBUG
        inline const Texture *GetSelectedTexture() const
        {
            return mpSelectedTexture;
        }

        inline void SetSelectedTexture(const Texture *pTexture)
        {
            mShowTextureBrowser = true;
            mpSelectedTexture = pTexture;
        }

#endif

    protected:
        const GraphicsSystemArgs mArgs;

        BaseGraphicsSystem(const GraphicsSystemArgs &rArgs) : mArgs(rArgs)
        {
        }
        virtual ~BaseGraphicsSystem() = default;

        // Non-interface methods
        virtual void OnInitialize() {}
        virtual void OnFinalize() {}
#ifdef _DEBUG
        inline void DebugMenuCallback(int result);
        inline void DebugMenuItemCallback(int &result);
#endif
        // Template interface methods
        void Resize();
        void Present();
        double GetPresentElapsedTime() const;
        double GetGpuElapsedTime() const;

    private:
        bool mInitialized{false};
        const Texture *mpBackbuffer;
        std::vector<Buffer *> mBuffers;
        std::vector<GraphicsContext *> mGraphicsContexts;
        std::vector<Shader *> mShaders;
        std::vector<Texture *> mTextures;
#ifdef _DEBUG
        const Texture *mpSelectedTexture{nullptr};
        bool mShowTextureBrowser{false};
#endif

        void Initialize();
        void Finalize();

        friend class BaseApplication;
    };

}

#include <FastCG/Graphics/BaseGraphicsSystem.inc>

#endif