#include <FastCG/BaseRenderingSystem.h>

namespace FastCG
{
    BaseRenderingSystem::BaseRenderingSystem(const RenderingSystemArgs &rArgs) : mArgs(rArgs)
    {
    }

    BaseRenderingSystem::~BaseRenderingSystem()
    {
        mDrawDebugTextRequests.clear();
    }

    void BaseRenderingSystem::DrawDebugTextsWithStandardFont() const
    {
        for (auto &rDrawTextRequest : mDrawDebugTextRequests)
        {
            mpStandardFont->DrawDebugText(rDrawTextRequest.text, rDrawTextRequest.x, (mArgs.rScreenHeight - rDrawTextRequest.y), rDrawTextRequest.color);
        }
    }

}