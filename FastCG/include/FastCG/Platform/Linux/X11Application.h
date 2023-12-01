#ifndef FASTCG_X11_APPLICATION_H
#define FASTCG_X11_APPLICATION_H

#ifdef FASTCG_LINUX

#include <FastCG/Platform/BaseApplication.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>

namespace FastCG
{
    class X11Application : public BaseApplication
    {
    public:
        X11Application(const ApplicationSettings &applicationSettings) : BaseApplication(applicationSettings) {}

        inline static X11Application *GetInstance()
        {
            return static_cast<X11Application *>(BaseApplication::GetInstance());
        }
        inline Display *GetDisplay()
        {
            return mpDisplay;
        }
        inline Window &GetWindow()
        {
            return mWindow;
        }
        Window &CreateSimpleWindow();
        Window &CreateWindow(XVisualInfo *pVisualInfo);

    protected:
        void OnPreInitialize() override;
        void OnPostFinalize() override;
        void RunMainLoop() override;

    private:
        Display *mpDisplay{nullptr};
        Window mWindow{None};
        Atom mDeleteWindowAtom{None};

        void DestroyCurrentWindow();
        void SetupCurrentWindow();
    };
}

#endif

#endif