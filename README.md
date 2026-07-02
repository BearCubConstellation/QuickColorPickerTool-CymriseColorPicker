# Cymrise Color Picker

轻量、简洁的 Windows 屏幕取色工具。用于快速读取屏幕任意像素的 HEX 与 RGB 值，并支持一键复制。

> 当前版本：**v1.1.2**

## 特性

- **轻量发布包**：不再打包完整 .NET 8 Desktop Runtime，避免 60MB+ 大包。
- **Windows 原生组件运行**：基于 .NET Framework 4.8，Windows 10 / 11 通常已内置，无需用户额外下载 .NET 8 Runtime。
- **DPI 感知取色**：针对 Windows 125%、150%、175% 等显示缩放进行坐标修正。
- **多显示器支持**：支持扩展屏、负坐标布局与跨屏取色。
- **实时悬浮预览**：取色时显示当前 HEX 值与颜色块，不遮挡主界面。
- **轻量交互**：左键确认，右键或 `Esc` 取消，`Space` / `Enter` 确认。
- **快速复制**：确认后自动复制 HEX；也可以单独复制 HEX 或 RGB。

## 使用方式

1. 下载名称带有 `lightweight-win-x64` 的 Release 压缩包。
2. 解压后直接运行 `ColorPicker.exe`。
3. 该版本不会弹出 .NET 8 Desktop Runtime 安装提示。

## 系统要求

- Windows 10 / 11 x64
- .NET Framework 4.8（Windows 10 新版本与 Windows 11 通常已内置）
- 推荐测试显示缩放：100%、125%、150%、175%、200%

## 本地构建

```powershell
dotnet restore
dotnet build -c Release
```

## 本地发布

生成轻量发布包：

```powershell
dotnet build -c Release
mkdir .\publish\win-x64
copy .\bin\Release\net48\ColorPicker.exe .\publish\win-x64\ColorPicker.exe
```

正式压缩包不包含完整 .NET 运行时，也不会包含 `ColorPicker.pdb`。

## v1.1.2

- 回退 v1.1.1 的完整运行时打包策略。
- 目标框架切换为 .NET Framework 4.8。
- 发布包改回轻量级，仅包含应用本体与必要配置文件。
- 保留 v1.1.0 的 DPI、多屏、坐标取色与界面优化。

## v1.1.0 重点修复

- 统一使用 Per-Monitor V2 DPI 感知模式。
- 按“物理屏幕坐标 → 虚拟桌面快照坐标”计算取色位置。
- 覆盖层覆盖整个虚拟桌面，而非仅主显示器。
- 移除会导致鼠标异常受限的光标裁剪逻辑。
- 修复键盘确认时使用未初始化或错误坐标的问题。
- 增强覆盖层关闭、截图释放与剪贴板复制失败处理。
- 全面更新为更简洁、现代的桌面工具界面。

## 开源协议

[MIT License](LICENSE)
