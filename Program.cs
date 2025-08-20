// Copyright (c) 2025 Cymrise
// Licensed under the MIT License.

using System;
using System.Windows.Forms;

namespace CymriseColorPicker
{
    static class Program
    {
        [STAThread]
        static void Main()
        {
            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);
            Application.Run(new MainForm()); // MainForm
        }
    }
}