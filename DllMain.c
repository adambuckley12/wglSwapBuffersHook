
#include <windows.h>
#include <stdlib.h>
#include <strsafe.h>
#include "include/MinHook.h"
#include <gl/GL.h>
#include <stdbool.h>
//#include <gl/GL.h>
//#include <gl/GLU.h>

FARPROC wglSwapBuffers = NULL;
HMODULE m_opengl_dll;


int contextCreated = 0;
HGLRC myContext;
HGLRC gameContext;
void initialise(void); 
int unload = 0;


long __stdcall wgl_swap_buffers(_In_ HDC hdc) {


    OutputDebugString("wgl\n");
    //OutputDebugString("wglSwapHook Called\n");
    gameContext = wglGetCurrentContext();

    //Create our own context if it hasn't been created yet
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

    //Draw something (a rectangle here)
    glColor3f(1.0f, 0, 0);
    glBegin(GL_QUADS);
    glVertex2f(0, 190.0f);
    glVertex2f(100.0f, 190.0f);
    glVertex2f(100.0f, 290.0f);
    glVertex2f(0, 290.0f);
    glEnd();

    //Make thread to use games context again
    wglMakeCurrent(hdc, gameContext);

    return hdc;

}

void hooking(FARPROC wglSwapBuffers, HMODULE m_opengl_dll)
{


    //init 
    OutputDebugString(m_opengl_dll);
    MH_Initialize();
    OutputDebugString("Init Hook\n");

	MH_CreateHook((LPVOID)wglSwapBuffers, wgl_swap_buffers, 0); //problem with code is this 0 - should be pointer to trampoline func to call original Wgl so it renders i think??>?>?> 
    OutputDebugString("Created Hook\n");

    MH_EnableHook(wglSwapBuffers);
    OutputDebugString("Enabled Hook\n");

    while (!unload) {
        if (GetAsyncKeyState(VK_DELETE)) {
            unload = 1;
        }
        Sleep(250);
    }

}

void initialise(void)
{
    
    OutputDebugString("[Controller] Initialise called.\n");

    m_opengl_dll = GetModuleHandleA("opengl32.dll");

    while (!m_opengl_dll)
    {
        Sleep(500);
        m_opengl_dll = GetModuleHandle("opengl32.dll");
    }

    OutputDebugString("Found openGl\n");

    while (!(wglSwapBuffers))
    {
        wglSwapBuffers = GetProcAddress(m_opengl_dll, "wglSwapBuffers");
        Sleep(500);
    }


    OutputDebugString("Found wglswapbuffers\n");
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
    LPVOID* original_wgl_swapbuffers = (LPVOID*)malloc(sizeof(LPVOID));


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

