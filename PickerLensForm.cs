// Copyright (c) 2025-2026 Cymrise
// Licensed under the MIT License.

using System;
using System.Drawing;
using System.Windows.Forms;

namespace CymriseColorPicker
{
    internal sealed class PickerLensForm : Form
    {
        private const int WmNcHitTest = 0x0084;
        private const int HtTransparent = -1;
        private const int WsExNoActivate = 0x08000000;
        private const int WsExToolWindow = 0x00000080;

        private Color previewColor = Color.Black;
        private string hex = "#000000";

        public PickerLensForm()
        {
            AutoScaleMode = AutoScaleMode.None;
            FormBorderStyle = FormBorderStyle.None;
            ShowInTaskbar = false;
            TopMost = true;
            Size = new Size(184, 84);
            BackColor = Color.FromArgb(17, 24, 39);
            DoubleBuffered = true;
        }

        protected override bool ShowWithoutActivation => true;

        protected override CreateParams CreateParams
        {
            get
            {
                CreateParams parameters = base.CreateParams;
                parameters.ExStyle |= WsExNoActivate | WsExToolWindow;
                return parameters;
            }
        }

        protected override void WndProc(ref Message m)
        {
            if (m.Msg == WmNcHitTest)
            {
                m.Result = (IntPtr)HtTransparent;
                return;
            }

            base.WndProc(ref m);
        }

        public void UpdatePreview(Color color, Point cursorPosition, Rectangle virtualScreenBounds)
        {
            previewColor = color;
            hex = $"#{color.R:X2}{color.G:X2}{color.B:X2}";
            Location = ResolveLocation(cursorPosition, virtualScreenBounds);
            Invalidate();
        }

        protected override void OnPaint(PaintEventArgs e)
        {
            base.OnPaint(e);
            e.Graphics.Clear(BackColor);

            Rectangle swatchBounds = new Rectangle(12, 12, 60, 60);
            using SolidBrush colorBrush = new SolidBrush(previewColor);
            using Pen borderPen = new Pen(Color.FromArgb(148, 163, 184));
            using Font hexFont = new Font("Segoe UI Semibold", 12f, FontStyle.Bold);

            e.Graphics.FillRectangle(colorBrush, swatchBounds);
            e.Graphics.DrawRectangle(borderPen, swatchBounds);

            TextRenderer.DrawText(
                e.Graphics,
                "当前颜色",
                Font,
                new Point(86, 13),
                Color.FromArgb(148, 163, 184));

            TextRenderer.DrawText(
                e.Graphics,
                hex,
                hexFont,
                new Point(86, 34),
                Color.White);
        }

        private Point ResolveLocation(Point cursorPosition, Rectangle virtualScreenBounds)
        {
            const int offset = 20;
            int x = cursorPosition.X + offset;
            int y = cursorPosition.Y + offset;

            if (x + Width > virtualScreenBounds.Right)
            {
                x = cursorPosition.X - Width - offset;
            }

            if (y + Height > virtualScreenBounds.Bottom)
            {
                y = cursorPosition.Y - Height - offset;
            }

            x = Math.Min(Math.Max(x, virtualScreenBounds.Left), virtualScreenBounds.Right - Width);
            y = Math.Min(Math.Max(y, virtualScreenBounds.Top), virtualScreenBounds.Bottom - Height);
            return new Point(x, y);
        }
    }
}
