// Copyright (c) 2025-2026 Cymrise
// Licensed under the MIT License.

using System;
using System.Drawing;
using System.Drawing.Imaging;
using System.Runtime.InteropServices;
using System.Threading;
using System.Windows.Forms;

namespace CymriseColorPicker
{
    public partial class MainForm : Form
    {
        private bool isPicking;
        private Bitmap screenSnapshot;
        private OverlayForm overlayForm;
        private Rectangle virtualScreenBounds;

        public MainForm()
        {
            InitializeComponent();
            DoubleBuffered = true;
            UpdateSelectedColor(Color.FromArgb(59, 130, 246));
            lblStatus.Text = "就绪，可开始取色";
        }

        private void btnPick_Click(object sender, EventArgs e)
        {
            StartColorPicking();
        }

        private void btnCopyHex_Click(object sender, EventArgs e)
        {
            CopyToClipboard(txtHex.Text, "HEX");
        }

        private void btnCopyRgb_Click(object sender, EventArgs e)
        {
            CopyToClipboard(txtRgb.Text, "RGB");
        }

        private void StartColorPicking()
        {
            if (isPicking)
            {
                return;
            }

            try
            {
                // Hide before the snapshot is captured so the application itself cannot be sampled.
                Hide();
                Application.DoEvents();

                CaptureScreenSnapshot();
                overlayForm = new OverlayForm(screenSnapshot, virtualScreenBounds);
                overlayForm.ColorPicked += OverlayForm_ColorPicked;
                overlayForm.ColorPreview += OverlayForm_ColorPreview;
                overlayForm.PickingCancelled += OverlayForm_PickingCancelled;
                overlayForm.FormClosed += OverlayForm_FormClosed;

                isPicking = true;
                overlayForm.Show();
            }
            catch
            {
                StopColorPicking(closeOverlay: true);
                ShowMainWindow();
                lblStatus.Text = "无法启动取色，请重试";
            }
        }

        private void CaptureScreenSnapshot()
        {
            screenSnapshot?.Dispose();
            virtualScreenBounds = SystemInformation.VirtualScreen;

            if (virtualScreenBounds.Width <= 0 || virtualScreenBounds.Height <= 0)
            {
                throw new InvalidOperationException("无法获取虚拟桌面范围。");
            }

            screenSnapshot = new Bitmap(
                virtualScreenBounds.Width,
                virtualScreenBounds.Height,
                PixelFormat.Format32bppArgb);

            using Graphics graphics = Graphics.FromImage(screenSnapshot);
            graphics.CopyFromScreen(
                virtualScreenBounds.Location,
                Point.Empty,
                virtualScreenBounds.Size,
                CopyPixelOperation.SourceCopy);
        }

        private void OverlayForm_ColorPreview(object sender, ColorPreviewEventArgs e)
        {
            UpdateSelectedColor(e.PreviewColor);
        }

        private void OverlayForm_ColorPicked(object sender, ColorEventArgs e)
        {
            StopColorPicking(closeOverlay: false);
            ShowMainWindow();
            UpdateSelectedColor(e.SelectedColor);

            lblStatus.Text = TryCopyText(txtHex.Text)
                ? $"已复制 HEX：{txtHex.Text}"
                : $"已取色：{txtHex.Text}（复制失败，可点击复制）";
        }

        private void OverlayForm_PickingCancelled(object sender, EventArgs e)
        {
            StopColorPicking(closeOverlay: false);
            ShowMainWindow();
            lblStatus.Text = "已取消取色";
        }

        private void OverlayForm_FormClosed(object sender, FormClosedEventArgs e)
        {
            if (!isPicking)
            {
                return;
            }

            StopColorPicking(closeOverlay: false);
            ShowMainWindow();
            lblStatus.Text = "取色已结束";
        }

        private void StopColorPicking(bool closeOverlay)
        {
            OverlayForm overlay = overlayForm;
            overlayForm = null;
            isPicking = false;

            if (overlay != null)
            {
                overlay.ColorPicked -= OverlayForm_ColorPicked;
                overlay.ColorPreview -= OverlayForm_ColorPreview;
                overlay.PickingCancelled -= OverlayForm_PickingCancelled;
                overlay.FormClosed -= OverlayForm_FormClosed;

                if (closeOverlay && !overlay.IsDisposed)
                {
                    overlay.Close();
                }
            }

            screenSnapshot?.Dispose();
            screenSnapshot = null;
        }

        private void ShowMainWindow()
        {
            Show();
            Activate();
        }

        private void UpdateSelectedColor(Color color)
        {
            pnlColorPreview.BackColor = color;
            txtHex.Text = ColorToHex(color);
            txtRgb.Text = $"RGB({color.R}, {color.G}, {color.B})";
        }

        private void CopyToClipboard(string value, string formatName)
        {
            if (string.IsNullOrWhiteSpace(value))
            {
                lblStatus.Text = "暂无可复制的颜色值";
                return;
            }

            lblStatus.Text = TryCopyText(value)
                ? $"{formatName} 已复制到剪贴板"
                : "复制失败，请重试";
        }

        private static bool TryCopyText(string value)
        {
            for (int attempt = 0; attempt < 3; attempt++)
            {
                try
                {
                    Clipboard.SetText(value);
                    return true;
                }
                catch (ExternalException)
                {
                    Thread.Sleep(30);
                }
            }

            return false;
        }

        private static string ColorToHex(Color color)
        {
            return $"#{color.R:X2}{color.G:X2}{color.B:X2}";
        }

        private void MainForm_FormClosing(object sender, FormClosingEventArgs e)
        {
            StopColorPicking(closeOverlay: true);
        }
    }
}
