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
    public partial class Form_AppSettings : Form
    {
        private string filtertext = "\u2060Filter settings...";

        private static Form? _openingForm;

        public Form_AppSettings()
        {
            InitializeComponent();
            if (_openingForm != null)
            {
                this.Close();
                return;
            }

            _openingForm = this;

            textBox_filter.Text = filtertext;
            RemoveTabs();
        }

        private void Form_AppSettings_FormClosed(object sender, FormClosedEventArgs e)
        {
            _openingForm = null;
        }

        private void RemoveTabs()
        {
            while (this.tabControl_main.TabPages.Count > 1)
            {
                this.tabControl_main.TabPages.RemoveAt(1);
            }
        }

        private void textBox_filter_Enter(object sender, EventArgs e)
        {
            if (textBox_filter.Text == filtertext)
                textBox_filter.Text = "";
        }

        private void textBox_filter_Leave(object sender, EventArgs e)
        {
            if (textBox_filter.Text == "")
                textBox_filter.Text = filtertext;
        }

        private void treeView_settings_type_AfterSelect(object sender, TreeViewEventArgs e)
        {
            RemoveTabs();
            switch (treeView_settings_type.SelectedNode.Text)
            {
                case "General":
                    tabControl_main.TabPages.Add(this.tabPage_s_general);
                    break;

                default:
                    break;
            }

        }
    }
}
