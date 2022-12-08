#ifndef FASTCG_WORLD_SYSTEM
#define FASTCG_WORLD_SYSTEM

#include <FastCG/System.h>

#include <vector>
#include <memory>

#define FASTCG_TRACK_COMPONENT(className)                            \
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
    class GameObject;
    class DirectionalLight;
    class Component;
    class Camera;
    class Behaviour;
    class BaseApplication;

    struct WorldSystemArgs
    {
        const std::unique_ptr<RenderBatchStrategy> &pRenderBatchStrategy;
    };

    class WorldSystem
    {
        FASTCG_DECLARE_SYSTEM(WorldSystem, WorldSystemArgs);
        FASTCG_TRACK_COMPONENT(Renderable);
        FASTCG_TRACK_COMPONENT(DirectionalLight);
        FASTCG_TRACK_COMPONENT(PointLight);
        FASTCG_TRACK_COMPONENT(Camera);
        FASTCG_TRACK_COMPONENT(Behaviour);

    public:
        inline Camera *GetMainCamera()
        {
            return mpMainCamera;
        }

        void SetMainCamera(Camera *pCamera);
        void Initialize();
        void Update(float cpuStart, float frameDeltaTime);

    private:
        const WorldSystemArgs mArgs;

        Camera *mpMainCamera{nullptr};
        std::vector<GameObject *> mGameObjects;
        std::vector<Component *> mComponents;

        WorldSystem(const WorldSystemArgs &rArgs) : mArgs(rArgs) {}
        ~WorldSystem();

        void RegisterGameObject(GameObject *pGameObject);
        void UnregisterGameObject(GameObject *pGameObject);
        void RegisterComponent(Component *pComponent);
        void RegisterCamera(Camera *pCamera);
        void UnregisterComponent(Component *pComponent);
#ifdef _DEBUG
        void DebugMenuCallback(int result);
        void DebugMenuItemCallback(int& result);
#endif

        friend class GameObject;
        friend class BaseApplication;
    };

}

#endif