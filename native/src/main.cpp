#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0A00
#endif

#include <windows.h>
#include <windowsx.h>
#include <dwmapi.h>

#include <algorithm>
#include <cstring>
#include <string>

namespace
{
constexpr wchar_t kMainClass[] = L"CymriseColorPickerMain";
constexpr wchar_t kOverlayClass[] = L"CymriseColorPickerOverlay";
constexpr wchar_t kLensClass[] = L"CymriseColorPickerLens";
constexpr wchar_t kWindowTitle[] = L"Cymrise Color Picker";

constexpr COLORREF kInk = RGB(15, 23, 42);
constexpr COLORREF kCanvas = RGB(248, 250, 252);
constexpr COLORREF kMuted = RGB(100, 116, 139);
constexpr COLORREF kBlue = RGB(37, 99, 235);
constexpr COLORREF kBlueSoft = RGB(239, 246, 255);
constexpr COLORREF kGreen = RGB(5, 150, 105);

HINSTANCE gInstance = nullptr;
HWND gMainWindow = nullptr;
HWND gOverlayWindow = nullptr;
HWND gLensWindow = nullptr;
HDC gDesktopDc = nullptr;
RECT gVirtualBounds{};
POINT gLastCursor{};
COLORREF gSelectedColor = RGB(52, 52, 52);
bool gPicking = false;
std::wstring gStatus = L"就绪，可开始取色";

struct UiLayout
{
    RECT header{};
    RECT card{};
    RECT swatch{};
    RECT hexCaption{};
    RECT hexValue{};
    RECT rgbCaption{};
    RECT rgbValue{};
    RECT copyHex{};
    RECT copyRgb{};
    RECT pickButton{};
    RECT hint{};
    RECT status{};
    RECT footer{};
};

int Px(HWND window, int dip)
{
    const UINT dpi = window ? GetDpiForWindow(window) : 96;
    return MulDiv(dip, static_cast<int>(dpi), 96);
}

RECT MakeRect(int left, int top, int width, int height)
{
    return RECT{left, top, left + width, top + height};
}

bool Contains(const RECT& rect, POINT point)
{
    return PtInRect(&rect, point) != FALSE;
}

std::wstring ToHex(COLORREF color)
{
    wchar_t text[16]{};
    swprintf_s(text, L"#%02X%02X%02X", GetRValue(color), GetGValue(color), GetBValue(color));
    return text;
}

std::wstring ToRgb(COLORREF color)
{
    wchar_t text[40]{};
    swprintf_s(text, L"RGB(%u, %u, %u)", GetRValue(color), GetGValue(color), GetBValue(color));
    return text;
}

HFONT MakeFont(HWND window, int sizeDip, int weight = FW_NORMAL)
{
    return CreateFontW(
        -Px(window, sizeDip), 0, 0, 0, weight, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
}

void DrawTextBlock(HDC dc, HWND window, const std::wstring& text, RECT rect, int sizeDip, int weight, COLORREF color, UINT format)
{
    HFONT font = MakeFont(window, sizeDip, weight);
    HGDIOBJ previous = SelectObject(dc, font);
    SetBkMode(dc, TRANSPARENT);
    SetTextColor(dc, color);
    DrawTextW(dc, text.c_str(), static_cast<int>(text.size()), &rect, format);
    SelectObject(dc, previous);
    DeleteObject(font);
}

void FillRectColor(HDC dc, const RECT& rect, COLORREF color)
{
    HBRUSH brush = CreateSolidBrush(color);
    FillRect(dc, &rect, brush);
    DeleteObject(brush);
}

void FillRoundRect(HDC dc, const RECT& rect, COLORREF fill, COLORREF border, int radius)
{
    HBRUSH brush = CreateSolidBrush(fill);
    HPEN pen = CreatePen(PS_SOLID, 1, border);
    HGDIOBJ oldBrush = SelectObject(dc, brush);
    HGDIOBJ oldPen = SelectObject(dc, pen);
    RoundRect(dc, rect.left, rect.top, rect.right, rect.bottom, radius, radius);
    SelectObject(dc, oldBrush);
    SelectObject(dc, oldPen);
    DeleteObject(brush);
    DeleteObject(pen);
}

UiLayout CalculateLayout(HWND window)
{
    RECT client{};
    GetClientRect(window, &client);

    const int margin = Px(window, 20);
    const int cardWidth = std::max(Px(window, 340), client.right - margin * 2);
    const int headerHeight = Px(window, 90);
    const int cardTop = headerHeight + Px(window, 20);
    const int cardHeight = Px(window, 144);
    const int inside = Px(window, 16);
    const int copyWidth = Px(window, 72);
    const int copyHeight = Px(window, 30);
    const int swatchSize = Px(window, 100);

    UiLayout layout{};
    layout.header = MakeRect(0, 0, client.right, headerHeight);
    layout.card = MakeRect(margin, cardTop, cardWidth, cardHeight);
    layout.swatch = MakeRect(layout.card.left + inside, layout.card.top + Px(window, 22), swatchSize, swatchSize);

    const int contentLeft = layout.swatch.right + Px(window, 18);
    const int copyLeft = layout.card.right - inside - copyWidth;
    const int valueWidth = std::max(Px(window, 100), copyLeft - Px(window, 10) - contentLeft);

    layout.hexCaption = MakeRect(contentLeft, layout.card.top + Px(window, 20), valueWidth, Px(window, 16));
    layout.hexValue = MakeRect(contentLeft, layout.card.top + Px(window, 38), valueWidth, Px(window, 30));
    layout.copyHex = MakeRect(copyLeft, layout.card.top + Px(window, 37), copyWidth, copyHeight);

    layout.rgbCaption = MakeRect(contentLeft, layout.card.top + Px(window, 80), valueWidth, Px(window, 16));
    layout.rgbValue = MakeRect(contentLeft, layout.card.top + Px(window, 98), valueWidth, Px(window, 26));
    layout.copyRgb = MakeRect(copyLeft, layout.card.top + Px(window, 96), copyWidth, copyHeight);

    layout.pickButton = MakeRect(margin, layout.card.bottom + Px(window, 22), cardWidth, Px(window, 50));
    layout.hint = MakeRect(margin, layout.pickButton.bottom + Px(window, 16), cardWidth, Px(window, 20));
    layout.status = MakeRect(margin, layout.hint.bottom + Px(window, 8), cardWidth, Px(window, 22));
    layout.footer = MakeRect(margin, layout.status.bottom + Px(window, 7), cardWidth, Px(window, 18));
    return layout;
}

bool CopyText(const std::wstring& text)
{
    for (int attempt = 0; attempt < 3; ++attempt)
    {
        if (OpenClipboard(gMainWindow))
        {
            EmptyClipboard();
            const size_t bytes = (text.size() + 1) * sizeof(wchar_t);
            HGLOBAL memory = GlobalAlloc(GMEM_MOVEABLE, bytes);
            if (memory != nullptr)
            {
                void* target = GlobalLock(memory);
                if (target != nullptr)
                {
                    std::memcpy(target, text.c_str(), bytes);
                    GlobalUnlock(memory);
                    if (SetClipboardData(CF_UNICODETEXT, memory) != nullptr)
                    {
                        CloseClipboard();
                        return true;
                    }
                }
                GlobalFree(memory);
            }
            CloseClipboard();
        }
        Sleep(25);
    }
    return false;
}

void SetStatus(const std::wstring& text)
{
    gStatus = text;
    if (gMainWindow != nullptr)
    {
        InvalidateRect(gMainWindow, nullptr, FALSE);
    }
}

void ReleaseDesktopDc()
{
    if (gDesktopDc != nullptr)
    {
        ReleaseDC(nullptr, gDesktopDc);
        gDesktopDc = nullptr;
    }
}

void RefreshVirtualBounds()
{
    gVirtualBounds.left = GetSystemMetrics(SM_XVIRTUALSCREEN);
    gVirtualBounds.top = GetSystemMetrics(SM_YVIRTUALSCREEN);
    gVirtualBounds.right = gVirtualBounds.left + GetSystemMetrics(SM_CXVIRTUALSCREEN);
    gVirtualBounds.bottom = gVirtualBounds.top + GetSystemMetrics(SM_CYVIRTUALSCREEN);
}

POINT ClampToVirtualScreen(POINT point)
{
    point.x = std::clamp(point.x, gVirtualBounds.left, gVirtualBounds.right - 1);
    point.y = std::clamp(point.y, gVirtualBounds.top, gVirtualBounds.bottom - 1);
    return point;
}

COLORREF SampleAt(POINT screenPoint)
{
    if (gDesktopDc == nullptr)
    {
        return gSelectedColor;
    }

    const POINT point = ClampToVirtualScreen(screenPoint);
    const COLORREF color = GetPixel(gDesktopDc, point.x, point.y);
    return color == CLR_INVALID ? gSelectedColor : color;
}

void PaintLens(HWND window, HDC dc)
{
    RECT client{};
    GetClientRect(window, &client);
    FillRoundRect(dc, client, RGB(17, 24, 39), RGB(51, 65, 85), Px(window, 10));

    const int padding = Px(window, 12);
    const int swatchSize = Px(window, 60);
    RECT swatch = MakeRect(padding, padding, swatchSize, swatchSize);
    FillRoundRect(dc, swatch, gSelectedColor, RGB(148, 163, 184), Px(window, 8));

    RECT caption = MakeRect(swatch.right + Px(window, 14), padding, Px(window, 100), Px(window, 18));
    DrawTextBlock(dc, window, L"当前颜色", caption, 8, FW_BOLD, RGB(148, 163, 184), DT_LEFT | DT_VCENTER | DT_SINGLELINE);

    RECT value = MakeRect(swatch.right + Px(window, 14), padding + Px(window, 22), Px(window, 110), Px(window, 30));
    DrawTextBlock(dc, window, ToHex(gSelectedColor), value, 12, FW_SEMIBOLD, RGB(255, 255, 255), DT_LEFT | DT_VCENTER | DT_SINGLELINE);
}

void PositionLens(POINT cursor)
{
    if (gLensWindow == nullptr)
    {
        return;
    }

    const int width = Px(gLensWindow, 190);
    const int height = Px(gLensWindow, 88);
    const int offset = Px(gLensWindow, 20);
    int x = cursor.x + offset;
    int y = cursor.y + offset;

    if (x + width > gVirtualBounds.right)
    {
        x = cursor.x - width - offset;
    }
    if (y + height > gVirtualBounds.bottom)
    {
        y = cursor.y - height - offset;
    }

    x = std::clamp(x, gVirtualBounds.left, gVirtualBounds.right - width);
    y = std::clamp(y, gVirtualBounds.top, gVirtualBounds.bottom - height);

    SetWindowPos(gLensWindow, HWND_TOPMOST, x, y, width, height, SWP_NOACTIVATE | SWP_SHOWWINDOW);
    InvalidateRect(gLensWindow, nullptr, FALSE);
}

void PreviewAt(POINT cursor)
{
    gLastCursor = ClampToVirtualScreen(cursor);
    gSelectedColor = SampleAt(gLastCursor);
    PositionLens(gLastCursor);
}

void StopPicking(bool revealMain)
{
    gPicking = false;

    HWND lens = gLensWindow;
    HWND overlay = gOverlayWindow;
    gLensWindow = nullptr;
    gOverlayWindow = nullptr;

    if (lens != nullptr && IsWindow(lens))
    {
        DestroyWindow(lens);
    }
    if (overlay != nullptr && IsWindow(overlay))
    {
        DestroyWindow(overlay);
    }

    ReleaseDesktopDc();

    if (revealMain && gMainWindow != nullptr)
    {
        ShowWindow(gMainWindow, SW_SHOW);
        SetForegroundWindow(gMainWindow);
        InvalidateRect(gMainWindow, nullptr, FALSE);
    }
}

void FinishPicking()
{
    if (!gPicking)
    {
        return;
    }

    gSelectedColor = SampleAt(gLastCursor);
    const std::wstring hex = ToHex(gSelectedColor);
    SetStatus(CopyText(hex) ? L"已复制 HEX：" + hex : L"已取色，可点击复制按钮重试");
    StopPicking(true);
}

void CancelPicking()
{
    if (!gPicking)
    {
        return;
    }

    SetStatus(L"已取消取色");
    StopPicking(true);
}

bool StartPicking()
{
    if (gPicking)
    {
        return true;
    }

    RefreshVirtualBounds();
    if (gVirtualBounds.right <= gVirtualBounds.left || gVirtualBounds.bottom <= gVirtualBounds.top)
    {
        SetStatus(L"无法获取桌面范围，请重试");
        return false;
    }

    ShowWindow(gMainWindow, SW_HIDE);
    DwmFlush();

    gDesktopDc = GetDC(nullptr);
    if (gDesktopDc == nullptr)
    {
        ShowWindow(gMainWindow, SW_SHOW);
        SetStatus(L"无法读取屏幕颜色，请重试");
        return false;
    }

    gOverlayWindow = CreateWindowExW(
        WS_EX_TOOLWINDOW | WS_EX_TOPMOST | WS_EX_LAYERED,
        kOverlayClass, L"", WS_POPUP,
        gVirtualBounds.left, gVirtualBounds.top,
        gVirtualBounds.right - gVirtualBounds.left,
        gVirtualBounds.bottom - gVirtualBounds.top,
        nullptr, nullptr, gInstance, nullptr);

    if (gOverlayWindow == nullptr)
    {
        ReleaseDesktopDc();
        ShowWindow(gMainWindow, SW_SHOW);
        SetStatus(L"无法启动取色，请重试");
        return false;
    }

    // The input surface is fully transparent, so it does not alter the sampled pixel.
    SetLayeredWindowAttributes(gOverlayWindow, 0, 0, LWA_ALPHA);

    gLensWindow = CreateWindowExW(
        WS_EX_NOACTIVATE | WS_EX_TOOLWINDOW | WS_EX_TOPMOST,
        kLensClass, L"", WS_POPUP,
        0, 0, Px(gOverlayWindow, 190), Px(gOverlayWindow, 88),
        nullptr, nullptr, gInstance, nullptr);

    gPicking = true;
    ShowWindow(gOverlayWindow, SW_SHOW);
    SetWindowPos(gOverlayWindow, HWND_TOPMOST,
        gVirtualBounds.left, gVirtualBounds.top,
        gVirtualBounds.right - gVirtualBounds.left,
        gVirtualBounds.bottom - gVirtualBounds.top,
        SWP_SHOWWINDOW | SWP_NOACTIVATE);
    SetForegroundWindow(gOverlayWindow);
    SetFocus(gOverlayWindow);

    GetCursorPos(&gLastCursor);
    PreviewAt(gLastCursor);
    return true;
}

void DrawButton(HDC dc, HWND window, const RECT& rect, const std::wstring& text, COLORREF background, COLORREF foreground)
{
    FillRoundRect(dc, rect, background, background, Px(window, 9));
    DrawTextBlock(dc, window, text, rect, 9, FW_SEMIBOLD, foreground, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
}

void PaintMain(HWND window, HDC dc)
{
    RECT client{};
    GetClientRect(window, &client);
    const UiLayout layout = CalculateLayout(window);

    FillRectColor(dc, client, kCanvas);
    FillRectColor(dc, layout.header, kInk);

    const int margin = Px(window, 20);
    DrawTextBlock(dc, window, L"CYMRISE · UTILITY", MakeRect(margin, Px(window, 15), Px(window, 260), Px(window, 16)), 8, FW_BOLD, RGB(147, 197, 253), DT_LEFT | DT_VCENTER | DT_SINGLELINE);
    DrawTextBlock(dc, window, L"屏幕取色器", MakeRect(margin, Px(window, 38), client.right - margin * 2, Px(window, 32)), 18, FW_SEMIBOLD, RGB(255, 255, 255), DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);

    FillRoundRect(dc, layout.card, RGB(255, 255, 255), RGB(226, 232, 240), Px(window, 12));
    FillRoundRect(dc, layout.swatch, gSelectedColor, RGB(203, 213, 225), Px(window, 10));

    DrawTextBlock(dc, window, L"HEX", layout.hexCaption, 8, FW_BOLD, kMuted, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
    DrawTextBlock(dc, window, ToHex(gSelectedColor), layout.hexValue, 13, FW_SEMIBOLD, kInk, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);
    DrawButton(dc, window, layout.copyHex, L"复制", kBlueSoft, RGB(29, 78, 216));

    DrawTextBlock(dc, window, L"RGB", layout.rgbCaption, 8, FW_BOLD, kMuted, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
    DrawTextBlock(dc, window, ToRgb(gSelectedColor), layout.rgbValue, 10, FW_NORMAL, RGB(51, 65, 85), DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);
    DrawButton(dc, window, layout.copyRgb, L"复制", RGB(248, 250, 252), RGB(51, 65, 85));

    DrawButton(dc, window, layout.pickButton, L"开始取色", kBlue, RGB(255, 255, 255));
    DrawTextBlock(dc, window, L"左键确认 · Space / Enter 确认 · Esc / 右键取消", layout.hint, 8, FW_NORMAL, kMuted, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);
    DrawTextBlock(dc, window, gStatus, layout.status, 9, FW_NORMAL, kGreen, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);
    DrawTextBlock(dc, window, L"Cymrise Color Picker · v1.2.2", layout.footer, 8, FW_NORMAL, RGB(148, 163, 184), DT_CENTER | DT_VCENTER | DT_SINGLELINE);
}

LRESULT CALLBACK MainWindowProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_PAINT:
    {
        PAINTSTRUCT paint{};
        HDC dc = BeginPaint(window, &paint);
        PaintMain(window, dc);
        EndPaint(window, &paint);
        return 0;
    }
    case WM_SETCURSOR:
    {
        POINT point{};
        GetCursorPos(&point);
        ScreenToClient(window, &point);
        const UiLayout layout = CalculateLayout(window);
        if (Contains(layout.pickButton, point) || Contains(layout.copyHex, point) || Contains(layout.copyRgb, point))
        {
            SetCursor(LoadCursor(nullptr, IDC_HAND));
            return TRUE;
        }
        break;
    }
    case WM_LBUTTONUP:
    {
        const POINT point{GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
        const UiLayout layout = CalculateLayout(window);
        if (Contains(layout.pickButton, point))
        {
            StartPicking();
        }
        else if (Contains(layout.copyHex, point))
        {
            SetStatus(CopyText(ToHex(gSelectedColor)) ? L"HEX 已复制到剪贴板" : L"复制失败，请重试");
        }
        else if (Contains(layout.copyRgb, point))
        {
            SetStatus(CopyText(ToRgb(gSelectedColor)) ? L"RGB 已复制到剪贴板" : L"复制失败，请重试");
        }
        return 0;
    }
    case WM_KEYDOWN:
        if (wParam == VK_RETURN || wParam == VK_SPACE)
        {
            StartPicking();
            return 0;
        }
        break;
    case WM_DPICHANGED:
    {
        const RECT* suggested = reinterpret_cast<const RECT*>(lParam);
        SetWindowPos(window, nullptr, suggested->left, suggested->top,
            suggested->right - suggested->left, suggested->bottom - suggested->top,
            SWP_NOZORDER | SWP_NOACTIVATE);
        return 0;
    }
    case WM_DESTROY:
        StopPicking(false);
        PostQuitMessage(0);
        return 0;
    default:
        break;
    }
    return DefWindowProcW(window, message, wParam, lParam);
}

LRESULT CALLBACK OverlayWindowProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_SETCURSOR:
        SetCursor(LoadCursor(nullptr, IDC_CROSS));
        return TRUE;
    case WM_MOUSEMOVE:
    {
        POINT cursor{};
        GetCursorPos(&cursor);
        PreviewAt(cursor);
        return 0;
    }
    case WM_LBUTTONUP:
    {
        POINT cursor{};
        GetCursorPos(&cursor);
        PreviewAt(cursor);
        FinishPicking();
        return 0;
    }
    case WM_RBUTTONUP:
        CancelPicking();
        return 0;
    case WM_KEYDOWN:
        if (wParam == VK_ESCAPE)
        {
            CancelPicking();
            return 0;
        }
        if (wParam == VK_RETURN || wParam == VK_SPACE)
        {
            FinishPicking();
            return 0;
        }
        break;
    case WM_ACTIVATEAPP:
        if (wParam == FALSE && gPicking)
        {
            CancelPicking();
        }
        return 0;
    case WM_ERASEBKGND:
        return 1;
    case WM_PAINT:
    {
        PAINTSTRUCT paint{};
        BeginPaint(window, &paint);
        EndPaint(window, &paint);
        return 0;
    }
    case WM_CLOSE:
        CancelPicking();
        return 0;
    default:
        break;
    }
    return DefWindowProcW(window, message, wParam, lParam);
}

LRESULT CALLBACK LensWindowProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_NCHITTEST:
        return HTTRANSPARENT;
    case WM_PAINT:
    {
        PAINTSTRUCT paint{};
        HDC dc = BeginPaint(window, &paint);
        PaintLens(window, dc);
        EndPaint(window, &paint);
        return 0;
    }
    case WM_ERASEBKGND:
        return 1;
    default:
        break;
    }
    return DefWindowProcW(window, message, wParam, lParam);
}

bool RegisterWindowClasses(HINSTANCE instance)
{
    WNDCLASSEXW mainClass{};
    mainClass.cbSize = sizeof(mainClass);
    mainClass.style = CS_HREDRAW | CS_VREDRAW;
    mainClass.lpfnWndProc = MainWindowProc;
    mainClass.hInstance = instance;
    mainClass.hIcon = LoadIconW(instance, MAKEINTRESOURCEW(1));
    mainClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
    mainClass.lpszClassName = kMainClass;
    if (RegisterClassExW(&mainClass) == 0)
    {
        return false;
    }

    WNDCLASSEXW overlayClass{};
    overlayClass.cbSize = sizeof(overlayClass);
    overlayClass.lpfnWndProc = OverlayWindowProc;
    overlayClass.hInstance = instance;
    overlayClass.hCursor = LoadCursor(nullptr, IDC_CROSS);
    overlayClass.lpszClassName = kOverlayClass;
    if (RegisterClassExW(&overlayClass) == 0)
    {
        return false;
    }

    WNDCLASSEXW lensClass{};
    lensClass.cbSize = sizeof(lensClass);
    lensClass.lpfnWndProc = LensWindowProc;
    lensClass.hInstance = instance;
    lensClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
    lensClass.lpszClassName = kLensClass;
    return RegisterClassExW(&lensClass) != 0;
}
} // namespace

int WINAPI wWinMain(HINSTANCE instance, HINSTANCE, PWSTR, int showCommand)
{
    SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    gInstance = instance;

    if (!RegisterWindowClasses(instance))
    {
        return 1;
    }

    const DWORD style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
    const UINT systemDpi = GetDpiForSystem();
    RECT initial{0, 0, 520, 450};
    AdjustWindowRectExForDpi(&initial, style, FALSE, 0, systemDpi);

    gMainWindow = CreateWindowExW(
        0, kMainClass, kWindowTitle, style,
        CW_USEDEFAULT, CW_USEDEFAULT,
        initial.right - initial.left, initial.bottom - initial.top,
        nullptr, nullptr, instance, nullptr);

    if (gMainWindow == nullptr)
    {
        return 1;
    }

    ShowWindow(gMainWindow, showCommand);
    UpdateWindow(gMainWindow);

    MSG message{};
    while (GetMessageW(&message, nullptr, 0, 0) > 0)
    {
        TranslateMessage(&message);
        DispatchMessageW(&message);
    }

    return static_cast<int>(message.wParam);
}
