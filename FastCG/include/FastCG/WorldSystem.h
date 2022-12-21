#ifndef FASTCG_WORLD_SYSTEM
#define FASTCG_WORLD_SYSTEM

#include <FastCG/System.h>
#include <FastCG/RenderingStatistics.h>
#include <FastCG/RenderingPath.h>
#include <FastCG/IWorldRenderer.h>
#include <FastCG/GraphicsSystem.h>
#include <FastCG/GameObject.h>

#include <vector>
#include <string>
#include <memory>
#include <algorithm>

#define FASTCG_SUPPORT_COMPONENT_TRACKING(className)                 \
public:                                                              \
    inline const std::vector<className *> &Get##className##s() const \
    {                                                                \
        return m##className##s;                                      \
    }                                                                \
                                                                     \
private:                                                             \
    std::vector<className *> m##className##s;

namespace FastCG
{
    class RenderBatchStrategy;
    class Renderable;
    class PointLight;
    class DirectionalLight;
    class Component;
    class Camera;
    class Behaviour;

    struct WorldSystemArgs
    {
        RenderingPath renderingPath;
        const uint32_t &rScreenWidth;
        const uint32_t &rScreenHeight;
        const glm::vec4 &rClearColor;
        const glm::vec4 &rAmbientLight;
        RenderingStatistics &rRenderingStatistics;
    };

    class WorldSystem
    {
        FASTCG_DECLARE_SYSTEM(WorldSystem, WorldSystemArgs);
        FASTCG_SUPPORT_COMPONENT_TRACKING(Renderable);
        FASTCG_SUPPORT_COMPONENT_TRACKING(DirectionalLight);
        FASTCG_SUPPORT_COMPONENT_TRACKING(PointLight);
        FASTCG_SUPPORT_COMPONENT_TRACKING(Camera);
        FASTCG_SUPPORT_COMPONENT_TRACKING(Behaviour);

    public:
        inline const IWorldRenderer *GetWorldRenderer() const
        {
            return const_cast<WorldSystem *>(this)->GetWorldRenderer();
        }

        inline IWorldRenderer *GetWorldRenderer()
        {
            return mpWorldRenderer.get();
        }

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

    private:
        const WorldSystemArgs mArgs;
        std::unique_ptr<RenderBatchStrategy> mpRenderBatchStrategy;
        std::unique_ptr<IWorldRenderer> mpWorldRenderer;
        RenderingContext *mpRenderingContext;
        Camera *mpMainCamera{nullptr};
        std::vector<GameObject *> mGameObjects;
        std::vector<Component *> mComponents;
#ifdef _DEBUG
        GameObject *mpSelectedGameObject{nullptr};
        bool mShowSceneHierarchy{false};
        bool mShowObjectInspector{false};
#endif

        WorldSystem(const WorldSystemArgs &rArgs);
        virtual ~WorldSystem();

        inline float GetAspectRatio() const
        {
            return mArgs.rScreenWidth / (float)mArgs.rScreenHeight;
        }

        void Initialize();
        void Update(float cpuStart, float frameDeltaTime);
        void Render();
        void RegisterGameObject(GameObject *pGameObject);
        void UnregisterGameObject(GameObject *pGameObject);
        void RegisterComponent(Component *pComponent);
        void RegisterCamera(Camera *pCamera);
        void UnregisterComponent(Component *pComponent);
#ifdef _DEBUG
        void DebugMenuCallback(int result);
        void DebugMenuItemCallback(int &result);
#endif
        void Finalize();

        friend class GameObject;
    };

}

#endif