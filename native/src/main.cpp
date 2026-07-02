#include <windows.h>
#include <windowsx.h>
#include <dwmapi.h>

#include <algorithm>
#include <cwchar>
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

HINSTANCE gInstance = nullptr;
HWND gMainWindow = nullptr;
HWND gOverlayWindow = nullptr;
HWND gLensWindow = nullptr;
HDC gSnapshotDc = nullptr;
HBITMAP gSnapshotBitmap = nullptr;
HBITMAP gSnapshotPrevious = nullptr;
RECT gVirtualBounds{};
POINT gLastCursor{};
COLORREF gSelectedColor = RGB(59, 130, 246);
bool gPicking = false;
std::wstring gStatus = L"就绪，可开始取色";

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
                    memcpy(target, text.c_str(), bytes);
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
        Sleep(30);
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

void ReleaseSnapshot()
{
    if (gSnapshotDc != nullptr)
    {
        if (gSnapshotPrevious != nullptr)
        {
            SelectObject(gSnapshotDc, gSnapshotPrevious);
        }
        if (gSnapshotBitmap != nullptr)
        {
            DeleteObject(gSnapshotBitmap);
        }
        DeleteDC(gSnapshotDc);
    }

    gSnapshotDc = nullptr;
    gSnapshotBitmap = nullptr;
    gSnapshotPrevious = nullptr;
}

bool CaptureVirtualScreen()
{
    ReleaseSnapshot();

    gVirtualBounds.left = GetSystemMetrics(SM_XVIRTUALSCREEN);
    gVirtualBounds.top = GetSystemMetrics(SM_YVIRTUALSCREEN);
    gVirtualBounds.right = gVirtualBounds.left + GetSystemMetrics(SM_CXVIRTUALSCREEN);
    gVirtualBounds.bottom = gVirtualBounds.top + GetSystemMetrics(SM_CYVIRTUALSCREEN);

    const int width = gVirtualBounds.right - gVirtualBounds.left;
    const int height = gVirtualBounds.bottom - gVirtualBounds.top;
    if (width <= 0 || height <= 0)
    {
        return false;
    }

    HDC desktopDc = GetDC(nullptr);
    if (desktopDc == nullptr)
    {
        return false;
    }

    gSnapshotDc = CreateCompatibleDC(desktopDc);
    gSnapshotBitmap = CreateCompatibleBitmap(desktopDc, width, height);
    ReleaseDC(nullptr, desktopDc);

    if (gSnapshotDc == nullptr || gSnapshotBitmap == nullptr)
    {
        ReleaseSnapshot();
        return false;
    }

    gSnapshotPrevious = static_cast<HBITMAP>(SelectObject(gSnapshotDc, gSnapshotBitmap));
    HDC sourceDc = GetDC(nullptr);
    const BOOL copied = BitBlt(
        gSnapshotDc, 0, 0, width, height, sourceDc,
        gVirtualBounds.left, gVirtualBounds.top,
        SRCCOPY | CAPTUREBLT);
    ReleaseDC(nullptr, sourceDc);

    if (!copied)
    {
        ReleaseSnapshot();
        return false;
    }

    return true;
}

POINT ClampToVirtualScreen(POINT point)
{
    point.x = std::clamp(point.x, gVirtualBounds.left, gVirtualBounds.right - 1);
    point.y = std::clamp(point.y, gVirtualBounds.top, gVirtualBounds.bottom - 1);
    return point;
}

COLORREF SampleAt(POINT screenPoint)
{
    const POINT point = ClampToVirtualScreen(screenPoint);
    const int x = point.x - gVirtualBounds.left;
    const int y = point.y - gVirtualBounds.top;
    return GetPixel(gSnapshotDc, x, y);
}

void PaintLens(HWND window, HDC dc)
{
    RECT client{};
    GetClientRect(window, &client);
    FillRectColor(dc, client, RGB(17, 24, 39));

    const int padding = Px(window, 12);
    const int swatchSize = Px(window, 60);
    RECT swatch = MakeRect(padding, padding, swatchSize, swatchSize);
    FillRectColor(dc, swatch, gSelectedColor);
    FrameRect(dc, &swatch, static_cast<HBRUSH>(GetStockObject(WHITE_BRUSH)));

    RECT caption = MakeRect(swatch.right + Px(window, 14), padding, Px(window, 94), Px(window, 18));
    DrawTextBlock(dc, window, L"当前颜色", caption, 8, FW_BOLD, RGB(148, 163, 184), DT_LEFT | DT_VCENTER | DT_SINGLELINE);

    RECT value = MakeRect(swatch.right + Px(window, 14), padding + Px(window, 22), Px(window, 104), Px(window, 30));
    DrawTextBlock(dc, window, ToHex(gSelectedColor), value, 12, FW_SEMIBOLD, RGB(255, 255, 255), DT_LEFT | DT_VCENTER | DT_SINGLELINE);
}

void PositionLens(POINT cursor)
{
    if (gLensWindow == nullptr)
    {
        return;
    }

    const int width = Px(gLensWindow, 184);
    const int height = Px(gLensWindow, 84);
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

    ReleaseSnapshot();

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

    ShowWindow(gMainWindow, SW_HIDE);
    DwmFlush();

    if (!CaptureVirtualScreen())
    {
        ShowWindow(gMainWindow, SW_SHOW);
        SetStatus(L"无法创建屏幕快照，请重试");
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
        ReleaseSnapshot();
        ShowWindow(gMainWindow, SW_SHOW);
        SetStatus(L"无法启动取色，请重试");
        return false;
    }

    SetLayeredWindowAttributes(gOverlayWindow, 0, 1, LWA_ALPHA);

    gLensWindow = CreateWindowExW(
        WS_EX_NOACTIVATE | WS_EX_TOOLWINDOW | WS_EX_TOPMOST,
        kLensClass, L"", WS_POPUP,
        0, 0, Px(gOverlayWindow, 184), Px(gOverlayWindow, 84),
        nullptr, nullptr, gInstance, nullptr);

    gPicking = true;
    ShowWindow(gOverlayWindow, SW_SHOW);
    SetWindowPos(gOverlayWindow, HWND_TOPMOST,
        gVirtualBounds.left, gVirtualBounds.top,
        gVirtualBounds.right - gVirtualBounds.left,
        gVirtualBounds.bottom - gVirtualBounds.top,
        SWP_SHOWWINDOW);
    SetForegroundWindow(gOverlayWindow);
    SetFocus(gOverlayWindow);

    GetCursorPos(&gLastCursor);
    PreviewAt(gLastCursor);
    return true;
}

RECT PickButtonRect(HWND window)
{
    const int margin = Px(window, 18);
    return MakeRect(margin, Px(window, 226), Px(window, 404), Px(window, 44));
}

RECT CopyHexRect(HWND window)
{
    return MakeRect(Px(window, 324), Px(window, 116), Px(window, 80), Px(window, 28));
}

RECT CopyRgbRect(HWND window)
{
    return MakeRect(Px(window, 324), Px(window, 168), Px(window, 80), Px(window, 28));
}

void DrawButton(HDC dc, HWND window, const RECT& rect, const std::wstring& text, COLORREF background, COLORREF foreground)
{
    FillRoundRect(dc, rect, background, background, Px(window, 8));
    DrawTextBlock(dc, window, text, rect, 9, FW_SEMIBOLD, foreground, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
}

void PaintMain(HWND window, HDC dc)
{
    RECT client{};
    GetClientRect(window, &client);
    FillRectColor(dc, client, kCanvas);

    const int margin = Px(window, 18);
    const int cardWidth = Px(window, 404);

    RECT header = MakeRect(0, 0, client.right, Px(window, 68));
    FillRectColor(dc, header, kInk);

    DrawTextBlock(dc, window, L"CYMRISE · UTILITY", MakeRect(margin, Px(window, 12), Px(window, 240), Px(window, 14)), 8, FW_BOLD, RGB(147, 197, 253), DT_LEFT | DT_VCENTER | DT_SINGLELINE);
    DrawTextBlock(dc, window, L"屏幕取色器", MakeRect(margin, Px(window, 28), Px(window, 260), Px(window, 28)), 15, FW_SEMIBOLD, RGB(255, 255, 255), DT_LEFT | DT_VCENTER | DT_SINGLELINE);

    RECT card = MakeRect(margin, Px(window, 84), cardWidth, Px(window, 126));
    FillRoundRect(dc, card, RGB(255, 255, 255), RGB(226, 232, 240), Px(window, 12));

    RECT swatch = MakeRect(card.left + Px(window, 14), card.top + Px(window, 16), Px(window, 92), Px(window, 92));
    FillRoundRect(dc, swatch, gSelectedColor, RGB(203, 213, 225), Px(window, 10));

    const int contentLeft = swatch.right + Px(window, 16);
    DrawTextBlock(dc, window, L"HEX", MakeRect(contentLeft, card.top + Px(window, 15), Px(window, 160), Px(window, 16)), 8, FW_BOLD, kMuted, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
    DrawTextBlock(dc, window, ToHex(gSelectedColor), MakeRect(contentLeft, card.top + Px(window, 33), Px(window, 176), Px(window, 28)), 12, FW_SEMIBOLD, kInk, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
    DrawButton(dc, window, CopyHexRect(window), L"复制", kBlueSoft, RGB(29, 78, 216));

    DrawTextBlock(dc, window, L"RGB", MakeRect(contentLeft, card.top + Px(window, 67), Px(window, 160), Px(window, 16)), 8, FW_BOLD, kMuted, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
    DrawTextBlock(dc, window, ToRgb(gSelectedColor), MakeRect(contentLeft, card.top + Px(window, 85), Px(window, 176), Px(window, 22)), 9, FW_NORMAL, RGB(51, 65, 85), DT_LEFT | DT_VCENTER | DT_SINGLELINE);
    DrawButton(dc, window, CopyRgbRect(window), L"复制", RGB(248, 250, 252), RGB(51, 65, 85));

    DrawButton(dc, window, PickButtonRect(window), L"开始取色", kBlue, RGB(255, 255, 255));

    DrawTextBlock(dc, window, L"左键确认 · Space / Enter 确认 · Esc / 右键取消", MakeRect(margin, Px(window, 282), cardWidth, Px(window, 18)), 8, FW_NORMAL, kMuted, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    DrawTextBlock(dc, window, gStatus, MakeRect(margin, Px(window, 305), cardWidth, Px(window, 20)), 9, FW_NORMAL, RGB(5, 150, 105), DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    DrawTextBlock(dc, window, L"Cymrise Color Picker · Native Win32", MakeRect(margin, Px(window, 329), cardWidth, Px(window, 16)), 8, FW_NORMAL, RGB(148, 163, 184), DT_CENTER | DT_VCENTER | DT_SINGLELINE);
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
        if (Contains(PickButtonRect(window), point) || Contains(CopyHexRect(window), point) || Contains(CopyRgbRect(window), point))
        {
            SetCursor(LoadCursor(nullptr, IDC_HAND));
            return TRUE;
        }
        break;
    }
    case WM_LBUTTONUP:
    {
        const POINT point{GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
        if (Contains(PickButtonRect(window), point))
        {
            StartPicking();
        }
        else if (Contains(CopyHexRect(window), point))
        {
            SetStatus(CopyText(ToHex(gSelectedColor)) ? L"HEX 已复制到剪贴板" : L"复制失败，请重试");
        }
        else if (Contains(CopyRgbRect(window), point))
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
    RECT initial{0, 0, 440, 386};
    AdjustWindowRectExForDpi(&initial, style, FALSE, 0, 96);

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
