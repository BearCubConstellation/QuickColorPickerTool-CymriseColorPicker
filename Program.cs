// Copyright (c) 2025-2026 Cymrise
// Licensed under the MIT License.

using System;
using System.Windows.Forms;

namespace CymriseColorPicker
{
    internal static class Program
    {
        [STAThread]
        private static void Main()
        {
            // Must run before any window or control is created.
            Application.SetHighDpiMode(HighDpiMode.PerMonitorV2);
            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);
            Application.Run(new MainForm());
        }
    }
}
