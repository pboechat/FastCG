#ifdef FASTCG_ANDROID

#include <FastCG/Platform/Timer.h>
#include <FastCG/Platform/Android/AndroidApplication.h>
#include <FastCG/Graphics/GraphicsSystem.h>
#include <FastCG/Core/Log.h>
#include <FastCG/Core/Exception.h>

#include <android/native_window.h>

#include <cassert>

namespace
{
#define KEY(a, b)                    \
    {                                \
        (uint64_t) b, FastCG::Key::a \
    }

    std::unordered_map<uint64_t, FastCG::Key> gKeyLut = {
        KEY(BACKSPACE, AKEYCODE_BACK),
        KEY(RETURN, AKEYCODE_ENTER),
        KEY(ESCAPE, AKEYCODE_ESCAPE),
        KEY(SPACE, AKEYCODE_SPACE),
        KEY(ASTERISK, AKEYCODE_STAR),
        KEY(PLUS, AKEYCODE_PLUS),
        KEY(COMMA, AKEYCODE_COMMA),
        KEY(MINUS, AKEYCODE_MINUS),
        KEY(DOT, AKEYCODE_PERIOD),
        KEY(SLASH, AKEYCODE_SLASH),
        KEY(NUMBER_0, AKEYCODE_NUMPAD_0),
        KEY(NUMBER_1, AKEYCODE_NUMPAD_1),
        KEY(NUMBER_2, AKEYCODE_NUMPAD_2),
        KEY(NUMBER_3, AKEYCODE_NUMPAD_3),
        KEY(NUMBER_4, AKEYCODE_NUMPAD_4),
        KEY(NUMBER_5, AKEYCODE_NUMPAD_5),
        KEY(NUMBER_6, AKEYCODE_NUMPAD_6),
        KEY(NUMBER_7, AKEYCODE_NUMPAD_7),
        KEY(NUMBER_8, AKEYCODE_NUMPAD_8),
        KEY(NUMBER_9, AKEYCODE_NUMPAD_9),
        // KEY(COLON, AKEYCODE_UNKNOWN),
        KEY(SEMI_COLON, AKEYCODE_SEMICOLON),
        KEY(EQUALS, AKEYCODE_EQUALS),
        KEY(LEFT_ARROW, AKEYCODE_DPAD_LEFT),
        KEY(UP_ARROW, AKEYCODE_DPAD_UP),
        KEY(RIGHT_ARROW, AKEYCODE_DPAD_RIGHT),
        KEY(DOWN_ARROW, AKEYCODE_DPAD_DOWN),
        KEY(F1, AKEYCODE_F1),
        KEY(F2, AKEYCODE_F2),
        KEY(F3, AKEYCODE_F3),
        KEY(F4, AKEYCODE_F4),
        KEY(F5, AKEYCODE_F5),
        KEY(F6, AKEYCODE_F6),
        KEY(F7, AKEYCODE_F7),
        KEY(F8, AKEYCODE_F8),
        KEY(F9, AKEYCODE_F9),
        KEY(F10, AKEYCODE_F10),
        KEY(F11, AKEYCODE_F11),
        KEY(F12, AKEYCODE_F12),
        KEY(PAGE_UP, AKEYCODE_PAGE_UP),
        KEY(PAGE_DOWN, AKEYCODE_PAGE_DOWN),
        KEY(END, AKEYCODE_MOVE_END),
        KEY(HOME, AKEYCODE_MOVE_HOME),
        KEY(INSERT, AKEYCODE_INSERT),
        KEY(SHIFT, AKEYCODE_SHIFT_LEFT),
        KEY(CONTROL, AKEYCODE_CTRL_LEFT),
        KEY(OPEN_SQUARE_BRACKET, AKEYCODE_LEFT_BRACKET),
        KEY(BACKSLASH, AKEYCODE_BACKSLASH),
        KEY(CLOSE_SQUARE_BRACKET, AKEYCODE_RIGHT_BRACKET),
        KEY(LETTER_A, AKEYCODE_A),
        KEY(LETTER_B, AKEYCODE_B),
        KEY(LETTER_C, AKEYCODE_C),
        KEY(LETTER_D, AKEYCODE_D),
        KEY(LETTER_E, AKEYCODE_E),
        KEY(LETTER_F, AKEYCODE_F),
        KEY(LETTER_G, AKEYCODE_G),
        KEY(LETTER_H, AKEYCODE_H),
        KEY(LETTER_I, AKEYCODE_I),
        KEY(LETTER_J, AKEYCODE_J),
        KEY(LETTER_K, AKEYCODE_K),
        KEY(LETTER_L, AKEYCODE_L),
        KEY(LETTER_M, AKEYCODE_M),
        KEY(LETTER_N, AKEYCODE_N),
        KEY(LETTER_O, AKEYCODE_O),
        KEY(LETTER_P, AKEYCODE_P),
        KEY(LETTER_Q, AKEYCODE_Q),
        KEY(LETTER_R, AKEYCODE_R),
        KEY(LETTER_S, AKEYCODE_S),
        KEY(LETTER_T, AKEYCODE_T),
        KEY(LETTER_U, AKEYCODE_U),
        KEY(LETTER_V, AKEYCODE_V),
        KEY(LETTER_W, AKEYCODE_W),
        KEY(LETTER_X, AKEYCODE_X),
        KEY(LETTER_Y, AKEYCODE_Y),
        KEY(LETTER_Z, AKEYCODE_Z),
        // KEY(TILDE, AKEYCODE_UNKNOWN),
        KEY(DEL, AKEYCODE_DEL),
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
                (uint32_t)ANativeWindow_getWidth(app->window),
                (uint32_t)ANativeWindow_getHeight(app->window));

            FastCG::GraphicsSystem::GetInstance()->OnWindowInitialized();
        }
        break;
    case APP_CMD_TERM_WINDOW:
        FASTCG_LOG_VERBOSE(AndroidApplication, "Window Terminated");

        FastCG::GraphicsSystem::GetInstance()->OnWindowTerminated();
        break;
    case APP_CMD_WINDOW_RESIZED:
        // obtain the actual size of the window
        FastCG::AndroidApplication::GetInstance()->WindowResizeCallback(
            (uint32_t)ANativeWindow_getWidth(app->window),
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
    case AINPUT_EVENT_TYPE_MOTION:
    {
        size_t pointerCount = AMotionEvent_getPointerCount(event);
        // TODO: support multiple pointers
        assert(pointerCount > 0);

        float x = AMotionEvent_getX(event, 0);
        float y = AMotionEvent_getY(event, 0);
        assert(x > 0 && y > 0);
        int32_t action = AMotionEvent_getAction(event);
        switch (action)
        {
        case AMOTION_EVENT_ACTION_DOWN:
        case AMOTION_EVENT_ACTION_UP:
            FastCG::AndroidApplication::GetInstance()->MouseButtonCallback(FastCG::MouseButton::RIGHT_BUTTON,
                                                                           action == AMOTION_EVENT_ACTION_DOWN ? FastCG::MouseButtonState::PRESSED : FastCG::MouseButtonState::RELEASED);
            break;
        case AMOTION_EVENT_ACTION_MOVE:
            FastCG::AndroidApplication::GetInstance()->MouseMoveCallback((uint32_t)x, (uint32_t)y);
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
    case AINPUT_EVENT_TYPE_KEY:
    {
        int32_t keyCode = AKeyEvent_getKeyCode(event);
        int32_t action = AKeyEvent_getAction(event);
        bool pressed = action == AKEY_EVENT_ACTION_DOWN; // AKEY_EVENT_ACTION_MULTIPLE is considered an "unpress" action
        FastCG::AndroidApplication::GetInstance()->KeyboardCallback(TranslateKey(keyCode), pressed);
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
    void AndroidApplication::SetAndroidApp(android_app *androidApp)
    {
        assert(androidApp != nullptr);

        // FIXME: that call shouldn't be necessary anymore
        // https://github.com/android/ndk/issues/381
        FASTCG_COMPILER_WARN_PUSH
        FASTCG_COMPILER_WARN_IGNORE_DEPRECATED_DECLARATIONS
        app_dummy();
        FASTCG_COMPILER_WARN_POP

        mAndroidApp = androidApp;

        mAndroidApp->onAppCmd = ::onAppCmd;
        mAndroidApp->onInputEvent = ::onInputEvent;
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
                    goto __exit;
                }
            }

            RunMainLoopIteration(Timer::GetTime() - osStart);
        }
    __exit:
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

    uint64_t AndroidApplication::GetNativeKey(Key key) const
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