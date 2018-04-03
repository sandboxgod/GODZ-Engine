namespace RyotianEd
{
    partial class ResolveConflictForm
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
            this.localChangeTextBox = new System.Windows.Forms.TextBox();
            this.serverChangeTextBox = new System.Windows.Forms.TextBox();
            this.label1 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.useLocalButton = new System.Windows.Forms.Button();
            this.useServerButton = new System.Windows.Forms.Button();
            this.conflictLabel = new System.Windows.Forms.Label();
            this.SuspendLayout();
            // 
            // localChangeTextBox
            // 
            this.localChangeTextBox.Location = new System.Drawing.Point(28, 50);
            this.localChangeTextBox.Name = "localChangeTextBox";
            this.localChangeTextBox.ReadOnly = true;
            this.localChangeTextBox.Size = new System.Drawing.Size(179, 20);
            this.localChangeTextBox.TabIndex = 0;
            // 
            // serverChangeTextBox
            // 
            this.serverChangeTextBox.Location = new System.Drawing.Point(236, 50);
            this.serverChangeTextBox.Name = "serverChangeTextBox";
            this.serverChangeTextBox.ReadOnly = true;
            this.serverChangeTextBox.Size = new System.Drawing.Size(175, 20);
            this.serverChangeTextBox.TabIndex = 1;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(25, 25);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(34, 13);
            this.label1.TabIndex = 2;
            this.label1.Text = "Yours";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(240, 24);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(36, 13);
            this.label2.TabIndex = 3;
            this.label2.Text = "Theirs";
            // 
            // useLocalButton
            // 
            this.useLocalButton.Location = new System.Drawing.Point(28, 98);
            this.useLocalButton.Name = "useLocalButton";
            this.useLocalButton.Size = new System.Drawing.Size(108, 34);
            this.useLocalButton.TabIndex = 4;
            this.useLocalButton.Text = "Use Mine";
            this.useLocalButton.UseVisualStyleBackColor = true;
            this.useLocalButton.Click += new System.EventHandler(this.useLocalButton_Click);
            // 
            // useServerButton
            // 
            this.useServerButton.Location = new System.Drawing.Point(236, 98);
            this.useServerButton.Name = "useServerButton";
            this.useServerButton.Size = new System.Drawing.Size(108, 34);
            this.useServerButton.TabIndex = 4;
            this.useServerButton.Text = "Use Theirs";
            this.useServerButton.UseVisualStyleBackColor = true;
            this.useServerButton.Click += new System.EventHandler(this.useServerButton_Click);
            // 
            // conflictLabel
            // 
            this.conflictLabel.AutoSize = true;
            this.conflictLabel.Location = new System.Drawing.Point(25, 159);
            this.conflictLabel.Name = "conflictLabel";
            this.conflictLabel.Size = new System.Drawing.Size(67, 13);
            this.conflictLabel.TabIndex = 5;
            this.conflictLabel.Text = "Conflicts 1/3";
            // 
            // ResolveConflictForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(499, 206);
            this.Controls.Add(this.conflictLabel);
            this.Controls.Add(this.useServerButton);
            this.Controls.Add(this.useLocalButton);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.serverChangeTextBox);
            this.Controls.Add(this.localChangeTextBox);
            this.Name = "ResolveConflictForm";
            this.Text = "ResolveConflictForm";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.TextBox localChangeTextBox;
        private System.Windows.Forms.TextBox serverChangeTextBox;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Button useLocalButton;
        private System.Windows.Forms.Button useServerButton;
        private System.Windows.Forms.Label conflictLabel;

    }
}