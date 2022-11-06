using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Collections.Generic;
using System.Reflection;

namespace DownloadToolUI
{
    internal static class MyApp
    {

        public static void Quit()
        {
            Application.Exit();
        }

#pragma warning disable IDE1006 // 命名样式
        private static Form_ExitConfirm? exitConfirmDialog { get; set; }
#pragma warning restore IDE1006 // 命名样式
        public static void ConfirmQuit()
        {
            if (exitConfirmDialog == null)
            {
                exitConfirmDialog = new Form_ExitConfirm();
                DialogResult result = exitConfirmDialog.ShowDialog();
                if ((result == DialogResult.Yes))
                {
                    Quit();
                }
                exitConfirmDialog.Dispose();
                exitConfirmDialog = null;
            }

        }

/*        [DllImport("Kernel32.dll")]
        public static extern IntPtr LoadLibrary(string path);

        [DllImport("Kernel32.dll")]
        public static extern bool FreeLibrary(IntPtr hModule);

        [DllImport("Kernel32.dll")]
        private static extern IntPtr GetProcAddress(IntPtr hModule, string procName);

        public static Delegate LoadFunction<T>(IntPtr hModule, string functionName)
        {
            IntPtr functionAddress = GetProcAddress(hModule, functionName);
            if (functionAddress.ToInt64() == 0)
            {
                return null;
            }
            return Marshal.GetDelegateForFunctionPointer(functionAddress, typeof(T));
        }
*/
        public static uint PendingDownloadsCount {
            get {
                return 1;
            }
        }

    }
}
