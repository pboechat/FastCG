#ifndef FASTCG_WINDOWS_APPLICATION_H
#define FASTCG_WINDOWS_APPLICATION_H

#ifdef FASTCG_WINDOWS

#include <FastCG/Platform/BaseApplication.h>

#define NOMINMAX
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

        friend LRESULT(::WndProc)(HWND, UINT, WPARAM, LPARAM);

    protected:
        void OnPostInitialize() override;
        void OnPostFinalize() override;
        void RunMainLoop() override;

    private:
        HWND mHWnd{nullptr};

        void InitializeWindow();
        void TerminateWindow();
        void RunWindowedMainLoop();
        void RunConsoleMainLoop();
    };

}

#endif

#endif