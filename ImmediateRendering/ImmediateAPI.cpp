#include "ImmediateAPI.h"
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include "CustomVertex.h"
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "CommandType.h"

#define CUSTOMFVF (D3DFVF_XYZRHW | D3DFVF_DIFFUSE)

static ImmediateAPI API;
static bool Initialized = false;
static HWND HWnd;
static ImDrawList* ImGuiDrawList;

void ImmediateAPI::ResizeInternal(int width, int height)
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

void ImmediateAPI::InitD3D(HWND hWnd)
{
    DXGI_SWAP_CHAIN_DESC scd;

    // clear out the struct for use
    ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));

    // fill the swap chain description struct
    scd.BufferCount = 1;                                    // one back buffer
    scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;     // use 32-bit color
    scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;      // how swap chain is to be used
    scd.OutputWindow = hWnd;                                // the window to be used
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
    GetWindowRect(hWnd, &rect);
    ResizeInternal(rect.right - rect.left, rect.bottom - rect.top);
}

ImmediateAPI* ImmediateAPI::Initialize(HWND hWnd)
{
	if (!Initialized)
	{
        API.InitD3D(hWnd);

        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();

        // Setup Platform/Renderer backends
        ImGui_ImplWin32_Init(hWnd);
        ImGui_ImplDX11_Init(API.device, API.context);

        HWnd = hWnd;

		Initialized = true;
	}

	return &API;
}

void ImmediateAPI::BeginFrame(unsigned int color)
{
    float nColor[4] = { 
        (color >> 0 & 0xFF) / 255.0f,
        (color >> 8 & 0xFF) / 255.0f,
        (color >> 16 & 0xFF) / 255.0f, 
        (color >> 24 & 0xFF) / 255.0f
    };

    context->ClearRenderTargetView(API.backbuffer, nColor);

    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    ImGuiDrawList = ImGui::GetForegroundDrawList();
}

void ImmediateAPI::EndFrame()
{
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    swapchain->Present(0, 0);
}

void ImmediateAPI::Release()
{
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    swapchain->Release();
    backbuffer->Release();
    device->Release();
    context->Release();
    
    Initialized = false;
}

void ImmediateAPI::Resize(int width, int height)
{
    ImGui_ImplDX11_InvalidateDeviceObjects();

    // 1. Clear the existing references to the backbuffer
    ID3D11RenderTargetView* nullViews[] = { NULL };
    API.context->OMSetRenderTargets(ARRAYSIZE(nullViews), nullViews, NULL);
    API.backbuffer->Release();
    API.context->Flush();

    // 2. Resize the existing swapchain
    auto hr = API.swapchain->ResizeBuffers(0, width, height, DXGI_FORMAT_R8G8B8A8_UNORM, 0);
    if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
    {
        // You have to destroy the device, swapchain, and all resources and
        // recreate them to recover from this case. The device was hardware reset,
        // physically removed, or the driver was updated and/or restarted
        std::cout << "DXGI device error!" << std::endl;
    }

    ResizeInternal(width, height);

    ImGui_ImplDX11_CreateDeviceObjects();
}

ID3D11ShaderResourceView* ImmediateAPI::CreateImage(void* data, int length, int width, int height)
{
    // Create texture
    D3D11_TEXTURE2D_DESC desc;
    ZeroMemory(&desc, sizeof(desc));
    desc.Width = width;
    desc.Height = height;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.SampleDesc.Count = 1;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    desc.CPUAccessFlags = 0;

    int channels = 4;
    data = stbi_load_from_memory((stbi_uc*)data, length, &width, &height, &channels, 4);

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
        std::cout << "Texture is null!" << std::endl;
        return NULL;
    }

    ID3D11ShaderResourceView* srv;
    device->CreateShaderResourceView(pTexture, &srvDesc, &srv);
    pTexture->Release();

    return srv;
}

void ImmediateAPI::DrawList(byte* data, int length)
{
    Mat3x2 matrix = { 1, 0, 0, 1, 0, 0 };

    int offset = 0;
    while (offset < length)
    {
        switch (data[offset++])
        {
        case CommandType::SetMatrix:
        {
            matrix = *(Mat3x2*)(data + offset);
            offset += sizeof(Mat3x2);
            break;
        }
        case CommandType::DrawCircle:
        {
            auto circle = sCircle::Unpack(data, offset, matrix);
            ImGuiDrawList->AddCircle(circle.position, circle.radius, circle.style.color, 0, circle.style.thickness);
            break;
        }
        case CommandType::DrawEllipse:
        {
            auto ellipse = sEllipse::Unpack(data, offset, matrix);
            ImGuiDrawList->AddEllipse(ellipse.position, ellipse.radius, ellipse.style.color, 0, 0, ellipse.style.thickness);
            break;
        }
        case CommandType::DrawLine:
        {
            auto line = sLine::Unpack(data, offset, matrix);
            ImGuiDrawList->AddLine(line.from, line.to, line.style.color, line.style.thickness);
            break;
        }
        case CommandType::DrawPolyline:
        {
            auto polyline = sPolyline::Unpack(data, offset, matrix);
            ImGuiDrawList->AddPolyline(polyline.points, polyline.numPoints, polyline.style.color, ImDrawFlags_None, polyline.style.thickness);
            break;
        }
        case CommandType::DrawTriangle:
        {
            auto triangle = sTriangle::Unpack(data, offset, matrix);
            ImGuiDrawList->AddTriangle(triangle.a, triangle.b, triangle.c, triangle.style.color, triangle.style.thickness);
            break;
        }
        case CommandType::DrawQuad:
        {
            auto quad = sQuad::Unpack(data, offset, matrix);
            ImGuiDrawList->AddQuad(quad.a, quad.b, quad.c, quad.d, quad.style.color, quad.style.thickness);
            break;
        }
        case CommandType::DrawPolygon:
        {
            auto polygon = sPolygon::Unpack(data, offset, matrix);
            ImGuiDrawList->AddPolyline(polygon.points, polygon.numPoints, polygon.style.color, ImDrawFlags_Closed, polygon.style.thickness);
            break;
        }
        case CommandType::DrawImage:
        {
            auto image = sImage::Unpack(data, offset, matrix);
            ImGuiDrawList->AddImageQuad(image.srv, image.a, image.b, image.c, image.d);
            break;
        }
        default:
            throw;
        }
    }
}