#include <FastCG/Core/Log.h>
#include <FastCG/Core/Math.h>
#include <FastCG/Core/StringUtils.h>
#include <FastCG/Debug/DebugMenuSystem.h>
#include <FastCG/Graphics/GraphicsSystem.h>
#include <FastCG/Platform/Application.h>
#include <FastCG/Rendering/Camera.h>
#include <FastCG/Rendering/DirectionalLight.h>
#include <FastCG/Rendering/Fog.h>
#include <FastCG/Rendering/Material.h>
#include <FastCG/Rendering/MaterialDefinitionRegistry.h>
#include <FastCG/Rendering/OBJLoader.h>
#include <FastCG/Rendering/PointLight.h>
#include <FastCG/Rendering/Renderable.h>
#include <FastCG/World/Behaviour.h>
#include <FastCG/World/Component.h>
#include <FastCG/World/ComponentRegistry.h>
#include <FastCG/World/GameObject.h>
#include <FastCG/World/GameObjectDumper.h>
#include <FastCG/World/GameObjectLoader.h>
#include <FastCG/World/Transform.h>
#include <FastCG/World/WorldSystem.h>

#include <imgui.h>
#if _DEBUG
#include <ImGuiFileDialog.h>
#include <glm/gtc/type_ptr.hpp>
#endif

#include <algorithm>
#include <cassert>
#include <memory>
#include <unordered_set>

#ifdef max
#undef max
#endif

#define FASTCG_TRACK_COMPONENT(className, component)                                                                   \
    if (component->GetType().IsDerived(className::TYPE))                                                               \
    {                                                                                                                  \
        mp##className = static_cast<className *>(component);                                                           \
    }

#define FASTCG_UNTRACK_COMPONENT(className, component)                                                                 \
    if (component->GetType().IsDerived(className::TYPE))                                                               \
    {                                                                                                                  \
        mp##className = nullptr;                                                                                       \
    }

#define FASTCG_TRACK_COMPONENT_COLLECTION(className, component)                                                        \
    if (component->GetType().IsDerived(className::TYPE))                                                               \
    {                                                                                                                  \
        m##className##s.emplace_back(static_cast<className *>(component));                                             \
    }

#define FASTCG_UNTRACK_COMPONENT_COLLECTION(className, component)                                                      \
    if (component->GetType().IsDerived(className::TYPE))                                                               \
    {                                                                                                                  \
        auto it = std::find(m##className##s.begin(), m##className##s.end(), component);                                \
        if (it == m##className##s.end())                                                                               \
        {                                                                                                              \
            FASTCG_THROW_EXCEPTION(Exception, "Couldn't untrack component (component: %s)", #className);               \
        }                                                                                                              \
        m##className##s.erase(it);                                                                                     \
    }

namespace
{
#if _DEBUG
    void DisplaySceneHierarchy(FastCG::GameObject *pGameObject, FastCG::GameObject *&rpSelectedGameObject,
                               FastCG::GameObject *&rpRemovedGameObject,
                               std::unordered_set<const FastCG::GameObject *> &rVisitedGameObjects)
    {
        auto it = rVisitedGameObjects.find(pGameObject);
        if (it != rVisitedGameObjects.end())
        {
            return;
        }

        rVisitedGameObjects.emplace(pGameObject);

        ImGui::PushID(pGameObject);
        {
            auto &rChildren = pGameObject->GetTransform()->GetChildren();
            ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_OpenOnDoubleClick;
            if (pGameObject == rpSelectedGameObject)
            {
                flags |= ImGuiTreeNodeFlags_Selected;
            }

            auto DisplayPopup = [&pGameObject, &rpSelectedGameObject, &rpRemovedGameObject]() {
                if (ImGui::BeginPopup("SceneHierarchy_ItemContextMenu"))
                {
                    if (ImGui::MenuItem("Add"))
                    {
                        auto *pNewChild = FastCG::GameObject::Instantiate("GameObject");
                        pNewChild->GetTransform()->SetParent(pGameObject->GetTransform());

                        rpSelectedGameObject = pNewChild;
                    }
                    if (ImGui::MenuItem("Remove"))
                    {
                        rpRemovedGameObject = pGameObject;
                    }
                    if (ImGui::MenuItem("Dump"))
                    {
                        ImGuiFileDialog::Instance()->OpenDialog(
                            "SceneHierarchy_DumpDialogKey", "Choose File", ".json,.scene",
                            IGFD::FileDialogConfig{.path = ".", .userDatas = (void *)pGameObject});
                    }
                    ImGui::EndPopup();
                }
            };

            auto SetupDragAndDrop = [&pGameObject]() {
                if (ImGui::BeginDragDropSource())
                {
                    ImGui::SetDragDropPayload("GameObject", pGameObject, sizeof(FastCG::GameObject));
                    ImGui::Text("Dragging %s", pGameObject->GetName().c_str());
                    ImGui::EndDragDropSource();
                }

                if (ImGui::BeginDragDropTarget())
                {
                    if (const auto *pPayload = ImGui::AcceptDragDropPayload("GameObject"))
                    {
                        auto *pOtherGameObject = (FastCG::GameObject *)pPayload->Data;
                        pOtherGameObject->GetTransform()->SetParent(pGameObject->GetTransform());
                    }
                    ImGui::EndDragDropTarget();
                }
            };
            if (rChildren.empty())
            {
                ImGui::TreeNodeEx(pGameObject->GetName().c_str(),
                                  flags | ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen);

                if (ImGui::IsItemClicked(1))
                {
                    ImGui::OpenPopup("SceneHierarchy_ItemContextMenu");
                }
                if (ImGui::IsItemClicked(0) || ImGui::IsItemClicked(1))
                {
                    rpSelectedGameObject = pGameObject;
                }
                DisplayPopup();
                SetupDragAndDrop();
            }
            else
            {
                auto nodeOpen = ImGui::TreeNodeEx(pGameObject->GetName().c_str(), flags);

                if (ImGui::IsItemClicked(1))
                {
                    ImGui::OpenPopup("SceneHierarchy_ItemContextMenu");
                }
                if (ImGui::IsItemClicked(0) || ImGui::IsItemClicked(1))
                {
                    rpSelectedGameObject = pGameObject;
                }
                DisplayPopup();
                SetupDragAndDrop();

                if (nodeOpen)
                {
                    for (auto *pChild : rChildren)
                    {
                        DisplaySceneHierarchy(pChild->GetGameObject(), rpSelectedGameObject, rpRemovedGameObject,
                                              rVisitedGameObjects);
                    }
                    ImGui::TreePop();
                }
                else
                {
                    for (auto *pChild : rChildren)
                    {
                        rVisitedGameObjects.emplace(pChild->GetGameObject());
                    }
                }
            }
        }
        ImGui::PopID();
    }

    void DisplaySceneHierarchy(const std::vector<FastCG::GameObject *> &rGameObjects,
                               FastCG::GameObject *&rpSelectedGameObject,
                               FastCG::IInspectableProperty *&rpSelectedInspectableProperty)
    {
        if (ImGui::Begin("Scene Hierarchy"))
        {
            if (ImGui::BeginDragDropTarget())
            {
                if (const auto *pPayload = ImGui::AcceptDragDropPayload("GameObject"))
                {
                    auto *pGameObject = (FastCG::GameObject *)pPayload->Data;
                    pGameObject->GetTransform()->SetParent(nullptr);
                }
                ImGui::EndDragDropTarget();
            }

            std::unordered_set<const FastCG::GameObject *> visitedGameObjects;

            FastCG::GameObject *pSelectedGameObject = rpSelectedGameObject, *pRemovedGameObject = nullptr;
            for (auto *pGameObject : rGameObjects)
            {
                if (pGameObject->GetTransform()->GetParent() != nullptr)
                {
                    continue;
                }
                DisplaySceneHierarchy(pGameObject, pSelectedGameObject, pRemovedGameObject, visitedGameObjects);
            }

            if (pSelectedGameObject != rpSelectedGameObject)
            {
                rpSelectedInspectableProperty = nullptr;
                rpSelectedGameObject = pSelectedGameObject;
            }
            if (pRemovedGameObject != nullptr)
            {
                FastCG::GameObject::Destroy(pRemovedGameObject);
            }

            auto bgSize = ImGui::GetContentRegionAvail();
            bgSize.x = std::max<float>(1, bgSize.x);
            bgSize.y = std::max<float>(1, bgSize.y);
            ImGui::InvisibleButton("SceneHierarchy_Background", bgSize);

            if (ImGui::IsItemClicked(1))
            {
                ImGui::OpenPopup("SceneHierarchy_WindowContextMenu");
            }

            if (ImGui::BeginPopup("SceneHierarchy_WindowContextMenu"))
            {
                if (ImGui::MenuItem("Add"))
                {
                    rpSelectedGameObject = FastCG::GameObject::Instantiate("GameObject");
                }
                if (ImGui::MenuItem("Load"))
                {
                    ImGuiFileDialog::Instance()->OpenDialog("SceneHierarchy_LoadDialogKey", "Choose File",
                                                            ".json,.scene,.obj", IGFD::FileDialogConfig{.path = "."});
                }
                ImGui::EndPopup();
            }
        }
        ImGui::End();

        if (ImGuiFileDialog::Instance()->Display("SceneHierarchy_LoadDialogKey"))
        {
            if (ImGuiFileDialog::Instance()->IsOk())
            {
                auto filePath = ImGuiFileDialog::Instance()->GetFilePathName();
                if (FastCG::StringUtils::EndsWith(filePath, ".json") ||
                    FastCG::StringUtils::EndsWith(filePath, ".scene"))
                {
                    rpSelectedGameObject = FastCG::GameObjectLoader::Load(filePath);
                }
                else if (FastCG::StringUtils::EndsWith(filePath, ".obj"))
                {
                    rpSelectedGameObject = FastCG::OBJLoader::Load(
                        filePath,
                        std::make_shared<FastCG::Material>(FastCG::MaterialArgs{
                            "Default", FastCG::MaterialDefinitionRegistry::GetInstance()->GetMaterialDefinition(
                                           "OpaqueSolidColor")}));
                }
            }
            ImGuiFileDialog::Instance()->Close();
        }

        if (ImGuiFileDialog::Instance()->Display("SceneHierarchy_DumpDialogKey"))
        {
            if (ImGuiFileDialog::Instance()->IsOk())
            {
                auto *pGameObject = (FastCG::GameObject *)ImGuiFileDialog::Instance()->GetUserDatas();
                auto filePath = ImGuiFileDialog::Instance()->GetFilePathName();
                FastCG::GameObjectDumper::Dump(
                    filePath, pGameObject,
                    (FastCG::GameObjectDumperOptionMaskType)FastCG::GameObjectDumperOption::ENCODE_DATA);
            }
            ImGuiFileDialog::Instance()->Close();
        }
    }

    template <typename T>
    using ImGuiDragFn = bool (*)(const char *, T *, float, T, T, const char *, ImGuiSliderFlags);

    template <typename T>
    struct ImGuiDragFnSelector;

    template <>
    struct ImGuiDragFnSelector<int>
    {
        static constexpr auto value = &ImGui::DragInt;
    };

    template <>
    struct ImGuiDragFnSelector<float>
    {
        static constexpr auto value = &ImGui::DragFloat;
    };

    template <>
    struct ImGuiDragFnSelector<glm::vec2>
    {
        static constexpr auto value = &ImGui::DragFloat2;
    };

    template <>
    struct ImGuiDragFnSelector<glm::vec3>
    {
        static constexpr auto value = &ImGui::DragFloat3;
    };

    template <>
    struct ImGuiDragFnSelector<glm::vec4>
    {
        static constexpr auto value = &ImGui::DragFloat4;
    };

    template <typename T>
    struct ImGuiDragFnFormatSelector;

#define FASTCG_DECLARE_IMGUI_DRAG_FN_FORMAT_SELECTOR(type, format)                                                     \
    template <>                                                                                                        \
    struct ImGuiDragFnFormatSelector<type>                                                                             \
    {                                                                                                                  \
        static constexpr char const *value = format;                                                                   \
    }

    FASTCG_DECLARE_IMGUI_DRAG_FN_FORMAT_SELECTOR(float, "%.3f");
    FASTCG_DECLARE_IMGUI_DRAG_FN_FORMAT_SELECTOR(double, "%.3lf");
    FASTCG_DECLARE_IMGUI_DRAG_FN_FORMAT_SELECTOR(int32_t, "%d");
    FASTCG_DECLARE_IMGUI_DRAG_FN_FORMAT_SELECTOR(uint32_t, "%u");
    FASTCG_DECLARE_IMGUI_DRAG_FN_FORMAT_SELECTOR(int64_t, "%ll");
    FASTCG_DECLARE_IMGUI_DRAG_FN_FORMAT_SELECTOR(uint64_t, "%llu");
    FASTCG_DECLARE_IMGUI_DRAG_FN_FORMAT_SELECTOR(glm::vec2, "%.3f");
    FASTCG_DECLARE_IMGUI_DRAG_FN_FORMAT_SELECTOR(glm::vec3, "%.3f");
    FASTCG_DECLARE_IMGUI_DRAG_FN_FORMAT_SELECTOR(glm::vec4, "%.3f");

    template <typename T, typename U>
    struct InspectablePropertyDragScalarFnSelector
    {
        static void value(FastCG::IInspectableProperty *pInspectableProperty, float speed = 1,
                          const char *pFormat = ImGuiDragFnFormatSelector<T>::value, ImGuiSliderFlags sliderFlags = 0)
        {
            auto readonly = pInspectableProperty->IsReadOnly();
            if (readonly)
            {
                ImGui::BeginDisabled();
            }
            auto *pInspectableDelimitedProperty =
                static_cast<FastCG::IInspectableDelimitedProperty *>(pInspectableProperty);
            T min, max, value;
            pInspectableDelimitedProperty->GetMin(&min);
            pInspectableDelimitedProperty->GetMax(&max);
            pInspectableDelimitedProperty->GetValue(&value);
            auto proxyValue = (U)value;
            if (ImGuiDragFnSelector<U>::value(pInspectableDelimitedProperty->GetName().c_str(), &proxyValue, speed,
                                              (U)min, (U)max, pFormat, sliderFlags) &&
                !pInspectableProperty->IsReadOnly())
            {
                value = (T)proxyValue;
                pInspectableDelimitedProperty->SetValue((void *)&value);
            }
            if (readonly)
            {
                ImGui::EndDisabled();
            }
        }
    };

    template <typename T>
    struct InspectablePropertyDragScalarFnSelector<T, void>
    {
        static void value(FastCG::IInspectableProperty *pInspectableProperty, float speed = 1,
                          const char *pFormat = ImGuiDragFnFormatSelector<T>::value, ImGuiSliderFlags sliderFlags = 0)
        {
            auto readonly = pInspectableProperty->IsReadOnly();
            if (readonly)
            {
                ImGui::BeginDisabled();
            }
            auto *pInspectableDelimitedProperty =
                static_cast<FastCG::IInspectableDelimitedProperty *>(pInspectableProperty);
            T min, max, value;
            pInspectableDelimitedProperty->GetMin(&min);
            pInspectableDelimitedProperty->GetMax(&max);
            pInspectableDelimitedProperty->GetValue(&value);
            if (ImGuiDragFnSelector<T>::value(pInspectableDelimitedProperty->GetName().c_str(), &value, speed, min, max,
                                              pFormat, sliderFlags) &&
                !pInspectableProperty->IsReadOnly())
            {
                pInspectableDelimitedProperty->SetValue((void *)&value);
            }
            if (readonly)
            {
                ImGui::EndDisabled();
            }
        }
    };

    template <typename T>
    struct InspectablePropertyDragVectorFnSelector
    {
        static void value(FastCG::IInspectableProperty *pInspectableProperty, float speed = 1,
                          const char *pFormat = ImGuiDragFnFormatSelector<T>::value, ImGuiSliderFlags sliderFlags = 0)
        {
            auto readonly = pInspectableProperty->IsReadOnly();
            if (readonly)
            {
                ImGui::BeginDisabled();
            }
            auto *pInspectableDelimitedProperty =
                static_cast<FastCG::IInspectableDelimitedProperty *>(pInspectableProperty);
            T min, max, value;
            pInspectableDelimitedProperty->GetMin(&min);
            pInspectableDelimitedProperty->GetMax(&max);
            pInspectableDelimitedProperty->GetValue(&value);
            auto scalarMin = glm::min(min, min).x;
            auto scalarMax = glm::max(max, max).x;
            if (ImGuiDragFnSelector<T>::value(pInspectableDelimitedProperty->GetName().c_str(), &value[0], speed,
                                              scalarMin, scalarMax, pFormat, sliderFlags) &&
                !pInspectableProperty->IsReadOnly())
            {
                pInspectableDelimitedProperty->SetValue((void *)&value);
            }
            if (readonly)
            {
                ImGui::EndDisabled();
            }
        }
    };

    template <typename T>
    struct ImGuiScalarProxy
    {
        using value = void;
    };

#define FASTCG_DECLARE_IMGUI_SCALAR_PROXY(type, proxyType)                                                             \
    template <>                                                                                                        \
    struct ImGuiScalarProxy<type>                                                                                      \
    {                                                                                                                  \
        using value = proxyType;                                                                                       \
    }

    FASTCG_DECLARE_IMGUI_SCALAR_PROXY(uint32_t, int32_t);
    FASTCG_DECLARE_IMGUI_SCALAR_PROXY(int64_t, int32_t);
    FASTCG_DECLARE_IMGUI_SCALAR_PROXY(uint64_t, int32_t);
    FASTCG_DECLARE_IMGUI_SCALAR_PROXY(double, float);

    template <typename T>
    struct InspectablePropertyDisplayFnSelector
        : InspectablePropertyDragScalarFnSelector<T, typename ImGuiScalarProxy<T>::value>
    {
    };

    template <>
    struct InspectablePropertyDisplayFnSelector<bool>
    {
        static void value(FastCG::IInspectableProperty *pInspectableProperty)
        {
            auto readonly = pInspectableProperty->IsReadOnly();
            if (readonly)
            {
                ImGui::BeginDisabled();
            }
            bool value;
            pInspectableProperty->GetValue(&value);
            if (ImGui::Checkbox(pInspectableProperty->GetName().c_str(), &value))
            {
                pInspectableProperty->SetValue(&value);
            }
            if (readonly)
            {
                ImGui::EndDisabled();
            }
        }
    };

    template <>
    struct InspectablePropertyDisplayFnSelector<std::string>
    {
        static void value(FastCG::IInspectableProperty *pInspectableProperty)
        {
            auto readonly = pInspectableProperty->IsReadOnly();
            if (readonly)
            {
                ImGui::BeginDisabled();
            }
            std::string value;
            pInspectableProperty->GetValue((void *)&value);
            if (ImGui::InputText(pInspectableProperty->GetName().c_str(), &value[0], value.size()) &&
                !pInspectableProperty->IsReadOnly())
            {
                pInspectableProperty->SetValue((void *)&value[0]);
            }
            if (readonly)
            {
                ImGui::EndDisabled();
            }
        }
    };

    template <>
    struct InspectablePropertyDisplayFnSelector<FastCG::Material>
    {
        static void value(FastCG::IInspectableProperty *pInspectableProperty,
                          FastCG::IInspectableProperty *&rpSelectedInspectableProperty)
        {
            assert(pInspectableProperty->IsConst() && pInspectableProperty->IsRef());
            std::shared_ptr<FastCG::Material> pMaterial;
            pInspectableProperty->GetValue((void *)&pMaterial);
            if (pMaterial == nullptr)
            {
                ImGui::Text("Material: <unassigned>");
            }
            else
            {
                ImGui::Text("Material: %s", pMaterial->GetName().c_str());
                ImGui::SameLine();
                if (ImGui::Button("View"))
                {
                    FastCG::WorldSystem::GetInstance()->SetSelectedMaterial(pMaterial);
                }
            }
            ImGui::SameLine();
            if (ImGui::Button("Assign"))
            {
                rpSelectedInspectableProperty = pInspectableProperty;
            }
        }
    };

    template <>
    struct InspectablePropertyDisplayFnSelector<FastCG::Texture>
    {
        static void value(FastCG::IInspectableProperty *pInspectableProperty,
                          FastCG::IInspectableProperty *&rpSelectedInspectableProperty)
        {
            assert(pInspectableProperty->IsConst() && pInspectableProperty->IsRawPtr());
            const FastCG::Texture *pTexture;
            pInspectableProperty->GetValue((void *)&pTexture);
            if (pTexture == nullptr)
            {
                ImGui::Text("Texture: <unassigned>");
            }
            else
            {
                ImGui::Text("Texture: %s", pTexture->GetName().c_str());
                ImGui::SameLine();
                if (ImGui::Button("View"))
                {
                    FastCG::GraphicsSystem::GetInstance()->SetSelectedTexture(pTexture);
                }
            }
            ImGui::SameLine();
            if (ImGui::Button("Assign"))
            {
                rpSelectedInspectableProperty = pInspectableProperty;
            }
        }
    };

    template <>
    struct InspectablePropertyDisplayFnSelector<FastCG::Mesh>
    {
        static void value(FastCG::IInspectableProperty *pInspectableProperty,
                          FastCG::IInspectableProperty *&rpSelectedInspectableProperty)
        {
            assert(pInspectableProperty->IsConst() && pInspectableProperty->IsRef());
            std::shared_ptr<FastCG::Mesh> pMesh;
            pInspectableProperty->GetValue((void *)&pMesh);
            if (pMesh == nullptr)
            {
                ImGui::Text("Mesh: <unassigned>");
            }
            else
            {
                ImGui::Text("Mesh: %s", pMesh->GetName().c_str());
                ImGui::SameLine();
                if (ImGui::Button("View"))
                {
                    // TODO: implement a mesh viewer
                }
            }
            ImGui::SameLine();
            if (ImGui::Button("Assign"))
            {
                rpSelectedInspectableProperty = pInspectableProperty;
            }
        }
    };

    template <>
    struct InspectablePropertyDisplayFnSelector<glm::vec2> : InspectablePropertyDragVectorFnSelector<glm::vec2>
    {
    };

    template <>
    struct InspectablePropertyDisplayFnSelector<glm::vec3> : InspectablePropertyDragVectorFnSelector<glm::vec3>
    {
    };

    template <>
    struct InspectablePropertyDisplayFnSelector<glm::vec4> : InspectablePropertyDragVectorFnSelector<glm::vec4>
    {
    };

    std::unordered_set<std::shared_ptr<FastCG::Material>> GetMaterials(
        const std::vector<FastCG::GameObject *> &rGameObjects)
    {
        std::unordered_set<std::shared_ptr<FastCG::Material>> materials;
        for (auto *pGameObject : rGameObjects)
        {
            const auto *pRenderable = pGameObject->GetComponent<FastCG::Renderable>();
            if (pRenderable == nullptr)
            {
                continue;
            }

            const auto &rpMaterial = pRenderable->GetMaterial();
            if (rpMaterial == nullptr)
            {
                continue;
            }

            auto it = materials.find(rpMaterial);
            if (it != materials.end())
            {
                continue;
            }
            materials.emplace(rpMaterial);
        }
        return materials;
    }

    std::unordered_set<std::shared_ptr<FastCG::Mesh>> GetMeshes(const std::vector<FastCG::GameObject *> &rGameObjects)
    {
        std::unordered_set<std::shared_ptr<FastCG::Mesh>> meshes;
        for (auto *pGameObject : rGameObjects)
        {
            const auto *pRenderable = pGameObject->GetComponent<FastCG::Renderable>();
            if (pRenderable == nullptr)
            {
                continue;
            }

            const auto &rpMesh = pRenderable->GetMesh();
            if (rpMesh == nullptr)
            {
                continue;
            }

            auto it = meshes.find(rpMesh);
            if (it != meshes.end())
            {
                continue;
            }
            meshes.emplace(rpMesh);
        }
        return meshes;
    }

    template <typename IteratorT, typename ItemT, typename CallbackT>
    void DisplayItemTable(const char *pTitle, const IteratorT &rBegin, const IteratorT &rEnd, ItemT &rSelectedItem,
                          const CallbackT &rCallback)
    {
        if (ImGui::BeginTable(pTitle, 1, ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders))
        {
            ImGui::TableSetupColumn(pTitle);
            ImGui::TableHeadersRow();

            for (auto it = rBegin; it != rEnd; ++it)
            {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ItemT currItem = (*it);
                ImGui::Selectable(rCallback(currItem), currItem == rSelectedItem, ImGuiSelectableFlags_SpanAllColumns);
                if (ImGui::IsItemClicked())
                {
                    rSelectedItem = currItem;
                }
            }
            ImGui::EndTable();
        }
    }

    void DisplayInspectablePropertyAssignDialog(const std::vector<FastCG::GameObject *> &rGameObjects,
                                                FastCG::IInspectableProperty *&rpInspectableProperty)
    {
        if (rpInspectableProperty == nullptr)
        {
            return;
        }

        // FIXME: don't use static storage to store temporary values (side-effects!)
        static std::shared_ptr<FastCG::Material> spMaterial;
        static std::shared_ptr<FastCG::Mesh> spMesh;
        const FastCG::Texture *spTexture;

        void *pValue;

        if (ImGui::Begin("Assign"))
        {
            switch (rpInspectableProperty->GetType())
            {
            case FastCG::InspectablePropertyType::MATERIAL: {
                auto materials = GetMaterials(rGameObjects);
                DisplayItemTable(
                    "Materials", materials.begin(), materials.end(), spMaterial,
                    [](const std::shared_ptr<FastCG::Material> &rpMaterial) { return rpMaterial->GetName().c_str(); });
                pValue = &spMaterial;
            }
            break;
            case FastCG::InspectablePropertyType::MESH: {
                auto meshes = GetMeshes(rGameObjects);
                DisplayItemTable("Meshes", meshes.begin(), meshes.end(), spMesh,
                                 [](const std::shared_ptr<FastCG::Mesh> &rpMesh) { return rpMesh->GetName().c_str(); });
                pValue = &spMesh;
            }
            break;
            case FastCG::InspectablePropertyType::TEXTURE: {
                auto textures = FastCG::GraphicsSystem::GetInstance()->GetTextures();
                DisplayItemTable("Textures", textures.begin(), textures.end(), spTexture,
                                 [](const FastCG::Texture *pTexture) { return pTexture->GetName().c_str(); });
                pValue = &spTexture;
            }
            break;
            default:
                FASTCG_THROW_EXCEPTION(FastCG::Exception,
                                       "Don't know how to deferred assign to inspectable property (type: %s)",
                                       FastCG::GetInspectablePropertyTypeString(rpInspectableProperty->GetType()));
                break;
            }
            bool close = false;
            if (ImGui::Button("OK"))
            {
                rpInspectableProperty->SetValue(pValue);
                close = true;
            }
            ImGui::SameLine();
            if (ImGui::Button("Cancel"))
            {
                close = true;
            }
            if (close)
            {
                rpInspectableProperty = nullptr;
                // clean up temporary values statically stored!
                spMaterial = nullptr;
                spMesh = nullptr;
                spTexture = nullptr;
                ImGui::CloseCurrentPopup();
            }
            ImGui::End();
        }
    }

    void DisplayTextureAssignDialog(std::weak_ptr<FastCG::Material> &rpMaterial, std::string &rTextureName)
    {
        auto pMaterial = rpMaterial.lock();

        if (pMaterial == nullptr || rTextureName.empty())
        {
            return;
        }

        // FIXME: don't use static storage to store temporary values (side-effects!)
        static const FastCG::Texture *spTexture;

        if (ImGui::Begin("Assign"))
        {
            auto textures = FastCG::GraphicsSystem::GetInstance()->GetTextures();
            DisplayItemTable("Textures", textures.begin(), textures.end(), spTexture,
                             [](const FastCG::Texture *pTexture) { return pTexture->GetName().c_str(); });
            bool close = false;
            if (ImGui::Button("OK"))
            {
                pMaterial->SetTexture(rTextureName, spTexture);
                close = true;
            }
            ImGui::SameLine();
            if (ImGui::Button("Cancel"))
            {
                close = true;
            }
            if (close)
            {
                rTextureName = {};
                // clean up temporary values statically stored!
                spTexture = nullptr;
                ImGui::CloseCurrentPopup();
            }
            ImGui::End();
        }
    }

    void DisplayInspectable(const std::string &, FastCG::Inspectable *, FastCG::IInspectableProperty *&);

    void FillInInspectable(FastCG::Inspectable *pInspectable,
                           FastCG::IInspectableProperty *&rpSelectedInspectableProperty)
    {
        for (size_t i = 0; i < pInspectable->GetInspectablePropertyCount(); ++i)
        {
            auto *pInspectableProperty = pInspectable->GetInspectableProperty(i);
            ImGui::PushID(pInspectableProperty);
            {
                switch (pInspectableProperty->GetType())
                {
                case FastCG::InspectablePropertyType::INSPECTABLE: {
                    auto readonly = pInspectableProperty->IsReadOnly();
                    if (readonly)
                    {
                        ImGui::BeginDisabled();
                    }
                    FastCG::Inspectable *pOtherInspectable;
                    pInspectableProperty->GetValue(&pOtherInspectable);
                    DisplayInspectable(pInspectableProperty->GetName().c_str(), pOtherInspectable,
                                       rpSelectedInspectableProperty);
                    if (readonly)
                    {
                        ImGui::EndDisabled();
                    }
                }
                break;
                case FastCG::InspectablePropertyType::ENUM: {
                    auto readonly = pInspectableProperty->IsReadOnly();
                    if (readonly)
                    {
                        ImGui::BeginDisabled();
                    }
                    auto pInspectableEnumProperty =
                        static_cast<FastCG::IInspectableEnumProperty *>(pInspectableProperty);
                    auto ppItems = pInspectableEnumProperty->GetItems();
                    int currentItem = (int)pInspectableEnumProperty->GetSelectedItem();
                    if (ImGui::Combo(pInspectableEnumProperty->GetName().c_str(), &currentItem, ppItems,
                                     (int)pInspectableEnumProperty->GetItemCount()))
                    {
                        pInspectableEnumProperty->SetSelectedItem((size_t)currentItem);
                    }
                    if (readonly)
                    {
                        ImGui::EndDisabled();
                    }
                }
                break;
                case FastCG::InspectablePropertyType::BOOL:
                    InspectablePropertyDisplayFnSelector<bool>::value(pInspectableProperty);
                    break;
                case FastCG::InspectablePropertyType::INT32:
                    InspectablePropertyDisplayFnSelector<int32_t>::value(pInspectableProperty);
                    break;
                case FastCG::InspectablePropertyType::UINT32:
                    InspectablePropertyDisplayFnSelector<uint32_t>::value(pInspectableProperty);
                    break;
                case FastCG::InspectablePropertyType::INT64:
                    InspectablePropertyDisplayFnSelector<int64_t>::value(pInspectableProperty);
                    break;
                case FastCG::InspectablePropertyType::UINT64:
                    InspectablePropertyDisplayFnSelector<uint64_t>::value(pInspectableProperty);
                    break;
                case FastCG::InspectablePropertyType::FLOAT:
                    InspectablePropertyDisplayFnSelector<float>::value(pInspectableProperty);
                    break;
                case FastCG::InspectablePropertyType::DOUBLE:
                    InspectablePropertyDisplayFnSelector<double>::value(pInspectableProperty);
                    break;
                case FastCG::InspectablePropertyType::VEC2:
                    InspectablePropertyDisplayFnSelector<glm::vec2>::value(pInspectableProperty);
                    break;
                case FastCG::InspectablePropertyType::VEC3:
                    InspectablePropertyDisplayFnSelector<glm::vec3>::value(pInspectableProperty);
                    break;
                case FastCG::InspectablePropertyType::VEC4:
                    InspectablePropertyDisplayFnSelector<glm::vec4>::value(pInspectableProperty);
                    break;
                case FastCG::InspectablePropertyType::STRING:
                    InspectablePropertyDisplayFnSelector<std::string>::value(pInspectableProperty);
                    break;
                case FastCG::InspectablePropertyType::MATERIAL:
                    InspectablePropertyDisplayFnSelector<FastCG::Material>::value(pInspectableProperty,
                                                                                  rpSelectedInspectableProperty);
                    break;
                case FastCG::InspectablePropertyType::MESH:
                    InspectablePropertyDisplayFnSelector<FastCG::Mesh>::value(pInspectableProperty,
                                                                              rpSelectedInspectableProperty);
                    break;
                case FastCG::InspectablePropertyType::TEXTURE:
                    InspectablePropertyDisplayFnSelector<FastCG::Texture>::value(pInspectableProperty,
                                                                                 rpSelectedInspectableProperty);
                    break;
                default:
                    assert(false);
                }
            }
            ImGui::PopID();
        }
    }

    void DisplayComponent(const std::string &rName, FastCG::Component *pComponent,
                          FastCG::IInspectableProperty *&rpSelectedInspectableProperty)
    {
        ImGui::PushID(pComponent);
        {
            bool nodeOpened = ImGui::TreeNodeEx(rName.c_str(), ImGuiTreeNodeFlags_SpanFullWidth |
                                                                   ImGuiTreeNodeFlags_OpenOnDoubleClick);

            if (ImGui::IsItemClicked(1))
            {
                ImGui::OpenPopup("Inspectable_ItemContextMenu");
            }

            if (ImGui::BeginPopup("Inspectable_ItemContextMenu"))
            {
                if (ImGui::MenuItem("Remove"))
                {
                    FastCG::Component::Destroy(pComponent);
                }
                ImGui::EndPopup();
            }

            if (nodeOpened)
            {
                FillInInspectable(pComponent, rpSelectedInspectableProperty);
                ImGui::TreePop();
            }
        }
        ImGui::PopID();
    }

    void DisplayInspectable(const std::string &rName, FastCG::Inspectable *pInspectable,
                            FastCG::IInspectableProperty *&rpSelectedInspectableProperty)
    {
        ImGui::PushID(pInspectable);
        {
            if (ImGui::TreeNodeEx(rName.c_str(),
                                  ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_OpenOnDoubleClick))
            {
                if (pInspectable != nullptr)
                {
                    FillInInspectable(pInspectable, rpSelectedInspectableProperty);
                }
                ImGui::TreePop();
            }
        }
        ImGui::PopID();
    }

    void DisplayObjectInspector(FastCG::GameObject *pGameObject,
                                FastCG::IInspectableProperty *&rpSelectedInspectableProperty)
    {
        if (ImGui::Begin("Object Inspector"))
        {
            if (pGameObject != nullptr)
            {
                ImGui::PushID(pGameObject);
                {
                    if (ImGui::TreeNodeEx("Transform",
                                          ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_OpenOnDoubleClick))
                    {
                        auto *pTransform = pGameObject->GetTransform();

                        auto scale = pTransform->GetScale();
                        if (ImGui::DragFloat3("Scale", &scale[0], 0.1f))
                        {
                            pTransform->SetScale(scale);
                        }

                        auto rotation = pTransform->GetRotation();
                        auto eulerAngles = glm::degrees(glm::eulerAngles(rotation));
                        if (ImGui::DragFloat3("Rotation", &eulerAngles[0], 0.1f))
                        {
                            eulerAngles = glm::radians(eulerAngles);
                            pTransform->SetRotation(glm::quat(eulerAngles));
                        }

                        auto position = pTransform->GetPosition();
                        if (ImGui::DragFloat3("Position", &position[0], 0.1f))
                        {
                            pTransform->SetPosition(position);
                        }
                        ImGui::TreePop();
                    }

                    for (auto *pComponent : pGameObject->GetComponents())
                    {
                        DisplayComponent(pComponent->GetType().GetName(), pComponent, rpSelectedInspectableProperty);
                    }

                    auto bgSize = ImGui::GetContentRegionAvail();
                    bgSize.x = std::max<float>(1, bgSize.x);
                    bgSize.y = std::max<float>(1, bgSize.y);
                    ImGui::InvisibleButton("ObjectInspector_Background", bgSize);

                    if (ImGui::IsItemClicked(1))
                    {
                        ImGui::OpenPopup("ObjectInspector_WindowContextMenu");
                    }

                    if (ImGui::BeginPopup("ObjectInspector_WindowContextMenu"))
                    {
                        if (ImGui::BeginMenu("Add"))
                        {
                            for (const auto &rComponentRegister : FastCG::ComponentRegistry::GetComponentRegisters())
                            {
                                if (pGameObject->HasComponent(rComponentRegister.GetType()))
                                {
                                    continue;
                                }
                                if (ImGui::MenuItem(rComponentRegister.GetType().GetName().c_str()))
                                {
                                    rComponentRegister.Instantiate(pGameObject);
                                }
                            }
                            ImGui::EndMenu();
                        }
                        ImGui::EndPopup();
                    }
                }
                ImGui::PopID();
            }
        }
        ImGui::End();
    }

    void DisplayMaterialBrowser(const std::vector<FastCG::GameObject *> &rGameObjects,
                                std::weak_ptr<FastCG::Material> &rpSelectedMaterial, std::string &rSelectedTextureName)
    {
        auto pSelectedMaterial = rpSelectedMaterial.lock();

        if (ImGui::Begin("Material Browser"))
        {
            auto textureWidth = (uint32_t)(ImGui::GetWindowSize().x * 0.333f);

            for (const auto &rpMaterial : GetMaterials(rGameObjects))
            {
                ImGui::PushID(rpMaterial.get());
                {
                    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_OpenOnDoubleClick;
                    if (rpMaterial == pSelectedMaterial)
                    {
                        flags |= ImGuiTreeNodeFlags_Selected;
                    }

                    auto nodeOpened = ImGui::TreeNodeEx(rpMaterial->GetName().c_str(), flags);

                    if (ImGui::IsItemClicked(0))
                    {
                        rpSelectedMaterial = rpMaterial;
                    }

                    if (nodeOpened)
                    {
                        ImGui::Text("Shader: %s", rpMaterial->GetShader()->GetName().c_str());
                        for (size_t i = 0; i < rpMaterial->GetConstantCount(); ++i)
                        {
                            const auto &rConstant = rpMaterial->GetConstantAt(i);
                            if (rConstant.IsVec4())
                            {
                                glm::vec4 value;
                                rpMaterial->GetConstant(rConstant.GetName(), value);
                                ImGui::Text("%s: (%.3f, %.3f, %.3f, %.3f)", rConstant.GetName().c_str(), value.x,
                                            value.y, value.z, value.w);
                            }
                            else if (rConstant.IsVec2())
                            {
                                glm::vec2 value;
                                rpMaterial->GetConstant(rConstant.GetName(), value);
                                ImGui::Text("%s: (%.3f, %.3f)", rConstant.GetName().c_str(), value.x, value.y);
                            }
                            else if (rConstant.IsFloat())
                            {
                                float value;
                                rpMaterial->GetConstant(rConstant.GetName(), value);
                                ImGui::Text("%s: %.3f", rConstant.GetName().c_str(), value);
                            }
                            else
                            {
                                assert(false);
                            }
                        }
                        for (size_t i = 0; i < rpMaterial->GetTextureCount(); ++i)
                        {
                            std::string name;
                            const FastCG::Texture *pTexture;
                            rpMaterial->GetTextureAt(i, name, pTexture);

                            ImGui::PushID(name.c_str());
                            {
                                if (pTexture == nullptr)
                                {
                                    ImGui::Text("%s: <unassigned>", name.c_str());
                                }
                                else
                                {
                                    ImGui::Text("%s: %s", name.c_str(), pTexture->GetName().c_str());
                                }
                                ImGui::SameLine();
                                if (ImGui::Button("View"))
                                {
                                    FastCG::GraphicsSystem::GetInstance()->SetSelectedTexture(pTexture);
                                }
                                ImGui::SameLine();
                                if (ImGui::Button("Assign"))
                                {
                                    rpSelectedMaterial = rpMaterial;
                                    rSelectedTextureName = name;
                                }
                                if (pTexture != nullptr)
                                {
                                    DisplayTexture(pTexture, textureWidth);
                                }
                            }
                            ImGui::PopID();
                        }
                        ImGui::TreePop();
                    }
                }
                ImGui::PopID();
            }
        }
        ImGui::End();
    }

    void DisplayGizmosOptions(ImGuizmo::OPERATION &rOperation, ImGuizmo::MODE &rMode, glm::vec3 &rTranslateSnap,
                              float &rRotateSnap, float &rScaleSnap)
    {
        if (ImGui::Begin("Gizmos Options"))
        {
            if (ImGui::RadioButton("Translate", rOperation == ImGuizmo::TRANSLATE))
            {
                rOperation = ImGuizmo::TRANSLATE;
            }
            ImGui::SameLine();
            if (ImGui::RadioButton("Rotate", rOperation == ImGuizmo::ROTATE))
            {
                rOperation = ImGuizmo::ROTATE;
            }
            ImGui::SameLine();
            if (ImGui::RadioButton("Scale", rOperation == ImGuizmo::SCALE))
            {
                rOperation = ImGuizmo::SCALE;
            }
            if (rOperation != ImGuizmo::SCALE)
            {
                if (ImGui::RadioButton("World", rMode == ImGuizmo::WORLD))
                {
                    rMode = ImGuizmo::WORLD;
                }
                ImGui::SameLine();
                if (ImGui::RadioButton("Local", rMode == ImGuizmo::LOCAL))
                {
                    rMode = ImGuizmo::LOCAL;
                }
            }

            switch (rOperation)
            {
            case ImGuizmo::TRANSLATE_X:
            case ImGuizmo::TRANSLATE_Y:
            case ImGuizmo::TRANSLATE_Z:
                ImGui::InputFloat3("Snap", &rTranslateSnap.x);
                break;
            case ImGuizmo::ROTATE_X:
            case ImGuizmo::ROTATE_Y:
            case ImGuizmo::ROTATE_Z:
                ImGui::InputFloat("Snap", &rRotateSnap);
                break;
            case ImGuizmo::SCALE_X:
            case ImGuizmo::SCALE_Y:
            case ImGuizmo::SCALE_Z:
                ImGui::InputFloat("Snap", &rScaleSnap);
                break;
            default:
                break;
            }
            ImGui::End();
        }
    }

    void DisplayGizmos(FastCG::GameObject *pGameObject, ImGuizmo::OPERATION operation, ImGuizmo::MODE mode,
                       const glm::vec3 &rTranslateSnap, float rotateSnap, float scaleSnap)
    {
        if (pGameObject != nullptr)
        {
            auto *pTransform = pGameObject->GetTransform();
            auto model = pTransform->GetModel();
            auto *pCamera = FastCG::WorldSystem::GetInstance()->GetMainCamera();
            if (pCamera != nullptr)
            {
                auto view = pCamera->GetView();
                auto projection = pCamera->GetProjection();
                const float *pSnap = nullptr;
                if ((operation & ImGuizmo::TRANSLATE) != 0)
                {
                    pSnap = &rTranslateSnap.x;
                }
                else if ((operation & ImGuizmo::ROTATE) != 0)
                {
                    pSnap = &rotateSnap;
                }
                else if ((operation & ImGuizmo::SCALE) != 0)
                {
                    pSnap = &scaleSnap;
                }
                if (pSnap != nullptr)
                {
                    if (ImGuizmo::Manipulate(glm::value_ptr(view), glm::value_ptr(projection), operation, mode,
                                             glm::value_ptr(model), nullptr, pSnap))
                    {
                        pTransform->SetModel(model);
                    }
                }
            }
        }
    }
#endif
}

namespace FastCG
{
    WorldSystem::WorldSystem(const WorldSystemArgs &rArgs) : mArgs(rArgs)
    {
    }

    WorldSystem::~WorldSystem() = default;

    void WorldSystem::Initialize()
    {
        FASTCG_LOG_DEBUG(RenderingSystem, "\tInitializing world system");

#if _DEBUG
        DebugMenuSystem::GetInstance()->AddCallback(
            "World System", std::bind(&WorldSystem::DebugMenuCallback, this, std::placeholders::_1));
        DebugMenuSystem::GetInstance()->AddItem(
            "World System", std::bind(&WorldSystem::DebugMenuItemCallback, this, std::placeholders::_1));
#endif
    }

#if _DEBUG
    void WorldSystem::DebugMenuCallback(int result)
    {
        if (mShowSceneHierarchy)
        {
            DisplaySceneHierarchy(mGameObjects, mpSelectedGameObject, mpSelectedInspectableProperty);
        }
        if (mShowObjectInspector)
        {
            DisplayObjectInspector(mpSelectedGameObject, mpSelectedInspectableProperty);
        }
        if (mShowMaterialBrowser)
        {
            DisplayMaterialBrowser(mGameObjects, mpSelectedMaterial, mSelectedTextureName);
        }
        DisplayInspectablePropertyAssignDialog(mGameObjects, mpSelectedInspectableProperty);
        DisplayTextureAssignDialog(mpSelectedMaterial, mSelectedTextureName);
        {
            if (mShowGizmosOptions)
            {
                DisplayGizmosOptions(mOperation, mMode, mTranslateSnap, mRotateSnap, mScaleSnap);
            }
            DisplayGizmos(mpSelectedGameObject, mOperation, mMode, mTranslateSnap, mRotateSnap, mScaleSnap);
        }
    }

    void WorldSystem::DebugMenuItemCallback(int &result)
    {
        ImGui::Checkbox("Scene Hierarchy", &mShowSceneHierarchy);
        ImGui::Checkbox("Object Inspector", &mShowObjectInspector);
        ImGui::Checkbox("Material Browser", &mShowMaterialBrowser);
        ImGui::Checkbox("Gizmos Options", &mShowGizmosOptions);
    }
#endif

    void WorldSystem::RegisterGameObject(GameObject *pGameObject)
    {
        assert(pGameObject != nullptr);
        mGameObjects.emplace_back(pGameObject);
    }

    void WorldSystem::UnregisterGameObject(GameObject *pGameObject)
    {
        assert(pGameObject != nullptr);
        auto it = std::find(mGameObjects.begin(), mGameObjects.end(), pGameObject);
        assert(it != mGameObjects.end());
        mGameObjects.erase(it);
#if _DEBUG
        if (mpSelectedGameObject == pGameObject)
        {
            mpSelectedGameObject = nullptr;
            mpSelectedInspectableProperty = nullptr;
        }
#endif
    }

    void WorldSystem::RegisterComponent(Component *pComponent)
    {
        assert(pComponent != nullptr);

        FASTCG_TRACK_COMPONENT_COLLECTION(DirectionalLight, pComponent);
        FASTCG_TRACK_COMPONENT_COLLECTION(PointLight, pComponent);
        FASTCG_TRACK_COMPONENT(Fog, pComponent);
        FASTCG_TRACK_COMPONENT_COLLECTION(Behaviour, pComponent);

        mComponents.emplace_back(pComponent);
    }

    void WorldSystem::RegisterCamera(Camera *pCamera)
    {
        mCameras.emplace_back(pCamera);
        SetMainCamera(pCamera);
    }

    void WorldSystem::UnregisterCamera(Camera *pCamera)
    {
        auto it = std::find(mCameras.cbegin(), mCameras.cend(), pCamera);
        assert(it != mCameras.cend());

        mCameras.erase(it);

        if (mpMainCamera == pCamera)
        {
            if (mCameras.empty())
            {
                mpMainCamera = nullptr;
            }
            else
            {
                mpMainCamera = mCameras[0];
            }
        }
    }

    void WorldSystem::UnregisterComponent(Component *pComponent)
    {
        assert(pComponent != nullptr);

        FASTCG_UNTRACK_COMPONENT_COLLECTION(DirectionalLight, pComponent);
        FASTCG_UNTRACK_COMPONENT_COLLECTION(PointLight, pComponent);
        FASTCG_UNTRACK_COMPONENT(Fog, pComponent);
        FASTCG_UNTRACK_COMPONENT_COLLECTION(Behaviour, pComponent);
        FASTCG_UNTRACK_COMPONENT_COLLECTION(Component, pComponent);
    }

    void WorldSystem::SetMainCamera(Camera *pCamera)
    {
        if (pCamera != nullptr)
        {
            pCamera->SetEnabled(true);
        }

        for (auto *pOtherCamera : mCameras)
        {
            if (pOtherCamera == pCamera)
            {
                continue;
            }

            pOtherCamera->SetEnabled(false);
        }

        mpMainCamera = pCamera;
        mpMainCamera->SetAspectRatio(GetAspectRatio());
    }

    void WorldSystem::Resize()
    {
        if (mpMainCamera != nullptr)
        {
            mpMainCamera->SetAspectRatio(GetAspectRatio());
        }
    }

    void WorldSystem::Update(float time, float deltaTime)
    {
        for (auto *pGameObject : mGameObjects)
        {
            if (pGameObject->GetTransform()->GetParent() != nullptr)
            {
                continue;
            }
            pGameObject->GetTransform()->Update();
        }

        for (auto *pBehaviour : mBehaviours)
        {
            pBehaviour->Update((float)time, (float)deltaTime);
        }
    }

    void WorldSystem::Finalize()
    {
        while (!mGameObjects.empty())
        {
            GameObject::Destroy(mGameObjects.back());
        }

        mpMainCamera = nullptr;

        assert(mCameras.empty());
        assert(mDirectionalLights.empty());
        assert(mPointLights.empty());
        assert(mBehaviours.empty());
        assert(mComponents.empty());
    }

}