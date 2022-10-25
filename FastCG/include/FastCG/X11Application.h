#ifndef FASTCG_X11_APPLICATION_H
#define FASTCG_X11_APPLICATION_H

#ifdef FASTCG_LINUX

#include <FastCG/BaseApplication.h>

#ifdef FASTCG_OPENGL
#include <GL/glxew.h>
#endif

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

        Display *GetDisplay();
        Window &GetWindow();
        void CloseDisplay();

    protected:
        void RunMainLoop() override;

    private:
        Display *mpDisplay{nullptr};
        Colormap mpColorMap{None};
        Window mWindow{None};
        Atom mDeleteWindowAtom{None};

        void OpenDisplay();
        void CreateWindow();
    };
}

#endif

#endif