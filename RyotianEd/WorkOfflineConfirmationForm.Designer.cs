namespace RyotianEd
{
    partial class WorkOfflineConfirmationForm
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
            this.textBox1 = new System.Windows.Forms.TextBox();
            this.workOfflineButton1 = new System.Windows.Forms.Button();
            this.cancelButton1 = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // textBox1
            // 
            this.textBox1.BackColor = System.Drawing.SystemColors.Control;
            this.textBox1.Enabled = false;
            this.textBox1.Location = new System.Drawing.Point(16, 33);
            this.textBox1.Name = "textBox1";
            this.textBox1.Size = new System.Drawing.Size(743, 20);
            this.textBox1.TabIndex = 0;
            this.textBox1.Text = "You are not connected to the database. If you choose to work offline then you wil" +
                "l not be able to save. Only exporting will be allowed.";
            // 
            // workOfflineButton1
            // 
            this.workOfflineButton1.Location = new System.Drawing.Point(171, 112);
            this.workOfflineButton1.Name = "workOfflineButton1";
            this.workOfflineButton1.Size = new System.Drawing.Size(172, 36);
            this.workOfflineButton1.TabIndex = 1;
            this.workOfflineButton1.Text = "Work Offline";
            this.workOfflineButton1.UseVisualStyleBackColor = true;
            this.workOfflineButton1.Click += new System.EventHandler(this.workOfflineButton1_Click);
            // 
            // cancelButton1
            // 
            this.cancelButton1.Location = new System.Drawing.Point(467, 109);
            this.cancelButton1.Name = "cancelButton1";
            this.cancelButton1.Size = new System.Drawing.Size(200, 39);
            this.cancelButton1.TabIndex = 2;
            this.cancelButton1.Text = "Cancel";
            this.cancelButton1.UseVisualStyleBackColor = true;
            this.cancelButton1.Click += new System.EventHandler(this.cancelButton1_Click);
            // 
            // WorkOfflineConfirmationForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(802, 171);
            this.Controls.Add(this.cancelButton1);
            this.Controls.Add(this.workOfflineButton1);
            this.Controls.Add(this.textBox1);
            this.Name = "WorkOfflineConfirmationForm";
            this.Text = "Work Offline Confirmation Form";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.TextBox textBox1;
        private System.Windows.Forms.Button workOfflineButton1;
        private System.Windows.Forms.Button cancelButton1;

    }
}