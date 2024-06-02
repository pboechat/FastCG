#ifdef FASTCG_ANDROID

#include <FastCG/Core/Exception.h>
#include <FastCG/Core/Log.h>
#include <FastCG/Graphics/GraphicsSystem.h>
#include <FastCG/Platform/Android/AndroidApplication.h>
#include <FastCG/Platform/Timer.h>

#include <android/native_window.h>

#include <cassert>

namespace
{
#ifdef MAP_KEY
#undef MAP_KEY
#endif

#define MAP_KEY(a, b)                                                                                                  \
    {                                                                                                                  \
        (uint64_t) a, FastCG::Key::b                                                                                   \
    }

    // TODO: complete mapping
    std::unordered_map<uint64_t, FastCG::Key> KEY_LUT = {
        MAP_KEY(AKEYCODE_BACK, BACKSPACE),
        MAP_KEY(AKEYCODE_ENTER, RETURN),
        MAP_KEY(AKEYCODE_ESCAPE, ESCAPE),
        MAP_KEY(AKEYCODE_SPACE, SPACE),
        MAP_KEY(AKEYCODE_STAR, ASTERISK),
        MAP_KEY(AKEYCODE_PLUS, PLUS),
        MAP_KEY(AKEYCODE_COMMA, COMMA),
        MAP_KEY(AKEYCODE_MINUS, MINUS),
        MAP_KEY(AKEYCODE_PERIOD, DOT),
        MAP_KEY(AKEYCODE_SLASH, SLASH),
        MAP_KEY(AKEYCODE_NUMPAD_0, NUMBER_0),
        MAP_KEY(AKEYCODE_NUMPAD_1, NUMBER_1),
        MAP_KEY(AKEYCODE_NUMPAD_2, NUMBER_2),
        MAP_KEY(AKEYCODE_NUMPAD_3, NUMBER_3),
        MAP_KEY(AKEYCODE_NUMPAD_4, NUMBER_4),
        MAP_KEY(AKEYCODE_NUMPAD_5, NUMBER_5),
        MAP_KEY(AKEYCODE_NUMPAD_6, NUMBER_6),
        MAP_KEY(AKEYCODE_NUMPAD_7, NUMBER_7),
        MAP_KEY(AKEYCODE_NUMPAD_8, NUMBER_8),
        MAP_KEY(AKEYCODE_NUMPAD_9, NUMBER_9),
        MAP_KEY(AKEYCODE_SEMICOLON, SEMI_COLON),
        MAP_KEY(AKEYCODE_EQUALS, EQUALS),
        MAP_KEY(AKEYCODE_DPAD_LEFT, LEFT_ARROW),
        MAP_KEY(AKEYCODE_DPAD_UP, UP_ARROW),
        MAP_KEY(AKEYCODE_DPAD_RIGHT, RIGHT_ARROW),
        MAP_KEY(AKEYCODE_DPAD_DOWN, DOWN_ARROW),
        MAP_KEY(AKEYCODE_F1, F1),
        MAP_KEY(AKEYCODE_F2, F2),
        MAP_KEY(AKEYCODE_F3, F3),
        MAP_KEY(AKEYCODE_F4, F4),
        MAP_KEY(AKEYCODE_F5, F5),
        MAP_KEY(AKEYCODE_F6, F6),
        MAP_KEY(AKEYCODE_F7, F7),
        MAP_KEY(AKEYCODE_F8, F8),
        MAP_KEY(AKEYCODE_F9, F9),
        MAP_KEY(AKEYCODE_F10, F10),
        MAP_KEY(AKEYCODE_F11, F11),
        MAP_KEY(AKEYCODE_F12, F12),
        MAP_KEY(AKEYCODE_PAGE_UP, PAGE_UP),
        MAP_KEY(AKEYCODE_PAGE_DOWN, PAGE_DOWN),
        MAP_KEY(AKEYCODE_MOVE_END, END),
        MAP_KEY(AKEYCODE_MOVE_HOME, HOME),
        MAP_KEY(AKEYCODE_INSERT, INSERT),
        MAP_KEY(AKEYCODE_SHIFT_LEFT, SHIFT),
        MAP_KEY(AKEYCODE_CTRL_LEFT, CONTROL),
        MAP_KEY(AKEYCODE_BACKSLASH, BACKSLASH),
        MAP_KEY(AKEYCODE_A, LETTER_A),
        MAP_KEY(AKEYCODE_B, LETTER_B),
        MAP_KEY(AKEYCODE_C, LETTER_C),
        MAP_KEY(AKEYCODE_D, LETTER_D),
        MAP_KEY(AKEYCODE_E, LETTER_E),
        MAP_KEY(AKEYCODE_F, LETTER_F),
        MAP_KEY(AKEYCODE_G, LETTER_G),
        MAP_KEY(AKEYCODE_H, LETTER_H),
        MAP_KEY(AKEYCODE_I, LETTER_I),
        MAP_KEY(AKEYCODE_J, LETTER_J),
        MAP_KEY(AKEYCODE_K, LETTER_K),
        MAP_KEY(AKEYCODE_L, LETTER_L),
        MAP_KEY(AKEYCODE_M, LETTER_M),
        MAP_KEY(AKEYCODE_N, LETTER_N),
        MAP_KEY(AKEYCODE_O, LETTER_O),
        MAP_KEY(AKEYCODE_P, LETTER_P),
        MAP_KEY(AKEYCODE_Q, LETTER_Q),
        MAP_KEY(AKEYCODE_R, LETTER_R),
        MAP_KEY(AKEYCODE_S, LETTER_S),
        MAP_KEY(AKEYCODE_T, LETTER_T),
        MAP_KEY(AKEYCODE_U, LETTER_U),
        MAP_KEY(AKEYCODE_V, LETTER_V),
        MAP_KEY(AKEYCODE_W, LETTER_W),
        MAP_KEY(AKEYCODE_X, LETTER_X),
        MAP_KEY(AKEYCODE_Y, LETTER_Y),
        MAP_KEY(AKEYCODE_Z, LETTER_Z),
        MAP_KEY(AKEYCODE_DEL, DEL),
    };

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

#ifdef CASE_RETURN_STRING
#undef CASE_RETURN_STRING
#endif

#define CASE_RETURN_STRING(str)                                                                                        \
    case str:                                                                                                          \
        return #str

    const char *GetAInputEventTypeString(int32_t eventType)
    {
        switch (eventType)
        {
            CASE_RETURN_STRING(AINPUT_EVENT_TYPE_MOTION);
            CASE_RETURN_STRING(AINPUT_EVENT_TYPE_KEY);
            CASE_RETURN_STRING(AINPUT_EVENT_TYPE_FOCUS);
        default:
            FASTCG_THROW_EXCEPTION(FastCG::Exception, "Android: Unhandled AInputEventType %d", eventType);
            return nullptr;
        }
    }

    const char *GetAMotionEventActionString(int32_t action)
    {
        switch (action)
        {
            CASE_RETURN_STRING(AMOTION_EVENT_ACTION_DOWN);
            CASE_RETURN_STRING(AMOTION_EVENT_ACTION_UP);
            CASE_RETURN_STRING(AMOTION_EVENT_ACTION_MOVE);
            CASE_RETURN_STRING(AMOTION_EVENT_ACTION_CANCEL);
            CASE_RETURN_STRING(AMOTION_EVENT_ACTION_OUTSIDE);
            CASE_RETURN_STRING(AMOTION_EVENT_ACTION_POINTER_DOWN);
            CASE_RETURN_STRING(AMOTION_EVENT_ACTION_POINTER_UP);
        default:
            FASTCG_THROW_EXCEPTION(FastCG::Exception, "Android: Unhandled AMotionEventAction %d", action);
            return nullptr;
        }
    }

    const char *GetAKeyEventActionString(int32_t action)
    {
        switch (action)
        {
            CASE_RETURN_STRING(AKEY_EVENT_ACTION_DOWN);
            CASE_RETURN_STRING(AKEY_EVENT_ACTION_UP);
            CASE_RETURN_STRING(AKEY_EVENT_ACTION_MULTIPLE);
        default:
            FASTCG_THROW_EXCEPTION(FastCG::Exception, "Android: Unhandled AKeyEventAction %d", action);
            return nullptr;
        }
    }

#undef CASE_RETURN_STRING
}

void onAppCmd(android_app *app, int32_t cmd)
{
    switch (cmd)
    {
    case APP_CMD_INIT_WINDOW:
        if (app->window != nullptr)
        {
            FASTCG_LOG_VERBOSE(AndroidApplication, "Window Initialized");

            // obtain the actual size of the window
            FastCG::AndroidApplication::GetInstance()->WindowResizeCallback(
                (uint32_t)ANativeWindow_getWidth(app->window), (uint32_t)ANativeWindow_getHeight(app->window));

            FastCG::AndroidApplication::GetInstance()->OnPostWindowInitialize(app->window);
        }
        break;
    case APP_CMD_TERM_WINDOW:
        FASTCG_LOG_VERBOSE(AndroidApplication, "Window Terminated");
        FastCG::AndroidApplication::GetInstance()->OnPreWindowTerminate(app->window);
        break;
    case APP_CMD_WINDOW_RESIZED:
        // obtain the actual size of the window
        FastCG::AndroidApplication::GetInstance()->WindowResizeCallback((uint32_t)ANativeWindow_getWidth(app->window),
                                                                        (uint32_t)ANativeWindow_getHeight(app->window));
        break;
    case APP_CMD_WINDOW_REDRAW_NEEDED:
        // not implemented
        break;
    case APP_CMD_CONTENT_RECT_CHANGED:
        // not implemented
        break;
    case APP_CMD_GAINED_FOCUS:
        // not implemented
        break;
    case APP_CMD_LOST_FOCUS:
        // not implemented
        break;
    case APP_CMD_CONFIG_CHANGED:
        // not implemented
        break;
    case APP_CMD_LOW_MEMORY:
        // not implemented
        break;
    case APP_CMD_START:
        // not implemented
        break;
    case APP_CMD_RESUME:
        FASTCG_LOG_VERBOSE(AndroidApplication, "App Resumed");
        FastCG::AndroidApplication::GetInstance()->SetPaused(false);
        break;
    case APP_CMD_SAVE_STATE:
        // not implemented
        break;
    case APP_CMD_PAUSE:
        FASTCG_LOG_VERBOSE(AndroidApplication, "App Paused");
        FastCG::AndroidApplication::GetInstance()->SetPaused(true);
        break;
    case APP_CMD_STOP:
        // not implemented
        break;
    case APP_CMD_DESTROY:
        // not implemented
        break;
    }
}

int32_t onInputEvent(android_app *app, AInputEvent *event)
{
    int32_t eventType = AInputEvent_getType(event);
    switch (eventType)
    {
    case AINPUT_EVENT_TYPE_MOTION: {
        int32_t action = AMotionEvent_getAction(event);
        switch (action)
        {
        case AMOTION_EVENT_ACTION_DOWN:
        case AMOTION_EVENT_ACTION_UP: {
            auto state = action == AMOTION_EVENT_ACTION_DOWN ? FastCG::MouseButtonState::PRESSED
                                                             : FastCG::MouseButtonState::RELEASED;
            // press both buttons at the same time
            FastCG::AndroidApplication::GetInstance()->MouseButtonCallback(FastCG::MouseButton::LEFT_BUTTON, state);
            FastCG::AndroidApplication::GetInstance()->MouseButtonCallback(FastCG::MouseButton::RIGHT_BUTTON, state);
        }
        case AMOTION_EVENT_ACTION_MOVE: {
            // pointer motion tracking stops when action up occurs,
            // so we also update the pointer position whenever there's a pointer up/down event

            float x0 = AMotionEvent_getX(event, 0), y0 = AMotionEvent_getY(event, 0);
            if (x0 > 0 && y0 > 0) // ignore out-of-window motions
            {
                FastCG::AndroidApplication::GetInstance()->MouseMoveCallback((uint32_t)x0, (uint32_t)y0);
            }
        }
        break;
        case AMOTION_EVENT_ACTION_CANCEL:
            // not implemented
            break;
        case AMOTION_EVENT_ACTION_OUTSIDE:
            // not implemented
            break;
        case AMOTION_EVENT_ACTION_POINTER_DOWN:
        case AMOTION_EVENT_ACTION_POINTER_UP:
            // TODO: support multiple pointers
            break;
        }

        return 1;
    }
    case AINPUT_EVENT_TYPE_KEY: {
        int32_t keyCode = AKeyEvent_getKeyCode(event);
        int32_t action = AKeyEvent_getAction(event);
        bool pressed = action == AKEY_EVENT_ACTION_DOWN; // AKEY_EVENT_ACTION_MULTIPLE is considered an "unpress" action
        FastCG::Key key;
        if (TranslateToKey(keyCode, key))
        {
            FastCG::AndroidApplication::GetInstance()->KeyboardCallback(key, pressed);
        }
        return 1;
    }
    case AINPUT_EVENT_TYPE_FOCUS:
        // not implemented
        break;
    }
    return 0;
}

namespace FastCG
{
    void AndroidApplication::SetAndroidApp(android_app *pAndroidApp)
    {
        assert(pAndroidApp != nullptr);

        // FIXME: that call shouldn't be necessary anymore
        // https://github.com/android/ndk/issues/381
        FASTCG_COMPILER_WARN_PUSH
        FASTCG_COMPILER_WARN_IGNORE_DEPRECATED_DECLARATIONS
        app_dummy();
        FASTCG_COMPILER_WARN_POP

        mAndroidApp = pAndroidApp;

        mAndroidApp->onAppCmd = ::onAppCmd;
        mAndroidApp->onInputEvent = ::onInputEvent;
    }

    void AndroidApplication::OnPostWindowInitialize(ANativeWindow *pWindow)
    {
        if (!mSettings.headless)
        {
            GraphicsSystem::GetInstance()->OnPostWindowInitialize(pWindow);
        }
    }

    void AndroidApplication::OnPreWindowTerminate(ANativeWindow *pWindow)
    {
        if (!mSettings.headless)
        {
            GraphicsSystem::GetInstance()->OnPreWindowTerminate(pWindow);
        }
    }

    void AndroidApplication::RunMainLoop()
    {
        int events;
        android_poll_source *source;

        while (mRunning)
        {
            auto osStart = Timer::GetTime();

            while (ALooper_pollAll(0, nullptr, &events, (void **)&source) >= 0)
            {
                if (source != nullptr)
                {
                    source->process(mAndroidApp, source);
                }

                if (mAndroidApp->destroyRequested != 0)
                {
                    mRunning = false;
                    goto __exitMainLoop;
                }
            }

            RunMainLoopIteration(Timer::GetTime() - osStart);
        }
    __exitMainLoop:
        return;
    }

    void AndroidApplication::OnPreInitialize()
    {
        BaseApplication::OnPreInitialize();

        if (mAndroidApp->activity->vm->AttachCurrentThread(&mJniEnv, NULL) != JNI_OK)
        {
            FASTCG_THROW_EXCEPTION(Exception, "Couldn't obtain the JNI environment for the current thread");
        }
    }

    void AndroidApplication::OnPostFinalize()
    {
        // since ending the current thread doesn't make the Android application to end,
        // we need to call the main activity's finish() method explicitly

        jobject activity = mAndroidApp->activity->clazz;
        jclass activityClass = mJniEnv->GetObjectClass(activity);

        jmethodID safeFinishMethod = mJniEnv->GetMethodID(activityClass, "finish", "()V");
        mJniEnv->CallVoidMethod(activity, safeFinishMethod);

        mAndroidApp->activity->vm->DetachCurrentThread();

        BaseApplication::OnPostFinalize();
    }

}

#endif