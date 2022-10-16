#ifndef FASTCG_BASE_RENDERING_SYSTEM_H
#define FASTCG_BASE_RENDERING_SYSTEM_H

#include <FastCG/RenderingStatistics.h>
#include <FastCG/MeshBatch.h>
#include <FastCG/PointsRenderer.h>
#include <FastCG/PointLight.h>
#include <FastCG/LineRenderer.h>
#include <FastCG/FontRegistry.h>
#include <FastCG/Font.h>
#include <FastCG/DirectionalLight.h>
#include <FastCG/Camera.h>
#include <FastCG/BaseApplication.h>

#include <glm/glm.hpp>

#include <vector>
#include <string>
#include <memory>
#include <cstdint>

namespace FastCG
{
    struct RenderingSystemArgs
    {
        RenderingPath renderingPath;
        const uint32_t &rScreenWidth;
        const uint32_t &rScreenHeight;
        const glm::vec4 &rClearColor;
        const glm::vec4 &rAmbientLight;
        const std::vector<DirectionalLight *> &rDirectionalLights;
        const std::vector<PointLight *> &rPointLights;
        const std::vector<LineRenderer *> &rLineRenderers;
        const std::vector<PointsRenderer *> &rPointsRenderers;
        const std::vector<std::unique_ptr<MeshBatch>> &rMeshBatches;
        RenderingStatistics &rRenderingStatistics;
    };

    class BaseRenderingSystem
    {
    public:
        static constexpr const char *const DEFAULT_FONT_NAME = "verdana";

        virtual void Render(const Camera *pMainCamera) = 0;
        virtual void DrawDebugTexts() = 0;

        inline void Initialize()
        {
            OnInitialize();
        }

        inline void OnAssetsInitialized()
        {
            mpStandardFont = FontRegistry::Find(DEFAULT_FONT_NAME);
            OnStart();
        }

        inline void Finalize()
        {
            OnFinalize();
        }

        inline void DrawDebugText(const std::string &rText, uint32_t x, uint32_t y, const std::shared_ptr<Font> &pFont, const glm::vec4 &rColor)
        {
            mDrawDebugTextRequests.emplace_back(DrawDebugTextRequest{rText, x, y, pFont, rColor});
        }

        inline void DrawDebugText(const std::string &rText, uint32_t x, uint32_t y, const glm::vec4 &rColor)
        {
            mDrawDebugTextRequests.emplace_back(DrawDebugTextRequest{rText, x, y, mpStandardFont, rColor});
        }

        friend class BaseApplication;

    protected:
        struct DrawDebugTextRequest
        {
            std::string text;
            uint32_t x;
            uint32_t y;
            std::shared_ptr<Font> pFont;
            glm::vec4 color;
        };

        const RenderingSystemArgs mArgs;

        BaseRenderingSystem(const RenderingSystemArgs &rArgs);
        virtual ~BaseRenderingSystem();

        virtual void OnInitialize() = 0;
        virtual void OnStart() = 0;
        virtual void OnFinalize() = 0;
        void DrawDebugTextsWithStandardFont() const;

    private:
        static BaseRenderingSystem *s_mpInstance;

        std::shared_ptr<Font> mpStandardFont{nullptr};
        std::vector<DrawDebugTextRequest> mDrawDebugTextRequests;
    };

}

#endif