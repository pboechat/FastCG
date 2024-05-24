#include <FastCG/Core/Exception.h>
#include <FastCG/Core/Log.h>
#include <FastCG/Debug/DebugMenuSystem.h>
#include <FastCG/Graphics/GraphicsUtils.h>

#include <imgui.h>

#include <algorithm>
#include <cassert>
#include <functional>

namespace
{
#if _DEBUG
    template <class TextureT>
    void DisplayTexture(const TextureT *pTexture, uint32_t textureWidth)
    {
        float textureHeight = textureWidth * (pTexture->GetHeight() / (float)pTexture->GetWidth());
        ImVec2 uv1 =
#if defined FASTCG_OPENGL
        // FIXME: instead of inverting the V coordinate here we could invert the Y axis of the ImGUI bitmap
        { 0,
          1 }
#else
            {0, 0}
#endif
        ;
        ImVec2 uv2 =
#if defined FASTCG_OPENGL
        { 1,
          0 }
#else
            {1, 1}
#endif
        ;
        ImGui::Image((void *)pTexture, ImVec2((float)textureWidth, textureHeight), uv1, uv2);
    }

    template <class BufferT, class GraphicsContextT, class ShaderT, class TextureT>
    void DisplayTextureBrowser(
        const FastCG::BaseGraphicsSystem<BufferT, GraphicsContextT, ShaderT, TextureT> *pRenderingSystem,
        const TextureT *&rpSelectedTexture)
    {
        if (ImGui::Begin("Texture Browser"))
        {
            auto textureWidth = (uint32_t)(ImGui::GetWindowSize().x * 0.8f);

            for (const auto *pTexture : pRenderingSystem->GetTextures())
            {
                ImGui::PushID(pTexture);
                {
                    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_OpenOnDoubleClick;
                    if (pTexture == rpSelectedTexture)
                    {
                        flags |= ImGuiTreeNodeFlags_Selected;
                    }
                    auto nodeOpened = ImGui::TreeNodeEx(pTexture->GetName().c_str(), flags);

                    if (ImGui::IsItemClicked())
                    {
                        rpSelectedTexture = pTexture;
                    }

                    if (nodeOpened)
                    {
                        ImGui::Text("Dimensions: %ux%u", pTexture->GetWidth(), pTexture->GetHeight());
                        ImGui::Text("Type: %s", GetTextureTypeString(pTexture->GetType()));
                        ImGui::Text("Format: %s", GetTextureFormatString(pTexture->GetFormat()));
                        ImGui::Text("Filter: %s", GetTextureFilterString(pTexture->GetFilter()));
                        ImGui::Text("Wrap Mode: %s", GetTextureWrapModeString(pTexture->GetWrapMode()));

                        DisplayTexture(pTexture, textureWidth);

                        ImGui::TreePop();
                    }
                }
                ImGui::PopID();
            }
        }
        ImGui::End();
    }
#endif

}

namespace FastCG
{
    template <class BufferT, class GraphicsContextT, class ShaderT, class TextureT>
    const TextureT *BaseGraphicsSystem<BufferT, GraphicsContextT, ShaderT, TextureT>::GetMissingTexture(
        TextureType textureType) const
    {
        auto it = mMissingTextures.find(textureType);
        if (it != mMissingTextures.end())
        {
            return it->second;
        }
        else
        {
            assert(false);
            return nullptr;
        }
    }

    template <class BufferT, class GraphicsContextT, class ShaderT, class TextureT>
    const ShaderT *BaseGraphicsSystem<BufferT, GraphicsContextT, ShaderT, TextureT>::FindShader(
        const std::string &rName) const
    {
        auto it = std::find_if(mShaders.cbegin(), mShaders.cend(), [&rName](const auto &pShader) {
            return strcmp(pShader->GetName().c_str(), rName.c_str()) == 0;
        });
        if (it == mShaders.cend())
        {
            return nullptr;
        }
        return *it;
    }

    template <class BufferT, class GraphicsContextT, class ShaderT, class TextureT>
    void BaseGraphicsSystem<BufferT, GraphicsContextT, ShaderT, TextureT>::Initialize()
    {
        FASTCG_LOG_DEBUG(BaseGraphicsSystem, "\tInitializing graphics system");

#if _DEBUG
        DebugMenuSystem::GetInstance()->AddCallback(
            "Rendering System", std::bind(&BaseGraphicsSystem::DebugMenuCallback, this, std::placeholders::_1));
        DebugMenuSystem::GetInstance()->AddItem(
            "Rendering System", std::bind(&BaseGraphicsSystem::DebugMenuItemCallback, this, std::placeholders::_1));
#endif

        OnInitialize();

        CreateDebugObjects();

        mInitialized = true;
    }

    template <class BufferT, class GraphicsContextT, class ShaderT, class TextureT>
    void BaseGraphicsSystem<BufferT, GraphicsContextT, ShaderT, TextureT>::Finalize()
    {
        OnPreFinalize();

        for (const auto *pShader : mShaders)
        {
            delete pShader;
        }
        mShaders.clear();

        for (const auto *pBuffer : mBuffers)
        {
            delete pBuffer;
        }
        mBuffers.clear();

        for (const auto *pTexture : mTextures)
        {
            delete pTexture;
        }
        mTextures.clear();

        for (const auto *pGraphicsContext : mGraphicsContexts)
        {
            delete pGraphicsContext;
        }
        mGraphicsContexts.clear();

        OnPostFinalize();

        mInitialized = false;
    }

    template <class BufferT, class GraphicsContextT, class ShaderT, class TextureT>
    void BaseGraphicsSystem<BufferT, GraphicsContextT, ShaderT, TextureT>::CreateDebugObjects()
    {
        // Create backbuffer handler (fake texture)
        mpBackbuffer =
            CreateTexture({"Fake Backbuffer", 1, 1, 1, 1, TextureType::TEXTURE_2D, TextureUsageFlagBit::RENDER_TARGET,
                           TextureFormat::R8G8B8A8_UNORM, TextureFilter::POINT_FILTER, TextureWrapMode::CLAMP});

        uint8_t data[] = {251, 72, 196, 255}; // pink
        mMissingTextures.emplace(
            TextureType::TEXTURE_2D,
            CreateTexture({"Missing 2D Texture", 1, 1, 1, 1, TextureType::TEXTURE_2D, TextureUsageFlagBit::SAMPLED,
                           TextureFormat::R8G8B8A8_UNORM, TextureFilter::LINEAR_FILTER, TextureWrapMode::CLAMP, data}));
    }

    template <class BufferT, class GraphicsContextT, class ShaderT, class TextureT>
    BufferT *BaseGraphicsSystem<BufferT, GraphicsContextT, ShaderT, TextureT>::CreateBuffer(
        const typename Buffer::Args &rArgs)
    {
        mBuffers.emplace_back(new BufferT{rArgs});
        return mBuffers.back();
    }

    template <class BufferT, class GraphicsContextT, class ShaderT, class TextureT>
    GraphicsContextT *BaseGraphicsSystem<BufferT, GraphicsContextT, ShaderT, TextureT>::CreateGraphicsContext(
        const typename GraphicsContext::Args &rArgs)
    {
        mGraphicsContexts.emplace_back(new GraphicsContextT{rArgs});
        return mGraphicsContexts.back();
    }

    template <class BufferT, class GraphicsContextT, class ShaderT, class TextureT>
    ShaderT *BaseGraphicsSystem<BufferT, GraphicsContextT, ShaderT, TextureT>::CreateShader(
        const typename Shader::Args &rArgs)
    {
        mShaders.emplace_back(new ShaderT{rArgs});
        return mShaders.back();
    }

    template <class BufferT, class GraphicsContextT, class ShaderT, class TextureT>
    TextureT *BaseGraphicsSystem<BufferT, GraphicsContextT, ShaderT, TextureT>::CreateTexture(
        const typename Texture::Args &rArgs)
    {
        mTextures.emplace_back(new TextureT{rArgs});
        return mTextures.back();
    }

    template <class BufferT, class GraphicsContextT, class ShaderT, class TextureT>
    void BaseGraphicsSystem<BufferT, GraphicsContextT, ShaderT, TextureT>::DestroyBuffer(const BufferT *pBuffer)
    {
        assert(pBuffer != nullptr);
        auto it = std::find(mBuffers.cbegin(), mBuffers.cend(), pBuffer);
        if (it != mBuffers.cend())
        {
            mBuffers.erase(it);
            delete pBuffer;
        }
        else
        {
            FASTCG_THROW_EXCEPTION(Exception, "Couldn't destroy Buffer '%s'", pBuffer->GetName().c_str());
        }
    }

    template <class BufferT, class GraphicsContextT, class ShaderT, class TextureT>
    void BaseGraphicsSystem<BufferT, GraphicsContextT, ShaderT, TextureT>::DestroyGraphicsContext(
        const GraphicsContext *pGraphicsContext)
    {
        assert(pGraphicsContext != nullptr);
        auto it = std::find(mGraphicsContexts.cbegin(), mGraphicsContexts.cend(), pGraphicsContext);
        if (it != mGraphicsContexts.cend())
        {
            mGraphicsContexts.erase(it);
            delete pGraphicsContext;
        }
        else
        {
            FASTCG_THROW_EXCEPTION(Exception, "Couldn't destroy GraphicsContext '%s'",
                                   pGraphicsContext->GetName().c_str());
        }
    }

    template <class BufferT, class GraphicsContextT, class ShaderT, class TextureT>
    void BaseGraphicsSystem<BufferT, GraphicsContextT, ShaderT, TextureT>::DestroyShader(const Shader *pShader)
    {
        assert(pShader != nullptr);
        auto it = std::find(mShaders.cbegin(), mShaders.cend(), pShader);
        if (it != mShaders.cend())
        {
            mShaders.erase(it);
            delete pShader;
        }
        else
        {
            FASTCG_THROW_EXCEPTION(Exception, "Couldn't destroy Shader '%s'", pShader->GetName().c_str());
        }
    }

    template <class BufferT, class GraphicsContextT, class ShaderT, class TextureT>
    void BaseGraphicsSystem<BufferT, GraphicsContextT, ShaderT, TextureT>::DestroyTexture(const Texture *pTexture)
    {
        assert(pTexture != nullptr);
        auto it = std::find(mTextures.cbegin(), mTextures.cend(), pTexture);
        if (it != mTextures.cend())
        {
            mTextures.erase(it);
            delete pTexture;
        }
        else
        {
            FASTCG_THROW_EXCEPTION(Exception, "Couldn't destroy Texture '%s'", pTexture->GetName().c_str());
        }
    }

#if _DEBUG
    template <class BufferT, class GraphicsContextT, class ShaderT, class TextureT>
    void BaseGraphicsSystem<BufferT, GraphicsContextT, ShaderT, TextureT>::DebugMenuCallback(int result)
    {
        if (mShowTextureBrowser)
        {
            DisplayTextureBrowser(this, mpSelectedTexture);
        }
    }

    template <class BufferT, class GraphicsContextT, class ShaderT, class TextureT>
    void BaseGraphicsSystem<BufferT, GraphicsContextT, ShaderT, TextureT>::DebugMenuItemCallback(int &result)
    {
        ImGui::Checkbox("Texture Browser", &mShowTextureBrowser);
    }
#endif

}