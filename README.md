# Cymrise Color Picker

轻量级 Windows 屏幕取色器，使用原生 Win32 C++ 实现。

> 当前版本：**v1.2.0 Native**

## 下载与运行

下载 Release 中的 `CymriseColorPicker-v1.2.0-win-x64.zip`，解压后直接运行：

```text
ColorPicker.exe
```

用户无需安装 .NET 或其他应用运行时。发布包仅包含一个 Windows x64 原生可执行文件。

## 功能

- Per-Monitor V2 DPI 感知：支持 100%、125%、150%、175%、200% 缩放。
- 多显示器与负坐标布局：主屏、扩展屏、左侧/上方副屏均可取色。
- 完整虚拟桌面截图采样：鼠标位置与取色像素使用同一物理坐标系。
- 实时悬浮色值预览。
- 左键 / `Space` / `Enter` 确认，右键 / `Esc` 取消。
- 自动复制 HEX，并支持独立复制 HEX / RGB。
- 不锁定鼠标，不写注册表，不联网。

## 系统要求

- Windows 10 或 Windows 11 x64
- 无额外应用运行时要求

## 开发构建

仅开发者需要 Windows、Visual Studio Build Tools（含 Desktop C++）和 CMake：

```powershell
cmake -S native -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release
```

产物位于：

```text
build\Release\ColorPicker.exe
```

## v1.2.0

- 从 WinForms / .NET 发布链迁移为原生 Win32 C++。
- 发布包改为单个原生 `ColorPicker.exe`。
- 保留并重做高 DPI、多显示器、虚拟桌面坐标与现代化界面能力。

## 开源协议

[MIT License](LICENSE)
