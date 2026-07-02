# Cymrise Color Picker

轻量、简洁的 Windows 屏幕取色工具。用于快速读取屏幕任意像素的 HEX 与 RGB 值，并支持一键复制。

> 当前版本：**v1.1.0**

## 特性

- **DPI 感知取色**：针对 Windows 125%、150%、175% 等显示缩放进行坐标修正。
- **多显示器支持**：支持扩展屏、负坐标布局与跨屏取色。
- **实时悬浮预览**：取色时显示当前 HEX 值与颜色块，不遮挡主界面。
- **轻量交互**：左键确认，右键或 `Esc` 取消，`Space` / `Enter` 确认。
- **快速复制**：确认后自动复制 HEX；也可以单独复制 HEX 或 RGB。

## 使用方式

1. 启动应用后点击 **开始取色**。
2. 移动鼠标至目标颜色位置。
3. 使用左键、`Space` 或 `Enter` 确认。
4. HEX 会自动复制到剪贴板；在主界面可按需复制 HEX 或 RGB。

## 系统要求

- Windows 10 / 11
- .NET 8 Desktop Runtime（框架依赖发布包）
- 推荐测试显示缩放：100%、125%、150%、175%、200%

## 构建

```powershell
dotnet restore
dotnet build -c Release
```

## 发布

生成轻量的单文件、框架依赖包：

```powershell
dotnet publish -c Release -r win-x64 --self-contained false `
  -p:PublishSingleFile=true `
  -p:IncludeNativeLibrariesForSelfExtract=true `
  -o .\publish\win-x64
```

需要免安装运行时的独立包时，将 `--self-contained false` 改为 `--self-contained true`。独立包体积会明显增大。

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
