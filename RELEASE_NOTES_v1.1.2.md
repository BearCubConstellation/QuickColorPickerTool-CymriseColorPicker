# Cymrise Color Picker v1.1.2

## 轻量版修正

- 回退 v1.1.1 的完整运行时打包策略。
- 不再把 .NET 8 Desktop Runtime 塞进发布包。
- 目标框架切换为 .NET Framework 4.8。
- Windows 10 / 11 通常可直接运行，不再弹出 .NET 8 Desktop Runtime 安装提示。
- 发布包名称：`CymriseColorPicker-v1.1.2-lightweight-win-x64.zip`。
- 正式包不包含 `ColorPicker.pdb`。

## 保留内容

- 保留 v1.1.0 的 DPI 缩放取色修复。
- 保留多显示器与负坐标布局修复。
- 保留现代化界面与取色悬浮预览。
