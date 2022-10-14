#ifndef FASTCG_WINDOWS_APPLICATION_H
#define FASTCG_WINDOWS_APPLICATION_H

#include <FastCG/FastCG.h>

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

        inline HDC GetDeviceContextHandle() const
        {
            return mHDC;
        }

        friend LRESULT WndProc(HWND, UINT, WPARAM, LPARAM);

    protected:
        void RunMainLoop() override;
        void InitializePresentation() override;
        void FinalizePresentation() override;
        void Present() override;

    private:
        HINSTANCE mHInstance{0};
        HWND mHWnd{0};
        HDC mHDC{0};
    };

}

#endif

#endif