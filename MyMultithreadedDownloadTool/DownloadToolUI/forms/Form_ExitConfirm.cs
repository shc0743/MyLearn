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
    public partial class Form_ExitConfirm : Form
    {
        public Form_ExitConfirm()
        {
            InitializeComponent();

            textBox.Text = "There are " + MyApp.PendingDownloadsCount +
                " pending download tasks. " +
                "Are you sure you want to exit the application?";
        }

        private void button_yes_Click(object sender, EventArgs e)
        {
            DialogResult = DialogResult.Yes;
            Close();
        }

        private void button_no_Click(object sender, EventArgs e)
        {
            DialogResult = DialogResult.Cancel;
            Close();
        }
    }
}
