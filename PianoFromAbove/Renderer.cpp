/*************************************************************************************************
*
* File: Renderer.cpp
*
* Description: Implements the rendering objects. Just a wrapper to Direct3D.
*
* Copyright (c) 2010 Brian Pantano. All rights reserved.
*
*************************************************************************************************/
#include "Globals.h"
#include "Renderer.h"

#include "RectPixelShader.h"
#include "RectVertexShader.h"
#include "NotePixelShader.h"
#include "NoteVertexShader.h"
#include "NoteVertexShaderSameWidth.h"
#include "BackgroundPixelShader.h"
#include "BackgroundVertexShader.h"

#include <ShlObj.h>
#include <Config.h>

ComPtr<IWICImagingFactory> Renderer11::s_pWICFactory;

Renderer11::Renderer11() {}

Renderer11::~Renderer11() {
    imguiClearFontCache();
}

tuple<HRESULT, const char*> Renderer11::Init(HWND hWnd, bool bLimitFPS) {
    HRESULT res;

    // Create DXGI factory
    res = CreateDXGIFactory1(IID_PPV_ARGS(&m_pFactory));
    if (FAILED(res))
        return make_tuple(res, "CreateDXGIFactory1");

    // Create device
    // TODO: Allow device selection for people with multiple GPUs
    m_hWnd = hWnd;
    m_bLimitFPS = bLimitFPS;
#ifndef SOFTWARE_RENDER_ONLY
    for (UINT i = 0;; i++) {
        ComPtr<IDXGIAdapter1> adapter;
        if (m_pFactory->EnumAdapters1(i, &adapter) == DXGI_ERROR_NOT_FOUND)
            break;

        DXGI_ADAPTER_DESC1 desc = {};
        res = adapter->GetDesc1(&desc);
        if (FAILED(res))
            return make_tuple(res, "GetDesc1");

        if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
            continue;

        D3D_FEATURE_LEVEL featureLevel;
        static const D3D_FEATURE_LEVEL wanted[] = { D3D_FEATURE_LEVEL_11_0 };
        res = D3D11CreateDevice(adapter.Get(), D3D_DRIVER_TYPE_UNKNOWN, NULL,
            D3D11_CREATE_DEVICE_BGRA_SUPPORT, wanted, _countof(wanted),
            D3D11_SDK_VERSION, &m_pDevice, &featureLevel, &m_pContext);
        if (FAILED(res))
            continue;
        m_pAdapter = adapter;
        break;
    }

    if (m_pDevice == nullptr) //Oh we love software rendering!
#endif
    {
        D3D_FEATURE_LEVEL featureLevel;
        static const D3D_FEATURE_LEVEL wanted[] = { D3D_FEATURE_LEVEL_11_0 };
        res = D3D11CreateDevice(NULL, D3D_DRIVER_TYPE_WARP, NULL,
            D3D11_CREATE_DEVICE_BGRA_SUPPORT, wanted, _countof(wanted),
            D3D11_SDK_VERSION, &m_pDevice, &featureLevel, &m_pContext);
        if (FAILED(res))
            return make_tuple(res, "D3D11CreateDevice (WARP)");
    }

    // Create shaders
    res = m_pDevice->CreateVertexShader(g_pRectVertexShader, sizeof(g_pRectVertexShader), NULL, &m_pRectVS);
    if (FAILED(res)) return make_tuple(res, "CreateVertexShader (rect)");
    res = m_pDevice->CreatePixelShader(g_pRectPixelShader, sizeof(g_pRectPixelShader), NULL, &m_pRectPS);
    if (FAILED(res)) return make_tuple(res, "CreatePixelShader (rect)");

    // Create rect input layout
    D3D11_INPUT_ELEMENT_DESC rect_input[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT,  0, 0,                            D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR",    0, DXGI_FORMAT_B8G8R8A8_UNORM, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
    res = m_pDevice->CreateInputLayout(rect_input, _countof(rect_input), g_pRectVertexShader, sizeof(g_pRectVertexShader), &m_pRectInputLayout);
    if (FAILED(res)) return make_tuple(res, "CreateInputLayout (rect)");

    // Create note shaders
    res = m_pDevice->CreateVertexShader(g_pNoteVertexShader, sizeof(g_pNoteVertexShader), NULL, &m_pNoteVS);
    if (FAILED(res)) return make_tuple(res, "CreateVertexShader (note)");
    res = m_pDevice->CreateVertexShader(g_pNoteVertexShaderSameWidth, sizeof(g_pNoteVertexShaderSameWidth), NULL, &m_pNoteSameWidthVS);
    if (FAILED(res)) return make_tuple(res, "CreateVertexShader (same width note)");
    res = m_pDevice->CreatePixelShader(g_pNotePixelShader, sizeof(g_pNotePixelShader), NULL, &m_pNotePS);
    if (FAILED(res)) return make_tuple(res, "CreatePixelShader (note)");

    // Create background shaders
    res = m_pDevice->CreateVertexShader(g_pBackgroundVertexShader, sizeof(g_pBackgroundVertexShader), NULL, &m_pBackgroundVS);
    if (FAILED(res)) return make_tuple(res, "CreateVertexShader (background)");
    res = m_pDevice->CreatePixelShader(g_pBackgroundPixelShader, sizeof(g_pBackgroundPixelShader), NULL, &m_pBackgroundPS);
    if (FAILED(res)) return make_tuple(res, "CreatePixelShader (background)");

    // Create blend state
    // PFA is weird and inverts blending operations (0 is opaque, 255 is transparent)
    D3D11_BLEND_DESC bd = {};
    bd.RenderTarget[0] = {
        TRUE,
        D3D11_BLEND_INV_SRC_ALPHA, D3D11_BLEND_SRC_ALPHA, D3D11_BLEND_OP_ADD,
        D3D11_BLEND_INV_SRC_ALPHA, D3D11_BLEND_SRC_ALPHA, D3D11_BLEND_OP_ADD,
        D3D11_COLOR_WRITE_ENABLE_ALL
    };
    res = m_pDevice->CreateBlendState(&bd, &m_pBlendState);
    if (FAILED(res)) return make_tuple(res, "CreateBlendState");

    // Create rasterizer state
    D3D11_RASTERIZER_DESC rd = {};
    rd.FillMode = D3D11_FILL_SOLID;
    rd.CullMode = D3D11_CULL_NONE;
    rd.DepthClipEnable = TRUE;
    res = m_pDevice->CreateRasterizerState(&rd, &m_pRasterizerState);
    if (FAILED(res)) return make_tuple(res, "CreateRasterizerState");

    // Create depth stencil states
    D3D11_DEPTH_STENCIL_DESC dd = {};
    res = m_pDevice->CreateDepthStencilState(&dd, &m_pDepthDisabledState);
    if (FAILED(res)) return make_tuple(res, "CreateDepthStencilState (disabled)");

    dd.DepthEnable = TRUE;
    dd.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    dd.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
    res = m_pDevice->CreateDepthStencilState(&dd, &m_pDepthLessEqualState);
    if (FAILED(res)) return make_tuple(res, "CreateDepthStencilState (less-equal)");

    dd.DepthFunc = D3D11_COMPARISON_LESS;
    res = m_pDevice->CreateDepthStencilState(&dd, &m_pDepthLessState);
    if (FAILED(res)) return make_tuple(res, "CreateDepthStencilState (less)");

    // Create background sampler
    D3D11_SAMPLER_DESC sd = {};
    sd.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
    sd.AddressU = sd.AddressV = sd.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    sd.MaxLOD = D3D11_FLOAT32_MAX;
    res = m_pDevice->CreateSamplerState(&sd, &m_pBackgroundSampler);
    if (FAILED(res)) return make_tuple(res, "CreateSamplerState");

    // Create constant buffer (replaces DX12 root constants)
    D3D11_BUFFER_DESC cbDesc = {};
    cbDesc.ByteWidth = (sizeof(RootConstants) + 15) & ~15;
    cbDesc.Usage = D3D11_USAGE_DYNAMIC;
    cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    res = m_pDevice->CreateBuffer(&cbDesc, NULL, &m_pConstantBuffer);
    if (FAILED(res)) return make_tuple(res, "CreateBuffer (constant buffer)");

    // Create fixed size constants structured buffer
    D3D11_BUFFER_DESC fixedDesc = {};
    fixedDesc.ByteWidth = sizeof(FixedSizeConstants);
    fixedDesc.Usage = D3D11_USAGE_DEFAULT;
    fixedDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    fixedDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
    fixedDesc.StructureByteStride = sizeof(FixedSizeConstants);
    res = m_pDevice->CreateBuffer(&fixedDesc, NULL, &m_pFixedBuffer);
    if (FAILED(res)) return make_tuple(res, "CreateBuffer (fixed buffer)");

    D3D11_SHADER_RESOURCE_VIEW_DESC fixedSrvDesc = {};
    fixedSrvDesc.Format = DXGI_FORMAT_UNKNOWN;
    fixedSrvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
    fixedSrvDesc.Buffer.NumElements = 1;
    res = m_pDevice->CreateShaderResourceView(m_pFixedBuffer.Get(), &fixedSrvDesc, &m_pFixedSRV);
    if (FAILED(res)) return make_tuple(res, "CreateSRV (fixed buffer)");

    // Create track color structured buffer
    D3D11_BUFFER_DESC trackDesc = {};
    trackDesc.ByteWidth = MaxTrackColors * 16 * sizeof(TrackColor);
    trackDesc.Usage = D3D11_USAGE_DEFAULT;
    trackDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    trackDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
    trackDesc.StructureByteStride = sizeof(TrackColor);
    res = m_pDevice->CreateBuffer(&trackDesc, NULL, &m_pTrackColorBuffer);
    if (FAILED(res)) return make_tuple(res, "CreateBuffer (track color buffer)");

    D3D11_SHADER_RESOURCE_VIEW_DESC trackSrvDesc = {};
    trackSrvDesc.Format = DXGI_FORMAT_UNKNOWN;
    trackSrvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
    trackSrvDesc.Buffer.NumElements = MaxTrackColors * MaxChannelColors;
    res = m_pDevice->CreateShaderResourceView(m_pTrackColorBuffer.Get(), &trackSrvDesc, &m_pTrackColorSRV);
    if (FAILED(res)) return make_tuple(res, "CreateSRV (track color buffer)");

    // Create dynamic rect vertex buffer
    D3D11_BUFFER_DESC vbDesc = {};
    vbDesc.ByteWidth = RectsPerPass * 4 * sizeof(RectVertex);
    vbDesc.Usage = D3D11_USAGE_DYNAMIC;
    vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    res = m_pDevice->CreateBuffer(&vbDesc, NULL, &m_pRectVertexBuffer);
    if (FAILED(res)) return make_tuple(res, "CreateBuffer (rect vertex buffer)");

    // Create dynamic note structured buffer
    D3D11_BUFFER_DESC noteDesc = {};
    noteDesc.ByteWidth = NotesPerPass * sizeof(NoteData);
    noteDesc.Usage = D3D11_USAGE_DYNAMIC;
    noteDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    noteDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    noteDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
    noteDesc.StructureByteStride = sizeof(NoteData);
    res = m_pDevice->CreateBuffer(&noteDesc, NULL, &m_pNoteBuffer);
    if (FAILED(res)) return make_tuple(res, "CreateBuffer (note buffer)");

    D3D11_SHADER_RESOURCE_VIEW_DESC noteSrvDesc = {};
    noteSrvDesc.Format = DXGI_FORMAT_UNKNOWN;
    noteSrvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
    noteSrvDesc.Buffer.NumElements = NotesPerPass;
    res = m_pDevice->CreateShaderResourceView(m_pNoteBuffer.Get(), &noteSrvDesc, &m_pNoteSRV);
    if (FAILED(res)) return make_tuple(res, "CreateSRV (note buffer)");

    // Create index buffer
    vector<uint32_t> index_buffer_vec(IndexBufferCount);
    for (uint32_t i = 0; i < IndexBufferCount / 6; i++) {
        index_buffer_vec[i * 6] = i * 4;
        index_buffer_vec[i * 6 + 1] = i * 4 + 1;
        index_buffer_vec[i * 6 + 2] = i * 4 + 2;
        index_buffer_vec[i * 6 + 3] = i * 4;
        index_buffer_vec[i * 6 + 4] = i * 4 + 2;
        index_buffer_vec[i * 6 + 5] = i * 4 + 3;
    }

    D3D11_BUFFER_DESC ibDesc = {};
    ibDesc.ByteWidth = IndexBufferCount * sizeof(uint32_t);
    ibDesc.Usage = D3D11_USAGE_IMMUTABLE;
    ibDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    D3D11_SUBRESOURCE_DATA ibData = { index_buffer_vec.data() };
    res = m_pDevice->CreateBuffer(&ibDesc, &ibData, &m_pIndexBuffer);
    if (FAILED(res)) return make_tuple(res, "CreateBuffer (index buffer)");

    // Make the swap chain
    auto res2 = CreateWindowDependentObjects(hWnd);
    if (FAILED(get<0>(res2)))
        return res2;

    return make_tuple(S_OK, "");
}

tuple<HRESULT, const char*> Renderer11::CreateWindowDependentObjects(HWND hWnd) {
    HRESULT res;
    if (m_pSwapChain) {
        // Release window-dependent objects
        m_pContext->OMSetRenderTargets(0, nullptr, nullptr);
        m_pContext->Flush();
        m_pRenderTargetView.Reset();
        m_pBackBuffer.Reset();
        m_pBackBufferSurface.Reset();
        m_pDepthStencilView.Reset();
        m_pDepthBuffer.Reset();

        // Resize the swap chain
        res = m_pSwapChain->ResizeBuffers(1, 0, 0, DXGI_FORMAT_B8G8R8A8_UNORM, DXGI_SWAP_CHAIN_FLAG_GDI_COMPATIBLE);
        if (FAILED(res))
            return make_tuple(res, "ResizeBuffers");
    }
    else {
        // Create swap chain with GDI compatibility
        DXGI_SWAP_CHAIN_DESC scd = {};
        scd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
        scd.SampleDesc.Count = 1;
        scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        scd.BufferCount = 1;
        scd.OutputWindow = hWnd;
        scd.Windowed = TRUE;
        scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
        scd.Flags = DXGI_SWAP_CHAIN_FLAG_GDI_COMPATIBLE;
        res = m_pFactory->CreateSwapChain(m_pDevice.Get(), &scd, &m_pSwapChain);
        if (FAILED(res))
            return make_tuple(res, "CreateSwapChain");

        // Disable ALT+ENTER
        // TODO: Make fullscreen work
        m_pFactory->MakeWindowAssociation(hWnd, DXGI_MWA_NO_ALT_ENTER);
    }

    // Get backbuffer and its GDI-compatible surface
    res = m_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&m_pBackBuffer));
    if (FAILED(res))
        return make_tuple(res, "GetBuffer");
    res = m_pBackBuffer.As(&m_pBackBufferSurface);
    if (FAILED(res))
        return make_tuple(res, "QueryInterface (IDXGISurface1)");

    // Read backbuffer width and height
    D3D11_TEXTURE2D_DESC bbDesc = {};
    m_pBackBuffer->GetDesc(&bbDesc);
    m_iBufferWidth = static_cast<win32_t>(bbDesc.Width);
    m_iBufferHeight = static_cast<win32_t>(bbDesc.Height);

    // Create render target view
    res = m_pDevice->CreateRenderTargetView(m_pBackBuffer.Get(), nullptr, &m_pRenderTargetView);
    if (FAILED(res))
        return make_tuple(res, "CreateRenderTargetView");

    // Create depth buffer
    D3D11_TEXTURE2D_DESC depthDesc = {};
    depthDesc.Width = m_iBufferWidth;
    depthDesc.Height = m_iBufferHeight;
    depthDesc.MipLevels = 1;
    depthDesc.ArraySize = 1;
    depthDesc.Format = DXGI_FORMAT_D32_FLOAT;
    depthDesc.SampleDesc.Count = 1;
    depthDesc.Usage = D3D11_USAGE_DEFAULT;
    depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    res = m_pDevice->CreateTexture2D(&depthDesc, NULL, &m_pDepthBuffer);
    if (FAILED(res))
        return make_tuple(res, "CreateTexture2D (depth buffer)");
    res = m_pDevice->CreateDepthStencilView(m_pDepthBuffer.Get(), NULL, &m_pDepthStencilView);
    if (FAILED(res))
        return make_tuple(res, "CreateDepthStencilView");

    // Resize screenshot output buffer (BGR24)
    m_vScreenshotOutput.resize(static_cast<size_t>(m_iBufferWidth) * m_iBufferHeight * ScreenshotBytesPerPixel);

    // Create background image texture
    m_pBackgroundTexture.Reset();
    m_pBackgroundTextureSRV.Reset();
    D3D11_TEXTURE2D_DESC bgDesc = {};
    bgDesc.Width = m_iBufferWidth;
    bgDesc.Height = m_iBufferHeight;
    bgDesc.MipLevels = 1;
    bgDesc.ArraySize = 1;
    bgDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    bgDesc.SampleDesc.Count = 1;
    bgDesc.Usage = D3D11_USAGE_DEFAULT;
    bgDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    res = m_pDevice->CreateTexture2D(&bgDesc, NULL, &m_pBackgroundTexture);
    if (FAILED(res))
        return make_tuple(res, "CreateTexture2D (background texture)");
    res = m_pDevice->CreateShaderResourceView(m_pBackgroundTexture.Get(), nullptr, &m_pBackgroundTextureSRV);
    if (FAILED(res))
        return make_tuple(res, "CreateSRV (background texture)");

    // Scale and upload background image
    UploadBackgroundBitmap();

    // Set up root constants
    // https://github.com/ocornut/imgui/blob/master/backends/imgui_impl_dx12.cpp#L99
    float L = 0;
    float R = (float)m_iBufferWidth;
    float T = 0;
    float B = (float)m_iBufferHeight;
    float mvp[4][4] = {
        {2.0f / (R - L), 0.0f, 0.0f, 0.0f},
        {0.0f, 2.0f / (T - B), 0.0f, 0.0f},
        {0.0f, 0.0f, 0.5f, 0.0f},
        {(R + L) / (L - R), (T + B) / (B - T), 0.5f, 1.0f},
    };
    memcpy(m_RootConstants.proj, mvp, sizeof(mvp));

    return make_tuple(S_OK, "");
}

HRESULT Renderer11::ResetDeviceIfNeeded() {
    HRESULT hr = m_pDevice->GetDeviceRemovedReason();
    if (FAILED(hr)) { return ResetDevice(); }
    return S_OK;
}

HRESULT Renderer11::ResetDevice() {
    auto res = CreateWindowDependentObjects(m_hWnd);
    if (FAILED(get<0>(res)))
        return get<0>(res);
    return S_OK;
}

HRESULT Renderer11::ClearAndBeginScene(DWORD color) {
    // Clear the intermediate buffers
    m_vRectsIntermediate.clear();
    m_vNotesIntermediate.clear();
    m_vTextCommands.clear();
    m_iRectSplit = -1;

    // Upload constant buffer
    D3D11_MAPPED_SUBRESOURCE mapped;
    if (SUCCEEDED(m_pContext->Map(m_pConstantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped))) {
        memcpy(mapped.pData, &m_RootConstants, sizeof(m_RootConstants));
        m_pContext->Unmap(m_pConstantBuffer.Get(), 0);
    }
    m_pContext->VSSetConstantBuffers(0, 1, m_pConstantBuffer.GetAddressOf());
    m_pContext->PSSetConstantBuffers(0, 1, m_pConstantBuffer.GetAddressOf());

    // Upload fixed constants if changed
    if (memcmp(&m_FixedConstants, &m_OldFixedConstants, sizeof(FixedSizeConstants))) {
        memcpy(&m_OldFixedConstants, &m_FixedConstants, sizeof(FixedSizeConstants));
        m_pContext->UpdateSubresource(m_pFixedBuffer.Get(), 0, NULL, &m_FixedConstants, 0, 0);
    }
    if (memcmp(&m_TrackColors, &m_OldTrackColors, sizeof(m_TrackColors))) {
        memcpy(&m_OldTrackColors, &m_TrackColors, sizeof(m_TrackColors));
        m_pContext->UpdateSubresource(m_pTrackColorBuffer.Get(), 0, NULL, &m_TrackColors, 0, 0);
    }

    // Set up render state
    m_pContext->OMSetRenderTargets(1, m_pRenderTargetView.GetAddressOf(), m_pDepthStencilView.Get());
    D3D11_VIEWPORT vp = { 0, 0, (float)m_iBufferWidth, (float)m_iBufferHeight, 0.0f, 1.0f };
    m_pContext->RSSetViewports(1, &vp);
    m_pContext->RSSetState(m_pRasterizerState.Get());
    float bf[4] = {};
    m_pContext->OMSetBlendState(m_pBlendState.Get(), bf, 0xFFFFFFFF);
    m_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    m_pContext->IASetIndexBuffer(m_pIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

    // Send a clear render target command
    float float_color[4] = { (float)((color >> 16) & 0xFF) / 255.0f, (float)((color >> 8) & 0xFF) / 255.0f, (float)(color & 0xFF) / 255.0f, 1.0f };
    if (!Config::GetConfig().GetVideoSettings().bSameWidth || Config::GetConfig().GetVideoSettings().bOR) {
        m_pContext->ClearDepthStencilView(m_pDepthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
    }
    m_pContext->ClearRenderTargetView(m_pRenderTargetView.Get(), float_color);

    return S_OK;
}

__forceinline void Renderer11::AutoSetNotePipeline(bool inloop) {
    static Config& config = Config::GetConfig();
    static const VideoSettings& cVideo = config.GetVideoSettings();
    if (cVideo.bSameWidth) {
        if (cVideo.bOR) {
            SetPipeline(Pipeline::SameWidthNoteOR);
        }
        else {
            SetPipeline(Pipeline::SameWidthNote);
        }
    }
    else {
        if (cVideo.bOR) {
            SetPipeline(Pipeline::NoteOR);
        }
        else {
            SetPipeline(Pipeline::Note);
        }
    }
}

__forceinline void Renderer11::AutoSetRectPipeline(bool inloop) {
    SetPipeline(Pipeline::Rect);
}

HRESULT Renderer11::DrawRectRange(size_t startVert, size_t endVert) {
    if (startVert >= endVert) return S_OK;
    SetPipeline(Pipeline::Rect);
    HRESULT res = S_OK;
    for (size_t i = startVert; i < endVert; i += (size_t)RectsPerPass * 4) {
        size_t count = min(endVert - i, (size_t)RectsPerPass * 4);

        D3D11_MAPPED_SUBRESOURCE mapped;
        res = m_pContext->Map(m_pRectVertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
        if (FAILED(res)) return res;
        memcpy(mapped.pData, m_vRectsIntermediate.data() + i, count * sizeof(RectVertex));
        m_pContext->Unmap(m_pRectVertexBuffer.Get(), 0);

        UINT stride = sizeof(RectVertex), offset = 0;
        m_pContext->IASetVertexBuffers(0, 1, m_pRectVertexBuffer.GetAddressOf(), &stride, &offset);
        m_pContext->DrawIndexed((UINT)(count / 4 * 6), 0, 0);
    }
    return res;
}

HRESULT Renderer11::EndScene(bool draw_bg) {
    HRESULT res = S_OK;

    // Draw background
    if (draw_bg) {
        SetPipeline(Pipeline::Background);
        m_pContext->Draw(3, 0);
    }

    // Flush the intermediate rect buffer
    size_t rect_count = m_vRectsIntermediate.size();
    size_t rect_split = min(m_iRectSplit < 0 ? rect_count : (size_t)m_iRectSplit, rect_count);

    // First rect batch (before notes)
    res = DrawRectRange(0, rect_split);
    if (FAILED(res)) return res;

    // Flush the intermediate note buffer
    if (!m_vNotesIntermediate.empty()) {
        for (idx_t i = 0; i < m_vNotesIntermediate.size(); i += NotesPerPass) {
            if (i == 0) AutoSetNotePipeline();

            auto remaining = m_vNotesIntermediate.size() - i;
            auto note_count = min(remaining, (size_t)NotesPerPass);

            // Unbind SRV before mapping (DX11 best practice)
            ID3D11ShaderResourceView* null_srv = nullptr;
            m_pContext->VSSetShaderResources(3, 1, &null_srv);

            D3D11_MAPPED_SUBRESOURCE mapped;
            res = m_pContext->Map(m_pNoteBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
            if (FAILED(res)) return res;
            memcpy(mapped.pData, &m_vNotesIntermediate[i], note_count * sizeof(NoteData));
            m_pContext->Unmap(m_pNoteBuffer.Get(), 0);

            // Rebind SRV
            m_pContext->VSSetShaderResources(3, 1, m_pNoteSRV.GetAddressOf());

            // Draw the notes
            m_pContext->DrawIndexed((UINT)(note_count * 6), 0, 0);
        }
    }

    // Draw the second rect batch (after notes)
    res = DrawRectRange(rect_split, rect_count);
    if (FAILED(res)) return res;

    // Unbind render target before acquiring GDI DC
    ID3D11RenderTargetView* nullRT = nullptr;
    m_pContext->OMSetRenderTargets(1, &nullRT, nullptr);
    m_pContext->Flush();

    // Flush queued text last — GDI draws on top of everything
    return FlushText();
}

HRESULT Renderer11::Present() {
    return m_pSwapChain->Present(m_bLimitFPS ? 1 : 0, 0);
}

HRESULT Renderer11::DrawRect(float x, float y, float cx, float cy, DWORD color, float flipcenter, bool flip) {
    return DrawRect(x, y, cx, cy, color, color, color, color, flipcenter, flip);
}

HRESULT Renderer11::DrawRect(float x, float y, float cx, float cy, DWORD c1, DWORD c2, DWORD c3, DWORD c4, float flipcenter, bool flip) {
    m_vRectsIntermediate.insert(m_vRectsIntermediate.end(), {
        {x, y, c1},
        {x + cx, y, c2},
        {x + cx, y + cy, c3},
        {x, y + cy, c4},
        });
    return S_OK;
}

HRESULT Renderer11::DrawSkew(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, DWORD color, float flipcenter, bool flip) {
    return DrawSkew(x1, y1, x2, y2, x3, y3, x4, y4, color, color, color, color, flipcenter, flip);
}

HRESULT Renderer11::DrawSkew(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, DWORD c1, DWORD c2, DWORD c3, DWORD c4, float flipcenter, bool flip) {
    m_vRectsIntermediate.insert(m_vRectsIntermediate.end(), {
        {x1, y1, c1},
        {x2, y2, c2},
        {x3, y3, c3},
        {x4, y4, c4},
        });
    return S_OK;
}

void Renderer11::SetPipeline(Pipeline pipeline) {
    switch (pipeline) {
    case Pipeline::Rect:
        m_pContext->VSSetShader(m_pRectVS.Get(), NULL, 0);
        m_pContext->PSSetShader(m_pRectPS.Get(), NULL, 0);
        m_pContext->IASetInputLayout(m_pRectInputLayout.Get());
        m_pContext->OMSetDepthStencilState(m_pDepthDisabledState.Get(), 0);
        break;
    case Pipeline::Note:
        m_pContext->VSSetShader(m_pNoteVS.Get(), NULL, 0);
        m_pContext->PSSetShader(m_pNotePS.Get(), NULL, 0);
        m_pContext->IASetInputLayout(NULL);
        m_pContext->OMSetDepthStencilState(m_pDepthLessEqualState.Get(), 0);
        m_pContext->VSSetShaderResources(1, 1, m_pFixedSRV.GetAddressOf());
        m_pContext->VSSetShaderResources(2, 1, m_pTrackColorSRV.GetAddressOf());
        m_pContext->VSSetShaderResources(3, 1, m_pNoteSRV.GetAddressOf());
        break;
    case Pipeline::SameWidthNote:
        m_pContext->VSSetShader(m_pNoteSameWidthVS.Get(), NULL, 0);
        m_pContext->PSSetShader(m_pNotePS.Get(), NULL, 0);
        m_pContext->IASetInputLayout(NULL);
        m_pContext->OMSetDepthStencilState(m_pDepthDisabledState.Get(), 0);
        m_pContext->VSSetShaderResources(1, 1, m_pFixedSRV.GetAddressOf());
        m_pContext->VSSetShaderResources(2, 1, m_pTrackColorSRV.GetAddressOf());
        m_pContext->VSSetShaderResources(3, 1, m_pNoteSRV.GetAddressOf());
        break;
    case Pipeline::NoteOR:
        m_pContext->VSSetShader(m_pNoteVS.Get(), NULL, 0);
        m_pContext->PSSetShader(m_pNotePS.Get(), NULL, 0);
        m_pContext->IASetInputLayout(NULL);
        m_pContext->OMSetDepthStencilState(m_pDepthLessState.Get(), 0);
        m_pContext->VSSetShaderResources(1, 1, m_pFixedSRV.GetAddressOf());
        m_pContext->VSSetShaderResources(2, 1, m_pTrackColorSRV.GetAddressOf());
        m_pContext->VSSetShaderResources(3, 1, m_pNoteSRV.GetAddressOf());
        break;
    case Pipeline::SameWidthNoteOR:
        m_pContext->VSSetShader(m_pNoteSameWidthVS.Get(), NULL, 0);
        m_pContext->PSSetShader(m_pNotePS.Get(), NULL, 0);
        m_pContext->IASetInputLayout(NULL);
        m_pContext->OMSetDepthStencilState(m_pDepthLessState.Get(), 0);
        m_pContext->VSSetShaderResources(1, 1, m_pFixedSRV.GetAddressOf());
        m_pContext->VSSetShaderResources(2, 1, m_pTrackColorSRV.GetAddressOf());
        m_pContext->VSSetShaderResources(3, 1, m_pNoteSRV.GetAddressOf());
        break;
    case Pipeline::Background:
        m_pContext->VSSetShader(m_pBackgroundVS.Get(), NULL, 0);
        m_pContext->PSSetShader(m_pBackgroundPS.Get(), NULL, 0);
        m_pContext->IASetInputLayout(NULL);
        m_pContext->OMSetDepthStencilState(m_pDepthDisabledState.Get(), 0);
        m_pContext->PSSetShaderResources(0, 1, m_pBackgroundTextureSRV.GetAddressOf());
        m_pContext->PSSetSamplers(0, 1, m_pBackgroundSampler.GetAddressOf());
        break;
    }
}

//-----------------------------------------------------------------------------
// Text rendering
//-----------------------------------------------------------------------------

void Renderer11::AddText(const wstring& Text, win32_t Size, win32_t X, win32_t Y, COLORREF Color, DWORD Alignment, win32_t PadX, win32_t PadY, COLORREF bgColor) {
    m_vTextCommands.push_back({Text, Size, X, Y, Color, Alignment, PadX, PadY, bgColor});
}

SIZE Renderer11::CalcTextSize(const wstring& Text, win32_t Size) {
    SIZE Result = { 0, 0 };

    HDC DC = GetDC(m_hWnd);
    HFONT imguiFont = imguiFont2GDI(PHIFON_compressed_data, PHIFON_compressed_size, Size);
    HFONT OldFont = (HFONT)SelectObject(DC, imguiFont);

    GetTextExtentPoint32W(DC, Text.c_str(), Text.size(), &Result);

    SelectObject(DC, OldFont);
    ReleaseDC(m_hWnd, DC);

    return Result;
}

void Renderer11::AddGDIRect(win32_t X, win32_t Y, win32_t W, win32_t H, COLORREF Color) {
    m_vTextCommands.push_back({ L"", 0, X+W/2, Y+H/2, 0x00000000, ALIGN_CENTER|ALIGN_MIDDLE, W/2, H/2, Color});
}

HRESULT Renderer11::FlushText() {
    if (m_vTextCommands.empty()) return S_OK;

    HDC DXDC = NULL;
    HRESULT res = m_pBackBufferSurface->GetDC(FALSE, &DXDC);
    if (FAILED(res)) return res;

    SetBkMode(DXDC, TRANSPARENT);

    for (const TextCommand& CMD : m_vTextCommands) {
        HFONT imguiFont = (CMD.Text.empty() || CMD.Size<=0) ? NULL : imguiFont2GDI(PHIFON_compressed_data, PHIFON_compressed_size, CMD.Size);
        HFONT OldFont = CMD.Text.empty() ? NULL : (HFONT)SelectObject(DXDC, imguiFont);
        SetTextColor(DXDC, CMD.Color & 0x00FFFFFF);

        // Measure text for alignment
        SIZE TextSize = {0,0};
        if (!CMD.Text.empty()) {
            GetTextExtentPoint32W(DXDC, CMD.Text.c_str(), CMD.Text.size(), &TextSize);
        }

        // Apply horizontal alignment
        win32_t TextX;
        switch (CMD.Alignment & 0xFF00) {
        default:
        case ALIGN_LEFT: TextX = CMD.X; break;
        case ALIGN_CENTER: TextX = CMD.X - TextSize.cx / 2; break;
        case ALIGN_RIGHT: TextX = CMD.X - TextSize.cx; break;
        }

        // Apply vertical alignment
        win32_t TextY;
        switch (CMD.Alignment & 0x00FF) {
        default:
        case ALIGN_TOP: TextY = CMD.Y; break;
        case ALIGN_MIDDLE: TextY = CMD.Y - TextSize.cy / 2; break;
        case ALIGN_BOTTOM: TextY = CMD.Y - TextSize.cy; break;
        }

        if (CMD.bgColor & 0xFF000000) {
            HDC MEMdc = CreateCompatibleDC(DXDC);
            HBITMAP BMP = CreateCompatibleBitmap(DXDC, 1, 1);
            SelectObject(MEMdc, BMP);
            HBRUSH Brush = CreateSolidBrush(CMD.bgColor & 0x00FFFFFF);
            SelectObject(MEMdc, Brush);
            PatBlt(MEMdc, 0, 0, 1, 1, PATCOPY);
            union
            {
                DWORD Bits;
                BLENDFUNCTION Func;
            } BlendFunc;
            BlendFunc.Bits = (CMD.bgColor & 0xFF000000) >> 8;
            AlphaBlend(DXDC, TextX - CMD.PadX, TextY - CMD.PadY, TextSize.cx + 2 * CMD.PadX, TextSize.cy + 2 * CMD.PadY, MEMdc, 0, 0, 1, 1, BlendFunc.Func);
            DeleteDC(MEMdc);
            DeleteObject(BMP);
            DeleteObject(Brush);
        }
        if (!CMD.Text.empty()) {
            TextOutW(DXDC, TextX, TextY, CMD.Text.c_str(), CMD.Text.size());
            SelectObject(DXDC, OldFont);
        }
    }

    m_vTextCommands.clear();
    m_pBackBufferSurface->ReleaseDC(nullptr);

    return S_OK;
}

//-----------------------------------------------------------------------------
// Screenshot (BitBlt from back buffer surface, BGR24 output)
//-----------------------------------------------------------------------------

HRESULT Renderer11::Screenshot(char* Output) {
    if (!m_pBackBufferSurface) return E_FAIL;

    // Back buffer is already flushed and RT unbound from FlushText
    HDC DXDC = NULL;
    HRESULT hr = m_pBackBufferSurface->GetDC(FALSE, &DXDC);
    if (FAILED(hr)) return hr;

    HDC MEMdc = CreateCompatibleDC(DXDC);
    HBITMAP BMP = CreateCompatibleBitmap(DXDC, m_iBufferWidth, m_iBufferHeight);
    SelectObject(MEMdc, BMP);
    BitBlt(MEMdc, 0, 0, m_iBufferWidth, m_iBufferHeight, DXDC, 0, 0, SRCCOPY);
    BITMAPINFO bmpInfo = {};
    bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmpInfo.bmiHeader.biWidth = m_iBufferWidth;
    bmpInfo.bmiHeader.biHeight = -m_iBufferHeight;
    bmpInfo.bmiHeader.biPlanes = 1;
    bmpInfo.bmiHeader.biBitCount = 32;
    bmpInfo.bmiHeader.biCompression = BI_RGB;
    GetDIBits(MEMdc, BMP, 0, m_iBufferHeight, Output, &bmpInfo, DIB_RGB_COLORS);
    DeleteDC(MEMdc);
    DeleteObject(BMP);

    m_pBackBufferSurface->ReleaseDC(nullptr);
    return S_OK;
}

//-----------------------------------------------------------------------------
// Background image
//-----------------------------------------------------------------------------

bool Renderer11::LoadBackgroundBitmap(wstring path) {
    // Initialize the WIC factory if it hasn't been initialized yet
    if (!s_pWICFactory) {
        if (FAILED(CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&s_pWICFactory))))
            return false;
    }

    // Make a decoder for the image
    ComPtr<IWICBitmapDecoder> decoder;
    if (FAILED(s_pWICFactory->CreateDecoderFromFilename(path.c_str(), NULL, GENERIC_READ, WICDecodeMetadataCacheOnLoad, &decoder)))
        return false;

    // Decode the first frame of the image
    ComPtr<IWICBitmapFrameDecode> frame;
    if (FAILED(decoder->GetFrame(0, &frame)))
        return false;

    // Create a WIC format converter
    ComPtr<IWICFormatConverter> converter;
    if (FAILED(s_pWICFactory->CreateFormatConverter(&converter)))
        return false;

    // Get the loaded image's pixel format
    WICPixelFormatGUID orig_pixel_format;
    if (FAILED(frame->GetPixelFormat(&orig_pixel_format)))
        return false;

    // Make sure the image can be converted to RGBA8 format
    BOOL can_convert;
    if (FAILED(converter->CanConvert(orig_pixel_format, GUID_WICPixelFormat32bppRGBA, &can_convert)) || !can_convert)
        return false;

    // Run image conversion
    if (FAILED(converter->Initialize(frame.Get(), GUID_WICPixelFormat32bppRGBA, WICBitmapDitherTypeErrorDiffusion, 0, 0, WICBitmapPaletteTypeCustom)))
        return false;

    // Store the converted image
    if (FAILED(converter.As(&m_pUnscaledBackground)))
        return false;

    // Upload the new background to the GPU
    return UploadBackgroundBitmap();
}

bool Renderer11::UploadBackgroundBitmap() {
    // Don't bother if an image wasn't loaded in the first place
    if (!m_pUnscaledBackground || !m_pBackgroundTexture)
        return false;

    // Create a WIC bitmap scaler
    ComPtr<IWICBitmapScaler> scaler;
    if (FAILED(s_pWICFactory->CreateBitmapScaler(&scaler)))
        return false;

    // Resize the image to the desired resolution
    if (FAILED(scaler->Initialize(m_pUnscaledBackground.Get(), m_iBufferWidth, m_iBufferHeight, WICBitmapInterpolationModeHighQualityCubic)))
        return false;

    // Copy the scaled bitmap to a new buffer
    vector<BYTE> scaled;
    scaled.resize(m_iBufferWidth * m_iBufferHeight * 4);
    if (FAILED(scaler->CopyPixels(NULL, m_iBufferWidth * 4, scaled.size(), scaled.data())))
        return false;

    // Upload to GPU
    m_pContext->UpdateSubresource(m_pBackgroundTexture.Get(), 0, NULL, scaled.data(), m_iBufferWidth * 4, 0);

    return true;
}
