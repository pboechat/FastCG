#include <FastCG/Platform/Linux/X11Application.h>

#ifdef FASTCG_LINUX

#include <FastCG/Graphics/GraphicsSystem.h>
#include <FastCG/Platform/Timer.h>

#include <X11/Xatom.h>
#include <X11/extensions/render.h>

#include <cassert>
#include <unordered_map>

namespace
{
#ifdef MAP_KEY
#undef MAP_KEY
#endif

#define MAP_KEY(a, b) {(uint64_t)a, FastCG::Key::b}

    std::unordered_map<uint64_t, FastCG::Key> KEY_LUT = {MAP_KEY(XK_BackSpace, BACKSPACE),
                                                         MAP_KEY(XK_Return, RETURN),
                                                         MAP_KEY(XK_Escape, ESCAPE),
                                                         MAP_KEY(XK_space, SPACE),
                                                         MAP_KEY(XK_exclam, EXCLAMATION_MARK),
                                                         MAP_KEY(XK_quotedbl, DOUBLE_QUOTES),
                                                         MAP_KEY(XK_numbersign, HASH),
                                                         MAP_KEY(XK_dollar, DOLLAR),
                                                         MAP_KEY(XK_ampersand, AMPERSAND),
                                                         MAP_KEY(XK_percent, PERCENT),
                                                         MAP_KEY(XK_apostrophe, APOSTROPHE),
                                                         MAP_KEY(XK_parenleft, OPENING_ROUND_BRACKET),
                                                         MAP_KEY(XK_parenright, CLOSING_ROUND_BRACKET),
                                                         MAP_KEY(XK_asterisk, ASTERISK),
                                                         MAP_KEY(XK_plus, PLUS),
                                                         MAP_KEY(XK_comma, COMMA),
                                                         MAP_KEY(XK_minus, MINUS),
                                                         MAP_KEY(XK_period, DOT),
                                                         MAP_KEY(XK_slash, SLASH),
                                                         MAP_KEY(XK_0, NUMBER_0),
                                                         MAP_KEY(XK_1, NUMBER_1),
                                                         MAP_KEY(XK_2, NUMBER_2),
                                                         MAP_KEY(XK_3, NUMBER_3),
                                                         MAP_KEY(XK_4, NUMBER_4),
                                                         MAP_KEY(XK_5, NUMBER_5),
                                                         MAP_KEY(XK_6, NUMBER_6),
                                                         MAP_KEY(XK_7, NUMBER_7),
                                                         MAP_KEY(XK_8, NUMBER_8),
                                                         MAP_KEY(XK_9, NUMBER_9),
                                                         MAP_KEY(XK_colon, COLON),
                                                         MAP_KEY(XK_semicolon, SEMI_COLON),
                                                         MAP_KEY(XK_less, LESS_THAN),
                                                         MAP_KEY(XK_equal, EQUALS),
                                                         MAP_KEY(XK_greater, GREATER_THAN),
                                                         MAP_KEY(XK_question, QUESTION_MARK),
                                                         MAP_KEY(XK_at, AT),
                                                         MAP_KEY(XK_bracketleft, OPENING_SQUARE_BRACKET),
                                                         MAP_KEY(XK_backslash, BACKSLASH),
                                                         MAP_KEY(XK_bracketright, CLOSING_SQUARE_BRACKET),
                                                         MAP_KEY(XK_asciicircum, CIRCUMFLEX),
                                                         MAP_KEY(XK_underscore, UNDERSCORE),
                                                         MAP_KEY(XK_grave, GRAVE),
                                                         MAP_KEY(XK_a, LETTER_A),
                                                         MAP_KEY(XK_b, LETTER_B),
                                                         MAP_KEY(XK_c, LETTER_C),
                                                         MAP_KEY(XK_d, LETTER_D),
                                                         MAP_KEY(XK_e, LETTER_E),
                                                         MAP_KEY(XK_f, LETTER_F),
                                                         MAP_KEY(XK_g, LETTER_G),
                                                         MAP_KEY(XK_h, LETTER_H),
                                                         MAP_KEY(XK_i, LETTER_I),
                                                         MAP_KEY(XK_j, LETTER_J),
                                                         MAP_KEY(XK_k, LETTER_K),
                                                         MAP_KEY(XK_l, LETTER_L),
                                                         MAP_KEY(XK_m, LETTER_M),
                                                         MAP_KEY(XK_n, LETTER_N),
                                                         MAP_KEY(XK_o, LETTER_O),
                                                         MAP_KEY(XK_p, LETTER_P),
                                                         MAP_KEY(XK_q, LETTER_Q),
                                                         MAP_KEY(XK_r, LETTER_R),
                                                         MAP_KEY(XK_s, LETTER_S),
                                                         MAP_KEY(XK_t, LETTER_T),
                                                         MAP_KEY(XK_u, LETTER_U),
                                                         MAP_KEY(XK_v, LETTER_V),
                                                         MAP_KEY(XK_w, LETTER_W),
                                                         MAP_KEY(XK_x, LETTER_X),
                                                         MAP_KEY(XK_y, LETTER_Y),
                                                         MAP_KEY(XK_z, LETTER_Z),
                                                         MAP_KEY(XK_braceleft, OPENING_CURLY_BRACKET),
                                                         MAP_KEY(XK_bar, VERTICAL_BAR),
                                                         MAP_KEY(XK_braceright, CLOSING_CURLY_BRACKET),
                                                         MAP_KEY(XK_asciitilde, TILDE),
                                                         MAP_KEY(XK_Delete, DEL),
                                                         MAP_KEY(XK_Left, LEFT_ARROW),
                                                         MAP_KEY(XK_Up, UP_ARROW),
                                                         MAP_KEY(XK_Right, RIGHT_ARROW),
                                                         MAP_KEY(XK_Down, DOWN_ARROW),
                                                         MAP_KEY(XK_F1, F1),
                                                         MAP_KEY(XK_F2, F2),
                                                         MAP_KEY(XK_F3, F3),
                                                         MAP_KEY(XK_F4, F4),
                                                         MAP_KEY(XK_F5, F5),
                                                         MAP_KEY(XK_F6, F6),
                                                         MAP_KEY(XK_F7, F7),
                                                         MAP_KEY(XK_F8, F8),
                                                         MAP_KEY(XK_F9, F9),
                                                         MAP_KEY(XK_F10, F10),
                                                         MAP_KEY(XK_F11, F11),
                                                         MAP_KEY(XK_F12, F12),
                                                         MAP_KEY(XK_Page_Up, PAGE_UP),
                                                         MAP_KEY(XK_Page_Down, PAGE_DOWN),
                                                         MAP_KEY(XK_End, END),
                                                         MAP_KEY(XK_Home, HOME),
                                                         MAP_KEY(XK_Insert, INSERT),
                                                         MAP_KEY(XK_Num_Lock, NUMLOCK),
                                                         MAP_KEY(XK_KP_0, KEYPAD_0),
                                                         MAP_KEY(XK_KP_1, KEYPAD_1),
                                                         MAP_KEY(XK_KP_2, KEYPAD_2),
                                                         MAP_KEY(XK_KP_3, KEYPAD_3),
                                                         MAP_KEY(XK_KP_4, KEYPAD_4),
                                                         MAP_KEY(XK_KP_5, KEYPAD_5),
                                                         MAP_KEY(XK_KP_6, KEYPAD_6),
                                                         MAP_KEY(XK_KP_7, KEYPAD_7),
                                                         MAP_KEY(XK_KP_8, KEYPAD_8),
                                                         MAP_KEY(XK_KP_9, KEYPAD_9),
                                                         MAP_KEY(XK_KP_Decimal, KEYPAD_DECIMAL),
                                                         MAP_KEY(XK_KP_Divide, KEYPAD_DIVIDE),
                                                         MAP_KEY(XK_KP_Multiply, KEYPAD_MULTIPLY),
                                                         MAP_KEY(XK_KP_Subtract, KEYPAD_SUBTRACT),
                                                         MAP_KEY(XK_KP_Add, KEYPAD_ADD),
                                                         MAP_KEY(XK_Shift_L, SHIFT),
                                                         MAP_KEY(XK_Control_L, CONTROL)};

#undef MAP_KEY

    bool TranslateToKey(uint64_t key, FastCG::Key &rKey)
    {
        auto it = KEY_LUT.find(key);
        if (it == KEY_LUT.end())
        {
            return false;
        }
        rKey = it->second;
        return true;
    }

    Bool WaitForMapNotify(Display *mpDisplay, XEvent *pEvent, char *pArg)
    {
        return mpDisplay != nullptr && pEvent != nullptr && pArg != nullptr && (pEvent->type == MapNotify) &&
               (pEvent->xmap.window == *(Window *)pArg);
    }

}

namespace FastCG
{
    void X11Application::OnPreInitialize()
    {
        mpDisplay = XOpenDisplay(nullptr);
        if (mpDisplay == nullptr)
        {
            FASTCG_THROW_EXCEPTION(Exception, "X11: Couldn't open X server display");
        }
    }

    void X11Application::OnPostInitialize()
    {
        if (!mSettings.graphics.headless)
        {
            // NOTE: on X11 the graphics system is responsible for creating the window
            GraphicsSystem::GetInstance()->OnPostWindowInitialize(nullptr);
        }

        BaseApplication::OnPostInitialize();
    }

    void X11Application::OnPostFinalize()
    {
        if (!mSettings.graphics.headless)
        {
            // NOTE: on X11 the graphics system is responsible for creating the window
            GraphicsSystem::GetInstance()->OnPreWindowTerminate(nullptr);
            DestroyCurrentWindow();
        }

        if (mpDisplay != nullptr)
        {
            XCloseDisplay(mpDisplay);
            mpDisplay = nullptr;
        }

        BaseApplication::OnPostFinalize();
    }

    void X11Application::RunMainLoop()
    {
        if (mSettings.graphics.headless)
        {
            RunConsoleMainLoop();
        }
        else
        {
            RunWindowedMainLoop();
        }
    }

    void X11Application::RunConsoleMainLoop()
    {
        while (mRunning)
        {
            auto osStart = Timer::GetTime();
            // TODO: read key input from console
            RunMainLoopIteration(Timer::GetTime() - osStart);
        }
    }

    void X11Application::RunWindowedMainLoop()
    {
        XEvent event;
        while (mRunning)
        {
            auto osStart = Timer::GetTime();

            while (XPending(mpDisplay))
            {
                XNextEvent(mpDisplay, &event);
                if (event.type == KeyPress || event.type == KeyRelease)
                {
                    char buffer[128] = {0};
                    KeySym keySym;
                    XLookupString(&event.xkey, buffer, sizeof(buffer), &keySym, NULL);
                    Key key;
                    if (TranslateToKey((uint64_t)keySym, key))
                    {
                        KeyboardCallback(key, event.type == KeyPress);
                    }
                }
                else if (event.type == ButtonPress || event.type == ButtonRelease)
                {
                    MouseButtonCallback((MouseButton)(event.xbutton.button - 1), event.xbutton.state == 16
                                                                                     ? MouseButtonState::PRESSED
                                                                                     : MouseButtonState::RELEASED);
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
                        goto __exitMainLoop;
                    }
                }
            }

            RunMainLoopIteration(Timer::GetTime() - osStart);
        }
    __exitMainLoop:
        return;
    }

    Window &X11Application::CreateSimpleWindow()
    {
        assert(mpDisplay != nullptr);

        DestroyCurrentWindow();

        auto defaultScreen = DefaultScreen(mpDisplay);
        mWindow = XCreateSimpleWindow(mpDisplay, RootWindow(mpDisplay, defaultScreen), 0, 0, GetScreenWidth(),
                                      GetScreenHeight(), 1, BlackPixel(mpDisplay, defaultScreen),
                                      WhitePixel(mpDisplay, defaultScreen));

        SetupCurrentWindow();

        return mWindow;
    }

    Window &X11Application::CreateWindow(XVisualInfo *pVisualInfo)
    {
        assert(mpDisplay != nullptr);
        assert(pVisualInfo != nullptr);

        DestroyCurrentWindow();

        auto defaultScreen = DefaultScreen(mpDisplay);

        auto root = RootWindow(mpDisplay, defaultScreen);

        XSetWindowAttributes windowAttribs;
        windowAttribs.colormap = XCreateColormap(mpDisplay, root, pVisualInfo->visual, AllocNone);
        windowAttribs.event_mask = StructureNotifyMask;

        mWindow = XCreateWindow(mpDisplay, root, 0, 0, GetScreenWidth(), GetScreenHeight(), 0, pVisualInfo->depth,
                                InputOutput, pVisualInfo->visual, CWColormap | CWEventMask, &windowAttribs);
        if (mWindow == None)
        {
            FASTCG_THROW_EXCEPTION(Exception, "X11: Couldn't create a window");
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
        XSelectInput(mpDisplay, mWindow,
                     StructureNotifyMask | ExposureMask | PointerMotionMask | ButtonPressMask | ButtonReleaseMask |
                         KeyPressMask | KeyReleaseMask);
        XMapWindow(mpDisplay, mWindow);
        XStoreName(mpDisplay, mWindow, GetWindowTitle().c_str());
        XEvent event;
        XIfEvent(mpDisplay, &event, WaitForMapNotify, (char *)&mWindow);
        if ((mDeleteWindowAtom = XInternAtom(mpDisplay, "WM_DELETE_WINDOW", 0)) != None)
        {
            XSetWMProtocols(mpDisplay, mWindow, &mDeleteWindowAtom, 1);
        }
    }

}

#endif