
#include <windows.h>
#include <stdlib.h>
#include <strsafe.h>
#include "include/MinHook.h"
#include <gl/GL.h>
#include <stdbool.h>
//#include <gl/GL.h>
//#include <gl/GLU.h>
#include <GLFW/glfw3.h>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

FARPROC wglSwapBuffers = NULL;
HMODULE m_opengl_dll;
int contextCreated = 0;
HGLRC myContext;
HGLRC gameContext;
void initialise(void); 
int unload = 0;
typedef int (WINAPI* PFNWGLSWAPBUFFERS)(HDC);
PFNWGLSWAPBUFFERS pfnOrigWglSwapBuffers;

static GLFWwindow* window;
static ImGuiContext* imgui_context;
const char* glsl_version = "#version 130";

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

bool init_window()
{

    // Setup window
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return 1;


    // GL 3.0 + GLSL 130
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    // Create window with graphics context
    window = glfwCreateWindow(1280, 720, "Dear ImGui GLFW+OpenGL3 example", NULL, NULL);
    if (window == NULL)
        return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    return true;
}

bool init_imgui()
{
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    imgui_context = ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    return true;
}

long wgl_swap_buffers(_In_ HDC hdc) {


    OutputDebugString(L"wglSwapHook Called\n");


    if (window == nullptr)
    {
        init_window();
    }

    if (glfwWindowShouldClose(window))
    {
        return 0;
    }

    if (imgui_context == nullptr)
    {
        init_imgui();
    }


    // Poll and handle events (inputs, window resize, etc.)
    // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
    // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
    // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
    // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
    glfwPollEvents();

    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    bool show = true;

    ImGui::ShowDemoWindow(&show);

    /*
    gameContext = wglGetCurrentContext();

    if (contextCreated == 0)
    {

        //Create new contextpMC
        myContext = wglCreateContext(hdc);

        //Make thread use our context
        wglMakeCurrent(hdc, myContext);


        //Setup our context
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(0.0, 640, 480, 0.0, 1.0, -1.0);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glClearColor(0, 0, 0, 1.0);
    }

    //Make thread use our context
    wglMakeCurrent(hdc, myContext);

    //Draw
    glColor3f(0, 1.0f, 0);
    glBegin(GL_QUADS);
	
	//shape
    glVertex2f(300, 500.0f);
    glVertex2f(100.0f, 500.0f);
    glVertex2f(100.0f, 290.0f);
    glVertex2f(300, 290.0f);
	
	
    glEnd();
    //Make thread to use games context again
    wglMakeCurrent(hdc, gameContext);
*/
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

    while (!unload) {
        if (GetAsyncKeyState(VK_DELETE)) {
            unload = 1;
        }
        Sleep(250);
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

    while (!unload) {
        Sleep(500);
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
        CloseHandle(CreateThread(0, 0, (LPTHREAD_START_ROUTINE)initialise, 0, 0, 0));

        break;
    }
    default: {

    } break;
    }
    return 1;
}

