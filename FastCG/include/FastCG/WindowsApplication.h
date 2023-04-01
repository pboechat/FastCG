#ifndef FASTCG_WINDOWS_APPLICATION_H
#define FASTCG_WINDOWS_APPLICATION_H

#ifdef FASTCG_WINDOWS

#include <FastCG/BaseApplication.h>

#include <Windows.h>

namespace FastCG
{
    class WindowsApplication : public BaseApplication
    {
    public:
        WindowsApplication(const ApplicationSettings &applicationSettings) : BaseApplication(applicationSettings) {}

        inline static WindowsApplication *GetInstance()
        {
            return static_cast<WindowsApplication *>(BaseApplication::GetInstance());
        }
        inline HWND GetWindow()
        {
            return mHWnd;
        }
        uint64_t GetNativeKey(Key key) const override;

        friend LRESULT WndProc(HWND, UINT, WPARAM, LPARAM);

    protected:
        void OnPreInitialize() override;
        void OnPostFinalize() override;
        void RunMainLoop() override;

    private:
        HINSTANCE mHInstance{0};
        HWND mHWnd{0};
    };

}

#endif

#endif