// Copyright (c) 2025 Cymrise
// Licensed under the MIT License.

using System;
using System.Drawing;
namespace CymriseColorPicker
{
    public class ColorPreviewEventArgs : EventArgs
    {
        public Color PreviewColor { get; }

        public ColorPreviewEventArgs(Color color)
        {
            PreviewColor = color;
        }
    }
}