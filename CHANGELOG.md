# Changelog

## v1.3.1

### Improved

- Tightened the three bottom text rows into one compact status group.
- Increased the size and hierarchy of HEX, RGB, Copy controls, and the primary picker action.
- Unified both Copy buttons with the same blue-tinted visual treatment.
- Refined the dynamic title shadow for better readability on the dark header.

## v1.3.0

### Added

- Added an English executable, `ColorPicker-en.exe`, built from the same native source as the Chinese edition.
- Release packages now include separate `zh-CN` and `en-US` Windows x64 archives.
- Added a bilingual README with a latest-release download link.

### Improved

- Further reduced header height, bottom spacing, control heights, and default window size for a true small-tool layout.
- The header title starts white and changes to the confirmed picked color.
- Cleaned obsolete duplicate source and superseded release-note files from the active repository tree.

### Fixed

- Preserved frozen-desktop picking, full-screen click capture, and real-time preview-lens tracking.

## v1.2.x

- Migrated from WinForms/.NET to native Win32 C++ with a static C++ runtime.
- Added Per-Monitor V2 DPI support, multi-monitor / negative-coordinate support, frozen desktop picking, and the project GitHub entry.
