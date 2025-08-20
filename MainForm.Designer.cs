// Copyright (c) 2025 Cymrise
// Licensed under the MIT License.

namespace CymriseColorPicker
{
    partial class MainForm
    {
        private System.ComponentModel.IContainer components = null;
        
        // 声明控件
        protected internal System.Windows.Forms.Button btnPick;
        protected internal System.Windows.Forms.Label lblColor;
        protected internal System.Windows.Forms.TextBox txtHex;
        protected internal System.Windows.Forms.Label lblStatus;
        protected internal System.Windows.Forms.Label lblTitle;
        protected internal System.Windows.Forms.Label lblRgb;
        protected internal System.Windows.Forms.Label author;
        protected internal System.Windows.Forms.TextBox txtRgb;


        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        private void InitializeComponent()
        {
            this.btnPick = new System.Windows.Forms.Button();
            this.lblColor = new System.Windows.Forms.Label();
            this.txtHex = new System.Windows.Forms.TextBox();
            this.lblStatus = new System.Windows.Forms.Label();
            this.lblTitle = new System.Windows.Forms.Label();
            this.author = new System.Windows.Forms.Label();
            this.SuspendLayout();
            // 
            // btnPick
            // 
            this.btnPick.Location = new System.Drawing.Point(20, 80);
            this.btnPick.Name = "btnPick";
            this.btnPick.Size = new System.Drawing.Size(100, 30);
            this.btnPick.TabIndex = 0;
            this.btnPick.Text = "点击取色";
            this.btnPick.UseVisualStyleBackColor = true;
            this.btnPick.Click += new System.EventHandler(this.btnPick_Click);
            // 
            // lblColor
            // 
            this.lblColor.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.lblColor.Location = new System.Drawing.Point(20, 20);
            this.lblColor.Name = "lblColor";
            this.lblColor.Size = new System.Drawing.Size(100, 50);
            this.lblColor.TabIndex = 1;
            // 
            // txtHex
            // 
            this.txtHex.Location = new System.Drawing.Point(170, 12);
            this.txtHex.Name = "txtHex";
            this.txtHex.ReadOnly = true;
            this.txtHex.Size = new System.Drawing.Size(100, 21);
            this.txtHex.TabIndex = 2;
            // 
            // lblStatus
            // 
            this.lblStatus.Location = new System.Drawing.Point(20, 120);
            this.lblStatus.Name = "lblStatus";
            this.lblStatus.Size = new System.Drawing.Size(205, 20);
            this.lblStatus.TabIndex = 3;
            this.lblStatus.Text = "就绪";
            // 
            // lblTitle
            // 
            this.lblTitle.AutoSize = true;
            this.lblTitle.Location = new System.Drawing.Point(130, 15);
            this.lblTitle.Name = "lblTitle";
            this.lblTitle.Size = new System.Drawing.Size(41, 12);
            this.lblTitle.TabIndex = 4;
            this.lblTitle.Text = "HEX:";


            // RGB标题标签
            this.lblRgb = new System.Windows.Forms.Label();
            this.lblRgb.AutoSize = true;
            this.lblRgb.Location = new System.Drawing.Point(130, 60);
            this.lblRgb.Name = "lblRgb";
            this.lblRgb.Size = new System.Drawing.Size(35, 12);
            this.lblRgb.Text = "RGB:";
            
            // RGB值文本框
            this.txtRgb = new System.Windows.Forms.TextBox();
            this.txtRgb.Location = new System.Drawing.Point(175, 55);
            this.txtRgb.Name = "txtRgb";
            this.txtRgb.ReadOnly = true;
            this.txtRgb.Size = new System.Drawing.Size(100, 21);
            this.txtRgb.TabIndex = 5;

            // 作者信�?
            this.author.Location = new System.Drawing.Point(220, 118);
            this.author.Name = "author";
            this.author.Size = new System.Drawing.Size(250, 20);
            this.author.TabIndex = 6;
            this.author.Text = "By Cymrise";

            // 禁止更改大小
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
            this.MaximizeBox = false;
            this.MinimizeBox = true;

            // 
            // MainForm
            // 
            this.ClientSize = new System.Drawing.Size(300, 140);
            this.Controls.Add(this.lblTitle);
            this.Controls.Add(this.lblStatus);
            this.Controls.Add(this.txtHex);
            this.Controls.Add(this.lblColor);
            this.Controls.Add(this.btnPick);
            this.Controls.Add(this.lblRgb);
            this.Controls.Add(this.txtRgb);
            this.Controls.Add(this.author);
            this.Name = "MainForm";
            this.Text = "屏幕取色�?;
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.MainForm_FormClosing);
            this.ResumeLayout(false);
            this.PerformLayout();

        }
    }
}