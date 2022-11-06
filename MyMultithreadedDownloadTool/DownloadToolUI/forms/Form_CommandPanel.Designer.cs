namespace DownloadToolUI
{
    partial class Form_CommandPanel
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
            this.panel = new System.Windows.Forms.Panel();
            this.panel_content1 = new System.Windows.Forms.Panel();
            this.listBox = new System.Windows.Forms.ListBox();
            this.textBox_command = new System.Windows.Forms.TextBox();
            this.panel.SuspendLayout();
            this.panel_content1.SuspendLayout();
            this.SuspendLayout();
            // 
            // panel
            // 
            this.panel.Controls.Add(this.panel_content1);
            this.panel.Controls.Add(this.textBox_command);
            this.panel.Dock = System.Windows.Forms.DockStyle.Fill;
            this.panel.Location = new System.Drawing.Point(0, 0);
            this.panel.Name = "panel";
            this.panel.Padding = new System.Windows.Forms.Padding(10, 11, 10, 11);
            this.panel.Size = new System.Drawing.Size(304, 166);
            this.panel.TabIndex = 0;
            // 
            // panel_content1
            // 
            this.panel_content1.Controls.Add(this.listBox);
            this.panel_content1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.panel_content1.Location = new System.Drawing.Point(10, 34);
            this.panel_content1.Name = "panel_content1";
            this.panel_content1.Padding = new System.Windows.Forms.Padding(0, 11, 0, 0);
            this.panel_content1.Size = new System.Drawing.Size(284, 121);
            this.panel_content1.TabIndex = 2;
            // 
            // listBox
            // 
            this.listBox.Dock = System.Windows.Forms.DockStyle.Fill;
            this.listBox.FormattingEnabled = true;
            this.listBox.ItemHeight = 15;
            this.listBox.Location = new System.Drawing.Point(0, 11);
            this.listBox.Name = "listBox";
            this.listBox.Size = new System.Drawing.Size(284, 110);
            this.listBox.TabIndex = 0;
            // 
            // textBox_command
            // 
            this.textBox_command.Dock = System.Windows.Forms.DockStyle.Top;
            this.textBox_command.Location = new System.Drawing.Point(10, 11);
            this.textBox_command.Name = "textBox_command";
            this.textBox_command.Size = new System.Drawing.Size(284, 23);
            this.textBox_command.TabIndex = 0;
            this.textBox_command.TextChanged += new System.EventHandler(this.textBox_command_TextChanged);
            this.textBox_command.KeyDown += new System.Windows.Forms.KeyEventHandler(this.textBox_command_KeyDown);
            // 
            // Form_CommandPanel
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 15F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(304, 166);
            this.Controls.Add(this.panel);
            this.Font = new System.Drawing.Font("Consolas", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.SizableToolWindow;
            this.Margin = new System.Windows.Forms.Padding(3, 2, 3, 2);
            this.MinimizeBox = false;
            this.Name = "Form_CommandPanel";
            this.ShowInTaskbar = false;
            this.StartPosition = System.Windows.Forms.FormStartPosition.Manual;
            this.Text = "Command Panel";
            this.TopMost = true;
            this.Deactivate += new System.EventHandler(this.Form_CommandPanel_Deactivate);
            this.panel.ResumeLayout(false);
            this.panel.PerformLayout();
            this.panel_content1.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private Panel panel;
        private TextBox textBox_command;
        private Panel panel_content1;
        private ListBox listBox;
    }
}