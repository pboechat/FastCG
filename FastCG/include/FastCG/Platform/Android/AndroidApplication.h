#ifndef FASTCG_ANDROID_APPLICATION_H
#define FASTCG_ANDROID_APPLICATION_H

#ifdef FASTCG_ANDROID

#include <FastCG/Platform/BaseApplication.h>

#include <android_native_app_glue.h>

void onAppCmd(android_app *, int32_t);
int32_t onInputEvent(android_app *, AInputEvent *);

namespace FastCG
{
    class AndroidApplication : public BaseApplication
    {
    public:
        AndroidApplication(const ApplicationSettings &applicationSettings = {}) : BaseApplication(applicationSettings)
        {
        }

        inline static AndroidApplication *GetInstance()
        {
            return static_cast<AndroidApplication *>(BaseApplication::GetInstance());
        }
        inline ANativeWindow *GetWindow()
        {
            return mAndroidApp->window;
        }
        inline const char *GetInternalDataPath() const
        {
            return mAndroidApp->activity->internalDataPath;
        }
        inline JNIEnv *GetJniEnv()
        {
            return mJniEnv;
        }
        inline bool IsPaused() const
        {
            return mPaused;
        }

    protected:
        void RunMainLoop() override;

    private:
        android_app *mAndroidApp{nullptr};
        JNIEnv *mJniEnv{nullptr};
        bool mPaused{false};

        void SetJNIEnv(JNIEnv *pJniEnv);
        void SetAndroidApp(android_app *pAndroidApp);
        void OnPostWindowInitialize(ANativeWindow *pWindow);
        void OnPreWindowTerminate(ANativeWindow *pWindow);

        inline void SetPaused(bool paused)
        {
            mPaused = paused;
        }

        friend void ::onAppCmd(android_app *, int32_t);
        friend int32_t(::onInputEvent)(android_app *, AInputEvent *);
        friend void ::android_main(android_app *);
    };
}

#endif

#endif