#include "includes.h"

// Unload the DLL
void unloadDll(HINSTANCE hinstDLL, FARPROC wglSwapBuffers)  // handle to DLL module)
{	
	// Remove All Hooks:
	MH_DisableHook(wglSwapBuffers);
	MH_RemoveHook(wglSwapBuffers);
	MH_Uninitialize();
			
	//Unload DLL	
	FreeLibrary(hinstDLL);
}
