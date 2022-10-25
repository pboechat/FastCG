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

        HDC GetDeviceContext();
        HWND GetWindow();
        void DestroyDeviceContext();

        friend LRESULT WndProc(HWND, UINT, WPARAM, LPARAM);

    protected:
        void RunMainLoop() override;

    private:
        HINSTANCE mHInstance{0};
        HWND mHWnd{0};
        HDC mHDC{0};

        void CreateAndSetupDeviceContext();
        void CreateWindow();
    };

}

#endif

#endif