# Cymrise Color Picker

轻量级 Windows 屏幕取色器，使用原生 Win32 C++ 实现。

> 当前版本：**v1.2.2 Native**

## 下载与运行

下载 Release 中的 `CymriseColorPicker-v1.2.2-win-x64.zip`，解压后直接运行：

```text
ColorPicker.exe
```

用户无需安装 .NET 或其他应用运行时。发布包仅包含一个 Windows x64 原生可执行文件。

## 功能

- Per-Monitor V2 DPI 感知：支持 100%、125%、150%、175%、200% 缩放。
- 多显示器与负坐标布局：主屏、扩展屏、左侧/上方副屏均可取色。
- **即时进入取色**：不再拷贝整张虚拟桌面；启动后直接以物理鼠标坐标读取单个像素。
- **自适应排版**：窗口内容按当前 DPI 和客户区宽度计算，避免标题、状态文字、按钮互相遮挡或被裁剪。
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

## v1.2.2

- 扩大默认窗口并重构为按 DPI / 客户区尺寸动态计算的布局。
- 修复标题、卡片内容、提示语、状态语和底部版本信息的遮挡、裁剪与显示不全。
- 删除“截取完整虚拟桌面”的同步步骤，解决点击开始取色后明显卡顿的问题。
- 取色过程改为直接读取当前鼠标物理坐标的单像素颜色，保留多屏与非 100% 缩放准确性。

## 开源协议

[MIT License](LICENSE)
