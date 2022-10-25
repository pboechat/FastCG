#include <FastCG/X11Application.h>

#ifdef FASTCG_LINUX

#include <FastCG/RenderingSystem.h>

#include <X11/Xatom.h>
#include <X11/extensions/render.h>

namespace
{
    Bool WaitForMapNotify(Display *display, XEvent *event, char *arg)
    {
        return display != nullptr &&
               event != nullptr &&
               arg != nullptr &&
               (event->type == MapNotify) &&
               (event->xmap.window == *(Window *)arg);
    }

}

namespace FastCG
{
    void X11Application::RunMainLoop()
    {
        XEvent event;
        while (mRunning)
        {
            XNextEvent(mpDisplay, &event);
            if (event.type == KeyPress || event.type == KeyRelease)
            {
                char buf[128] = {0};
                KeySym keySym;
                XLookupString(&event.xkey, buf, sizeof buf, &keySym, NULL);
                KeyboardCallback((int)keySym, event.type == KeyPress);
            }
            else if (event.type == ClientMessage)
            {
                if (static_cast<Atom>(event.xclient.data.l[0]) == mDeleteWindowAtom)
                {
                    mRunning = false;
                }
            }
            else if (event.type == ConfigureNotify)
            {
                WindowResizeCallback((int)event.xconfigure.width, (int)event.xconfigure.height);
            }
        }
    }

    Display *X11Application::GetDisplay()
    {
        if (mpDisplay != nullptr)
        {
            return mpDisplay;
        }

        OpenDisplay();

        return mpDisplay;
    }

    void X11Application::OpenDisplay()
    {
        mpDisplay = XOpenDisplay(nullptr);
        if (mpDisplay == nullptr)
        {
            FASTCG_THROW_EXCEPTION(Exception, "Couldn't connect to X server");
        }
    }

    Window &X11Application::GetWindow()
    {
        if (mWindow != None)
        {
            return mWindow;
        }

        CreateWindow();

        return mWindow;
    }

    void X11Application::CreateWindow()
    {
        auto *pVisualInfo = RenderingSystem::GetInstance()->GetVisualInfo();

        auto root = RootWindow(mpDisplay, DefaultScreen(mpDisplay));

        auto colorMap = XCreateColormap(mpDisplay, root, pVisualInfo->visual, AllocNone);

        XSetWindowAttributes attr = {
            0,
        };

        attr.colormap = colorMap;
        attr.background_pixmap = None;
        attr.border_pixmap = None;
        attr.border_pixel = 0;
        attr.event_mask = StructureNotifyMask |
                          EnterWindowMask |
                          LeaveWindowMask |
                          ExposureMask |
                          ButtonPressMask |
                          ButtonReleaseMask |
                          OwnerGrabButtonMask |
                          KeyPressMask |
                          KeyReleaseMask;

        int attrMask = CWColormap |
                       CWBorderPixel |
                       CWEventMask;
        mWindow = XCreateWindow(mpDisplay, root, 0, 0, GetScreenWidth(), GetScreenHeight(), 0, pVisualInfo->depth, InputOutput, pVisualInfo->visual, attrMask, &attr);
        if (mWindow == 0)
        {
            FASTCG_THROW_EXCEPTION(Exception, "Failed to create the window");
        }

        XTextProperty textProproperty;
        textProproperty.value = (unsigned char *)GetWindowTitle().c_str();
        textProproperty.encoding = XA_STRING;
        textProproperty.format = 8;
        textProproperty.nitems = (unsigned long)GetWindowTitle().size();

        XSizeHints hints;
        hints.x = 0;
        hints.y = 0;
        hints.width = (int)GetScreenWidth();
        hints.height = (int)GetScreenHeight();
        hints.flags = USPosition | USSize;

        XWMHints *pStartupState;
        pStartupState = XAllocWMHints();
        pStartupState->initial_state = NormalState;
        pStartupState->flags = StateHint;

        XSetWMProperties(mpDisplay,
                         mWindow,
                         &textProproperty,
                         &textProproperty,
                         nullptr,
                         0,
                         &hints,
                         pStartupState,
                         nullptr);

        XFree(pStartupState);
        XMapWindow(mpDisplay, mWindow);
        XEvent event;
        XIfEvent(mpDisplay, &event, WaitForMapNotify, (char *)&mWindow);
        if ((mDeleteWindowAtom = XInternAtom(mpDisplay, "WM_DELETE_WINDOW", 0)) != None)
        {
            XSetWMProtocols(mpDisplay, mWindow, &mDeleteWindowAtom, 1);
        }
    }

    void X11Application::CloseDisplay()
    {
        if (mpDisplay == nullptr)
        {
            return;
        }
        if (mWindow != None)
        {
            XDestroyWindow(mpDisplay, mWindow);
            mWindow = None;
        }
        XCloseDisplay(mpDisplay);
        mpDisplay = nullptr;
    }
}

#endif