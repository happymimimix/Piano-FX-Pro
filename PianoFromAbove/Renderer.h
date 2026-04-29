/*************************************************************************************************
*
> File: Renderer.h
*
> Description: Defines rendering objects. Only one for now.
*
> Copyright (c) 2010 Brian Pantano. All rights reserved.
*
*************************************************************************************************/
#pragma once

#include <Windows.h>
#include <d3d11.h>
#include <dxgi.h>
#include <wrl/client.h>
#include <string>
#include <vector>
#include <wincodec.h>
#include <Fonts.h>
#include <imguiCompressedFont2GDI.h>
#include <Misc.h>

using Microsoft::WRL::ComPtr;

// Text alignment flags
constexpr DWORD ALIGN_LEFT   = 0x0100;
constexpr DWORD ALIGN_CENTER = 0x0200;
constexpr DWORD ALIGN_RIGHT  = 0x0300;
constexpr DWORD ALIGN_TOP    = 0x0001;
constexpr DWORD ALIGN_MIDDLE = 0x0002;
constexpr DWORD ALIGN_BOTTOM = 0x0003;

enum class Pipeline : uint8_t {
    Rect,
    Note,
    SameWidthNote,
    NoteOR,
    SameWidthNoteOR,
    Background,
};

struct RectVertex {
    float x;
    float y;
    DWORD color;
};

struct NoteData {
    uint8_t key;
    uint8_t channel;
    uint16_t track;
    float pos;
    float length;
};
static_assert(sizeof(NoteData) == 12);

struct TrackColor {
    DWORD primary;
    DWORD dark;
    DWORD darker;
};

struct RootConstants {
    float proj[4][4];
    float deflate;
    float notes_y;
    float notes_cy;
    float white_cx;
    float timespan;
};
static_assert(sizeof(RootConstants) % 4 == 0);

struct FixedSizeConstants {
    float note_x[128];
    float bends[16];
};

#ifdef LIMIT_COLORS
constexpr unsigned long MaxTrackColors = 1<<8; //Limit track color for faster rendering
#else
constexpr unsigned long MaxTrackColors = 0xFFFF; //Theoretic maximum number of tracks possible in MIDI format
#endif
constexpr unsigned long MaxChannelColors = 1<<4; //Theoretic maximum number of channels possible in MIDI format

class Renderer11 {
public:
    Renderer11() {
        PtrToIsWrapRenderer = &m_bSoftware;
    }
    ~Renderer11() {
        imguiClearFontCache();
        PtrToIsWrapRenderer = nullptr;
    }

    tuple<HRESULT, const char*> Init(HWND hWnd, bool bLimitFPS);
    HRESULT ResetDeviceIfNeeded();
    HRESULT ResetDevice();
    HRESULT ClearAndBeginScene(DWORD color);
    HRESULT EndScene(bool draw_bg = false);
    HRESULT Present();
    void AddText(const wstring& Text, win32_t Size, win32_t X, win32_t Y, COLORREF Color, DWORD Alignment = ALIGN_LEFT|ALIGN_TOP, win32_t OffsetX = 0, win32_t OffsetY = 0, win32_t PadX = 0, win32_t PadY = 0, COLORREF bgColor=0x00000000);
    SIZE CalcTextSize(const wstring& Text, win32_t Size);
    void AddGDIRect(win32_t X, win32_t Y, win32_t W, win32_t H, COLORREF Color);
    HRESULT DrawRect(float x, float y, float cx, float cy, DWORD color, float flipcenter = 0.0f, bool flip = false);
    HRESULT DrawRect(float x, float y, float cx, float cy, DWORD c1, DWORD c2, DWORD c3, DWORD c4, float flipcenter = 0.0f, bool flip = false);
    HRESULT DrawSkew(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, DWORD color, float flipcenter = 0.0f, bool flip = false);
    HRESULT DrawSkew(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, DWORD c1, DWORD c2, DWORD c3, DWORD c4, float flipcenter = 0.0f, bool flip = false);

    bool GetLimitFPS() const { return m_bLimitFPS; }
    void SetLimitFPS(bool bLimitFPS) { m_bLimitFPS = bLimitFPS; }

    win32_t GetBufferWidth() const { return m_iBufferWidth; }
    win32_t GetBufferHeight() const { return m_iBufferHeight; }

    void SetPipeline(Pipeline pipeline);
    __forceinline void AutoSetNotePipeline(bool inloop = false);
    __forceinline void AutoSetRectPipeline(bool inloop = false);

    RootConstants& GetRootConstants() { return m_RootConstants; };
    FixedSizeConstants& GetFixedSizeConstants() { return m_FixedConstants; };
    TrackColor* GetTrackColors() { return m_TrackColors; };

    inline void PushNoteData(NoteData data) { m_vNotesIntermediate.push_back(data); };
    idx_t GetRenderedNotesCount() { return static_cast<idx_t>(m_vNotesIntermediate.size()); };
    void SplitRect() { m_iRectSplit = static_cast<sidx_t>(m_vRectsIntermediate.size()); }

    HRESULT Screenshot(char* Output);
    bool LoadBackgroundBitmap(wstring path);

private:
    tuple<HRESULT, const char*> CreateWindowDependentObjects(HWND hWnd);
    HRESULT DrawRectRange(size_t startVert, size_t endVert);
    HRESULT FlushText();
    bool UploadBackgroundBitmap();

    static constexpr unsigned RectsPerPass = 1 << (1 << 4);
    static constexpr unsigned NotesPerPass = 1 << (1 << 4) << 4;
    static constexpr unsigned IndexBufferCount = max(RectsPerPass, NotesPerPass) * 6;

    static ComPtr<IWICImagingFactory> s_pWICFactory;

    win32_t m_iBufferWidth = 0;
    win32_t m_iBufferHeight = 0;
    bool m_bLimitFPS = false;
    bool m_bSoftware = false;

    HWND m_hWnd = NULL;
    ComPtr<IDXGIFactory1> m_pFactory;
    ComPtr<ID3D11Device> m_pDevice;
    ComPtr<ID3D11DeviceContext> m_pContext;
    ComPtr<IDXGISwapChain> m_pSwapChain;
    ComPtr<ID3D11Texture2D> m_pBackBuffer;
    ComPtr<IDXGISurface1> m_pBackBufferSurface;
    ComPtr<ID3D11RenderTargetView> m_pRenderTargetView;
    ComPtr<ID3D11Texture2D> m_pDepthBuffer;
    ComPtr<ID3D11DepthStencilView> m_pDepthStencilView;
    ComPtr<ID3D11VertexShader> m_pRectVS;
    ComPtr<ID3D11PixelShader> m_pRectPS;
    ComPtr<ID3D11InputLayout> m_pRectInputLayout;
    ComPtr<ID3D11VertexShader> m_pNoteVS;
    ComPtr<ID3D11VertexShader> m_pNoteSameWidthVS;
    ComPtr<ID3D11PixelShader> m_pNotePS;
    ComPtr<ID3D11VertexShader> m_pBackgroundVS;
    ComPtr<ID3D11PixelShader> m_pBackgroundPS;
    ComPtr<ID3D11BlendState> m_pBlendState;
    ComPtr<ID3D11RasterizerState> m_pRasterizerState;
    ComPtr<ID3D11DepthStencilState> m_pDepthDisabledState;
    ComPtr<ID3D11DepthStencilState> m_pDepthLessEqualState;
    ComPtr<ID3D11DepthStencilState> m_pDepthLessState;
    ComPtr<ID3D11SamplerState> m_pBackgroundSampler;
    ComPtr<ID3D11Buffer> m_pIndexBuffer;
    ComPtr<ID3D11Buffer> m_pRectVertexBuffer;
    ComPtr<ID3D11Buffer> m_pNoteBuffer;
    ComPtr<ID3D11Buffer> m_pConstantBuffer;
    ComPtr<ID3D11Buffer> m_pFixedBuffer;
    ComPtr<ID3D11Buffer> m_pTrackColorBuffer;
    ComPtr<ID3D11ShaderResourceView> m_pFixedSRV;
    ComPtr<ID3D11ShaderResourceView> m_pTrackColorSRV;
    ComPtr<ID3D11ShaderResourceView> m_pNoteSRV;

    RootConstants m_RootConstants = {};
    FixedSizeConstants m_FixedConstants = {};
    FixedSizeConstants m_OldFixedConstants = {};
    TrackColor m_TrackColors[MaxTrackColors * MaxChannelColors] = {};
    TrackColor m_OldTrackColors[MaxTrackColors * MaxChannelColors] = {};

    ComPtr<ID3D11Texture2D> m_pBackgroundTexture;
    ComPtr<ID3D11ShaderResourceView> m_pBackgroundTextureSRV;
    ComPtr<IWICBitmapSource> m_pUnscaledBackground;

    vector<RectVertex> m_vRectsIntermediate;
    vector<NoteData> m_vNotesIntermediate;
    sidx_t m_iRectSplit = -1;
    
    struct TextCommand {
        wstring Text;
        win32_t Size;
        win32_t X;
        win32_t Y;
        COLORREF Color;
        DWORD Alignment;
        win32_t OffsetX;
        win32_t OffsetY;
        win32_t PadX;
        win32_t PadY;
        COLORREF bgColor;
    };
    vector<TextCommand> m_vTextCommands;
};
