#ifndef COLOR_PICKER_UI_TEXTS_H
#define COLOR_PICKER_UI_TEXTS_H

#ifdef COLORPICKER_LANG_EN
constexpr wchar_t kAppTitle[] = L"ColorPicker";
constexpr wchar_t kScreenTitle[] = L"Screen Color Picker";
constexpr wchar_t kCurrentColor[] = L"Current color";
constexpr wchar_t kCopy[] = L"Copy";
constexpr wchar_t kPickColor[] = L"Pick Color";
constexpr wchar_t kHint[] = L"Left click to pick · Space/Enter confirm · Esc/right click cancel";
constexpr wchar_t kReady[] = L"Ready to pick a color";
constexpr wchar_t kCannotBounds[] = L"Unable to read desktop bounds. Please try again.";
constexpr wchar_t kCannotFreeze[] = L"Unable to freeze the desktop. Please try again.";
constexpr wchar_t kCannotStart[] = L"Unable to start color picking. Please try again.";
constexpr wchar_t kCopiedHexPrefix[] = L"HEX copied: ";
constexpr wchar_t kPickedNoCopy[] = L"Color picked. Use Copy to retry.";
constexpr wchar_t kCancelled[] = L"Color picking cancelled";
constexpr wchar_t kHexCopied[] = L"HEX copied to clipboard";
constexpr wchar_t kRgbCopied[] = L"RGB copied to clipboard";
constexpr wchar_t kCopyFailed[] = L"Copy failed. Please try again.";
constexpr wchar_t kAboutTitle[] = L"About ColorPicker";
constexpr wchar_t kAboutSubtitle[] = L"A lightweight Windows screen color picker";
constexpr wchar_t kProjectGithub[] = L"Project GitHub";
constexpr wchar_t kOpenGithubHint[] = L"Click the link to open the project page in your browser";
constexpr wchar_t kFooter[] = L"By cymrise";
#else
constexpr wchar_t kAppTitle[] = L"ColorPicker";
constexpr wchar_t kScreenTitle[] = L"屏幕取色器";
constexpr wchar_t kCurrentColor[] = L"当前颜色";
constexpr wchar_t kCopy[] = L"复制";
constexpr wchar_t kPickColor[] = L"开始取色";
constexpr wchar_t kHint[] = L"左键确认 · Space/Enter 确认 · Esc/右键取消";
constexpr wchar_t kReady[] = L"就绪，可开始取色";
constexpr wchar_t kCannotBounds[] = L"无法获取桌面范围，请重试";
constexpr wchar_t kCannotFreeze[] = L"无法冻结桌面，请重试";
constexpr wchar_t kCannotStart[] = L"无法启动取色，请重试";
constexpr wchar_t kCopiedHexPrefix[] = L"已复制 HEX：";
constexpr wchar_t kPickedNoCopy[] = L"已取色，可点击复制按钮重试";
constexpr wchar_t kCancelled[] = L"已取消取色";
constexpr wchar_t kHexCopied[] = L"HEX 已复制到剪贴板";
constexpr wchar_t kRgbCopied[] = L"RGB 已复制到剪贴板";
constexpr wchar_t kCopyFailed[] = L"复制失败，请重试";
constexpr wchar_t kAboutTitle[] = L"关于 ColorPicker";
constexpr wchar_t kAboutSubtitle[] = L"轻量级 Windows 屏幕取色工具";
constexpr wchar_t kProjectGithub[] = L"项目 GitHub";
constexpr wchar_t kOpenGithubHint[] = L"点击链接将在浏览器中打开项目主页";
constexpr wchar_t kFooter[] = L"By cymrise";
#endif

#endif
