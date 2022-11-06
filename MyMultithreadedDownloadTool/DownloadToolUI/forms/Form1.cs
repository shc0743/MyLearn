namespace DownloadToolUI
{
    public partial class Form_Main : Form
    {

        public Form_Main()
        {
            InitializeComponent();

        }

        private void exitToolStripMenuItem_Click(object sender, EventArgs e)
        {
            MyApp.ConfirmQuit();
        }

        private void closeToolStripMenuItem_Click(object sender, EventArgs e)
        {
            Close();
        }

        private void exitApplicationToolStripMenuItem_Click(object sender, EventArgs e)
        {
            MyApp.ConfirmQuit();
        }

        private void Form_Main_FormClosing(object sender, FormClosingEventArgs e)
        {
            if (
                (e.CloseReason == CloseReason.UserClosing) ||
                (e.CloseReason == CloseReason.WindowsShutDown && MyApp.PendingDownloadsCount > 0)
            ) {
                e.Cancel = true;
                Form_CloseConfirm form = new Form_CloseConfirm();
                DialogResult result = form.ShowDialog();
                form.Dispose();
                if (result == DialogResult.Yes)
                {
                    this.ShowInTaskbar = false;
                    this.WindowState = FormWindowState.Minimized;
                    this.Hide();
                }
            }
        }

        private void Form_Main_KeyDown(object sender, KeyEventArgs e)
        {

        }

        private void openLicenseToolStripMenuItem_Click(object sender, EventArgs e)
        {
            System.Diagnostics.Process.Start("explorer.exe https://www.gnu.org/licenses/lgpl-3.0.html");
        }

        private void newTaskToolStripMenuItem_Click(object sender, EventArgs e)
        {
            Form_NewDownloadTask dlg = new();
            dlg.MdiParent = this;
            dlg.WindowState = FormWindowState.Normal;
            dlg.Show();
        }

        private void advancedTaskToolStripMenuItem_Click(object sender, EventArgs e)
        {
            Form_NewDownloadTask dlg = new();
            dlg.MdiParent = this;
            dlg.WindowState = FormWindowState.Normal;
            dlg.Show();
        }

        private void newSimpleTaskToolStripMenuItem_Click(object sender, EventArgs e)
        {
            Form_NewDownloadTask dlg = new();
            dlg.MdiParent = this;
            dlg.GetTabControl().TabPages.RemoveAt(2);
            dlg.WindowState = FormWindowState.Normal;
            dlg.Show();
        }

        private void commandPromptToolStripMenuItem_Click(object sender, EventArgs e)
        {
            System.Diagnostics.Process.Start("cmd.exe /c start cmd.exe");
        }

        private void commandPanelToolStripMenuItem_Click(object sender, EventArgs e)
        {
            //int x = 0, y = 0;
            //int w = 0, h = 0;
            //int a = 0, b = 0;
            int x, y, a, w;
            x = this.Location.X;
            y = this.Location.Y;
            a = this.Width;
            //b = this.Height;
            Form_CommandPanel dlg = new();
            w = dlg.Width;
            //h = dlg.Height;
            x = x + ((a / 2) - (w / 2));
            y = y + 20;
            dlg.Location = new Point(x, y);
            dlg.Show();
        }

        private void settingsToolStripMenuItem_Click_1(object sender, EventArgs e)
        {
            try
            {
                Form_AppSettings dlg = new();
                dlg.MdiParent = this;
                dlg.WindowState = FormWindowState.Maximized;
                dlg.Show();
            }
            catch (ObjectDisposedException) { }
        }

        private void printToolStripMenuItem_Click(object sender, EventArgs e)
        {
            PrintDialog dlg = new PrintDialog();
            dlg.ShowDialog();
        }
    }
}