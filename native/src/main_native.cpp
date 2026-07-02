#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0A00
#endif

#include <windows.h>
#include <windowsx.h>
#include <dwmapi.h>

#include <cstring>
#include <string>

namespace
{
constexpr wchar_t kMainClass[] = L"CymriseColorPickerMain";
constexpr wchar_t kOverlayClass[] = L"CymriseColorPickerOverlay";
constexpr wchar_t kLensClass[] = L"CymriseColorPickerLens";
constexpr wchar_t kTitle[] = L"Cymrise Color Picker";

constexpr COLORREF kInk = RGB(15, 23, 42);
constexpr COLORREF kCanvas = RGB(248, 250, 252);
constexpr COLORREF kMuted = RGB(100, 116, 139);
constexpr COLORREF kBlue = RGB(37, 99, 235);
constexpr COLORREF kBlueSoft = RGB(239, 246, 255);
constexpr COLORREF kGreen = RGB(5, 150, 105);

HINSTANCE gInstance = nullptr;
HWND gMain = nullptr;
HWND gOverlay = nullptr;
HWND gLens = nullptr;
HDC gDesktopDc = nullptr;
int gVirtualLeft = 0;
int gVirtualTop = 0;
int gVirtualRight = 0;
int gVirtualBottom = 0;
POINT gCursor{};
COLORREF gColor = RGB(52, 52, 52);
bool gPicking = false;
std::wstring gStatus = L"就绪，可开始取色";

struct Layout
{
    RECT header{};
    RECT card{};
    RECT swatch{};
    RECT hexLabel{};
    RECT hexValue{};
    RECT hexCopy{};
    RECT rgbLabel{};
    RECT rgbValue{};
    RECT rgbCopy{};
    RECT pick{};
    RECT hint{};
    RECT status{};
    RECT footer{};
};

int Dp(HWND hwnd, int value)
{
    const UINT dpi = hwnd == nullptr ? 96 : GetDpiForWindow(hwnd);
    return MulDiv(value, static_cast<int>(dpi), 96);
}

int Limit(int value, int low, int high)
{
    if (high < low)
    {
        return low;
    }
    return value < low ? low : (value > high ? high : value);
}

RECT Rect(int left, int top, int width, int height)
{
    return RECT{left, top, left + width, top + height};
}

bool Hit(const RECT& rect, POINT point)
{
    return PtInRect(&rect, point) != FALSE;
}

std::wstring Hex(COLORREF color)
{
    wchar_t buffer[16]{};
    swprintf_s(buffer, L"#%02X%02X%02X", GetRValue(color), GetGValue(color), GetBValue(color));
    return buffer;
}

std::wstring Rgb(COLORREF color)
{
    wchar_t buffer[40]{};
    swprintf_s(buffer, L"RGB(%u, %u, %u)", GetRValue(color), GetGValue(color), GetBValue(color));
    return buffer;
}

HFONT Font(HWND hwnd, int size, int weight = FW_NORMAL)
{
    return CreateFontW(-Dp(hwnd, size), 0, 0, 0, weight, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
        DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
}

void Text(HDC dc, HWND hwnd, const std::wstring& value, RECT rect, int size, int weight, COLORREF color, UINT align)
{
    HFONT font = Font(hwnd, size, weight);
    HGDIOBJ previous = SelectObject(dc, font);
    SetBkMode(dc, TRANSPARENT);
    SetTextColor(dc, color);
    DrawTextW(dc, value.c_str(), static_cast<int>(value.size()), &rect, align);
    SelectObject(dc, previous);
    DeleteObject(font);
}

void Fill(HDC dc, const RECT& rect, COLORREF color)
{
    HBRUSH brush = CreateSolidBrush(color);
    FillRect(dc, &rect, brush);
    DeleteObject(brush);
}

void Round(HDC dc, const RECT& rect, COLORREF fill, COLORREF stroke, int radius)
{
    HBRUSH brush = CreateSolidBrush(fill);
    HPEN pen = CreatePen(PS_SOLID, 1, stroke);
    HGDIOBJ oldBrush = SelectObject(dc, brush);
    HGDIOBJ oldPen = SelectObject(dc, pen);
    RoundRect(dc, rect.left, rect.top, rect.right, rect.bottom, radius, radius);
    SelectObject(dc, oldBrush);
    SelectObject(dc, oldPen);
    DeleteObject(brush);
    DeleteObject(pen);
}

Layout GetLayout(HWND hwnd)
{
    RECT client{};
    GetClientRect(hwnd, &client);
    const int width = static_cast<int>(client.right - client.left);
    const int margin = Dp(hwnd, 20);
    const int cardWidth = width - margin * 2;
    const int headerHeight = Dp(hwnd, 92);
    const int cardTop = headerHeight + Dp(hwnd, 20);
    const int cardHeight = Dp(hwnd, 146);
    const int inner = Dp(hwnd, 16);
    const int copyWidth = Dp(hwnd, 72);
    const int copyHeight = Dp(hwnd, 30);
    const int swatchSize = Dp(hwnd, 100);

    Layout layout{};
    layout.header = Rect(0, 0, width, headerHeight);
    layout.card = Rect(margin, cardTop, cardWidth, cardHeight);
    layout.swatch = Rect(layout.card.left + inner, layout.card.top + Dp(hwnd, 23), swatchSize, swatchSize);

    const int contentLeft = layout.swatch.right + Dp(hwnd, 18);
    const int copyLeft = layout.card.right - inner - copyWidth;
    const int valueWidth = copyLeft - Dp(hwnd, 12) - contentLeft;

    layout.hexLabel = Rect(contentLeft, layout.card.top + Dp(hwnd, 21), valueWidth, Dp(hwnd, 16));
    layout.hexValue = Rect(contentLeft, layout.card.top + Dp(hwnd, 39), valueWidth, Dp(hwnd, 30));
    layout.hexCopy = Rect(copyLeft, layout.card.top + Dp(hwnd, 38), copyWidth, copyHeight);

    layout.rgbLabel = Rect(contentLeft, layout.card.top + Dp(hwnd, 81), valueWidth, Dp(hwnd, 16));
    layout.rgbValue = Rect(contentLeft, layout.card.top + Dp(hwnd, 99), valueWidth, Dp(hwnd, 26));
    layout.rgbCopy = Rect(copyLeft, layout.card.top + Dp(hwnd, 97), copyWidth, copyHeight);

    layout.pick = Rect(margin, layout.card.bottom + Dp(hwnd, 22), cardWidth, Dp(hwnd, 50));
    layout.hint = Rect(margin, layout.pick.bottom + Dp(hwnd, 16), cardWidth, Dp(hwnd, 20));
    layout.status = Rect(margin, layout.hint.bottom + Dp(hwnd, 8), cardWidth, Dp(hwnd, 22));
    layout.footer = Rect(margin, layout.status.bottom + Dp(hwnd, 7), cardWidth, Dp(hwnd, 18));
    return layout;
}

bool Copy(const std::wstring& value)
{
    for (int i = 0; i < 3; ++i)
    {
        if (OpenClipboard(gMain))
        {
            EmptyClipboard();
            const size_t bytes = (value.size() + 1) * sizeof(wchar_t);
            HGLOBAL memory = GlobalAlloc(GMEM_MOVEABLE, bytes);
            if (memory != nullptr)
            {
                void* data = GlobalLock(memory);
                if (data != nullptr)
                {
                    std::memcpy(data, value.c_str(), bytes);
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

void Status(const std::wstring& value)
{
    gStatus = value;
    if (gMain != nullptr)
    {
        InvalidateRect(gMain, nullptr, FALSE);
    }
}

void RefreshDesktopBounds()
{
    gVirtualLeft = GetSystemMetrics(SM_XVIRTUALSCREEN);
    gVirtualTop = GetSystemMetrics(SM_YVIRTUALSCREEN);
    gVirtualRight = gVirtualLeft + GetSystemMetrics(SM_CXVIRTUALSCREEN);
    gVirtualBottom = gVirtualTop + GetSystemMetrics(SM_CYVIRTUALSCREEN);
}

POINT ClampPoint(POINT point)
{
    point.x = Limit(static_cast<int>(point.x), gVirtualLeft, gVirtualRight - 1);
    point.y = Limit(static_cast<int>(point.y), gVirtualTop, gVirtualBottom - 1);
    return point;
}

COLORREF ReadPixel(POINT point)
{
    if (gDesktopDc == nullptr)
    {
        return gColor;
    }
    const POINT safe = ClampPoint(point);
    const COLORREF color = GetPixel(gDesktopDc, static_cast<int>(safe.x), static_cast<int>(safe.y));
    return color == CLR_INVALID ? gColor : color;
}

void PaintLens(HWND hwnd, HDC dc)
{
    RECT client{};
    GetClientRect(hwnd, &client);
    Round(dc, client, RGB(17, 24, 39), RGB(51, 65, 85), Dp(hwnd, 10));
    const int padding = Dp(hwnd, 12);
    const int swatchSize = Dp(hwnd, 60);
    const RECT swatch = Rect(padding, padding, swatchSize, swatchSize);
    Round(dc, swatch, gColor, RGB(148, 163, 184), Dp(hwnd, 8));
    Text(dc, hwnd, L"当前颜色", Rect(swatch.right + Dp(hwnd, 14), padding, Dp(hwnd, 96), Dp(hwnd, 18)), 8, FW_BOLD, RGB(148, 163, 184), DT_LEFT | DT_VCENTER | DT_SINGLELINE);
    Text(dc, hwnd, Hex(gColor), Rect(swatch.right + Dp(hwnd, 14), padding + Dp(hwnd, 22), Dp(hwnd, 112), Dp(hwnd, 30)), 12, FW_SEMIBOLD, RGB(255, 255, 255), DT_LEFT | DT_VCENTER | DT_SINGLELINE);
}

void PlaceLens(POINT point)
{
    if (gLens == nullptr)
    {
        return;
    }
    const int width = Dp(gLens, 190);
    const int height = Dp(gLens, 88);
    const int offset = Dp(gLens, 20);
    int x = static_cast<int>(point.x) + offset;
    int y = static_cast<int>(point.y) + offset;
    if (x + width > gVirtualRight) x = static_cast<int>(point.x) - width - offset;
    if (y + height > gVirtualBottom) y = static_cast<int>(point.y) - height - offset;
    x = Limit(x, gVirtualLeft, gVirtualRight - width);
    y = Limit(y, gVirtualTop, gVirtualBottom - height);
    SetWindowPos(gLens, HWND_TOPMOST, x, y, width, height, SWP_NOACTIVATE | SWP_SHOWWINDOW);
    InvalidateRect(gLens, nullptr, FALSE);
}

void Preview(POINT point)
{
    gCursor = ClampPoint(point);
    gColor = ReadPixel(gCursor);
    PlaceLens(gCursor);
}

void StopPicking(bool showMain)
{
    gPicking = false;
    HWND lens = gLens;
    HWND overlay = gOverlay;
    gLens = nullptr;
    gOverlay = nullptr;
    if (lens != nullptr && IsWindow(lens)) DestroyWindow(lens);
    if (overlay != nullptr && IsWindow(overlay)) DestroyWindow(overlay);
    if (gDesktopDc != nullptr)
    {
        ReleaseDC(nullptr, gDesktopDc);
        gDesktopDc = nullptr;
    }
    if (showMain && gMain != nullptr)
    {
        ShowWindow(gMain, SW_SHOW);
        SetForegroundWindow(gMain);
        InvalidateRect(gMain, nullptr, FALSE);
    }
}

void Finish()
{
    if (!gPicking) return;
    gColor = ReadPixel(gCursor);
    const std::wstring value = Hex(gColor);
    Status(Copy(value) ? L"已复制 HEX：" + value : L"已取色，可点击复制按钮重试");
    StopPicking(true);
}

void Cancel()
{
    if (!gPicking) return;
    Status(L"已取消取色");
    StopPicking(true);
}

bool StartPicking()
{
    if (gPicking) return true;
    RefreshDesktopBounds();
    if (gVirtualRight <= gVirtualLeft || gVirtualBottom <= gVirtualTop)
    {
        Status(L"无法获取桌面范围，请重试");
        return false;
    }

    ShowWindow(gMain, SW_HIDE);
    DwmFlush();
    gDesktopDc = GetDC(nullptr);
    if (gDesktopDc == nullptr)
    {
        ShowWindow(gMain, SW_SHOW);
        Status(L"无法读取屏幕颜色，请重试");
        return false;
    }

    gOverlay = CreateWindowExW(WS_EX_TOOLWINDOW | WS_EX_TOPMOST | WS_EX_LAYERED, kOverlayClass, L"", WS_POPUP,
        gVirtualLeft, gVirtualTop, gVirtualRight - gVirtualLeft, gVirtualBottom - gVirtualTop,
        nullptr, nullptr, gInstance, nullptr);
    if (gOverlay == nullptr)
    {
        ReleaseDC(nullptr, gDesktopDc);
        gDesktopDc = nullptr;
        ShowWindow(gMain, SW_SHOW);
        Status(L"无法启动取色，请重试");
        return false;
    }

    SetLayeredWindowAttributes(gOverlay, 0, 0, LWA_ALPHA);
    gLens = CreateWindowExW(WS_EX_NOACTIVATE | WS_EX_TOOLWINDOW | WS_EX_TOPMOST, kLensClass, L"", WS_POPUP,
        0, 0, Dp(gOverlay, 190), Dp(gOverlay, 88), nullptr, nullptr, gInstance, nullptr);
    gPicking = true;
    ShowWindow(gOverlay, SW_SHOW);
    SetForegroundWindow(gOverlay);
    SetFocus(gOverlay);
    GetCursorPos(&gCursor);
    Preview(gCursor);
    return true;
}

void Button(HDC dc, HWND hwnd, const RECT& rect, const std::wstring& value, COLORREF back, COLORREF fore)
{
    Round(dc, rect, back, back, Dp(hwnd, 9));
    Text(dc, hwnd, value, rect, 10, FW_SEMIBOLD, fore, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
}

void PaintMain(HWND hwnd, HDC dc)
{
    RECT client{};
    GetClientRect(hwnd, &client);
    const Layout layout = GetLayout(hwnd);
    Fill(dc, client, kCanvas);
    Fill(dc, layout.header, kInk);

    const int margin = Dp(hwnd, 20);
    Text(dc, hwnd, L"CYMRISE · UTILITY", Rect(margin, Dp(hwnd, 15), Dp(hwnd, 260), Dp(hwnd, 16)), 8, FW_BOLD, RGB(147, 197, 253), DT_LEFT | DT_VCENTER | DT_SINGLELINE);
    Text(dc, hwnd, L"屏幕取色器", Rect(margin, Dp(hwnd, 39), static_cast<int>(client.right) - margin * 2, Dp(hwnd, 32)), 18, FW_SEMIBOLD, RGB(255, 255, 255), DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);

    Round(dc, layout.card, RGB(255, 255, 255), RGB(226, 232, 240), Dp(hwnd, 12));
    Round(dc, layout.swatch, gColor, RGB(203, 213, 225), Dp(hwnd, 10));
    Text(dc, hwnd, L"HEX", layout.hexLabel, 8, FW_BOLD, kMuted, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
    Text(dc, hwnd, Hex(gColor), layout.hexValue, 13, FW_SEMIBOLD, kInk, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);
    Button(dc, hwnd, layout.hexCopy, L"复制", kBlueSoft, RGB(29, 78, 216));
    Text(dc, hwnd, L"RGB", layout.rgbLabel, 8, FW_BOLD, kMuted, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
    Text(dc, hwnd, Rgb(gColor), layout.rgbValue, 10, FW_NORMAL, RGB(51, 65, 85), DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);
    Button(dc, hwnd, layout.rgbCopy, L"复制", RGB(248, 250, 252), RGB(51, 65, 85));
    Button(dc, hwnd, layout.pick, L"开始取色", kBlue, RGB(255, 255, 255));
    Text(dc, hwnd, L"左键确认 · Space / Enter 确认 · Esc / 右键取消", layout.hint, 8, FW_NORMAL, kMuted, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);
    Text(dc, hwnd, gStatus, layout.status, 9, FW_NORMAL, kGreen, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);
    Text(dc, hwnd, L"Cymrise Color Picker · v1.2.2", layout.footer, 8, FW_NORMAL, RGB(148, 163, 184), DT_CENTER | DT_VCENTER | DT_SINGLELINE);
}

LRESULT CALLBACK MainProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_PAINT:
    {
        PAINTSTRUCT paint{};
        HDC dc = BeginPaint(hwnd, &paint);
        PaintMain(hwnd, dc);
        EndPaint(hwnd, &paint);
        return 0;
    }
    case WM_SETCURSOR:
    {
        POINT point{};
        GetCursorPos(&point);
        ScreenToClient(hwnd, &point);
        const Layout layout = GetLayout(hwnd);
        if (Hit(layout.pick, point) || Hit(layout.hexCopy, point) || Hit(layout.rgbCopy, point))
        {
            SetCursor(LoadCursor(nullptr, IDC_HAND));
            return TRUE;
        }
        break;
    }
    case WM_LBUTTONUP:
    {
        const POINT point{GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
        const Layout layout = GetLayout(hwnd);
        if (Hit(layout.pick, point)) StartPicking();
        else if (Hit(layout.hexCopy, point)) Status(Copy(Hex(gColor)) ? L"HEX 已复制到剪贴板" : L"复制失败，请重试");
        else if (Hit(layout.rgbCopy, point)) Status(Copy(Rgb(gColor)) ? L"RGB 已复制到剪贴板" : L"复制失败，请重试");
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
        SetWindowPos(hwnd, nullptr, suggested->left, suggested->top, suggested->right - suggested->left, suggested->bottom - suggested->top, SWP_NOZORDER | SWP_NOACTIVATE);
        return 0;
    }
    case WM_DESTROY:
        StopPicking(false);
        PostQuitMessage(0);
        return 0;
    default:
        break;
    }
    return DefWindowProcW(hwnd, message, wParam, lParam);
}

LRESULT CALLBACK OverlayProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_SETCURSOR:
        SetCursor(LoadCursor(nullptr, IDC_CROSS));
        return TRUE;
    case WM_MOUSEMOVE:
    {
        POINT point{};
        GetCursorPos(&point);
        Preview(point);
        return 0;
    }
    case WM_LBUTTONUP:
    {
        POINT point{};
        GetCursorPos(&point);
        Preview(point);
        Finish();
        return 0;
    }
    case WM_RBUTTONUP:
        Cancel();
        return 0;
    case WM_KEYDOWN:
        if (wParam == VK_ESCAPE) { Cancel(); return 0; }
        if (wParam == VK_RETURN || wParam == VK_SPACE) { Finish(); return 0; }
        break;
    case WM_ACTIVATEAPP:
        if (wParam == FALSE && gPicking) Cancel();
        return 0;
    case WM_ERASEBKGND:
        return 1;
    case WM_PAINT:
    {
        PAINTSTRUCT paint{};
        BeginPaint(hwnd, &paint);
        EndPaint(hwnd, &paint);
        return 0;
    }
    case WM_CLOSE:
        Cancel();
        return 0;
    default:
        break;
    }
    return DefWindowProcW(hwnd, message, wParam, lParam);
}

LRESULT CALLBACK LensProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if (message == WM_NCHITTEST) return HTTRANSPARENT;
    if (message == WM_ERASEBKGND) return 1;
    if (message == WM_PAINT)
    {
        PAINTSTRUCT paint{};
        HDC dc = BeginPaint(hwnd, &paint);
        PaintLens(hwnd, dc);
        EndPaint(hwnd, &paint);
        return 0;
    }
    return DefWindowProcW(hwnd, message, wParam, lParam);
}

bool RegisterClasses(HINSTANCE instance)
{
    WNDCLASSEXW mainClass{};
    mainClass.cbSize = sizeof(mainClass);
    mainClass.style = CS_HREDRAW | CS_VREDRAW;
    mainClass.lpfnWndProc = MainProc;
    mainClass.hInstance = instance;
    mainClass.hIcon = LoadIconW(instance, MAKEINTRESOURCEW(1));
    mainClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
    mainClass.lpszClassName = kMainClass;
    if (RegisterClassExW(&mainClass) == 0) return false;

    WNDCLASSEXW overlayClass{};
    overlayClass.cbSize = sizeof(overlayClass);
    overlayClass.lpfnWndProc = OverlayProc;
    overlayClass.hInstance = instance;
    overlayClass.hCursor = LoadCursor(nullptr, IDC_CROSS);
    overlayClass.lpszClassName = kOverlayClass;
    if (RegisterClassExW(&overlayClass) == 0) return false;

    WNDCLASSEXW lensClass{};
    lensClass.cbSize = sizeof(lensClass);
    lensClass.lpfnWndProc = LensProc;
    lensClass.hInstance = instance;
    lensClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
    lensClass.lpszClassName = kLensClass;
    return RegisterClassExW(&lensClass) != 0;
}
} // namespace

int WINAPI wWinMain(HINSTANCE instance, HINSTANCE, PWSTR, int show)
{
    SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    gInstance = instance;
    if (!RegisterClasses(instance)) return 1;

    const DWORD style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
    RECT bounds{0, 0, 520, 450};
    AdjustWindowRectExForDpi(&bounds, style, FALSE, 0, GetDpiForSystem());
    gMain = CreateWindowExW(0, kMainClass, kTitle, style, CW_USEDEFAULT, CW_USEDEFAULT,
        static_cast<int>(bounds.right - bounds.left), static_cast<int>(bounds.bottom - bounds.top),
        nullptr, nullptr, instance, nullptr);
    if (gMain == nullptr) return 1;

    ShowWindow(gMain, show);
    UpdateWindow(gMain);

    MSG message{};
    while (GetMessageW(&message, nullptr, 0, 0) > 0)
    {
        TranslateMessage(&message);
        DispatchMessageW(&message);
    }
    return static_cast<int>(message.wParam);
}
