
#include <windows.h>
#include <stdlib.h>
#include <strsafe.h>
#include "include/MinHook.h"
#include <stdbool.h>
#include <GLFW/glfw3.h>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_opengl3.h>
#include <imgui/imgui_impl_glfw.h>
#include <winuser.h>


FARPROC wglSwapBuffers = NULL;
HMODULE m_opengl_dll;
int contextCreated = 0;
HGLRC myContext;
HGLRC gameContext; 
typedef int (WINAPI* PFNWGLSWAPBUFFERS)(HDC);
PFNWGLSWAPBUFFERS pfnOrigWglSwapBuffers;
static GLFWwindow* window;
static ImGuiIO io;
static ImGuiContext* imgui_context;
const char* glsl_version = "#version 130";



DWORD WINAPI init_context()
{
	
    GLFWwindow* currentContext = glfwGetCurrentContext();

    if (!currentContext)
    {
        OutputDebugString(L"No current (GLFW) context.\n");
        return FALSE;
    }
    window = currentContext;

    ImGui::SetCurrentContext(ImGui::CreateContext());
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer bindings
	
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    return TRUE;
}

long wgl_swap_buffers(_In_ HDC hdc) {


    OutputDebugString(L"wglSwapHook Called\n");

    if (window)
    {
		
       ImGui_ImplOpenGL3_NewFrame();
       ImGui_ImplGlfw_NewFrame();
       ImGui::NewFrame();

        bool show = true;
        ImGui::ShowDemoWindow(&show);
		
        // Rendering
        ImGui::Render();
        OutputDebugString(L"[Controller] Called ImGui::Render.\n");
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }
    return pfnOrigWglSwapBuffers(hdc);

}

void hooking(FARPROC wglSwapBuffers, HMODULE m_opengl_dll)
{
    MH_Initialize();
    OutputDebugString(L"Init Hook\n");

	MH_CreateHook((LPVOID)wglSwapBuffers, wgl_swap_buffers, (LPVOID*)&pfnOrigWglSwapBuffers); 
    OutputDebugString(L"Created Hook\n");

    MH_EnableHook(wglSwapBuffers);
    OutputDebugString(L"Enabled Hook\n");

    if (!init_context())
    {
        OutputDebugString(L"Failed to initialize context - no point in hooking...\n");
        return;
    }
}

void initialise(void)
{
    OutputDebugString(L"[Controller] Initialise called.\n");

    m_opengl_dll = GetModuleHandleA("opengl32.dll");

    while (!m_opengl_dll)
    {
        Sleep(500);
        m_opengl_dll = GetModuleHandle(L"opengl32.dll");
    }

    OutputDebugString(L"Found openGl\n");

    while (!(wglSwapBuffers))
    {
        wglSwapBuffers = GetProcAddress(m_opengl_dll, "wglSwapBuffers");
        Sleep(500);
    }

    OutputDebugString(L"Found wglswapbuffers\n");
    //OutputDebugString(mc.wgl_swap_buffers + "\n");

    hooking(wglSwapBuffers, m_opengl_dll);

}

VOID CALLBACK timer_callback(HWND hwnd, UINT uMsg, UINT idEvent, DWORD dwTime)
{
    OutputDebugString(L"Timer Callback");
    KillTimer(hwnd, idEvent);
    initialise();
    
}

void setuptimer()
{
    HWND windowHwnd = FindWindowA(NULL, "Simple example");
    UINT_PTR timerId = SetTimer(windowHwnd, 42, 1000, (TIMERPROC)timer_callback);

    if (timerId == 0)
    {
        OutputDebugString((LPCWSTR)GetLastError());
        OutputDebugString(L"[Controller] Failed to create timer.\n");
    }
    else
    {
        OutputDebugString(L"[Controller] Created timer.\n");
    }
}


BOOL WINAPI DllMain(
    HINSTANCE hinstDLL,  // handle to DLL module
    DWORD fdwReason,     // reason for calling function
    LPVOID lpvReserved)  // reserved
{
    DisableThreadLibraryCalls(hinstDLL);
    switch (fdwReason)
    {
    case DLL_PROCESS_ATTACH:
    {
		OutputDebugString(L"[Controller] DLL_PROCESS_ATTACH called.\n");
        CloseHandle(CreateThread(0, 0, (LPTHREAD_START_ROUTINE)setuptimer, 0, 0, 0));
        break;
    }
    default: {

    } break;
    }
    return 1;
}

