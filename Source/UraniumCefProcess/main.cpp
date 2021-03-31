//struct IUnknown; // Workaround for "combaseapi.h(229): error C2187

#include "UraniumCefApp.h"
#include <filesystem>
#include <Windows.h>
#include <winternl.h>

namespace CefModules
{
    HMODULE CEF3DLLHandle = nullptr;
    HMODULE ElfHandle = nullptr;
    HMODULE D3DHandle = nullptr;
    HMODULE GLESHandle = nullptr;
    HMODULE EGLHandle = nullptr;

    HMODULE LoadDllCEF(const std::filesystem::path path)
    {
        if (path.empty() || path.extension() != L".dll")
        {
            return nullptr;
        }
        HMODULE handle = LoadLibraryW(path.c_str());
        if (!handle)
        {
            //DWORD ErrorNum = GetLastError();
        }
        return handle;
    }

    void LoadCefModules()
    {
        auto currdir = std::filesystem::current_path();
        CEF3DLLHandle = LoadDllCEF(currdir / L"libcef.dll");
        ElfHandle = LoadDllCEF(currdir / L"chrome_elf.dll");
        D3DHandle = LoadDllCEF(currdir / L"d3dcompiler_47.dll");
        GLESHandle = LoadDllCEF(currdir / L"libGLESv2.dll");
        EGLHandle = LoadDllCEF(currdir / L"libEGL.dll");
    }

    void UnloadCefModules()
    {
        FreeLibrary(CEF3DLLHandle); CEF3DLLHandle = nullptr;
        FreeLibrary(ElfHandle); ElfHandle = nullptr;
        FreeLibrary(D3DHandle); D3DHandle = nullptr;
        FreeLibrary(GLESHandle); GLESHandle = nullptr;
        FreeLibrary(EGLHandle); EGLHandle = nullptr;
    }
}

int WINAPI WinMain( _In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd )
{
    CefModules::LoadCefModules();

    CefEnableHighDPISupport();
    CefMainArgs MainArgs(hInstance);

    CefRefPtr<CefApp> App(new UraniumCefApp);

    //SetGpuPriority();
    //SetPriorityClass(GetCurrentProcess(), ABOVE_NORMAL_PRIORITY_CLASS);

    int res = CefExecuteProcess(MainArgs, App, nullptr);
    CefModules::UnloadCefModules();
    return res;
}