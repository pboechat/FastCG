#include <FastCG/WorldSystem.h>
#include <FastCG/Transform.h>
#include <FastCG/Renderable.h>
#include <FastCG/PointLight.h>
#include <FastCG/MathT.h>
#include <FastCG/Inspectable.h>
#include <FastCG/Graphics/GraphicsSystem.h>
#include <FastCG/GameObject.h>
#include <FastCG/DirectionalLight.h>
#include <FastCG/DebugMenuSystem.h>
#include <FastCG/Component.h>
#include <FastCG/Camera.h>
#include <FastCG/Behaviour.h>
#include <FastCG/Application.h>

#include <imgui.h>

#include <unordered_set>
#include <cassert>
#include <algorithm>

#define FASTCG_TRACK_COMPONENT(className, component)                       \
    if (component->GetType().IsDerived(className::TYPE))                   \
    {                                                                      \
        m##className##s.emplace_back(static_cast<className *>(component)); \
    }

#define FASTCG_UNTRACK_COMPONENT(className, component)                                  \
    if (component->GetType().IsDerived(className::TYPE))                                \
    {                                                                                   \
        auto it = std::find(m##className##s.begin(), m##className##s.end(), component); \
        if (it == m##className##s.end())                                                \
        {                                                                               \
            FASTCG_THROW_EXCEPTION(Exception, "Error unregistering: %s", #className);   \
        }                                                                               \
        m##className##s.erase(it);                                                      \
    }

namespace
{
#ifdef _DEBUG
    void DisplaySceneHierarchy(FastCG::GameObject *pGameObject, FastCG::GameObject *&rpSelectedGameObject, std::unordered_set<FastCG::GameObject *> &rVisitedGameObjects)
    {
        auto it = rVisitedGameObjects.find(pGameObject);
        if (it != rVisitedGameObjects.end())
        {
            return;
        }

        rVisitedGameObjects.emplace(pGameObject);

        ImGui::PushID(pGameObject);
        auto &rChildren = pGameObject->GetTransform()->GetChildren();
        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_SpanFullWidth;
        if (pGameObject == rpSelectedGameObject)
        {
            flags |= ImGuiTreeNodeFlags_Selected | ImGuiTreeNodeFlags_DefaultOpen;
        }
        if (rChildren.empty())
        {
            ImGui::TreeNodeEx(pGameObject->GetName().c_str(), flags | ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen);
            if (ImGui::IsItemClicked())
            {
                rpSelectedGameObject = pGameObject;
            }
        }
        else
        {
            if (ImGui::TreeNodeEx(pGameObject->GetName().c_str(), flags))
            {
                if (ImGui::IsItemClicked())
                {
                    rpSelectedGameObject = pGameObject;
                }

                for (auto *pChild : rChildren)
                {
                    DisplaySceneHierarchy(pChild->GetGameObject(), rpSelectedGameObject, rVisitedGameObjects);
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
        ImGui::PopID();
    }

    void DisplaySceneHierarchy(const std::vector<FastCG::GameObject *> &rGameObjects, FastCG::GameObject *&rpSelectedGameObject)
    {
        if (ImGui::Begin("Scene Hierarchy"))
        {
            std::unordered_set<FastCG::GameObject *> visitedGameObjects;
            for (auto *pGameObject : rGameObjects)
            {
                if (pGameObject->GetTransform()->GetParent() != nullptr)
                {
                    continue;
                }
                DisplaySceneHierarchy(pGameObject, rpSelectedGameObject, visitedGameObjects);
            }
        }
        ImGui::End();
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
    struct ImGuiDragFnSelector<double>
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

#define FASTCG_DECLARE_IMGUI_DRAG_FN_FORMAT_SELECTOR(type, format) \
    template <>                                                    \
    struct ImGuiDragFnFormatSelector<type>                         \
    {                                                              \
        static constexpr char const *value = format;               \
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
        static void value(FastCG::IInspectableProperty *pInspectableProperty, float speed = 1, const char *pFormat = ImGuiDragFnFormatSelector<T>::value, ImGuiSliderFlags sliderFlags = 0)
        {
            auto readonly = pInspectableProperty->IsReadOnly();
            if (readonly)
            {
                ImGui::BeginDisabled();
            }
            auto *pInspectableDelimitedProperty = static_cast<FastCG::IInspectableDelimitedProperty *>(pInspectableProperty);
            T min, max, value;
            pInspectableDelimitedProperty->GetMin(&min);
            pInspectableDelimitedProperty->GetMax(&max);
            pInspectableDelimitedProperty->GetValue(&value);
            auto proxyValue = (U)value;
            if (ImGuiDragFnSelector<U>::value(pInspectableDelimitedProperty->GetName().c_str(), &proxyValue, speed, (U)min, (U)max, pFormat, sliderFlags) && !pInspectableProperty->IsReadOnly())
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
        static void value(FastCG::IInspectableProperty *pInspectableProperty, float speed = 1, const char *pFormat = ImGuiDragFnFormatSelector<T>::value, ImGuiSliderFlags sliderFlags = 0)
        {
            auto readonly = pInspectableProperty->IsReadOnly();
            if (readonly)
            {
                ImGui::BeginDisabled();
            }
            auto *pInspectableDelimitedProperty = static_cast<FastCG::IInspectableDelimitedProperty *>(pInspectableProperty);
            T min, max, value;
            pInspectableDelimitedProperty->GetMin(&min);
            pInspectableDelimitedProperty->GetMax(&max);
            pInspectableDelimitedProperty->GetValue(&value);
            if (ImGuiDragFnSelector<T>::value(pInspectableDelimitedProperty->GetName().c_str(), &value, speed, min, max, pFormat, sliderFlags) && !pInspectableProperty->IsReadOnly())
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
        static void value(FastCG::IInspectableProperty *pInspectableProperty, float speed = 1, const char *pFormat = ImGuiDragFnFormatSelector<T>::value, ImGuiSliderFlags sliderFlags = 0)
        {
            auto readonly = pInspectableProperty->IsReadOnly();
            if (readonly)
            {
                ImGui::BeginDisabled();
            }
            auto *pInspectableDelimitedProperty = static_cast<FastCG::IInspectableDelimitedProperty *>(pInspectableProperty);
            T min, max, value;
            pInspectableDelimitedProperty->GetMin(&min);
            pInspectableDelimitedProperty->GetMax(&max);
            pInspectableDelimitedProperty->GetValue(&value);
            auto scalarMin = glm::min(min, min).x;
            auto scalarMax = glm::max(max, max).x;
            if (ImGuiDragFnSelector<T>::value(pInspectableDelimitedProperty->GetName().c_str(), &value[0], speed, scalarMin, scalarMax, pFormat, sliderFlags) && !pInspectableProperty->IsReadOnly())
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

#define FASTCG_DECLARE_IMGUI_SCALAR_PROXY(type, proxyType) \
    template <>                                            \
    struct ImGuiScalarProxy<type>                          \
    {                                                      \
        using value = proxyType;                           \
    }

    FASTCG_DECLARE_IMGUI_SCALAR_PROXY(uint32_t, int32_t);
    FASTCG_DECLARE_IMGUI_SCALAR_PROXY(int64_t, int32_t);
    FASTCG_DECLARE_IMGUI_SCALAR_PROXY(uint64_t, int32_t);
    FASTCG_DECLARE_IMGUI_SCALAR_PROXY(double, float);

    template <typename T>
    struct InspectablePropertyDisplayFnSelector : InspectablePropertyDragScalarFnSelector<T, typename ImGuiScalarProxy<T>::value>
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
            if (ImGui::InputText(pInspectableProperty->GetName().c_str(), &value[0], value.size()) && !pInspectableProperty->IsReadOnly())
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
        static void value(FastCG::IInspectableProperty *pInspectableProperty)
        {
            assert(pInspectableProperty->IsConst() && pInspectableProperty->IsPtr());
            const FastCG::Material *pMaterial;
            pInspectableProperty->GetValue((void *)&pMaterial);
            ImGui::Text("Material: %s", pMaterial->GetName().c_str());
            ImGui::SameLine();
            if (ImGui::Button("View"))
            {
                FastCG::GraphicsSystem::GetInstance()->SetSelectedMaterial(pMaterial);
            }
        }
    };

    template <>
    struct InspectablePropertyDisplayFnSelector<FastCG::Texture>
    {
        static void value(FastCG::IInspectableProperty *pInspectableProperty)
        {
            assert(pInspectableProperty->IsConst() && pInspectableProperty->IsPtr());
            const FastCG::Texture *pTexture;
            pInspectableProperty->GetValue((void *)&pTexture);
            ImGui::Text("Texture: %s", pTexture->GetName().c_str());
            ImGui::SameLine();
            if (ImGui::Button("View"))
            {
                FastCG::GraphicsSystem::GetInstance()->SetSelectedTexture(pTexture);
            }
        }
    };

    template <>
    struct InspectablePropertyDisplayFnSelector<FastCG::Mesh>
    {
        static void value(FastCG::IInspectableProperty *pInspectableProperty)
        {
            assert(pInspectableProperty->IsConst() && pInspectableProperty->IsPtr());
            const FastCG::Mesh *pMesh;
            pInspectableProperty->GetValue((void *)&pMesh);
            ImGui::Text("Mesh: %s", pMesh->GetName().c_str());
            ImGui::SameLine();
            if (ImGui::Button("View"))
            {
                // TODO:
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

    void DisplayInspectable(const std::string &rName, FastCG::Inspectable *pInspectable)
    {
        if (ImGui::TreeNode(rName.c_str()))
        {
            if (pInspectable != nullptr)
            {
                for (size_t i = 0; i < pInspectable->GetInspectablePropertyCount(); ++i)
                {
                    auto *pInspectableProperty = pInspectable->GetInspectableProperty(i);
                    switch (pInspectableProperty->GetType())
                    {
                    case FastCG::InspectablePropertyType::INSPECTABLE:
                    {
                        auto readonly = pInspectableProperty->IsReadOnly();
                        if (readonly)
                        {
                            ImGui::BeginDisabled();
                        }
                        FastCG::Inspectable *pOtherInspectable;
                        pInspectableProperty->GetValue(&pOtherInspectable);
                        DisplayInspectable(pInspectableProperty->GetName().c_str(), pOtherInspectable);
                        if (readonly)
                        {
                            ImGui::EndDisabled();
                        }
                    }
                    break;
                    case FastCG::InspectablePropertyType::ENUM:
                    {
                        auto readonly = pInspectableProperty->IsReadOnly();
                        if (readonly)
                        {
                            ImGui::BeginDisabled();
                        }
                        auto pInspectableEnumProperty = static_cast<FastCG::IInspectableEnumProperty *>(pInspectableProperty);
                        auto ppItems = pInspectableEnumProperty->GetItems();
                        int currentItem = (int)pInspectableEnumProperty->GetSelectedItem();
                        if (ImGui::Combo(pInspectableEnumProperty->GetName().c_str(), &currentItem, ppItems, (int)pInspectableEnumProperty->GetItemCount()))
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
                        InspectablePropertyDisplayFnSelector<FastCG::Material>::value(pInspectableProperty);
                        break;
                    case FastCG::InspectablePropertyType::MESH:
                        InspectablePropertyDisplayFnSelector<FastCG::Mesh>::value(pInspectableProperty);
                        break;
                    case FastCG::InspectablePropertyType::TEXTURE:
                        InspectablePropertyDisplayFnSelector<FastCG::Texture>::value(pInspectableProperty);
                        break;
                    default:
                        assert(false);
                    }
                }
            }
            ImGui::TreePop();
        }
    }

    void DisplayObjectInspector(FastCG::GameObject *pGameObject)
    {
        if (ImGui::Begin("Object Inspector"))
        {
            if (pGameObject != nullptr)
            {
                if (ImGui::TreeNode("Transform"))
                {
                    auto *pTransform = pGameObject->GetTransform();

                    auto scale = pTransform->GetScale();
                    if (ImGui::DragFloat3("Scale", &scale[0], 0.1f))
                    {
                        pTransform->SetScale(scale);
                    }

                    auto rotation = pTransform->GetRotation();
                    auto eulerAngles = glm::eulerAngles(rotation);
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
                    DisplayInspectable(pComponent->GetType().GetName(), pComponent);
                }
            }
        }
        ImGui::End();
    }
}

namespace FastCG
{
    WorldSystem::WorldSystem(const WorldSystemArgs &rArgs) : mArgs(rArgs)
    {
    }

    WorldSystem::~WorldSystem() = default;

    void WorldSystem::Initialize()
    {
#ifdef _DEBUG
        DebugMenuSystem::GetInstance()->AddCallback("World System", std::bind(&WorldSystem::DebugMenuCallback, this, std::placeholders::_1));
        DebugMenuSystem::GetInstance()->AddItem("World System", std::bind(&WorldSystem::DebugMenuItemCallback, this, std::placeholders::_1));
#endif
    }

    void WorldSystem::DebugMenuCallback(int result)
    {
        if (mShowSceneHierarchy)
        {
            DisplaySceneHierarchy(mGameObjects, mpSelectedGameObject);
        }
        if (mShowObjectInspector)
        {
            DisplayObjectInspector(mpSelectedGameObject);
        }
    }

    void WorldSystem::DebugMenuItemCallback(int &result)
    {
        ImGui::Checkbox("Scene Hierarchy", &mShowSceneHierarchy);
        ImGui::Checkbox("Object Inspector", &mShowObjectInspector);
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
#ifdef _DEBUG
        if (mpSelectedGameObject == pGameObject)
        {
            mpSelectedGameObject = nullptr;
        }
#endif
    }

    void WorldSystem::RegisterComponent(Component *pComponent)
    {
        assert(pComponent != nullptr);

        FASTCG_TRACK_COMPONENT(DirectionalLight, pComponent);
        FASTCG_TRACK_COMPONENT(PointLight, pComponent);
        FASTCG_TRACK_COMPONENT(Behaviour, pComponent);

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

        mCameras.erase(it);
    }

    void WorldSystem::UnregisterComponent(Component *pComponent)
    {
        assert(pComponent != nullptr);

        FASTCG_UNTRACK_COMPONENT(DirectionalLight, pComponent);
        FASTCG_UNTRACK_COMPONENT(PointLight, pComponent);
        FASTCG_UNTRACK_COMPONENT(Behaviour, pComponent);
        FASTCG_UNTRACK_COMPONENT(Component, pComponent);
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

    void WorldSystem::Update(float cpuStart, float frameDeltaTime)
    {
        if (mpMainCamera != nullptr)
        {
            mpMainCamera->SetAspectRatio(GetAspectRatio());
        }

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
            pBehaviour->Update((float)cpuStart, (float)frameDeltaTime);
        }
    }

    void WorldSystem::Finalize()
    {
        auto gameObjectsToDestroy = mGameObjects;
        for (auto *pGameObject : gameObjectsToDestroy)
        {
            GameObject::Destroy(pGameObject);
        }

        mpMainCamera = nullptr;

        assert(mGameObjects.empty());
        assert(mCameras.empty());
        assert(mDirectionalLights.empty());
        assert(mPointLights.empty());
        assert(mBehaviours.empty());
        assert(mComponents.empty());
    }
}