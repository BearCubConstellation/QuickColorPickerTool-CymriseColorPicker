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