#include <FastCG/WorldSystem.h>
#include <FastCG/RenderBatchStrategy.h>
#include <FastCG/Renderable.h>
#include <FastCG/PointLight.h>
#include <FastCG/MathT.h>
#include <FastCG/Inspectable.h>
#include <FastCG/GameObject.h>
#include <FastCG/DirectionalLight.h>
#include <FastCG/DebugMenuSystem.h>
#include <FastCG/Component.h>
#include <FastCG/Camera.h>
#include <FastCG/Behaviour.h>

#include <imgui.h>

#include <unordered_set>
#include <cassert>
#include <algorithm>

#define FASTCG_REGISTER_COMPONENT(className, component)                    \
    if (component->GetType().IsDerived(className::TYPE))                   \
    {                                                                      \
        m##className##s.emplace_back(static_cast<className *>(component)); \
    }

#define FASTCG_UNREGISTER_COMPONENT(className, component)                               \
    if (component->GetType().IsDerived(className::TYPE))                                \
    {                                                                                   \
        auto it = std::find(m##className##s.begin(), m##className##s.end(), component); \
        if (it == m##className##s.end())                                                \
        {                                                                               \
            FASTCG_THROW_EXCEPTION(Exception, "Error unregistering: %s", #className);   \
        }                                                                               \
        m##className##s.erase(it);                                                      \
    }

namespace FastCG
{
    WorldSystem::~WorldSystem()
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
        assert(mRenderables.empty());
        assert(mBehaviours.empty());
        assert(mComponents.empty());
    }

    void WorldSystem::Initialize()
    {
#ifdef _DEBUG
        DebugMenuSystem::GetInstance()->AddCallback("World System", std::bind(&WorldSystem::DebugMenuCallback, this, std::placeholders::_1));
        DebugMenuSystem::GetInstance()->AddItem("World System", std::bind(&WorldSystem::DebugMenuItemCallback, this, std::placeholders::_1));
#endif
    }

#ifdef _DEBUG
    void DisplaySceneHierarchy(GameObject *pGameObject, GameObject *&rpSelectedGameObject, std::unordered_set<GameObject *> &rVisitedGameObjects)
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
            flags |= ImGuiTreeNodeFlags_Selected;
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

    void DisplaySceneHierarchy(const std::vector<GameObject *> &rGameObjects, GameObject *&rpSelectedGameObject)
    {
        std::unordered_set<GameObject *> visitedGameObjects;
        for (auto *pGameObject : rGameObjects)
        {
            DisplaySceneHierarchy(pGameObject, rpSelectedGameObject, visitedGameObjects);
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
        static constexpr char *const value = format;               \
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
        static void value(IInspectableProperty *pInspectableProperty, float speed = 1, const char *pFormat = ImGuiDragFnFormatSelector<T>::value, ImGuiSliderFlags sliderFlags = 0)
        {
            auto *pInspectableDelimitedProperty = static_cast<IInspectableDelimitedProperty *>(pInspectableProperty);
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
        }
    };

    template <typename T>
    struct InspectablePropertyDragScalarFnSelector<T, void>
    {
        static void value(IInspectableProperty *pInspectableProperty, float speed = 1, const char *pFormat = ImGuiDragFnFormatSelector<T>::value, ImGuiSliderFlags sliderFlags = 0)
        {
            auto *pInspectableDelimitedProperty = static_cast<IInspectableDelimitedProperty *>(pInspectableProperty);
            T min, max, value;
            pInspectableDelimitedProperty->GetMin(&min);
            pInspectableDelimitedProperty->GetMax(&max);
            pInspectableDelimitedProperty->GetValue(&value);
            if (ImGuiDragFnSelector<T>::value(pInspectableDelimitedProperty->GetName().c_str(), &value, speed, min, max, pFormat, sliderFlags) && !pInspectableProperty->IsReadOnly())
            {
                pInspectableDelimitedProperty->SetValue((void *)&value);
            }
        }
    };

    template <typename T>
    struct InspectablePropertyDragVectorFnSelector
    {
        static void value(IInspectableProperty *pInspectableProperty, float speed = 1, const char *pFormat = ImGuiDragFnFormatSelector<T>::value, ImGuiSliderFlags sliderFlags = 0)
        {
            auto *pInspectableDelimitedProperty = static_cast<IInspectableDelimitedProperty *>(pInspectableProperty);
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
        static void value(IInspectableProperty *pInspectableProperty)
        {
            bool value;
            pInspectableProperty->GetValue(&value);
            if (ImGui::Checkbox(pInspectableProperty->GetName().c_str(), &value))
            {
                pInspectableProperty->SetValue(&value);
            }
        }
    };

    template <>
    struct InspectablePropertyDisplayFnSelector<std::string>
    {
        static void value(IInspectableProperty *pInspectableProperty)
        {
            std::string value;
            pInspectableProperty->GetValue((void *)&value);
            if (ImGui::InputText(pInspectableProperty->GetName().c_str(), &value[0], value.size()) && !pInspectableProperty->IsReadOnly())
            {
                pInspectableProperty->SetValue((void *)&value[0]);
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

    void DisplayInspectable(const std::string &rName, Inspectable *pInspectable)
    {
        if (ImGui::TreeNode(rName.c_str()))
        {
            if (pInspectable != nullptr)
            {
                for (size_t i = 0; i < pInspectable->GetInspectablePropertyCount(); ++i)
                {
                    auto *pInspectableProperty = pInspectable->GetInspectableProperty(i);
                    auto readonly = pInspectableProperty->IsReadOnly();
                    if (readonly)
                    {
                        ImGui::BeginDisabled();
                    }
                    switch (pInspectableProperty->GetType())
                    {
                    case InspectablePropertyType::INSPECTABLE:
                    {
                        Inspectable *pOtherInspectable;
                        pInspectableProperty->GetValue(&pOtherInspectable);
                        DisplayInspectable(pInspectableProperty->GetName().c_str(), pOtherInspectable);
                    }
                    break;
                    case InspectablePropertyType::ENUM:
                    {
                        auto pInspectableEnumProperty = static_cast<IInspectableEnumProperty *>(pInspectableProperty);
                        auto ppItems = pInspectableEnumProperty->GetItems();
                        int currentItem = (int)pInspectableEnumProperty->GetSelectedItem();
                        if (ImGui::Combo(pInspectableEnumProperty->GetName().c_str(), &currentItem, ppItems, (int)pInspectableEnumProperty->GetItemCount()))
                        {
                            pInspectableEnumProperty->SetSelectedItem((size_t)currentItem);
                        }
                    }
                    break;
                    case InspectablePropertyType::BOOL:
                        InspectablePropertyDisplayFnSelector<bool>::value(pInspectableProperty);
                        break;
                    case InspectablePropertyType::STRING:
                        InspectablePropertyDisplayFnSelector<std::string>::value(pInspectableProperty);
                        break;
                    case InspectablePropertyType::INT32:
                        InspectablePropertyDisplayFnSelector<int32_t>::value(pInspectableProperty);
                        break;
                    case InspectablePropertyType::UINT32:
                        InspectablePropertyDisplayFnSelector<uint32_t>::value(pInspectableProperty);
                        break;
                    case InspectablePropertyType::INT64:
                        InspectablePropertyDisplayFnSelector<int64_t>::value(pInspectableProperty);
                        break;
                    case InspectablePropertyType::UINT64:
                        InspectablePropertyDisplayFnSelector<uint64_t>::value(pInspectableProperty);
                        break;
                    case InspectablePropertyType::FLOAT:
                        InspectablePropertyDisplayFnSelector<float>::value(pInspectableProperty);
                        break;
                    case InspectablePropertyType::DOUBLE:
                        InspectablePropertyDisplayFnSelector<double>::value(pInspectableProperty);
                        break;
                    case InspectablePropertyType::VEC2:
                        InspectablePropertyDisplayFnSelector<glm::vec2>::value(pInspectableProperty);
                        break;
                    case InspectablePropertyType::VEC3:
                        InspectablePropertyDisplayFnSelector<glm::vec3>::value(pInspectableProperty);
                        break;
                    case InspectablePropertyType::VEC4:
                        InspectablePropertyDisplayFnSelector<glm::vec4>::value(pInspectableProperty);
                        break;
                    default:
                        assert(false);
                    }
                    if (readonly)
                    {
                        ImGui::EndDisabled();
                    }
                }
            }
            ImGui::TreePop();
        }
    }

    void DisplayObjectInspector(GameObject *pGameObject)
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

    void WorldSystem::DebugMenuCallback(int result)
    {
        if (mShowSceneHierarchy)
        {
            if (ImGui::Begin("Scene Hierarchy"))
            {
                DisplaySceneHierarchy(mGameObjects, mpSelectedGameObject);
            }
            ImGui::End();
        }
        if (mShowObjectInspector)
        {
            if (ImGui::Begin("Object Inspector"))
            {
                DisplayObjectInspector(mpSelectedGameObject);
            }
            ImGui::End();
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

        FASTCG_REGISTER_COMPONENT(DirectionalLight, pComponent);
        FASTCG_REGISTER_COMPONENT(PointLight, pComponent);
        FASTCG_REGISTER_COMPONENT(Renderable, pComponent);
        FASTCG_REGISTER_COMPONENT(Behaviour, pComponent);

        if (pComponent->GetType().IsExactly(Camera::TYPE) && pComponent->IsEnabled())
        {
            RegisterCamera(static_cast<Camera *>(pComponent));
        }
        else if (pComponent->GetType().IsExactly(Renderable::TYPE))
        {
            mArgs.pRenderBatchStrategy->AddRenderable(static_cast<Renderable *>(pComponent));
        }

        mComponents.emplace_back(pComponent);
    }

    void WorldSystem::RegisterCamera(Camera *pCamera)
    {
        mCameras.emplace_back(pCamera);
        SetMainCamera(pCamera);
    }

    void WorldSystem::UnregisterComponent(Component *pComponent)
    {
        assert(pComponent != nullptr);

        FASTCG_UNREGISTER_COMPONENT(Camera, pComponent);
        FASTCG_UNREGISTER_COMPONENT(DirectionalLight, pComponent);
        FASTCG_UNREGISTER_COMPONENT(PointLight, pComponent);
        FASTCG_UNREGISTER_COMPONENT(Renderable, pComponent);
        FASTCG_UNREGISTER_COMPONENT(Behaviour, pComponent);
        FASTCG_UNREGISTER_COMPONENT(Component, pComponent);

        if (pComponent->GetType().IsExactly(Camera::TYPE) && pComponent->IsEnabled())
        {
            if (mCameras.size() > 1)
            {
                SetMainCamera(mCameras[0]);
            }
        }
        else if (pComponent->GetType().IsExactly(Renderable::TYPE))
        {
            mArgs.pRenderBatchStrategy->RemoveRenderable(static_cast<Renderable *>(pComponent));
        }
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
    }

    void WorldSystem::Update(float cpuStart, float frameDeltaTime)
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
            pBehaviour->Update((float)cpuStart, (float)frameDeltaTime);
        }
    }
}