#include <windows.h>

BOOL WINAPI CymriseAdjustWindowRectExForDpi(LPRECT rect, DWORD style, BOOL menu, DWORD exStyle, UINT requestedDpi);

#define AdjustWindowRectExForDpi CymriseAdjustWindowRectExForDpi
#define wWinMain CymriseColorPickerWinMain
#include "main.cpp"
#undef wWinMain
#undef AdjustWindowRectExForDpi

BOOL WINAPI CymriseAdjustWindowRectExForDpi(LPRECT rect, DWORD style, BOOL menu, DWORD exStyle, UINT)
{
    const UINT dpi = GetDpiForSystem();
    rect->right = rect->left + MulDiv(rect->right - rect->left, static_cast<int>(dpi), 96);
    rect->bottom = rect->top + MulDiv(rect->bottom - rect->top, static_cast<int>(dpi), 96);
    return AdjustWindowRectExForDpi(rect, style, menu, exStyle, dpi);
}

int WINAPI wWinMain(HINSTANCE instance, HINSTANCE previous, PWSTR commandLine, int showCommand)
{
    return CymriseColorPickerWinMain(instance, previous, commandLine, showCommand);
}
