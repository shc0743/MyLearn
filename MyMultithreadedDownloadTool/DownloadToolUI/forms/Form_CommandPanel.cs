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
    public partial class Form_CommandPanel : Form
    {
        public Form_CommandPanel()
        {
            InitializeComponent();
        }

        private void Form_CommandPanel_Deactivate(object sender, EventArgs e)
        {
            Close();
        }

        private void textBox_command_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyCode == Keys.Escape)
            {
                Close();
                return;
            }
        }

        private void textBox_command_TextChanged(object sender, EventArgs e)
        {

        }
    }
}
