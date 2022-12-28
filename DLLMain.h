#pragma once

#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx11.h"
#include <d3d11.h>


#define DIRECTINPUT_VERSION 0x0800



static ID3D11Device* g_pd3dDevice = nullptr;
static ID3D11DeviceContext* g_pd3dDeviceContext = nullptr;
static IDXGISwapChain* g_pSwapChain = nullptr;
static ID3D11RenderTargetView* g_mainRenderTargetView = nullptr;
bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void CreateRenderTarget();
void CleanupRenderTarget();
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

#include <TlHelp32.h>
#include <iostream>
#include <vector>
#include <chrono>
#include <thread>
#include <algorithm>
#include <random>
#include <string>
#include <fstream>
#include <sstream>

namespace v {
    namespace auth {
        int authenticate(std::string hwid, std::string token);
        std::vector<char>* getreq(std::string* params, int extra = 0);

        enum {
            authenticating = 52635,
            authenticated = 23017,
            not_authorized = 53965,
            outdated = 32288,
            banned = 97902,
            error = 69442,
            inited = 78517
        };
    }

    namespace info {
        inline DWORD minecraft_pid;
        inline HANDLE minecraft_handle;
        inline HWND minecraft_window;
        inline bool verified = true;
        inline std::string version = ("2.0.2");
        inline std::string hwid;
    }

    namespace launch {
        extern void all_threads();
    }

    namespace status {
        extern void thread();
    }

    namespace binds {
        extern void thread();
    }

    namespace gui {
        inline HWND hwnd;
        inline ImVec4 color1 = ImColor(0.9f, 0.24f, 0.24f);
        inline ImVec4 color2 = ImVec4(0.56f, 0.00f, 1.00f, 0.50f);
        inline ImVec4 color3 = ImVec4(1.00f, 1.00f, 1.00f, 0.15f);
        inline bool rainbow_mode = false;
        inline bool window_being_dragged = false;
        inline bool item_being_used = false;
        inline int tab_main = 1;
        inline char lithgui = 'A';
        inline bool memeversion = false;
        extern void move_window();
    }

    namespace local_player {
        inline bool moving = false;
        inline bool sprinting = false;
        inline bool jumping = false;
        inline bool inventory_open = false;
        inline int current_slot = 1;

        namespace binds {
            inline int slot1 = 0x31;
            inline int slot2 = 0x32;
            inline int slot3 = 0x33;
            inline int slot4 = 0x34;
            inline int slot5 = 0x35;
            inline int slot6 = 0x36;
            inline int slot7 = 0x37;
            inline int slot8 = 0x38;
            inline int slot9 = 0x39;

            inline int w = 'w';
            inline int a = 'a';
            inline int s = 's';
            inline int d = 'd';

            inline int jump = VK_SPACE;
            inline int sprint = VK_LCONTROL;
            inline int drop = 'q';
            inline int inventory = 'e';
        }
    }

    namespace scanner {
        extern void thread();
    }

    namespace modules {
        namespace clicker {
            namespace left {
                namespace jitter {
                    extern void thread();
                }

                extern void thread();

                inline bool enabled = false;
                inline float cps = 12.f;
                inline float blockhit_delay;
                inline float x_jitter;
                inline float y_jitter;
                inline bool inventory = false;
                inline bool blockhit = false;
                inline bool rmb_lock = true;
                inline bool slot[9] = { true, true, true, true, true, true, true, true, true };
                inline std::string bind = ("[NONE]");
                inline bool bind_pressed = false;
                inline size_t selected_bind = 0;
            }

            namespace right {
                extern void thread();

                inline bool enabled;
                inline float cps = 12.f;
                inline bool slot[9] = { true, true, true, true, true, true, true, true, true };
                inline std::string bind = ("[NONE]");
                inline bool bind_pressed;
                inline size_t selected_bind;
            }
        }

        namespace reach {
            extern void thread();

            extern void disable();

            inline std::vector<size_t> unlimit;
            inline std::vector<size_t> floats;
            inline std::vector<size_t> doubles;
            inline bool enabled;
            inline float minvalue;
            inline float maxvalue;
            inline bool while_moving = true;
            inline std::string bind = ("[NONE]");
            inline bool used;
            inline bool bind_pressed;
            inline size_t selected_bind;
        }

        namespace throwpot {
            extern void thread();

            inline bool enabled;
            inline bool throw_item;
            inline float return_pos;
            inline float throw_delay;
            inline bool slot[9];
            inline std::string bind = ("[NONE]");
            inline bool bind_pressed;
            inline size_t selected_bind;
        }

        namespace fly {
            extern void thread();

            extern void disable();

            inline std::vector<size_t> locationsfly;
            inline bool enabled;
            inline bool waiting;
            inline std::string bind = ("[NONE]");
            inline bool used;
            inline bool bind_pressed;
            inline size_t selected_bind;
        }

        namespace nametags {
            extern void thread();

            extern void disable();

            inline std::vector<size_t> locations1;
            inline std::vector<size_t> locations2;
            inline bool enabled;
            inline float multiplier = 1.f;
            inline std::string bind = ("[NONE]");
            inline bool used;
            inline bool bind_pressed;
            inline size_t selected_bind;
        }

        namespace esp {
            extern void thread();

            extern void disable();

            inline std::vector<size_t> locations;
            inline bool enabled;
            inline std::string bind = ("[NONE]");
            inline bool used;
            inline bool bind_pressed;
            inline size_t selected_bind;
        }

        namespace cavefinder {
            extern void thread();

            extern void disable();

            inline std::vector<size_t> locations;
            inline bool enabled;
            inline std::string bind = ("[NONE]");
            inline bool used;
            inline bool bind_pressed;
            inline size_t selected_bind;
        }

        namespace timer {
            extern void thread();

            extern void disable();

            inline std::vector<size_t> locations;
            inline bool enabled;
            inline float multiplier = 1.f;
            inline std::string bind = ("[NONE]");
            inline bool used;
            inline bool bind_pressed;
            inline size_t selected_bind;
        }

        namespace speed {
            extern void thread();

            extern void disable();

            inline std::vector<size_t> locations;
            inline bool enabled;
            inline float multiplier = 1.f;
            inline std::string bind = ("[NONE]");
            inline bool used;
            inline bool bind_pressed;
            inline size_t selected_bind;
        }

        namespace velocity {
            extern void thread();

            extern void disable();

            inline std::vector<size_t> locations;
            inline bool enabled;
            inline float min;
            inline float max;
            inline bool while_moving = true;
            inline std::string bind = ("[NONE]");
            inline bool used;
            inline bool bind_pressed;
            inline size_t selected_bind;
        }

        namespace bhop {
            extern void thread();

            extern void disable();

            inline std::vector<size_t> locations;
            inline bool enabled;
            inline float speed = 1.f;
            inline std::string bind = ("[NONE]");
            inline bool used;
            inline bool bind_pressed;
            inline size_t selected_bind;
        }

        namespace antiafk {
            extern void thread();

            inline bool enabled;
        }

        namespace destruct {
            extern void execute();

            inline bool pcaclean = false;
            inline bool traceless = true;

            inline std::string bind = ("[NONE]");
            inline bool bind_pressed;
            inline size_t selected_bind;
        }

        namespace hide {
            inline bool enabled;
            inline std::string bind = ("[NONE]");
            inline bool bind_pressed;
            inline size_t selected_bind;
        }
    }
   
}
