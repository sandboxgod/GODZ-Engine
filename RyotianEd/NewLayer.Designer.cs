namespace RyotianEd
{
    partial class NewLayer
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
            this.label1 = new System.Windows.Forms.Label();
            this.sectorNameTextBox1 = new System.Windows.Forms.TextBox();
            this.newSectorButton1 = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label1.Location = new System.Drawing.Point(2, 15);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(106, 20);
            this.label1.TabIndex = 0;
            this.label1.Text = "Sector Name:";
            // 
            // sectorNameTextBox1
            // 
            this.sectorNameTextBox1.Location = new System.Drawing.Point(124, 14);
            this.sectorNameTextBox1.Name = "sectorNameTextBox1";
            this.sectorNameTextBox1.Size = new System.Drawing.Size(274, 20);
            this.sectorNameTextBox1.TabIndex = 1;
            // 
            // newSectorButton1
            // 
            this.newSectorButton1.Location = new System.Drawing.Point(166, 72);
            this.newSectorButton1.Name = "newSectorButton1";
            this.newSectorButton1.Size = new System.Drawing.Size(152, 35);
            this.newSectorButton1.TabIndex = 2;
            this.newSectorButton1.Text = "Submit";
            this.newSectorButton1.UseVisualStyleBackColor = true;
            this.newSectorButton1.Click += new System.EventHandler(this.newSectorButton1_Click);
            // 
            // NewLayer
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(483, 168);
            this.Controls.Add(this.newSectorButton1);
            this.Controls.Add(this.sectorNameTextBox1);
            this.Controls.Add(this.label1);
            this.Name = "NewLayer";
            this.Text = "New Sector";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.TextBox sectorNameTextBox1;
        private System.Windows.Forms.Button newSectorButton1;
    }
}