namespace DownloadToolUI
{
    partial class Form_NewDownloadTask
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(Form_NewDownloadTask));
            this.panel1 = new System.Windows.Forms.Panel();
            this.panel3 = new System.Windows.Forms.Panel();
            this.button_schedule = new System.Windows.Forms.Button();
            this.button_cancel = new System.Windows.Forms.Button();
            this.button_download = new System.Windows.Forms.Button();
            this.panel2 = new System.Windows.Forms.Panel();
            this.textBox1 = new System.Windows.Forms.TextBox();
            this.label1 = new System.Windows.Forms.Label();
            this.tabControl = new System.Windows.Forms.TabControl();
            this.tabPage_dl = new System.Windows.Forms.TabPage();
            this.tabPage_schedule = new System.Windows.Forms.TabPage();
            this.tabPage_advanced = new System.Windows.Forms.TabPage();
            this.tabControl_adv = new System.Windows.Forms.TabControl();
            this.tabPage_adv1 = new System.Windows.Forms.TabPage();
            this.tabPage_adv3 = new System.Windows.Forms.TabPage();
            this.panel1.SuspendLayout();
            this.panel3.SuspendLayout();
            this.panel2.SuspendLayout();
            this.tabControl.SuspendLayout();
            this.tabPage_dl.SuspendLayout();
            this.tabPage_advanced.SuspendLayout();
            this.tabControl_adv.SuspendLayout();
            this.SuspendLayout();
            // 
            // panel1
            // 
            this.panel1.AllowDrop = true;
            this.panel1.Controls.Add(this.panel3);
            this.panel1.Controls.Add(this.panel2);
            resources.ApplyResources(this.panel1, "panel1");
            this.panel1.Name = "panel1";
            // 
            // panel3
            // 
            this.panel3.Controls.Add(this.button_schedule);
            this.panel3.Controls.Add(this.button_cancel);
            this.panel3.Controls.Add(this.button_download);
            resources.ApplyResources(this.panel3, "panel3");
            this.panel3.Name = "panel3";
            // 
            // button_schedule
            // 
            this.button_schedule.Cursor = System.Windows.Forms.Cursors.Hand;
            resources.ApplyResources(this.button_schedule, "button_schedule");
            this.button_schedule.Name = "button_schedule";
            this.button_schedule.UseVisualStyleBackColor = true;
            this.button_schedule.Click += new System.EventHandler(this.button_schedule_Click);
            // 
            // button_cancel
            // 
            this.button_cancel.Cursor = System.Windows.Forms.Cursors.Hand;
            resources.ApplyResources(this.button_cancel, "button_cancel");
            this.button_cancel.Name = "button_cancel";
            this.button_cancel.UseVisualStyleBackColor = true;
            this.button_cancel.Click += new System.EventHandler(this.button_cancel_Click);
            // 
            // button_download
            // 
            this.button_download.Cursor = System.Windows.Forms.Cursors.Hand;
            resources.ApplyResources(this.button_download, "button_download");
            this.button_download.Name = "button_download";
            this.button_download.UseVisualStyleBackColor = true;
            // 
            // panel2
            // 
            this.panel2.AllowDrop = true;
            this.panel2.Controls.Add(this.textBox1);
            this.panel2.Controls.Add(this.label1);
            resources.ApplyResources(this.panel2, "panel2");
            this.panel2.Name = "panel2";
            // 
            // textBox1
            // 
            this.textBox1.AllowDrop = true;
            resources.ApplyResources(this.textBox1, "textBox1");
            this.textBox1.Name = "textBox1";
            // 
            // label1
            // 
            resources.ApplyResources(this.label1, "label1");
            this.label1.Name = "label1";
            // 
            // tabControl
            // 
            this.tabControl.AllowDrop = true;
            this.tabControl.Controls.Add(this.tabPage_dl);
            this.tabControl.Controls.Add(this.tabPage_schedule);
            this.tabControl.Controls.Add(this.tabPage_advanced);
            resources.ApplyResources(this.tabControl, "tabControl");
            this.tabControl.Name = "tabControl";
            this.tabControl.SelectedIndex = 0;
            // 
            // tabPage_dl
            // 
            this.tabPage_dl.AllowDrop = true;
            this.tabPage_dl.Controls.Add(this.panel1);
            resources.ApplyResources(this.tabPage_dl, "tabPage_dl");
            this.tabPage_dl.Name = "tabPage_dl";
            this.tabPage_dl.UseVisualStyleBackColor = true;
            // 
            // tabPage_schedule
            // 
            resources.ApplyResources(this.tabPage_schedule, "tabPage_schedule");
            this.tabPage_schedule.Name = "tabPage_schedule";
            this.tabPage_schedule.UseVisualStyleBackColor = true;
            // 
            // tabPage_advanced
            // 
            this.tabPage_advanced.Controls.Add(this.tabControl_adv);
            resources.ApplyResources(this.tabPage_advanced, "tabPage_advanced");
            this.tabPage_advanced.Name = "tabPage_advanced";
            this.tabPage_advanced.UseVisualStyleBackColor = true;
            // 
            // tabControl_adv
            // 
            this.tabControl_adv.Controls.Add(this.tabPage_adv1);
            this.tabControl_adv.Controls.Add(this.tabPage_adv3);
            resources.ApplyResources(this.tabControl_adv, "tabControl_adv");
            this.tabControl_adv.Name = "tabControl_adv";
            this.tabControl_adv.SelectedIndex = 0;
            // 
            // tabPage_adv1
            // 
            resources.ApplyResources(this.tabPage_adv1, "tabPage_adv1");
            this.tabPage_adv1.Name = "tabPage_adv1";
            this.tabPage_adv1.UseVisualStyleBackColor = true;
            // 
            // tabPage_adv3
            // 
            resources.ApplyResources(this.tabPage_adv3, "tabPage_adv3");
            this.tabPage_adv3.Name = "tabPage_adv3";
            this.tabPage_adv3.UseVisualStyleBackColor = true;
            // 
            // Form_NewDownloadTask
            // 
            this.AcceptButton = this.button_download;
            this.AllowDrop = true;
            resources.ApplyResources(this, "$this");
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.CancelButton = this.button_cancel;
            this.Controls.Add(this.tabControl);
            this.Name = "Form_NewDownloadTask";
            this.panel1.ResumeLayout(false);
            this.panel3.ResumeLayout(false);
            this.panel2.ResumeLayout(false);
            this.panel2.PerformLayout();
            this.tabControl.ResumeLayout(false);
            this.tabPage_dl.ResumeLayout(false);
            this.tabPage_advanced.ResumeLayout(false);
            this.tabControl_adv.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private Panel panel1;
        private Panel panel2;
        private TextBox textBox1;
        private Label label1;
        private TabControl tabControl;
        private TabPage tabPage_dl;
        private TabPage tabPage_advanced;
        private TabPage tabPage_schedule;
        private Panel panel3;
        private Button button_cancel;
        private Button button_download;
        private Button button_schedule;
        private TabControl tabControl_adv;
        private TabPage tabPage_adv1;
        private TabPage tabPage_adv3;
    }
}