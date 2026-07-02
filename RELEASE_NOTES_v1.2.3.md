# ColorPicker v1.2.3

## 界面修复

- 程序标题统一为 `ColorPicker`。
- 移除顶部 `CYMRISE · UTILITY` 字样，收紧标题区高度，减少无效留白。
- 修复默认窗口尺寸与 DPI 缩放的计算方式，恢复完整可见的“开始取色”按钮。
- HEX 与 RGB 均使用独立、固定的可用文本区域，不再被复制按钮遮挡或省略。
- 增加窗口最小尺寸限制，防止手动缩小后出现内容裁剪。

## 项目信息

- 恢复底部灰色 `By cymrise` 入口。
- 点击 `By cymrise` 会打开项目介绍窗口；点击其中的 GitHub 链接可在浏览器打开仓库。

## 发布

- Windows x64 原生单文件发布包。
- 不包含 .NET Runtime、PDB、`.config` 或外部运行时依赖。
