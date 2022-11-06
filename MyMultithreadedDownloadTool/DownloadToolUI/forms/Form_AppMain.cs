using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Runtime.InteropServices;
using static System.Windows.Forms.VisualStyles.VisualStyleElement;

namespace DownloadToolUI
{
    public partial class Form_AppMain : Form
    {
        public Form_AppMain()
        {
            InitializeComponent();

            ulong hMenu = GetSystemMenu(this.Handle.ToInt64(), false);
            if (hMenu != 0) DeleteMenu(hMenu, 0xF060, 0x0);
        }

        [DllImport("user32.dll")]
        internal static extern ulong GetSystemMenu(long hwnd, bool bRevert);
        [DllImport("user32.dll")]
        internal static extern int DeleteMenu(ulong hMenu, uint uPos, uint flags);

        private void Form_AppMain_FormClosing(object sender, FormClosingEventArgs e)
        {
            if (e.CloseReason == CloseReason.UserClosing)
            {
                e.Cancel = true;
            }
        }

        private void button1_Click(object sender, EventArgs e)
        {
            Form_NewDownloadTask dlg = new();
            dlg.MdiParent = this.MdiParent;
            dlg.WindowState = FormWindowState.Normal;
            dlg.Show();
        }

        private void notifyIcon_Click(object sender, EventArgs e)
        {
            this.MdiParent.ShowInTaskbar = true;
            this.MdiParent.WindowState = FormWindowState.Normal;
            this.MdiParent.Show();
            this.MdiParent.Activate();
        }
    }
}
