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