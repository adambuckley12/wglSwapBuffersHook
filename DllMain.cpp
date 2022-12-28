#include <windows.h>
#include <stdlib.h>
#include <strsafe.h>
#include "include/MinHook.h"
#include <stdbool.h>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_opengl3.h>
#include <imgui/imgui_impl_win32.h>
#include "imgui/imgui_internal.h"
#include <winuser.h>
#include <GLFW/glfw3.h>  
#include <gl/GL.h>
#include "DLLMain.h"

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

FARPROC wglSwapBuffers = NULL;
HMODULE m_opengl_dll;
int contextCreated = 0; // for gl render 
HGLRC myContext; //For gl render
HGLRC gameContext; // for gl render
typedef int (WINAPI* PFNWGLSWAPBUFFERS)(HDC);
PFNWGLSWAPBUFFERS pfnOrigWglSwapBuffers;
static GLFWwindow* window;
static ImGuiIO io;
const char* glsl_version = "#version 130";
HWND windowHwnd;
HGLRC   g_GLRenderContext;
HDC     g_HDCDeviceContext;
HWND game_window = NULL;
bool showGUI = true;
ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
//CHEAT BOOLS

bool enable_esp = false;
bool enable_leftClicker = false;
bool enable_rightClicker = false;
bool enable_velocity = false;
bool enable_aimAssit = false;


const int width = 500;
const int height = 400;
//


DWORD WINAPI init_context()
{
    //Prepare OpenGlContext
    game_window = windowHwnd;
    
    OutputDebugString(L"Creating Context\n");
    // Setup Dear ImGui binding
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    io = ImGui::GetIO(); (void)io;
   
    ImGui::StyleColorsDark();
	
    // Setup Platform/Renderer bindings
    ImGui_ImplWin32_Init(game_window);
    ImGui_ImplOpenGL3_Init(glsl_version);
	
    OutputDebugString(L"initted\n");
    return TRUE;
	
}

void guiStyle()
{
    //colors
    ImVec4(1.00f, 1.00f, 1.00f, 0.15f);
    const auto style = &ImGui::GetStyle();
    const auto colors = style->Colors;

    style->WindowBorderSize = 1.f;
    style->WindowRounding = 0.f;
    style->WindowTitleAlign = ImVec2(0.5, 0.5);
    style->FrameRounding = 0.f;
    style->GrabRounding = 0.f;
    style->ChildRounding = 0.f;
    style->ItemSpacing = ImVec2(5, 10);
    style->FramePadding = ImVec2(1, 1);
    style->PopupRounding = 3.f;

    //colors[ImGuiCol_Text] = ImColor(133, 130, 132);
    //colors[ImGuiCol_TextDisabled] = ImVec4(1.00f, 1.00f, 1.00f, 0.25f);
    //colors[ImGuiCol_WindowBg] = ImColor(28, 28, 28);
    //colors[ImGuiCol_ChildBg] = ImVec4(0.05f, 0.05f, 0.05f, 1.00f);
    //colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 1.00f);

    colors[ImGuiCol_Border] = ImColor(51, 51, 51);
    colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);

    colors[ImGuiCol_TitleBg] = ImVec4(0.09f, 0.09f, 0.09f, 1.00f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.09f, 0.09f, 0.09f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.09f, 0.09f, 0.09f, 1.00f);

    colors[ImGuiCol_MenuBarBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.00f);
    colors[ImGuiCol_ScrollbarBg] = ImVec4(0.15f, 0.15f, 0.15f, 0.00f);
    colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.10f, 0.10f, 0.10f, 0.00f);
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.10f, 0.10f, 0.10f, 0.00f);
    colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.10f, 0.10f, 0.15f, 0.00f);
    colors[ImGuiCol_CheckMark] = ImVec4(v::gui::color1);

    colors[ImGuiCol_SliderGrab] = ImVec4(v::gui::color1); //used for sliders
    colors[ImGuiCol_SliderGrabActive] = ImVec4(v::gui::color1); //used for sliders

    colors[ImGuiCol_Button] = ImColor(20, 20, 20);
    colors[ImGuiCol_ButtonHovered] = ImColor(20, 20, 20);
    colors[ImGuiCol_ButtonActive] = ImColor(20, 20, 20);

    colors[ImGuiCol_Header] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.13f, 0.13f, 0.13f, 1.00f);
    colors[ImGuiCol_HeaderActive] = ImVec4(0.12f, 0.12f, 0.12f, 1.00f);

    colors[ImGuiCol_Separator] = ImVec4(v::gui::color3);
    colors[ImGuiCol_SeparatorHovered] = ImVec4(v::gui::color3);
    colors[ImGuiCol_SeparatorActive] = ImVec4(v::gui::color3);

    colors[ImGuiCol_ResizeGrip] = ImVec4(0.30f, 0.30f, 0.30f, 1.00f);
    colors[ImGuiCol_ResizeGripActive] = ImVec4(0.09f, 0.09f, 0.09f, 1.00f);
    colors[ImGuiCol_PlotLines] = ImVec4(0.40f, 0.12f, 0.99f, 0.90f);
    colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.40f, 0.12f, 0.99f, 0.90f);
    colors[ImGuiCol_PlotHistogram] = ImVec4(0.40f, 0.12f, 0.99f, 0.90f);
    colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.40f, 0.12f, 0.99f, 0.90f);
    colors[ImGuiCol_TextSelectedBg] = ImVec4(0.09f, 0.09f, 0.09f, 1.00f);

    colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.00f, 0.00f, 0.00f, 1.05f);
    colors[ImGuiCol_DragDropTarget] = ImVec4(0.00f, 0.00f, 0.00f, 0.40f);

    colors[ImGuiCol_WindowBg] = ImColor(28, 28, 28);
    colors[ImGuiCol_TitleBgActive] = ImColor(20, 20, 20);
    colors[ImGuiCol_FrameBg] = ImColor(45, 45, 45);
    colors[ImGuiCol_FrameBgHovered] = ImColor(45, 45, 45);
    colors[ImGuiCol_FrameBgActive] = ImColor(45, 45, 45);
    colors[ImGuiCol_ChildBg] = ImColor(23, 23, 23);
    //

        static bool testBool = false;
        static float leftClickerCPS = 12.0f;

        static float rightClickerCPS = 12.0f;
        static float blockHitChance = 50.0f;
        static int counter = 0;
        static float color[5];
        static float color2[5];
		
        ImGui::Begin(("##main"), NULL,
            ImGuiWindowFlags_NoCollapse | 
            ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoScrollbar);

        ImGui::Text("Tasty Client V0.1.");   
        ImGui::BeginChild("LeftClickerChild", ImVec2(200, 300), true);// Display some text (you can use a format strings too)
        ImGui::Checkbox("Left Clicker", &enable_leftClicker);      // Edit bools storing our window open/close state
        ImGui::SliderFloat("CPS", &leftClickerCPS, 5.f, 20.f, "%.1f");
        ImGui::Checkbox("Inventory", &enable_leftClicker);
        ImGui::Checkbox("Block Hit", &enable_leftClicker);
        ImGui::SliderFloat("Chance", &blockHitChance, 0, 100, "%1.0f");
		ImGui::EndChild();
        ImGui::SameLine();
		
        ImGui::BeginChild("RightClickerChild", ImVec2(200, 300), true);
        ImGui::Checkbox("Right Clicker", &testBool);
        ImGui::SliderFloat("CPS", &rightClickerCPS, 5.f, 20.f, "%.1f");
        ImGui::Checkbox("Blocks Only", &testBool);
		ImGui::Text("Start Delay (ms)");
        ImGui::SliderFloat("Delay", &rightClickerCPS, 0, 2000, "5.0f");
        ImGui::EndChild();
        ImGui::SameLine();
		
        ImGui::BeginChild("ESPChild", ImVec2(200, 300), true);
        ImGui::Checkbox("ESP", &enable_esp);
        ImGui::Text("ESP Color:");
		ImGui::ColorPicker3("1", color);
        ImGui::Checkbox("Tracers", &enable_esp);
        ImGui::Text("Tracer Color Color:");
        ImGui::ColorPicker3("2", color2);
        ImGui::EndChild();

        ImGui::End();
}

long wgl_swap_buffers(_In_ HDC hdc)
{
    OutputDebugString(L"wglSwapHook Called\n");
	if (contextCreated == 0) {
		init_context();
        contextCreated = 1;

	}
	
    if (GetAsyncKeyState(VK_RSHIFT))
    {
        OutputDebugString(L"Right Shift Key Pressed\n");
		Sleep(100);
        showGUI = !showGUI;
    }
    
    if (showGUI)
    {
        if (game_window)
        {
            // Poll and handle messages (inputs, window resize, etc.)
            // See the WndProc() function below for our to dispatch events to the Win32 backend.
            MSG msg;
            while (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
            {
                OutputDebugString(L"PeekMessage ran\n");
                ImGui_ImplWin32_WndProcHandler(msg.hwnd, msg.message, msg.wParam, msg.lParam);
            }

            OutputDebugString(L"New Frame Called");
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplWin32_NewFrame();
            ImGui::NewFrame();

			//GUI STYLE:
            guiStyle();

            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        }
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
    hooking(wglSwapBuffers, m_opengl_dll);

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
        windowHwnd = FindWindowA(NULL, "Simple example");
        if (windowHwnd == NULL)
        {
            windowHwnd = FindWindowA(NULL, "Minecraft 1.8.9");
            if (windowHwnd == NULL)
            {
                windowHwnd = FindWindowA(NULL, "Lunar Client (1.8.9-93d4dd4/master)");
            }
        }
		OutputDebugString(L"[Controller] DLL_PROCESS_ATTACH called.\n");
        CloseHandle(CreateThread(0, 0, (LPTHREAD_START_ROUTINE)initialise, 0, 0, 0));
        break;
    }
    default: {

    } break;
    }
    return 1;
}

/*VOID CALLBACK timer_callback(HWND hwnd, UINT uMsg, UINT idEvent, DWORD dwTime)
{
    OutputDebugString(L"Timer Callback");
    KillTimer(hwnd, idEvent);
    initialise();
    
}

void setuptimer()
{
    
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
}*/