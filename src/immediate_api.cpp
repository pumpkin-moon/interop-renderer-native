#include "immediate_api.hpp"
#include "iostream"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STBTT_STATIC
#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

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

ID3D11ShaderResourceView* CreateTexture(ID3D11Device* device, byte* data, int width, int height, int channels)
{
    DXGI_FORMAT format;
    switch (channels)
    {
    case 1:
        format = DXGI_FORMAT_R8_UNORM;
        break;
    case 2:
        format = DXGI_FORMAT_R8G8_UNORM;
        break;
    case 4:
        format = DXGI_FORMAT_R8G8B8A8_UNORM;
        break;
    default:
        throw;
    }

    // Create texture
    D3D11_TEXTURE2D_DESC desc;
    ZeroMemory(&desc, sizeof(desc));
    desc.Width = width;
    desc.Height = height;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Format = format;
    desc.SampleDesc.Count = 1;
    desc.Usage = D3D11_USAGE_IMMUTABLE;
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    desc.CPUAccessFlags = 0;

    ID3D11Texture2D* pTexture;
    D3D11_SUBRESOURCE_DATA subResource;
    subResource.pSysMem = data;
    subResource.SysMemPitch = desc.Width * channels;
    subResource.SysMemSlicePitch = 0;
    device->CreateTexture2D(&desc, &subResource, &pTexture);

    // Create texture view
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
    ZeroMemory(&srvDesc, sizeof(srvDesc));
    srvDesc.Format = format;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = desc.MipLevels;
    srvDesc.Texture2D.MostDetailedMip = 0;

    if (pTexture == NULL)
    {
        return NULL;
    }

    ID3D11ShaderResourceView* srv;
    device->CreateShaderResourceView(pTexture, &srvDesc, &srv);
    pTexture->Release();

    return srv;
}

Image ImmediateAPI::NewImage(byte* data, int length)
{
    Image image;
    const int channels = 4;
    data = stbi_load_from_memory((stbi_uc*)data, length, &image.width, &image.height, &image.channels, channels);
    image.resource = CreateTexture(device, data, image.width, image.height, channels);
    return image;
}

Image ImmediateAPI::NewImageRaw(byte* data, int width, int height, int channels)
{
    Image image;
    image.width = width;
    image.height = height;
    image.channels = channels;
    image.resource = CreateTexture(device, data, width, height, channels);
    return image;
}

void LoadGlyph(stbtt_fontinfo* info, float scale, Glyph* glyph, int codepoint)
{
    const unsigned char onEdgeValue = 128;
    const float pixelDistScale = 64.0f;

    int advance;
    int leftBearing;
    stbtt_GetCodepointHMetrics(info, codepoint, &advance, &leftBearing);

    glyph->codepoint = codepoint;
    glyph->bitmap = stbtt_GetCodepointSDF(info, scale, codepoint, 5, onEdgeValue, pixelDistScale, &glyph->width, &glyph->height, &glyph->x, &glyph->y);
    glyph->advance = advance * scale;
    glyph->leftBearing = leftBearing * scale;

    if (glyph->bitmap == NULL)
    {
        std::cout << "Codepoint " << codepoint << " is missing!" << std::endl;
    }
}

Font ImmediateAPI::NewFont(char* path, CodepointRange range)
{
    Font font;
    FILE* file;
    fopen_s(&file, path, "rb");

    if (file == NULL)
    {
        std::cout << "Could not open path '" << path << "'!" << std::endl;
        return font;
    }

    fseek(file, 0, SEEK_END);
    int length = ftell(file);
    fseek(file, 0, SEEK_SET);

    byte* fontBuffer = (byte*)malloc(length);

    fread(fontBuffer, length, 1, file);
    fclose(file);

    stbtt_fontinfo info;
    if (!stbtt_InitFont(&info, fontBuffer, 0))
    {
        std::cout << "Could not initialize font!" << std::endl;
        free(fontBuffer);
        return font;
    }

    int ascent, descent, lineGap;
    stbtt_GetFontVMetrics(&info, &ascent, &descent, &lineGap);

    int count = range.max - range.min;
    float scale = stbtt_ScaleForPixelHeight(&info, 64.0f);
    auto glyphs = (Glyph*)malloc(sizeof(Glyph) * count);
    auto rects = (stbrp_rect*)malloc(sizeof(stbrp_rect) * count);

    for (int i = 0; i < count; ++i) 
    {
        auto glyph = glyphs + i;
        LoadGlyph(&info, scale, glyph, range.min + i);

        rects[i].id = i;
        rects[i].w = glyph->width;
        rects[i].h = glyph->height;
    }

    const int dimension = 2048;

    stbrp_context context;

    stbrp_init_target(&context, dimension, dimension, NULL, NULL);
    stbrp_pack_rects(&context, rects, count);

    int textureWidth = 0;
    int textureHeight = 0;

    for (int i = 0; i < count; ++i) 
    {
        textureWidth = max(textureWidth, rects[i].x + rects[i].w);
        textureHeight = max(textureHeight, rects[i].y + rects[i].h);
    }

    textureWidth = 1 << (int)ceil(log2(textureWidth));
    textureHeight = 1 << (int)ceil(log2(textureHeight));

    font.bitmapData = (byte*)malloc(textureWidth * textureHeight);
    font.bitmapWidth = textureWidth;
    font.bitmapHeight = textureHeight;
    font.glyphRects = (Rect*)malloc(sizeof(Rect) * count);
    font.glyphRange = range;

    for (int i = 0; i < count; ++i)
    {
        auto id = rects[i].id;

        auto glyph = glyphs + id;
        auto rect = font.glyphRects + id;

        rect->x = rects[i].x;
        rect->y = rects[i].y;
        rect->w = rects[i].w;
        rect->h = rects[i].h;

        auto glyphArea = rect->w * rect->h;
        for (int j = 0; j < glyphArea; ++j)
        {
            auto x = j % rect->w + rect->x;
            auto y = j / rect->w + rect->y;

            auto k = x + y * textureWidth;
            font.bitmapData[k] = glyph->bitmap[j];
        }

        stbtt_FreeSDF(glyph->bitmap, NULL);
    }

    free(rects);
    free(glyphs);
    free(fontBuffer);

    return font;
}

Shader ImmediateAPI::NewShader(char* name, char* vertexName, char* pixelName, byte* data, int length)
{
    ID3DBlob* vs;
    ID3DBlob* ps;
    ID3DBlob* errors;

    D3DCompile(data, length, name, NULL, NULL, vertexName, "vs_4_0", 0, 0, &vs, &errors);
    D3DCompile(data, length, name, NULL, NULL, pixelName, "ps_4_0", 0, 0, &ps, &errors);

    ID3D11VertexShader* vertexShader;
    ID3D11PixelShader* pixelShader;

    device->CreateVertexShader(vs->GetBufferPointer(), vs->GetBufferSize(), NULL, &vertexShader);
    device->CreatePixelShader(ps->GetBufferPointer(), ps->GetBufferSize(), NULL, &pixelShader);

    return { vertexShader, pixelShader };
}

void ImmediateAPI::UseShader(Shader shader)
{
    context->VSSetShader(shader.vertexShader, NULL, 0);
    context->PSSetShader(shader.pixelShader, NULL, 0);
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

    ImGui::GetIO().Fonts->AddFontDefault();
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
