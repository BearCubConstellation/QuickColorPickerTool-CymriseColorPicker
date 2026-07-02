using System.Drawing;
using System.Runtime.InteropServices;

namespace ConnectorProbe
{
    internal static class NativeInteropProbe
    {
        [DllImport("user32.dll")]
        internal static extern bool GetCursorPos(out Point point);
    }
}
