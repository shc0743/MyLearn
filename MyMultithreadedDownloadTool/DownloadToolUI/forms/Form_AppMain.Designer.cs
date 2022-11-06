namespace DownloadToolUI
{
    partial class Form_AppMain
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(Form_AppMain));
            this.panel_main = new System.Windows.Forms.Panel();
            this.tabControl_main = new System.Windows.Forms.TabControl();
            this.tab_main_tabPage1 = new System.Windows.Forms.TabPage();
            this.panel_tab1_bottom = new System.Windows.Forms.Panel();
            this.listView1 = new System.Windows.Forms.ListView();
            this.panel_tab1_top = new System.Windows.Forms.Panel();
            this.button_newtask = new System.Windows.Forms.Button();
            this.notifyIcon = new System.Windows.Forms.NotifyIcon(this.components);
            this.panel_main.SuspendLayout();
            this.tabControl_main.SuspendLayout();
            this.tab_main_tabPage1.SuspendLayout();
            this.panel_tab1_bottom.SuspendLayout();
            this.panel_tab1_top.SuspendLayout();
            this.SuspendLayout();
            // 
            // panel_main
            // 
            this.panel_main.Controls.Add(this.tabControl_main);
            resources.ApplyResources(this.panel_main, "panel_main");
            this.panel_main.Name = "panel_main";
            // 
            // tabControl_main
            // 
            this.tabControl_main.Controls.Add(this.tab_main_tabPage1);
            resources.ApplyResources(this.tabControl_main, "tabControl_main");
            this.tabControl_main.Name = "tabControl_main";
            this.tabControl_main.SelectedIndex = 0;
            // 
            // tab_main_tabPage1
            // 
            this.tab_main_tabPage1.Controls.Add(this.panel_tab1_bottom);
            this.tab_main_tabPage1.Controls.Add(this.panel_tab1_top);
            resources.ApplyResources(this.tab_main_tabPage1, "tab_main_tabPage1");
            this.tab_main_tabPage1.Name = "tab_main_tabPage1";
            this.tab_main_tabPage1.UseVisualStyleBackColor = true;
            // 
            // panel_tab1_bottom
            // 
            this.panel_tab1_bottom.Controls.Add(this.listView1);
            resources.ApplyResources(this.panel_tab1_bottom, "panel_tab1_bottom");
            this.panel_tab1_bottom.Name = "panel_tab1_bottom";
            // 
            // listView1
            // 
            resources.ApplyResources(this.listView1, "listView1");
            this.listView1.Name = "listView1";
            this.listView1.UseCompatibleStateImageBehavior = false;
            // 
            // panel_tab1_top
            // 
            this.panel_tab1_top.Controls.Add(this.button_newtask);
            resources.ApplyResources(this.panel_tab1_top, "panel_tab1_top");
            this.panel_tab1_top.Name = "panel_tab1_top";
            // 
            // button_newtask
            // 
            this.button_newtask.Cursor = System.Windows.Forms.Cursors.Hand;
            resources.ApplyResources(this.button_newtask, "button_newtask");
            this.button_newtask.Name = "button_newtask";
            this.button_newtask.UseVisualStyleBackColor = true;
            this.button_newtask.Click += new System.EventHandler(this.button1_Click);
            // 
            // notifyIcon
            // 
            resources.ApplyResources(this.notifyIcon, "notifyIcon");
            this.notifyIcon.Click += new System.EventHandler(this.notifyIcon_Click);
            // 
            // Form_AppMain
            // 
            this.AllowDrop = true;
            resources.ApplyResources(this, "$this");
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.Controls.Add(this.panel_main);
            this.Name = "Form_AppMain";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.Form_AppMain_FormClosing);
            this.panel_main.ResumeLayout(false);
            this.tabControl_main.ResumeLayout(false);
            this.tab_main_tabPage1.ResumeLayout(false);
            this.panel_tab1_bottom.ResumeLayout(false);
            this.panel_tab1_top.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private Panel panel_main;
        private TabControl tabControl_main;
        private TabPage tab_main_tabPage1;
        private Panel panel_tab1_bottom;
        private ListView listView1;
        private Panel panel_tab1_top;
        private Button button_newtask;
        private NotifyIcon notifyIcon;
    }
}