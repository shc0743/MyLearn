namespace DownloadToolUI
{
    partial class Form_AppSettings
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(Form_AppSettings));
            this.splitContainer = new System.Windows.Forms.SplitContainer();
            this.treeView_settings_type = new System.Windows.Forms.TreeView();
            this.tabControl_main = new System.Windows.Forms.TabControl();
            this.tabPage_index = new System.Windows.Forms.TabPage();
            this.label1 = new System.Windows.Forms.Label();
            this.tabPage_s_general = new System.Windows.Forms.TabPage();
            this.panel = new System.Windows.Forms.Panel();
            this.panel_settingssplit = new System.Windows.Forms.Panel();
            this.textBox_filter = new System.Windows.Forms.TextBox();
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer)).BeginInit();
            this.splitContainer.Panel1.SuspendLayout();
            this.splitContainer.Panel2.SuspendLayout();
            this.splitContainer.SuspendLayout();
            this.tabControl_main.SuspendLayout();
            this.tabPage_index.SuspendLayout();
            this.panel.SuspendLayout();
            this.panel_settingssplit.SuspendLayout();
            this.SuspendLayout();
            // 
            // splitContainer
            // 
            resources.ApplyResources(this.splitContainer, "splitContainer");
            this.splitContainer.Name = "splitContainer";
            // 
            // splitContainer.Panel1
            // 
            resources.ApplyResources(this.splitContainer.Panel1, "splitContainer.Panel1");
            this.splitContainer.Panel1.Controls.Add(this.treeView_settings_type);
            // 
            // splitContainer.Panel2
            // 
            resources.ApplyResources(this.splitContainer.Panel2, "splitContainer.Panel2");
            this.splitContainer.Panel2.Controls.Add(this.tabControl_main);
            // 
            // treeView_settings_type
            // 
            resources.ApplyResources(this.treeView_settings_type, "treeView_settings_type");
            this.treeView_settings_type.Name = "treeView_settings_type";
            this.treeView_settings_type.Nodes.AddRange(new System.Windows.Forms.TreeNode[] {
            ((System.Windows.Forms.TreeNode)(resources.GetObject("treeView_settings_type.Nodes")))});
            this.treeView_settings_type.AfterSelect += new System.Windows.Forms.TreeViewEventHandler(this.treeView_settings_type_AfterSelect);
            // 
            // tabControl_main
            // 
            resources.ApplyResources(this.tabControl_main, "tabControl_main");
            this.tabControl_main.Controls.Add(this.tabPage_index);
            this.tabControl_main.Controls.Add(this.tabPage_s_general);
            this.tabControl_main.Name = "tabControl_main";
            this.tabControl_main.SelectedIndex = 0;
            // 
            // tabPage_index
            // 
            resources.ApplyResources(this.tabPage_index, "tabPage_index");
            this.tabPage_index.Controls.Add(this.label1);
            this.tabPage_index.Name = "tabPage_index";
            this.tabPage_index.UseVisualStyleBackColor = true;
            // 
            // label1
            // 
            resources.ApplyResources(this.label1, "label1");
            this.label1.Name = "label1";
            // 
            // tabPage_s_general
            // 
            resources.ApplyResources(this.tabPage_s_general, "tabPage_s_general");
            this.tabPage_s_general.Name = "tabPage_s_general";
            this.tabPage_s_general.UseVisualStyleBackColor = true;
            // 
            // panel
            // 
            resources.ApplyResources(this.panel, "panel");
            this.panel.Controls.Add(this.panel_settingssplit);
            this.panel.Controls.Add(this.textBox_filter);
            this.panel.Name = "panel";
            // 
            // panel_settingssplit
            // 
            resources.ApplyResources(this.panel_settingssplit, "panel_settingssplit");
            this.panel_settingssplit.Controls.Add(this.splitContainer);
            this.panel_settingssplit.Name = "panel_settingssplit";
            // 
            // textBox_filter
            // 
            resources.ApplyResources(this.textBox_filter, "textBox_filter");
            this.textBox_filter.Name = "textBox_filter";
            this.textBox_filter.Enter += new System.EventHandler(this.textBox_filter_Enter);
            this.textBox_filter.Leave += new System.EventHandler(this.textBox_filter_Leave);
            // 
            // Form_AppSettings
            // 
            resources.ApplyResources(this, "$this");
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.Controls.Add(this.panel);
            this.Name = "Form_AppSettings";
            this.FormClosed += new System.Windows.Forms.FormClosedEventHandler(this.Form_AppSettings_FormClosed);
            this.splitContainer.Panel1.ResumeLayout(false);
            this.splitContainer.Panel2.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer)).EndInit();
            this.splitContainer.ResumeLayout(false);
            this.tabControl_main.ResumeLayout(false);
            this.tabPage_index.ResumeLayout(false);
            this.tabPage_index.PerformLayout();
            this.panel.ResumeLayout(false);
            this.panel.PerformLayout();
            this.panel_settingssplit.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private Panel panel;
        private Panel panel_settingssplit;
        private SplitContainer splitContainer;
        private TreeView treeView_settings_type;
        private TabControl tabControl_main;
        private TabPage tabPage_index;
        private Label label1;
        private TabPage tabPage_s_general;
        private TextBox textBox_filter;
    }
}