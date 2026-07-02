# ColorPicker

轻量级 Windows 屏幕取色器，使用原生 Win32 C++ 实现。

> 当前版本：**v1.2.4 Native**

## 下载与运行

下载 Release 中的 `CymriseColorPicker-v1.2.4-win-x64.zip`，解压后直接运行：

```text
ColorPicker.exe
```

用户无需安装 .NET、Java、VC++ Runtime 或其他应用运行时。发布包仅包含一个 Windows x64 原生可执行文件。

## 功能

- Per-Monitor V2 DPI 感知：支持 100%、125%、150%、175%、200% 缩放。
- 多显示器与负坐标布局：主屏、扩展屏、左侧/上方副屏均可取色。
- 紧凑小工具界面：默认客户端尺寸为 410 × 320 DIP，并保留最小尺寸保护。
- HEX 与 RGB 使用独立可用区域，不会被复制按钮遮挡或省略。
- 点击开始取色后，程序会生成当前虚拟桌面快照并用覆盖层冻结画面；桌面不能再接收点击。
- 当前颜色浮窗由鼠标移动与 16ms 定时刷新共同驱动，不会停在原地。
- 左键 / `Space` / `Enter` 确认，右键 / `Esc` 取消。
- 自动复制 HEX，并支持独立复制 HEX / RGB。
- 点击底部 `By cymrise` 可查看项目 GitHub，并在浏览器中打开仓库。
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

## v1.2.4

- 默认窗口由 540 × 420 DIP 收紧为 410 × 320 DIP，更符合小工具定位。
- 点击取色后使用虚拟桌面快照绘制不透明覆盖层，真正冻结桌面并阻止底层点击。
- 颜色浮窗增加定时刷新与鼠标捕获，始终跟随当前鼠标位置。
- GitHub 项目链接更新为当前仓库地址。

## 开源协议

[MIT License](LICENSE)
