// Copyright (c) 2025 Cymrise
// Licensed under the MIT License.

using System;
using System.Drawing;
using System.Drawing.Imaging;
using System.Runtime.InteropServices;
using System.Windows.Forms;

namespace CymriseColorPicker
{
    public partial class MainForm : Form
    {
        // API声明
        [DllImport("user32.dll")]
        private static extern bool SetCursorPos(int x, int y);

        [DllImport("user32.dll")]
        private static extern bool ClipCursor(ref Rectangle lpRect);

        [DllImport("user32.dll")]
        private static extern bool ClipCursor(IntPtr lpRect);

        [DllImport("user32.dll")]
        private static extern bool GetCursorPos(out Point lpPoint);

        [DllImport("user32.dll")]
        private static extern bool ReleaseCapture();

        [DllImport("user32.dll")]
        private static extern int SendMessage(IntPtr hWnd, int Msg, int wParam, int lParam);

        private const int IDC_CROSS = 32515;
        private const int WM_NCLBUTTONDOWN = 0xA1;
        private const int HT_CAPTION = 0x2;

        private bool isPicking = false;
        private Bitmap screenSnapshot;
        private OverlayForm overlayForm;

        private IntPtr crossCursor; // 声明十字光标句柄

        public MainForm()
        {
            InitializeComponent();
            InitializeCursor();
            this.DoubleBuffered = true;
        }

        private void InitializeCursor()
        {
            // 预加载十字光�?
            crossCursor = LoadCursor(IntPtr.Zero, IDC_CROSS);
        }

        private void btnPick_Click(object sender, EventArgs e)
        {
            StartColorPicking();
        }

        private void StartColorPicking()
        {
            if (isPicking) return;
            isPicking = true;

            // 隐藏主窗�?
            // this.Hide();
        
            // 初始化颜色预�?
            lblColor.BackColor = Color.Transparent;
            txtHex.Text = "";
            txtRgb.Text = "";
            lblStatus.Text = "移动鼠标选择颜色";

            // 创建屏幕快照
            CaptureScreenSnapshot();

            // 创建并显示覆盖层
            overlayForm = new OverlayForm(screenSnapshot);
            overlayForm.ColorPicked += OverlayForm_ColorPicked;
            overlayForm.PickingCancelled += OverlayForm_PickingCancelled;
            // 颜色预览事件处理
            overlayForm.ColorPreview += OverlayForm_ColorPreview;
            overlayForm.Show();
        }

        private void CaptureScreenSnapshot()
        {
            // 获取所有屏幕的总区�?
            Rectangle totalBounds = Rectangle.Empty;
            foreach (Screen screen in Screen.AllScreens)
            {
                totalBounds = Rectangle.Union(totalBounds, screen.Bounds);
            }

            screenSnapshot = new Bitmap(totalBounds.Width, totalBounds.Height, PixelFormat.Format32bppArgb);

            using (Graphics g = Graphics.FromImage(screenSnapshot))
            {
                g.CopyFromScreen(totalBounds.Location, Point.Empty, totalBounds.Size);
            }
        }

        // 预览事件处理方法
        private void OverlayForm_ColorPreview(object sender, ColorPreviewEventArgs e)
        {
            // 实时更新颜色预览
            lblColor.BackColor = e.PreviewColor;
            txtHex.Text = ColorToHex(e.PreviewColor);
            txtRgb.Text = $"{e.PreviewColor.R}, {e.PreviewColor.G}, {e.PreviewColor.B}";
        }

        private void OverlayForm_ColorPicked(object sender, ColorEventArgs e)
        {
            EndColorPicking();
            
            // 显示主窗体并更新颜色
            this.Show();
            this.TopMost = true;
            this.TopMost = false;
            
            // 在这里处理获取到的颜�?
            lblColor.BackColor = e.SelectedColor;
            txtHex.Text = ColorToHex(e.SelectedColor);
            // RGB值显�?
            txtRgb.Text = $"{e.SelectedColor.R}, {e.SelectedColor.G}, {e.SelectedColor.B}";
            
            Clipboard.SetText(txtHex.Text);
            lblStatus.Text = $"颜色已复制到剪贴�? {txtHex.Text}";
        }

        private void OverlayForm_PickingCancelled(object sender, EventArgs e)
        {
            EndColorPicking();
            // this.Show();
            lblStatus.Text = "取色已取�?;
        }

        private void EndColorPicking()
        {
            if (!isPicking) return;
            
            isPicking = false;
            
            // 取消预览事件订阅
            if (overlayForm != null)
            {
                overlayForm.ColorPreview -= OverlayForm_ColorPreview;
            }
            
            // 清理资源
            overlayForm?.Dispose();
            overlayForm = null;
            
            screenSnapshot?.Dispose();
            screenSnapshot = null;
            
            // 释放光标限制
            ClipCursor(IntPtr.Zero);
        }

        private string ColorToHex(Color color)
        {
            return $"#{color.R:X2}{color.G:X2}{color.B:X2}";
        }

        // 允许拖动窗体
        protected override void OnMouseDown(MouseEventArgs e)
        {
            base.OnMouseDown(e);
            if (e.Button == MouseButtons.Left)
            {
                ReleaseCapture();
                SendMessage(Handle, WM_NCLBUTTONDOWN, HT_CAPTION, 0);
            }
        }

        private void MainForm_FormClosing(object sender, FormClosingEventArgs e)
        {
            EndColorPicking();
        }

        // 声明缺失的API
        [DllImport("user32.dll")]
        private static extern IntPtr LoadCursor(IntPtr hInstance, int lpCursorName);
    }
}