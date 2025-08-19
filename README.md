# CymriseColorPicker
屏幕取色工具 - 快速获取屏幕上任意位置的颜色值

## 功能
- 实时颜色预览
- HEX/RGB格式显示
- 一键复制到剪贴板
- 快捷键支持（ESC取消，空格/回车确认）

## 使用说明
1. 点击"点击取色"按钮
2. 移动鼠标到目标位置
3. 左键点击或按空格/回车确认
4. 颜色值自动复制到剪贴板

## 编译
dotnet publish -c Release -r win-x64 --self-contained false /p:PublishSingleFile=true /p:IncludeNativeLibrariesForSelfExtract=true

## 截图
