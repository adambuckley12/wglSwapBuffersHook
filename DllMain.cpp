
#include <windows.h>
#include <stdlib.h>
#include <strsafe.h>
#include "include/MinHook.h"
#include <glad/include/glad/gl.h>
#include <stdbool.h>
//#include <gl/GL.h>
//#include <gl/GLU.h>
#include <GLFW/glfw3.h>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include <winuser.h>

#define IMGUI_IMPL_OPENGL_LOADER_GLAD2

#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
// About Desktop OpenGL function loaders:
//  Modern desktop OpenGL doesn't have a standard portable header file to load OpenGL function pointers.
//  Helper libraries are often used for this purpose! Here we are supporting a few common ones (gl3w, glew, glad).
//  You may use another loader/header of your choice (glext, glLoadGen, etc.), or chose to manually implement your own.
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GL3W)
#include <GL/gl3w.h>            // Initialize with gl3wInit()
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLEW)
#include <GL/glew.h>            // Initialize with glewInit()
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD)
#include <glad/glad.h>          // Initialize with gladLoadGL()
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD2)
#include <../include/glad/include/glad/gl.h>		// Initialize with gladLoadGL(...) or gladLoaderLoadGL()
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLBINDING2)
#define GLFW_INCLUDE_NONE       // GLFW including OpenGL headers causes ambiguity or multiple definition errors.
#include <glbinding/Binding.h>  // Initialize with glbinding::Binding::initialize()
#include <glbinding/gl/gl.h>
using namespace gl;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLBINDING3)
#define GLFW_INCLUDE_NONE       // GLFW including OpenGL headers causes ambiguity or multiple definition errors.
#include <glbinding/glbinding.h>// Initialize with glbinding::initialize()
#include <glbinding/gl/gl.h>
using namespace gl;
#else
#include IMGUI_IMPL_OPENGL_LOADER_CUSTOM
#endif

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
static ImGuiIO io;
static ImGuiContext* imgui_context;
const char* glsl_version = "#version 130";

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

DWORD WINAPI init_context()
{
    // Setup window
    glfwSetErrorCallback(glfw_error_callback);

    if (!glfwInit())
        return FALSE;

    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    // HWND activeWindowHandle = GetActiveWindow
    GLFWwindow* currentContext = glfwGetCurrentContext();

    if (!currentContext)
    {
        OutputDebugString(L"No current (GLFW) context.\n");
        return FALSE;
    }

    window = currentContext;
    glfwSwapInterval(1);

    // Initialize OpenGL loader
    bool err = gladLoadGL(glfwGetProcAddress) == 0;
    if (err)
    {
        OutputDebugString(L"Failed to initialize OpenGL loader.\n");
        return FALSE;
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    io = ImGui::GetIO(); (void)io;
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
        glfwPollEvents();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        bool show = true;

        ImGui::ShowDemoWindow(&show);

        // Rendering
        ImGui::Render();

        OutputDebugString(L"[Controller] Called ImGui::Render.\n");

        int display_w, display_h;

        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        //glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        //glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    /*gameContext = wglGetCurrentContext();

    if (gameContext == NULL);
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
        contextCreated++;
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
    UINT_PTR timerId = SetTimer(windowHwnd, 42, 10, (TIMERPROC)timer_callback);

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
    OutputDebugString(L"DLL MAIN CALLED\n");
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

