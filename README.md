# ColorPicker

轻量级 Windows 屏幕取色器，使用原生 Win32 C++ 实现。

> 当前版本：**v1.2.3 Native**

## 下载与运行

下载 Release 中的 `CymriseColorPicker-v1.2.3-win-x64.zip`，解压后直接运行：

```text
ColorPicker.exe
```

用户无需安装 .NET、Java、VC++ Runtime 或其他应用运行时。发布包仅包含一个 Windows x64 原生可执行文件。

## 功能

- Per-Monitor V2 DPI 感知：支持 100%、125%、150%、175%、200% 缩放。
- 多显示器与负坐标布局：主屏、扩展屏、左侧/上方副屏均可取色。
- 即时进入取色：不拷贝整张虚拟桌面，直接以物理鼠标坐标读取单像素。
- HEX 与 RGB 均完整显示，不会被复制按钮挤压或省略。
- 紧凑标题区、完整“开始取色”按钮和窗口最小尺寸保护。
- 点击底部 `By cymrise` 可查看项目 GitHub，并在浏览器中打开仓库。
- 左键 / `Space` / `Enter` 确认，右键 / `Esc` 取消。
- 自动复制 HEX，并支持独立复制 HEX / RGB。
- 不锁定鼠标，不写注册表，不联网。

## 系统要求

- Windows 10 或 Windows 11 x64
- 无额外应用运行时要求

## 开发构建

仅开发者需要 Windows、Visual Studio Build Tools（含 Desktop C++）和 CMake：

```powershell
cmake -S native -B build -A x64
cmake --build build --config Release
```

产物位于：

```text
build\Release\ColorPicker.exe
```

## v1.2.3

- 标题栏统一为 `ColorPicker`。
- 移除顶部 `CYMRISE · UTILITY`，收紧标题区域留白。
- 修复高 DPI 下窗口初始尺寸不随缩放变化的问题，恢复完整的开始取色按钮。
- HEX / RGB 使用独立文本空间，不会被复制按钮遮挡或省略。
- 恢复可点击的 `By cymrise`，新增 GitHub 项目介绍窗口与浏览器跳转。

## 开源协议

[MIT License](LICENSE)
