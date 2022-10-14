#include <FastCG/BaseRenderingSystem.h>

namespace FastCG
{
    BaseRenderingSystem *BaseRenderingSystem::s_mpInstance = nullptr;

    BaseRenderingSystem::BaseRenderingSystem(const RenderingSystemArgs &rArgs) : mArgs(rArgs)
    {
        if (s_mpInstance != nullptr)
        {
            FASTCG_THROW_EXCEPTION(Exception, "There can only be one BaseRenderingSystem instance");
        }

        s_mpInstance = this;
    }

    BaseRenderingSystem::~BaseRenderingSystem()
    {
        mDrawDebugTextRequests.clear();

        s_mpInstance = nullptr;
    }

    void BaseRenderingSystem::DrawDebugTexts()
    {
        BeforeDrawDebugTexts();
        for (auto &rDrawTextRequest : mDrawDebugTextRequests)
        {
            mpStandardFont->DrawDebugText(rDrawTextRequest.text, rDrawTextRequest.x, (mArgs.rScreenHeight - rDrawTextRequest.y), rDrawTextRequest.color);
        }
        mDrawDebugTextRequests.clear();
        AfterDrawDebugTexts();
    }

}