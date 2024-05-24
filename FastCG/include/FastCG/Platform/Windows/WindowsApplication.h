#ifndef FASTCG_WINDOWS_APPLICATION_H
#define FASTCG_WINDOWS_APPLICATION_H

#ifdef FASTCG_WINDOWS

#include <FastCG/Platform/BaseApplication.h>

#include <Windows.h>

LRESULT WndProc(HWND, UINT, WPARAM, LPARAM);

namespace FastCG
{
    class WindowsApplication : public BaseApplication
    {
    public:
        WindowsApplication(const ApplicationSettings &applicationSettings = {}) : BaseApplication(applicationSettings)
        {
        }

        inline static WindowsApplication *GetInstance()
        {
            return static_cast<WindowsApplication *>(BaseApplication::GetInstance());
        }
        inline HINSTANCE GetModule() const
        {
            return mInstance;
        }
        inline HWND GetWindow() const
        {
            return mHWnd;
        }

        friend LRESULT(::WndProc)(HWND, UINT, WPARAM, LPARAM);

    protected:
        void OnPreInitialize() override;
        void OnPostFinalize() override;
        void RunMainLoop() override;

    private:
        HINSTANCE mInstance{0};
        HWND mHWnd{0};
    };

}

#endif

#endif