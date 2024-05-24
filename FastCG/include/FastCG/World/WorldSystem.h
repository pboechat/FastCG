#ifndef FASTCG_WORLD_SYSTEM
#define FASTCG_WORLD_SYSTEM

#include <FastCG/Core/System.h>
#include <FastCG/Reflection/Inspectable.h>
#include <FastCG/World/GameObject.h>

#if _DEBUG
#include <ImGuizmo.h>
#include <glm/glm.hpp>
#endif

#include <string>
#include <vector>

#define FASTCG_COMPONENT_TRACKING(className)                                                                           \
public:                                                                                                                \
    inline const className *Get##className() const                                                                     \
    {                                                                                                                  \
        return mp##className;                                                                                          \
    }                                                                                                                  \
    inline className *Get##className()                                                                                 \
    {                                                                                                                  \
        return mp##className;                                                                                          \
    }                                                                                                                  \
                                                                                                                       \
private:                                                                                                               \
    className *mp##className{nullptr};

#define FASTCG_COMPONENT_COLLECTION_TRACKING(className)                                                                \
public:                                                                                                                \
    inline const std::vector<className *> &Get##className##s() const                                                   \
    {                                                                                                                  \
        return m##className##s;                                                                                        \
    }                                                                                                                  \
                                                                                                                       \
private:                                                                                                               \
    std::vector<className *> m##className##s;

namespace FastCG
{
    class RenderBatchStrategy;
    class Renderable;
    class PointLight;
    class Material;
    class Fog;
    class DirectionalLight;
    class Component;
    class Camera;
    class Behaviour;

    struct WorldSystemArgs
    {
        const uint32_t &rScreenWidth;
        const uint32_t &rScreenHeight;
    };

    class WorldSystem
    {
        FASTCG_DECLARE_SYSTEM(WorldSystem, WorldSystemArgs);
        FASTCG_COMPONENT_COLLECTION_TRACKING(DirectionalLight);
        FASTCG_COMPONENT_COLLECTION_TRACKING(PointLight);
        FASTCG_COMPONENT_TRACKING(Fog);
        FASTCG_COMPONENT_COLLECTION_TRACKING(Camera);
        FASTCG_COMPONENT_COLLECTION_TRACKING(Behaviour);

    public:
        inline const Camera *GetMainCamera() const
        {
            return const_cast<WorldSystem *>(this)->GetMainCamera();
        }

        inline Camera *GetMainCamera()
        {
            return mpMainCamera;
        }

        inline GameObject *FindFirstGameObject(const std::string &rGameObjectName) const
        {
            for (auto *pGameObject : mGameObjects)
            {
                if (pGameObject->GetName() == rGameObjectName)
                {
                    return pGameObject;
                }
            }
            return nullptr;
        }
        inline void FindGameObjects(const std::string &rGameObjectName, std::vector<GameObject *> &rGameObjects) const
        {
            for (auto *pGameObject : mGameObjects)
            {
                if (pGameObject->GetName() == rGameObjectName)
                {
                    rGameObjects.emplace_back(pGameObject);
                }
            }
        }
        template <typename ComponentT>
        inline void FindComponents(std::vector<ComponentT *> &rComponents) const
        {
            for (auto *pGameObject : mGameObjects)
            {
                auto *pComponent = pGameObject->GetComponent<ComponentT>();
                if (pComponent != nullptr)
                {
                    rComponents.emplace_back(pComponent);
                }
            }
        }
        void SetMainCamera(Camera *pCamera);
        void RegisterCamera(Camera *pCamera);
        void UnregisterCamera(Camera *pCamera);
#if _DEBUG
        inline void SetSelectedMaterial(const std::shared_ptr<Material> &rpMaterial)
        {
            mShowMaterialBrowser = true;
            mpSelectedMaterial = rpMaterial;
            mSelectedTextureName = {};
        }
#endif

    private:
        const WorldSystemArgs mArgs;
        Camera *mpMainCamera{nullptr};
        std::vector<GameObject *> mGameObjects;
        std::vector<Component *> mComponents;
#if _DEBUG
        GameObject *mpSelectedGameObject{nullptr};
        bool mShowSceneHierarchy{false};
        bool mShowObjectInspector{false};
        std::weak_ptr<Material> mpSelectedMaterial{};
        bool mShowMaterialBrowser{false};
        IInspectableProperty *mpSelectedInspectableProperty{nullptr};
        std::string mSelectedTextureName{};
        bool mShowGizmosOptions{false};
        ImGuizmo::OPERATION mOperation{ImGuizmo::TRANSLATE};
        ImGuizmo::MODE mMode{ImGuizmo::WORLD};
        glm::vec3 mTranslateSnap{0.1f, 0.1f, 0.1f};
        float mRotateSnap{5};
        float mScaleSnap{0.1f};
#endif

        WorldSystem(const WorldSystemArgs &rArgs);
        ~WorldSystem();

        inline float GetAspectRatio() const
        {
            return mArgs.rScreenWidth / (float)mArgs.rScreenHeight;
        }
        void Initialize();
        void RegisterGameObject(GameObject *pGameObject);
        void UnregisterGameObject(GameObject *pGameObject);
        void RegisterComponent(Component *pComponent);
        void UnregisterComponent(Component *pComponent);
        void Update(float time, float deltaTime);
#if _DEBUG
        void DebugMenuCallback(int result);
        void DebugMenuItemCallback(int &result);
#endif
        void Finalize();

        friend class GameObject;
        friend class Component;
    };

}

#endif