namespace RyotianEd
{
    partial class NewPackageForm
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
            this.packageNameTextBox1 = new System.Windows.Forms.TextBox();
            this.label1 = new System.Windows.Forms.Label();
            this.applyButton1 = new System.Windows.Forms.Button();
            this.cancelButton1 = new System.Windows.Forms.Button();
            this.typeComboBox1 = new System.Windows.Forms.ComboBox();
            this.label2 = new System.Windows.Forms.Label();
            this.SuspendLayout();
            // 
            // packageNameTextBox1
            // 
            this.packageNameTextBox1.Location = new System.Drawing.Point(103, 12);
            this.packageNameTextBox1.Name = "packageNameTextBox1";
            this.packageNameTextBox1.Size = new System.Drawing.Size(433, 20);
            this.packageNameTextBox1.TabIndex = 0;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(4, 15);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(81, 13);
            this.label1.TabIndex = 1;
            this.label1.Text = "Package Name";
            // 
            // applyButton1
            // 
            this.applyButton1.Location = new System.Drawing.Point(153, 92);
            this.applyButton1.Name = "applyButton1";
            this.applyButton1.Size = new System.Drawing.Size(134, 32);
            this.applyButton1.TabIndex = 2;
            this.applyButton1.Text = "Apply";
            this.applyButton1.UseVisualStyleBackColor = true;
            this.applyButton1.Click += new System.EventHandler(this.applyButton1_Click);
            // 
            // cancelButton1
            // 
            this.cancelButton1.Location = new System.Drawing.Point(293, 92);
            this.cancelButton1.Name = "cancelButton1";
            this.cancelButton1.Size = new System.Drawing.Size(134, 32);
            this.cancelButton1.TabIndex = 3;
            this.cancelButton1.Text = "Cancel";
            this.cancelButton1.UseVisualStyleBackColor = true;
            this.cancelButton1.Click += new System.EventHandler(this.cancelButton1_Click);
            // 
            // typeComboBox1
            // 
            this.typeComboBox1.FormattingEnabled = true;
            this.typeComboBox1.Items.AddRange(new object[] {
            "GODZ",
            "RESOURCE"});
            this.typeComboBox1.Location = new System.Drawing.Point(103, 38);
            this.typeComboBox1.Name = "typeComboBox1";
            this.typeComboBox1.Size = new System.Drawing.Size(184, 21);
            this.typeComboBox1.TabIndex = 4;
            this.typeComboBox1.SelectedIndexChanged += new System.EventHandler(this.typeComboBox1_SelectedIndexChanged);
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(4, 41);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(77, 13);
            this.label2.TabIndex = 5;
            this.label2.Text = "Package Type";
            // 
            // NewPackageForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(548, 192);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.typeComboBox1);
            this.Controls.Add(this.cancelButton1);
            this.Controls.Add(this.applyButton1);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.packageNameTextBox1);
            this.Name = "NewPackageForm";
            this.Text = "New Package";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.TextBox packageNameTextBox1;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Button applyButton1;
        private System.Windows.Forms.Button cancelButton1;
        private System.Windows.Forms.ComboBox typeComboBox1;
        private System.Windows.Forms.Label label2;
    }
}