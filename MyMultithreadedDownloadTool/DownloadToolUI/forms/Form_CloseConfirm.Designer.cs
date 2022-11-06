namespace DownloadToolUI
{
    partial class Form_CloseConfirm
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(Form_CloseConfirm));
            this.button_yes = new System.Windows.Forms.Button();
            this.button_no = new System.Windows.Forms.Button();
            this.label1 = new System.Windows.Forms.Label();
            this.panel_top = new System.Windows.Forms.Panel();
            this.textBox = new System.Windows.Forms.TextBox();
            this.panel_bottom = new System.Windows.Forms.Panel();
            this.checkBox_nsa = new System.Windows.Forms.CheckBox();
            this.button_exit = new System.Windows.Forms.Button();
            this.panel_top.SuspendLayout();
            this.panel_bottom.SuspendLayout();
            this.SuspendLayout();
            // 
            // button_yes
            // 
            this.button_yes.Cursor = System.Windows.Forms.Cursors.Hand;
            resources.ApplyResources(this.button_yes, "button_yes");
            this.button_yes.Name = "button_yes";
            this.button_yes.UseVisualStyleBackColor = true;
            this.button_yes.Click += new System.EventHandler(this.button_yes_Click);
            // 
            // button_no
            // 
            this.button_no.Cursor = System.Windows.Forms.Cursors.Hand;
            resources.ApplyResources(this.button_no, "button_no");
            this.button_no.Name = "button_no";
            this.button_no.UseVisualStyleBackColor = true;
            this.button_no.Click += new System.EventHandler(this.button_no_Click);
            // 
            // label1
            // 
            resources.ApplyResources(this.label1, "label1");
            this.label1.Name = "label1";
            // 
            // panel_top
            // 
            this.panel_top.Controls.Add(this.textBox);
            this.panel_top.Controls.Add(this.label1);
            resources.ApplyResources(this.panel_top, "panel_top");
            this.panel_top.Name = "panel_top";
            // 
            // textBox
            // 
            resources.ApplyResources(this.textBox, "textBox");
            this.textBox.Name = "textBox";
            this.textBox.TabStop = false;
            // 
            // panel_bottom
            // 
            this.panel_bottom.Controls.Add(this.checkBox_nsa);
            this.panel_bottom.Controls.Add(this.button_exit);
            this.panel_bottom.Controls.Add(this.button_yes);
            this.panel_bottom.Controls.Add(this.button_no);
            resources.ApplyResources(this.panel_bottom, "panel_bottom");
            this.panel_bottom.Name = "panel_bottom";
            // 
            // checkBox_nsa
            // 
            resources.ApplyResources(this.checkBox_nsa, "checkBox_nsa");
            this.checkBox_nsa.Cursor = System.Windows.Forms.Cursors.Hand;
            this.checkBox_nsa.Name = "checkBox_nsa";
            this.checkBox_nsa.UseVisualStyleBackColor = true;
            // 
            // button_exit
            // 
            this.button_exit.Cursor = System.Windows.Forms.Cursors.Hand;
            resources.ApplyResources(this.button_exit, "button_exit");
            this.button_exit.Name = "button_exit";
            this.button_exit.UseVisualStyleBackColor = true;
            this.button_exit.Click += new System.EventHandler(this.button_exit_Click);
            // 
            // Form_CloseConfirm
            // 
            this.AcceptButton = this.button_yes;
            resources.ApplyResources(this, "$this");
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.CancelButton = this.button_no;
            this.Controls.Add(this.panel_top);
            this.Controls.Add(this.panel_bottom);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
            this.MaximizeBox = false;
            this.Name = "Form_CloseConfirm";
            this.panel_top.ResumeLayout(false);
            this.panel_top.PerformLayout();
            this.panel_bottom.ResumeLayout(false);
            this.panel_bottom.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private Button button_yes;
        private Button button_no;
        private Label label1;
        private Panel panel_top;
        private TextBox textBox;
        private Panel panel_bottom;
        private Button button_exit;
        private CheckBox checkBox_nsa;
    }
}