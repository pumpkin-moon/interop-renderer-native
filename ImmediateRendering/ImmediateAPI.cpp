#include "ImmediateAPI.h"
#include "d3d9.h"
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx9.h"
#include "custom_vertex.h"
#include <iostream>

#define CUSTOMFVF (D3DFVF_XYZRHW | D3DFVF_DIFFUSE)

static ImmediateAPI API;
static bool Initialized = false;
static HWND HWnd;

D3DPRESENT_PARAMETERS create_params(HWND hWnd, int width, int height)
{
    D3DPRESENT_PARAMETERS d3dpp;

    ZeroMemory(&d3dpp, sizeof(d3dpp));
    d3dpp.Windowed = true;
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3dpp.hDeviceWindow = hWnd;
    d3dpp.BackBufferWidth = width;
    d3dpp.BackBufferHeight = height;

    return d3dpp;
}

LPDIRECT3DVERTEXBUFFER9 v_buffer;

void init_graphics()
{
    // create three vertices using the CUSTOMVERTEX struct built earlier
    CUSTOMVERTEX vertices[] =
    {
        { 320.0f, 50.0f, 0.5f, 1.0f, D3DCOLOR_XRGB(0, 0, 255), },
        { 520.0f, 400.0f, 0.5f, 1.0f, D3DCOLOR_XRGB(0, 255, 0), },
        { 120.0f, 400.0f, 0.5f, 1.0f, D3DCOLOR_XRGB(255, 0, 0), },
    };

    // create the vertex and store the pointer into v_buffer, which is created globally
    API.device->CreateVertexBuffer(3 * sizeof(CUSTOMVERTEX),
        0,
        CUSTOMFVF,
        D3DPOOL_MANAGED,
        &v_buffer,
        NULL);

    VOID* pVoid;    // the void pointer

    v_buffer->Lock(0, 0, (void**)&pVoid, 0);    // lock the vertex buffer
    memcpy(pVoid, vertices, sizeof(vertices));    // copy the vertices to the locked buffer
    v_buffer->Unlock();    // unlock the vertex buffer
}

ImmediateAPI* ImmediateAPI::init(HWND hWnd)
{
	if (!Initialized)
	{
		API.d3d = Direct3DCreate9(D3D_SDK_VERSION);

        RECT rect;
        GetWindowRect(hWnd, &rect);

        auto d3dpp = create_params(hWnd, rect.right - rect.left, rect.bottom - rect.top);

        API.d3d->CreateDevice(D3DADAPTER_DEFAULT,
            D3DDEVTYPE_HAL,
            hWnd,
            D3DCREATE_SOFTWARE_VERTEXPROCESSING,
            &d3dpp,
            &API.device);

        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

        // Setup Platform/Renderer backends
        ImGui_ImplWin32_Init(hWnd);
        ImGui_ImplDX9_Init(API.device);

        HWnd = hWnd;

        init_graphics();

		Initialized = true;
	}

	return &API;
}

void ImmediateAPI::render()
{
    API.device->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);
    API.device->BeginScene();

    ImGui_ImplDX9_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    ImGui::ShowDemoWindow(); // Show demo window! :)

    ImGui::Render();
    ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());

    API.device->SetFVF(CUSTOMFVF);
    API.device->SetStreamSource(0, v_buffer, 0, sizeof(CUSTOMVERTEX));
    API.device->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 1);

    API.device->EndScene();

    API.device->Present(NULL, NULL, NULL, NULL);
}

void ImmediateAPI::render_circles(Circle* circles, int count)
{
    API.device->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);
    API.device->BeginScene();

    ImGui_ImplDX9_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    auto drawList = ImGui::GetForegroundDrawList();
    for (int i = 0; i < count; ++i)
    {
        auto circle = circles[i];
        drawList->AddCircle(ImVec2(circle.x, circle.y), circle.radius, circle.color, 0, circle.thickness);
    }

    std::cout << "CmdBuffer: " << drawList->CmdBuffer.Size << " | " << drawList->CmdBuffer.Capacity << std::endl;
    std::cout << "VtxBuffer: " << drawList->VtxBuffer.Size << " | " << drawList->VtxBuffer.Capacity << std::endl;
    std::cout << "IdxBuffer: " << drawList->IdxBuffer.Size << " | " << drawList->IdxBuffer.Capacity << std::endl;

    ImGui::Render();
    ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());

    API.device->EndScene();

    API.device->Present(NULL, NULL, NULL, NULL);
}

void ImmediateAPI::release()
{
    ImGui_ImplDX9_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    v_buffer->Release();

    API.device->Release();
    API.d3d->Release();
    
    Initialized = false;
}

void ImmediateAPI::resize(int width, int height)
{
    ImGui_ImplDX9_InvalidateDeviceObjects();

    auto d3dpp = create_params(HWnd, width, height);
    API.device->Reset(&d3dpp);

    ImGui_ImplDX9_CreateDeviceObjects();
}
