#include <FastCG/RenderingEnums.h>
#include <FastCG/DebugMenuSystem.h>

#include <imgui.h>

#include <functional>

namespace
{
#ifdef _DEBUG
    template <class TextureT>
    void DisplayTexture(const TextureT *pTexture, uint32_t textureWidth)
    {
        float textureHeight = textureWidth * (pTexture->GetHeight() / (float)pTexture->GetWidth());
        // FIXME: Instead of inverting the V coordinate here we could invert the Y axis of the ImGUI bitmap
        ImGui::Image((void *)pTexture, ImVec2((float)textureWidth, textureHeight), ImVec2(0, 1), ImVec2(1, 0));
    }

    template <class BufferT, class MaterialT, class MeshT, class RenderingContextT, class ShaderT, class TextureT>
    void DisplayTextureBrowser(const FastCG::BaseRenderingSystem<BufferT, MaterialT, MeshT, RenderingContextT, ShaderT, TextureT> *pRenderingSystem, const TextureT *&rpSelectedTexture)
    {
        if (ImGui::Begin("Texture Browser"))
        {
            auto textureWidth = (uint32_t)(ImGui::GetWindowSize().x * 0.8f);

            for (size_t i = 0; i < pRenderingSystem->GetTextureCount(); ++i)
            {
                const auto *pTexture = pRenderingSystem->GetTextureAt(i);
                ImGui::PushID(pTexture);
                ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_SpanFullWidth;
                if (pTexture == rpSelectedTexture)
                {
                    flags |= ImGuiTreeNodeFlags_Selected;
                }
                if (ImGui::TreeNodeEx(pTexture->GetName().c_str(), flags))
                {

                    ImGui::Text("Dimensions: %ux%u", pTexture->GetWidth(), pTexture->GetHeight());
                    ImGui::Text("Type: %s", GetTextureTypeString(pTexture->GetType()));
                    ImGui::Text("Format: %s", GetTextureFormatString(pTexture->GetFormat()));
                    const auto &rBpp = pTexture->GetBitsPerPixel();
                    ImGui::Text("R: %d G: %d B: %d A: %d", rBpp.r, rBpp.g, rBpp.b, rBpp.a);
                    ImGui::Text("Data Type: %s", GetTextureDataTypeString(pTexture->GetDataType()));
                    ImGui::Text("Filter: %s", GetTextureFilterString(pTexture->GetFilter()));
                    ImGui::Text("Wrap Mode: %s", GetTextureWrapModeString(pTexture->GetWrapMode()));

                    DisplayTexture(pTexture, textureWidth);

                    if (ImGui::IsItemClicked())
                    {
                        rpSelectedTexture = pTexture;
                    }
                    ImGui::TreePop();
                }
                ImGui::PopID();
            }
        }
        ImGui::End();
    }

    template <class BufferT, class MaterialT, class MeshT, class RenderingContextT, class ShaderT, class TextureT>
    void DisplayMaterialBrowser(const FastCG::BaseRenderingSystem<BufferT, MaterialT, MeshT, RenderingContextT, ShaderT, TextureT> *pRenderingSystem, const MaterialT *&rpSelectedMaterial)
    {
        if (ImGui::Begin("Material Browser"))
        {
            auto textureWidth = (uint32_t)(ImGui::GetWindowSize().x * 0.333f);

            for (size_t i = 0; i < pRenderingSystem->GetMaterialCount(); ++i)
            {
                const auto *pMaterial = pRenderingSystem->GetMaterialAt(i);
                ImGui::PushID(pMaterial);
                ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_SpanFullWidth;
                if (pMaterial == rpSelectedMaterial)
                {
                    flags |= ImGuiTreeNodeFlags_Selected;
                }
                if (ImGui::TreeNodeEx(pMaterial->GetName().c_str(), flags))
                {
                    ImGui::Text("Shader: %s", pMaterial->GetShader()->GetName().c_str());
                    for (size_t i = 0; i < pMaterial->GetConstantCount(); ++i)
                    {
                        const auto &rConstant = pMaterial->GetConstantAt(i);
                        if (rConstant.IsVec4())
                        {
                            glm::vec4 value;
                            pMaterial->GetConstant(rConstant.GetName(), value);
                            ImGui::Text("%s: (%.3f, %.3f, %.3f, %.3f)", rConstant.GetName().c_str(), value.x, value.y, value.z, value.w);
                        }
                        else if (rConstant.IsVec2())
                        {
                            glm::vec2 value;
                            pMaterial->GetConstant(rConstant.GetName(), value);
                            ImGui::Text("%s: (%.3f, %.3f)", rConstant.GetName().c_str(), value.x, value.y);
                        }
                        else if (rConstant.IsFloat())
                        {
                            float value;
                            pMaterial->GetConstant(rConstant.GetName(), value);
                            ImGui::Text("%s: %.3f", rConstant.GetName().c_str(), value);
                        }
                        else
                        {
                            assert(false);
                        }
                    }
                    for (size_t i = 0; i < pMaterial->GetTextureCount(); ++i)
                    {
                        std::string name;
                        const TextureT *pTexture;
                        pMaterial->GetTextureAt(i, name, pTexture);
                        ImGui::Text("%s:", name.c_str());
                        if (pTexture != nullptr)
                        {
                            ImGui::Text("<%s>", pTexture->GetName().c_str());
                            DisplayTexture(pTexture, textureWidth);
                        }
                    }
                    ImGui::TreePop();
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
    template <class BufferT, class MaterialT, class MeshT, class RenderingContextT, class ShaderT, class TextureT>
    void BaseRenderingSystem<BufferT, MaterialT, MeshT, RenderingContextT, ShaderT, TextureT>::Initialize()
    {
#ifdef _DEBUG
        DebugMenuSystem::GetInstance()->AddCallback("Rendering System", std::bind(&BaseRenderingSystem::DebugMenuCallback, this, std::placeholders::_1));
        DebugMenuSystem::GetInstance()->AddItem("Rendering System", std::bind(&BaseRenderingSystem::DebugMenuItemCallback, this, std::placeholders::_1));
#endif
    }

#ifdef _DEBUG
    template <class BufferT, class MaterialT, class MeshT, class RenderingContextT, class ShaderT, class TextureT>
    void BaseRenderingSystem<BufferT, MaterialT, MeshT, RenderingContextT, ShaderT, TextureT>::DebugMenuCallback(int result)
    {
        if (mShowTextureBrowser)
        {
            DisplayTextureBrowser(this, mpSelectedTexture);
        }
        if (mShowMaterialBrowser)
        {
            DisplayMaterialBrowser(this, mpSelectedMaterial);
        }
    }

    template <class BufferT, class MaterialT, class MeshT, class RenderingContextT, class ShaderT, class TextureT>
    void BaseRenderingSystem<BufferT, MaterialT, MeshT, RenderingContextT, ShaderT, TextureT>::DebugMenuItemCallback(int &result)
    {
        ImGui::Checkbox("Texture Browser", &mShowTextureBrowser);
        ImGui::Checkbox("Material Browser", &mShowMaterialBrowser);
    }
#endif

}