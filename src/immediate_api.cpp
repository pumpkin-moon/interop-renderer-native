#include "immediate_api.hpp"
#include "iostream"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

static ImDrawList* drawList;

ImmediateAPI* ImmediateAPI::Initialize(HWND hWnd) 
{
    auto api = (ImmediateAPI*)malloc(sizeof(ImmediateAPI));

    if (api == NULL) {
        return NULL;
    }

    api->self = api;
    api->window = hWnd;

    api->InitD3D();
    api->InitImGui();

    return api;
}

ImDrawList* ImmediateAPI::GetDrawList()
{
    return drawList;
}

void ImmediateAPI::Release() {
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    swapchain->Release();
    backbuffer->Release();
    device->Release();
    context->Release();

    free(self);
}

void ImmediateAPI::Resize(int width, int height)
{
    ImGui_ImplDX11_InvalidateDeviceObjects();

    // 1. Clear the existing references to the backbuffer
    ID3D11RenderTargetView* nullViews[] = { NULL };
    context->OMSetRenderTargets(ARRAYSIZE(nullViews), nullViews, NULL);
    backbuffer->Release();
    context->Flush();

    // 2. Resize the existing swapchain
    auto hr = swapchain->ResizeBuffers(0, width, height, DXGI_FORMAT_R8G8B8A8_UNORM, 0);
    if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
    {
        // You have to destroy the device, swapchain, and all resources and
        // recreate them to recover from this case. The device was hardware reset,
        // physically removed, or the driver was updated and/or restarted
        std::cout << "DXGI device error!" << std::endl;
    }

    ResizeDevice(width, height);

    ImGui_ImplDX11_CreateDeviceObjects();
}

Image ImmediateAPI::CreateImage(void* data, int length)
{
    Image image;
    data = stbi_load_from_memory((stbi_uc*)data, length, &image.width, &image.height, &image.channels, 4);

    // Create texture
    D3D11_TEXTURE2D_DESC desc;
    ZeroMemory(&desc, sizeof(desc));
    desc.Width = image.width;
    desc.Height = image.height;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.SampleDesc.Count = 1;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    desc.CPUAccessFlags = 0;

    ID3D11Texture2D* pTexture;
    D3D11_SUBRESOURCE_DATA subResource;
    subResource.pSysMem = data;
    subResource.SysMemPitch = desc.Width * 4;
    subResource.SysMemSlicePitch = 0;
    device->CreateTexture2D(&desc, &subResource, &pTexture);

    // Create texture view
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
    ZeroMemory(&srvDesc, sizeof(srvDesc));
    srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = desc.MipLevels;
    srvDesc.Texture2D.MostDetailedMip = 0;

    if (pTexture == NULL)
    {
        return image;
    }

    ID3D11ShaderResourceView* srv;
    device->CreateShaderResourceView(pTexture, &srvDesc, &srv);
    pTexture->Release();

    image.resource = srv;
    return image;
}

void ImmediateAPI::BeginFrame(Color color)
{
    float nColor[4] = { color.r / 255.0f, color.g / 255.0f, color.b / 255.0f, color.a / 255.0f };
    context->ClearRenderTargetView(backbuffer, nColor);

    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    drawList = ImGui::GetForegroundDrawList();
}

void ImmediateAPI::EndFrame()
{
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    swapchain->Present(0, 0);
}

void ImmediateAPI::InitD3D()
{
    DXGI_SWAP_CHAIN_DESC scd;

    // clear out the struct for use
    ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));

    // fill the swap chain description struct
    scd.BufferCount = 1;                                    // one back buffer
    scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;     // use 32-bit color
    scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;      // how swap chain is to be used
    scd.OutputWindow = window;                              // the window to be used
    scd.SampleDesc.Count = 4;                               // how many multisamples
    scd.Windowed = TRUE;                                    // windowed/full-screen mode

    // create a device, device context and swap chain using the information in the scd struct
    D3D11CreateDeviceAndSwapChain(NULL,
        D3D_DRIVER_TYPE_HARDWARE,
        NULL,
        NULL,
        NULL,
        NULL,
        D3D11_SDK_VERSION,
        &scd,
        &swapchain,
        &device,
        NULL,
        &context);

    RECT rect;
    GetWindowRect(window, &rect);
    ResizeDevice(rect.right - rect.left, rect.bottom - rect.top);
}

void ImmediateAPI::InitImGui()
{
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(window);
    ImGui_ImplDX11_Init(device, context);
}

void ImmediateAPI::ResizeDevice(int width, int height)
{
    // get the address of the back buffer
    ID3D11Texture2D* pBackBuffer;
    swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);

    if (pBackBuffer == NULL)
    {
        std::cout << "'pBackBuffer' is null!" << std::endl;
        return;
    }

    // use the back buffer address to create the render target
    device->CreateRenderTargetView(pBackBuffer, NULL, &backbuffer);
    pBackBuffer->Release();

    // set the render target as the back buffer
    context->OMSetRenderTargets(1, &backbuffer, NULL);

    // Set the viewport
    D3D11_VIEWPORT viewport;
    ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.Width = float(width);
    viewport.Height = float(height);

    context->RSSetViewports(1, &viewport);
}
