// Copyright (c) 2025 Cymrise
// Licensed under the MIT License.

using System;
using System.Drawing;

namespace CymriseColorPicker
{
    public class ColorEventArgs : EventArgs
    {
        public Color SelectedColor { get; }

        public ColorEventArgs(Color color)
        {
            SelectedColor = color;
        }
    }
}