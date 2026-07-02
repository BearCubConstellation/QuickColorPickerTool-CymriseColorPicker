// Copyright (c) 2025-2026 Cymrise
// Licensed under the MIT License.

using System.Drawing;
using System.Windows.Forms;

namespace CymriseColorPicker
{
    partial class MainForm
    {
        private System.ComponentModel.IContainer components = null;

        private Panel headerPanel;
        private Panel cardPanel;
        private Panel pnlColorPreview;
        private Label lblKicker;
        private Label lblTitle;
        private Label lblHexCaption;
        private Label lblRgbCaption;
        private Label lblHint;
        private Label lblStatus;
        private Label lblFooter;
        private TextBox txtHex;
        private TextBox txtRgb;
        private Button btnPick;
        private Button btnCopyHex;
        private Button btnCopyRgb;

        protected override void Dispose(bool disposing)
        {
            if (disposing && components != null)
            {
                components.Dispose();
            }

            base.Dispose(disposing);
        }

        private void InitializeComponent()
        {
            headerPanel = new Panel();
            cardPanel = new Panel();
            pnlColorPreview = new Panel();
            lblKicker = new Label();
            lblTitle = new Label();
            lblHexCaption = new Label();
            lblRgbCaption = new Label();
            lblHint = new Label();
            lblStatus = new Label();
            lblFooter = new Label();
            txtHex = new TextBox();
            txtRgb = new TextBox();
            btnPick = new Button();
            btnCopyHex = new Button();
            btnCopyRgb = new Button();

            SuspendLayout();

            headerPanel.BackColor = Color.FromArgb(15, 23, 42);
            headerPanel.Dock = DockStyle.Top;
            headerPanel.Location = new Point(0, 0);
            headerPanel.Name = "headerPanel";
            headerPanel.Size = new Size(440, 68);

            lblKicker.AutoSize = true;
            lblKicker.Font = new Font("Segoe UI", 7.5F, FontStyle.Bold);
            lblKicker.ForeColor = Color.FromArgb(147, 197, 253);
            lblKicker.Location = new Point(20, 13);
            lblKicker.Name = "lblKicker";
            lblKicker.Text = "CYMRISE · UTILITY";

            lblTitle.AutoSize = true;
            lblTitle.Font = new Font("Segoe UI Semibold", 15F, FontStyle.Bold);
            lblTitle.ForeColor = Color.White;
            lblTitle.Location = new Point(18, 29);
            lblTitle.Name = "lblTitle";
            lblTitle.Text = "屏幕取色器";

            headerPanel.Controls.Add(lblKicker);
            headerPanel.Controls.Add(lblTitle);

            cardPanel.BackColor = Color.White;
            cardPanel.BorderStyle = BorderStyle.FixedSingle;
            cardPanel.Location = new Point(18, 84);
            cardPanel.Name = "cardPanel";
            cardPanel.Size = new Size(404, 124);

            pnlColorPreview.BackColor = Color.FromArgb(59, 130, 246);
            pnlColorPreview.BorderStyle = BorderStyle.FixedSingle;
            pnlColorPreview.Location = new Point(13, 15);
            pnlColorPreview.Name = "pnlColorPreview";
            pnlColorPreview.Size = new Size(92, 92);

            lblHexCaption.AutoSize = true;
            lblHexCaption.Font = new Font("Segoe UI", 8F, FontStyle.Bold);
            lblHexCaption.ForeColor = Color.FromArgb(100, 116, 139);
            lblHexCaption.Location = new Point(122, 14);
            lblHexCaption.Name = "lblHexCaption";
            lblHexCaption.Text = "HEX";

            txtHex.BackColor = Color.White;
            txtHex.BorderStyle = BorderStyle.FixedSingle;
            txtHex.Font = new Font("Segoe UI Semibold", 11F, FontStyle.Bold);
            txtHex.Location = new Point(122, 31);
            txtHex.Name = "txtHex";
            txtHex.ReadOnly = true;
            txtHex.Size = new Size(174, 27);
            txtHex.TabIndex = 1;
            txtHex.TextAlign = HorizontalAlignment.Center;

            btnCopyHex.BackColor = Color.FromArgb(239, 246, 255);
            btnCopyHex.Cursor = Cursors.Hand;
            btnCopyHex.FlatAppearance.BorderSize = 0;
            btnCopyHex.FlatStyle = FlatStyle.Flat;
            btnCopyHex.Font = new Font("Segoe UI", 8.5F, FontStyle.Bold);
            btnCopyHex.ForeColor = Color.FromArgb(29, 78, 216);
            btnCopyHex.Location = new Point(306, 31);
            btnCopyHex.Name = "btnCopyHex";
            btnCopyHex.Size = new Size(82, 27);
            btnCopyHex.TabIndex = 2;
            btnCopyHex.Text = "复制";
            btnCopyHex.UseVisualStyleBackColor = false;
            btnCopyHex.Click += btnCopyHex_Click;

            lblRgbCaption.AutoSize = true;
            lblRgbCaption.Font = new Font("Segoe UI", 8F, FontStyle.Bold);
            lblRgbCaption.ForeColor = Color.FromArgb(100, 116, 139);
            lblRgbCaption.Location = new Point(122, 68);
            lblRgbCaption.Name = "lblRgbCaption";
            lblRgbCaption.Text = "RGB";

            txtRgb.BackColor = Color.White;
            txtRgb.BorderStyle = BorderStyle.FixedSingle;
            txtRgb.Font = new Font("Segoe UI", 9.5F, FontStyle.Regular);
            txtRgb.Location = new Point(122, 84);
            txtRgb.Name = "txtRgb";
            txtRgb.ReadOnly = true;
            txtRgb.Size = new Size(174, 24);
            txtRgb.TabIndex = 3;
            txtRgb.TextAlign = HorizontalAlignment.Center;

            btnCopyRgb.BackColor = Color.FromArgb(248, 250, 252);
            btnCopyRgb.Cursor = Cursors.Hand;
            btnCopyRgb.FlatAppearance.BorderSize = 0;
            btnCopyRgb.FlatStyle = FlatStyle.Flat;
            btnCopyRgb.Font = new Font("Segoe UI", 8.5F, FontStyle.Bold);
            btnCopyRgb.ForeColor = Color.FromArgb(51, 65, 85);
            btnCopyRgb.Location = new Point(306, 82);
            btnCopyRgb.Name = "btnCopyRgb";
            btnCopyRgb.Size = new Size(82, 27);
            btnCopyRgb.TabIndex = 4;
            btnCopyRgb.Text = "复制";
            btnCopyRgb.UseVisualStyleBackColor = false;
            btnCopyRgb.Click += btnCopyRgb_Click;

            cardPanel.Controls.Add(pnlColorPreview);
            cardPanel.Controls.Add(lblHexCaption);
            cardPanel.Controls.Add(txtHex);
            cardPanel.Controls.Add(btnCopyHex);
            cardPanel.Controls.Add(lblRgbCaption);
            cardPanel.Controls.Add(txtRgb);
            cardPanel.Controls.Add(btnCopyRgb);

            btnPick.BackColor = Color.FromArgb(37, 99, 235);
            btnPick.Cursor = Cursors.Hand;
            btnPick.FlatAppearance.BorderSize = 0;
            btnPick.FlatStyle = FlatStyle.Flat;
            btnPick.Font = new Font("Segoe UI Semibold", 10.5F, FontStyle.Bold);
            btnPick.ForeColor = Color.White;
            btnPick.Location = new Point(18, 226);
            btnPick.Name = "btnPick";
            btnPick.Size = new Size(404, 44);
            btnPick.TabIndex = 0;
            btnPick.Text = "开始取色";
            btnPick.UseVisualStyleBackColor = false;
            btnPick.Click += btnPick_Click;

            lblHint.AutoSize = false;
            lblHint.Font = new Font("Segoe UI", 8.5F, FontStyle.Regular);
            lblHint.ForeColor = Color.FromArgb(100, 116, 139);
            lblHint.Location = new Point(18, 281);
            lblHint.Name = "lblHint";
            lblHint.Size = new Size(404, 18);
            lblHint.Text = "左键确认 · Space / Enter 确认 · Esc / 右键取消";
            lblHint.TextAlign = ContentAlignment.MiddleCenter;

            lblStatus.AutoSize = false;
            lblStatus.Font = new Font("Segoe UI", 9F, FontStyle.Regular);
            lblStatus.ForeColor = Color.FromArgb(5, 150, 105);
            lblStatus.Location = new Point(18, 304);
            lblStatus.Name = "lblStatus";
            lblStatus.Size = new Size(404, 20);
            lblStatus.TextAlign = ContentAlignment.MiddleCenter;

            lblFooter.AutoSize = false;
            lblFooter.Font = new Font("Segoe UI", 8F, FontStyle.Regular);
            lblFooter.ForeColor = Color.FromArgb(148, 163, 184);
            lblFooter.Location = new Point(18, 327);
            lblFooter.Name = "lblFooter";
            lblFooter.Size = new Size(404, 16);
            lblFooter.Text = "Cymrise Color Picker · v1.1.0";
            lblFooter.TextAlign = ContentAlignment.MiddleCenter;

            AutoScaleMode = AutoScaleMode.Dpi;
            BackColor = Color.FromArgb(248, 250, 252);
            ClientSize = new Size(440, 352);
            Controls.Add(headerPanel);
            Controls.Add(cardPanel);
            Controls.Add(btnPick);
            Controls.Add(lblHint);
            Controls.Add(lblStatus);
            Controls.Add(lblFooter);
            Font = new Font("Segoe UI", 9F, FontStyle.Regular);
            FormBorderStyle = FormBorderStyle.FixedSingle;
            MaximizeBox = false;
            MinimizeBox = true;
            Name = "MainForm";
            StartPosition = FormStartPosition.CenterScreen;
            Text = "Cymrise Color Picker";
            FormClosing += MainForm_FormClosing;

            ResumeLayout(false);
        }
    }
}
