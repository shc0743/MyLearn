using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace DownloadToolUI
{
    public partial class Form_NewDownloadTask : Form
    {
        public Form_NewDownloadTask()
        {
            InitializeComponent();
        }

        public TabControl GetTabControl()
        {
            return this.tabControl;
        }

        private void button_cancel_Click(object sender, EventArgs e)
        {
            this.Close();
        }

        private void button_schedule_Click(object sender, EventArgs e)
        {
            this.tabControl.SelectedIndex = 1;
        }
    }
}
