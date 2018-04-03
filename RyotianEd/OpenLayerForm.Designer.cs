namespace RyotianEd
{
    partial class OpenLayerForm
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
            this.sectorsComboBox1 = new System.Windows.Forms.ComboBox();
            this.label1 = new System.Windows.Forms.Label();
            this.confirmSectorButton1 = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // sectorsComboBox1
            // 
            this.sectorsComboBox1.FormattingEnabled = true;
            this.sectorsComboBox1.Location = new System.Drawing.Point(12, 31);
            this.sectorsComboBox1.Name = "sectorsComboBox1";
            this.sectorsComboBox1.Size = new System.Drawing.Size(429, 21);
            this.sectorsComboBox1.TabIndex = 0;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label1.Location = new System.Drawing.Point(12, 6);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(109, 20);
            this.label1.TabIndex = 1;
            this.label1.Text = "Select Sector:";
            // 
            // confirmSectorButton1
            // 
            this.confirmSectorButton1.Location = new System.Drawing.Point(140, 110);
            this.confirmSectorButton1.Name = "confirmSectorButton1";
            this.confirmSectorButton1.Size = new System.Drawing.Size(158, 44);
            this.confirmSectorButton1.TabIndex = 2;
            this.confirmSectorButton1.Text = "OK";
            this.confirmSectorButton1.UseVisualStyleBackColor = true;
            this.confirmSectorButton1.Click += new System.EventHandler(this.confirmSectorButton1_Click);
            // 
            // OpenLayerForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(453, 169);
            this.Controls.Add(this.confirmSectorButton1);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.sectorsComboBox1);
            this.Name = "OpenLayerForm";
            this.Text = "Select Sector";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.ComboBox sectorsComboBox1;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Button confirmSectorButton1;
    }
}