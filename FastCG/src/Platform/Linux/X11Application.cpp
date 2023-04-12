#include <FastCG/Platform/Linux/X11Application.h>

#ifdef FASTCG_LINUX

#include <FastCG/Graphics/GraphicsSystem.h>

#include <X11/Xatom.h>
#include <X11/extensions/render.h>

#include <unordered_map>
#include <cassert>

namespace
{
#define KEY(a, b)                    \
    {                                \
        (uint64_t) b, FastCG::Key::a \
    }

    std::unordered_map<uint64_t, FastCG::Key> gKeyLut = {
        KEY(BACKSPACE, XK_BackSpace),
        KEY(RETURN, XK_Return),
        KEY(ESCAPE, XK_Escape),
        KEY(SPACE, XK_space),
        KEY(ASTERISK, XK_asterisk),
        KEY(PLUS, XK_plus),
        KEY(COMMA, XK_comma),
        KEY(MINUS, XK_minus),
        KEY(DOT, XK_period),
        KEY(SLASH, XK_slash),
        KEY(NUMBER_0, XK_KP_0),
        KEY(NUMBER_1, XK_KP_1),
        KEY(NUMBER_2, XK_KP_2),
        KEY(NUMBER_3, XK_KP_3),
        KEY(NUMBER_4, XK_KP_4),
        KEY(NUMBER_5, XK_KP_5),
        KEY(NUMBER_6, XK_KP_6),
        KEY(NUMBER_7, XK_KP_7),
        KEY(NUMBER_8, XK_KP_8),
        KEY(NUMBER_9, XK_KP_9),
        KEY(COLON, XK_colon),
        KEY(SEMI_COLON, XK_semicolon),
        KEY(EQUALS, XK_equal),
        KEY(LEFT_ARROW, XK_Left),
        KEY(UP_ARROW, XK_Up),
        KEY(RIGHT_ARROW, XK_Right),
        KEY(DOWN_ARROW, XK_Down),
        KEY(F1, XK_F1),
        KEY(F2, XK_F2),
        KEY(F3, XK_F3),
        KEY(F4, XK_F4),
        KEY(F5, XK_F5),
        KEY(F6, XK_F6),
        KEY(F7, XK_F7),
        KEY(F8, XK_F8),
        KEY(F9, XK_F9),
        KEY(F10, XK_F10),
        KEY(F11, XK_F11),
        KEY(F12, XK_F12),
        KEY(PAGE_UP, XK_Page_Up),
        KEY(PAGE_DOWN, XK_Page_Down),
        KEY(END, XK_End),
        KEY(HOME, XK_Home),
        KEY(INSERT, XK_Insert),
        KEY(SHIFT, XK_Shift_L),
        KEY(CONTROL, XK_Control_L),
        KEY(OPEN_SQUARE_BRACKET, XK_bracketleft),
        KEY(BACKSLASH, XK_backslash),
        KEY(CLOSE_SQUARE_BRACKET, XK_bracketright),
        KEY(LETTER_A, XK_a),
        KEY(LETTER_B, XK_b),
        KEY(LETTER_C, XK_c),
        KEY(LETTER_D, XK_d),
        KEY(LETTER_E, XK_e),
        KEY(LETTER_F, XK_f),
        KEY(LETTER_G, XK_g),
        KEY(LETTER_H, XK_h),
        KEY(LETTER_I, XK_i),
        KEY(LETTER_J, XK_j),
        KEY(LETTER_K, XK_k),
        KEY(LETTER_L, XK_l),
        KEY(LETTER_M, XK_m),
        KEY(LETTER_N, XK_n),
        KEY(LETTER_O, XK_o),
        KEY(LETTER_P, XK_p),
        KEY(LETTER_Q, XK_q),
        KEY(LETTER_R, XK_r),
        KEY(LETTER_S, XK_s),
        KEY(LETTER_T, XK_t),
        KEY(LETTER_U, XK_u),
        KEY(LETTER_V, XK_v),
        KEY(LETTER_W, XK_w),
        KEY(LETTER_X, XK_x),
        KEY(LETTER_Y, XK_y),
        KEY(LETTER_Z, XK_z),
        KEY(TILDE, XK_itilde),
        KEY(DEL, XK_Delete),
    };

    FastCG::Key TranslateKey(uint64_t key)
    {
        auto it = gKeyLut.find(key);
        if (it == gKeyLut.end())
        {
            return FastCG::Key::UNKNOWN;
        }
        return it->second;
    }

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
    void X11Application::OnPreInitialize()
    {
        BaseApplication::OnPreInitialize();

        mpDisplay = XOpenDisplay(nullptr);
        if (mpDisplay == nullptr)
        {
            FASTCG_THROW_EXCEPTION(Exception, "Couldn't connect to X server");
        }
    }

    void X11Application::OnPostFinalize()
    {
        if (mpDisplay != nullptr)
        {
            DestroyCurrentWindow();

            XCloseDisplay(mpDisplay);
            mpDisplay = nullptr;
        }

        BaseApplication::OnPostFinalize();
    }

    void X11Application::RunMainLoop()
    {
        XEvent event;
        while (mRunning)
        {
            while (XPending(mpDisplay))
            {
                XNextEvent(mpDisplay, &event);
                if (event.type == KeyPress || event.type == KeyRelease)
                {
                    char buf[128] = {0};
                    KeySym keySym;
                    XLookupString(&event.xkey, buf, sizeof buf, &keySym, NULL);
                    KeyboardCallback(TranslateKey((uint64_t)keySym), event.type == KeyPress);
                }
                else if (event.type == ButtonPress || event.type == ButtonRelease)
                {
                    MouseButtonCallback((MouseButton)(event.xbutton.button - 1),
                                        event.xbutton.state == 16 ? MouseButtonState::PRESSED : MouseButtonState::RELEASED);
                }
                else if (event.type == MotionNotify)
                {
                    MouseMoveCallback((uint32_t)event.xmotion.x, (uint32_t)event.xmotion.y);
                }
                else if (event.type == ConfigureNotify)
                {
                    WindowResizeCallback((uint32_t)event.xconfigure.width, (uint32_t)event.xconfigure.height);
                }
                else if (event.type == ClientMessage)
                {
                    if (static_cast<Atom>(event.xclient.data.l[0]) == mDeleteWindowAtom)
                    {
                        mRunning = false;
                    }
                }
            }

            RunMainLoopIteration();
        }
    }

    Window &X11Application::CreateSimpleWindow()
    {
        DestroyCurrentWindow();

        auto defaultScreen = DefaultScreen(mpDisplay);
        mWindow = XCreateSimpleWindow(mpDisplay,
                                      RootWindow(mpDisplay, defaultScreen),
                                      0,
                                      0,
                                      1,
                                      1,
                                      1,
                                      BlackPixel(mpDisplay, defaultScreen),
                                      WhitePixel(mpDisplay, defaultScreen));

        SetupCurrentWindow();

        return mWindow;
    }

    Window &X11Application::CreateWindow(XVisualInfo *pVisualInfo)
    {
        assert(pVisualInfo != nullptr);

        DestroyCurrentWindow();

        auto root = RootWindow(mpDisplay, DefaultScreen(mpDisplay));

        XSetWindowAttributes windowAttribs;
        windowAttribs.colormap = XCreateColormap(mpDisplay, root, pVisualInfo->visual, AllocNone);

        mWindow = XCreateWindow(mpDisplay,
                                root,
                                0,
                                0,
                                GetScreenWidth(),
                                GetScreenHeight(),
                                0,
                                pVisualInfo->depth,
                                InputOutput,
                                pVisualInfo->visual,
                                CWColormap,
                                &windowAttribs);
        if (mWindow == None)
        {
            FASTCG_THROW_EXCEPTION(Exception, "Failed to create a window");
        }

        if (windowAttribs.colormap != None)
        {
            XFreeColormap(mpDisplay, windowAttribs.colormap);
        }

        SetupCurrentWindow();

        return mWindow;
    }

    void X11Application::DestroyCurrentWindow()
    {
        if (mWindow != None)
        {
            XDestroyWindow(mpDisplay, mWindow);
            mWindow = None;
        }
    }

    void X11Application::SetupCurrentWindow()
    {
        XSelectInput(mpDisplay, mWindow, StructureNotifyMask | ExposureMask | PointerMotionMask | ButtonPressMask | ButtonReleaseMask | KeyPressMask | KeyReleaseMask);
        XMapWindow(mpDisplay, mWindow);
        XStoreName(mpDisplay, mWindow, GetWindowTitle().c_str());
        XEvent event;
        XIfEvent(mpDisplay, &event, WaitForMapNotify, (char *)&mWindow);
        if ((mDeleteWindowAtom = XInternAtom(mpDisplay, "WM_DELETE_WINDOW", 0)) != None)
        {
            XSetWMProtocols(mpDisplay, mWindow, &mDeleteWindowAtom, 1);
        }
    }

    uint64_t X11Application::GetNativeKey(Key key) const
    {
        for (auto it = gKeyLut.cbegin(); it != gKeyLut.cend(); ++it)
        {
            if (it->second == key)
            {
                return it->first;
            }
        }
        return uint64_t(~0);
    }
}

#endif