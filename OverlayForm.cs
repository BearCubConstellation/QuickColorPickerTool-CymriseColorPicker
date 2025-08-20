// Copyright (c) 2025 Cymrise
// Licensed under the MIT License.

using System;
using System.Drawing;
using System.Runtime.InteropServices;
using System.Windows.Forms;

namespace CymriseColorPicker
{
    public class OverlayForm : Form
    {
        // 在覆盖层中声明所需的API
        [DllImport("user32.dll")]
        private static extern bool SetCursorPos(int x, int y);

        [DllImport("user32.dll")]
        private static extern bool ClipCursor(ref Rectangle lpRect);

        private readonly Bitmap screenSnapshot;
        private Point lastMousePos;

        public event EventHandler<ColorEventArgs> ColorPicked;
        public event EventHandler<ColorPreviewEventArgs> ColorPreview;
        public event EventHandler PickingCancelled;

        public OverlayForm(Bitmap snapshot)
        {
            screenSnapshot = snapshot;
            InitializeForm();
        }

        private void InitializeForm()
        {
            // 无边框全屏窗�?
            FormBorderStyle = FormBorderStyle.None;
            WindowState = FormWindowState.Maximized;
            TopMost = true;
            ShowInTaskbar = false;
            DoubleBuffered = true;

            // 透明背景设置
            BackColor = Color.Magenta;
            TransparencyKey = Color.Magenta;
            Opacity = 0.01; // 几乎透明但仍能捕获事�?

            // 设置十字光标
            Cursor = Cursors.Cross;

            // 事件处理
            MouseMove += OverlayForm_MouseMove;
            MouseClick += OverlayForm_MouseClick;
            KeyDown += OverlayForm_KeyDown;
            Paint += OverlayForm_Paint;
        }

        private void OverlayForm_Paint(object sender, PaintEventArgs e)
        {
            // 绘制屏幕快照
            e.Graphics.DrawImage(screenSnapshot, Point.Empty);
        }

        private void OverlayForm_MouseMove(object sender, MouseEventArgs e)
        {
            // 限制光标在屏幕范围内
            if (!ScreenBounds.Contains(e.Location))
            {
                Point newPos = e.Location;
                if (newPos.X < ScreenBounds.Left) newPos.X = ScreenBounds.Left;
                if (newPos.Y < ScreenBounds.Top) newPos.Y = ScreenBounds.Top;
                if (newPos.X > ScreenBounds.Right) newPos.X = ScreenBounds.Right;
                if (newPos.Y > ScreenBounds.Bottom) newPos.Y = ScreenBounds.Bottom;
                
                SetCursorPos(newPos.X, newPos.Y);
            }
            
            lastMousePos = e.Location;
            this.Text = $"X: {e.X} Y: {e.Y}";
            
            // 获取当前颜色并触发预览事�?
            Color currentColor = GetColorFromSnapshot(e.Location);
            ColorPreview?.Invoke(this, new ColorPreviewEventArgs(currentColor));
        }

        private Rectangle ScreenBounds
        {
            get
            {
                Rectangle bounds = Rectangle.Empty;
                foreach (Screen screen in Screen.AllScreens)
                {
                    bounds = Rectangle.Union(bounds, screen.Bounds);
                }
                return bounds;
            }
        }

        private void OverlayForm_MouseClick(object sender, MouseEventArgs e)
        {
            if (e.Button == MouseButtons.Left)
            {
                // 从快照获取颜�?
                Color color = GetColorFromSnapshot(e.Location);
                ColorPicked?.Invoke(this, new ColorEventArgs(color));
                this.Close();
            }
            else if (e.Button == MouseButtons.Right)
            {
                PickingCancelled?.Invoke(this, EventArgs.Empty);
                this.Close();
            }
        }

        private Color GetColorFromSnapshot(Point location)
        {
            // 确保位置在快照范围内
            location.X = Math.Max(0, Math.Min(location.X, screenSnapshot.Width - 1));
            location.Y = Math.Max(0, Math.Min(location.Y, screenSnapshot.Height - 1));
            
            return screenSnapshot.GetPixel(location.X, location.Y);
        }

        private void OverlayForm_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyCode == Keys.Escape)
            {
                PickingCancelled?.Invoke(this, EventArgs.Empty);
                this.Close();
            }
            else if (e.KeyCode == Keys.Space || e.KeyCode == Keys.Enter)
            {
                // 空格键或回车键也可取�?
                Color color = GetColorFromSnapshot(lastMousePos);
                ColorPicked?.Invoke(this, new ColorEventArgs(color));
                this.Close();
            }
        }

        protected override void OnShown(EventArgs e)
        {
            base.OnShown(e);
            
            // 将光标限制在屏幕范围�?
            Rectangle bounds = ScreenBounds;
            ClipCursor(ref bounds);
        }

        protected override void OnClosed(EventArgs e)
        {
            base.OnClosed(e);
            // 释放光标限制
            ClipCursor(IntPtr.Zero);
        }

        // 声明缺失的API
        [DllImport("user32.dll")]
        private static extern bool ClipCursor(IntPtr lpRect);
    }
}