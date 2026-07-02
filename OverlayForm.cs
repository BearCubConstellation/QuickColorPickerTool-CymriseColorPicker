// Copyright (c) 2025-2026 Cymrise
// Licensed under the MIT License.

using System;
using System.Drawing;
using System.Windows.Forms;

namespace CymriseColorPicker
{
    public class OverlayForm : Form
    {
        private readonly Bitmap screenSnapshot;
        private readonly Rectangle virtualScreenBounds;
        private Point lastScreenPoint;
        private PickerLensForm lensForm;

        public event EventHandler<ColorEventArgs> ColorPicked;
        public event EventHandler<ColorPreviewEventArgs> ColorPreview;
        public event EventHandler PickingCancelled;

        public OverlayForm(Bitmap snapshot, Rectangle snapshotBounds)
        {
            screenSnapshot = snapshot ?? throw new ArgumentNullException(nameof(snapshot));
            virtualScreenBounds = snapshotBounds;
            InitializeForm();
        }

        private void InitializeForm()
        {
            AutoScaleMode = AutoScaleMode.None;
            FormBorderStyle = FormBorderStyle.None;
            StartPosition = FormStartPosition.Manual;
            Bounds = virtualScreenBounds;
            TopMost = true;
            ShowInTaskbar = false;
            DoubleBuffered = true;
            KeyPreview = true;

            // This nearly transparent input surface leaves the desktop visible while the
            // immutable snapshot provides stable pixel sampling.
            BackColor = Color.Black;
            Opacity = 0.01d;
            Cursor = Cursors.Cross;

            MouseMove += OverlayForm_MouseMove;
            MouseDown += OverlayForm_MouseDown;
        }

        protected override void OnShown(EventArgs e)
        {
            base.OnShown(e);

            lastScreenPoint = ClampToVirtualScreen(Cursor.Position);
            lensForm = new PickerLensForm();
            lensForm.Show();
            PreviewAt(lastScreenPoint);

            BeginInvoke(new MethodInvoker(() =>
            {
                if (!IsDisposed)
                {
                    Activate();
                    Focus();
                }
            }));
        }

        protected override bool ProcessCmdKey(ref Message msg, Keys keyData)
        {
            Keys keyCode = keyData & Keys.KeyCode;

            if (keyCode == Keys.Escape)
            {
                CancelPicking();
                return true;
            }

            if (keyCode == Keys.Space || keyCode == Keys.Enter)
            {
                FinishPicking(lastScreenPoint);
                return true;
            }

            return base.ProcessCmdKey(ref msg, keyData);
        }

        protected override void OnFormClosed(FormClosedEventArgs e)
        {
            if (lensForm != null)
            {
                lensForm.Close();
                lensForm.Dispose();
                lensForm = null;
            }

            base.OnFormClosed(e);
        }

        private void OverlayForm_MouseMove(object sender, MouseEventArgs e)
        {
            PreviewAt(PointToScreen(e.Location));
        }

        private void OverlayForm_MouseDown(object sender, MouseEventArgs e)
        {
            if (e.Button == MouseButtons.Left)
            {
                FinishPicking(PointToScreen(e.Location));
            }
            else if (e.Button == MouseButtons.Right)
            {
                CancelPicking();
            }
        }

        private void PreviewAt(Point screenPoint)
        {
            lastScreenPoint = ClampToVirtualScreen(screenPoint);
            Color currentColor = GetColorFromSnapshot(lastScreenPoint);

            lensForm?.UpdatePreview(currentColor, lastScreenPoint, virtualScreenBounds);
            ColorPreview?.Invoke(this, new ColorPreviewEventArgs(currentColor));
        }

        private void FinishPicking(Point screenPoint)
        {
            Color color = GetColorFromSnapshot(ClampToVirtualScreen(screenPoint));

            try
            {
                ColorPicked?.Invoke(this, new ColorEventArgs(color));
            }
            finally
            {
                Close();
            }
        }

        private void CancelPicking()
        {
            try
            {
                PickingCancelled?.Invoke(this, EventArgs.Empty);
            }
            finally
            {
                Close();
            }
        }

        private Point ClampToVirtualScreen(Point screenPoint)
        {
            int x = Math.Min(Math.Max(screenPoint.X, virtualScreenBounds.Left), virtualScreenBounds.Right - 1);
            int y = Math.Min(Math.Max(screenPoint.Y, virtualScreenBounds.Top), virtualScreenBounds.Bottom - 1);
            return new Point(x, y);
        }

        private Color GetColorFromSnapshot(Point screenPoint)
        {
            int snapshotX = screenPoint.X - virtualScreenBounds.Left;
            int snapshotY = screenPoint.Y - virtualScreenBounds.Top;

            snapshotX = Math.Min(Math.Max(snapshotX, 0), screenSnapshot.Width - 1);
            snapshotY = Math.Min(Math.Max(snapshotY, 0), screenSnapshot.Height - 1);

            return screenSnapshot.GetPixel(snapshotX, snapshotY);
        }
    }
}
