﻿namespace RyotianEd
{
    partial class ClassDescriptionForm
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
            this.propertyGrid1 = new System.Windows.Forms.PropertyGrid();
            this.applyChangesButton = new System.Windows.Forms.Button();
            this.propertyListBox1 = new System.Windows.Forms.ListBox();
            this.classPickComboBox = new System.Windows.Forms.ComboBox();
            this.splitContainer1 = new System.Windows.Forms.SplitContainer();
            this.splitContainer1.Panel1.SuspendLayout();
            this.splitContainer1.Panel2.SuspendLayout();
            this.splitContainer1.SuspendLayout();
            this.SuspendLayout();
            // 
            // propertyGrid1
            // 
            this.propertyGrid1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.propertyGrid1.Location = new System.Drawing.Point(0, 0);
            this.propertyGrid1.Name = "propertyGrid1";
            this.propertyGrid1.Size = new System.Drawing.Size(586, 578);
            this.propertyGrid1.TabIndex = 1;
            // 
            // applyChangesButton
            // 
            this.applyChangesButton.Location = new System.Drawing.Point(87, 508);
            this.applyChangesButton.Name = "applyChangesButton";
            this.applyChangesButton.Size = new System.Drawing.Size(93, 25);
            this.applyChangesButton.TabIndex = 3;
            this.applyChangesButton.Text = "Apply";
            this.applyChangesButton.UseVisualStyleBackColor = true;
            this.applyChangesButton.Click += new System.EventHandler(this.applyChangesButton_Click);
            // 
            // propertyListBox1
            // 
            this.propertyListBox1.FormattingEnabled = true;
            this.propertyListBox1.Location = new System.Drawing.Point(12, 85);
            this.propertyListBox1.Name = "propertyListBox1";
            this.propertyListBox1.Size = new System.Drawing.Size(271, 394);
            this.propertyListBox1.TabIndex = 2;
            this.propertyListBox1.SelectedIndexChanged += new System.EventHandler(this.propertyListBox1_SelectedIndexChanged);
            // 
            // classPickComboBox
            // 
            this.classPickComboBox.FormattingEnabled = true;
            this.classPickComboBox.Location = new System.Drawing.Point(12, 39);
            this.classPickComboBox.Name = "classPickComboBox";
            this.classPickComboBox.Size = new System.Drawing.Size(251, 21);
            this.classPickComboBox.Sorted = true;
            this.classPickComboBox.TabIndex = 0;
            this.classPickComboBox.SelectedIndexChanged += new System.EventHandler(this.classPickComboBox_SelectedIndexChanged);
            // 
            // splitContainer1
            // 
            this.splitContainer1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.splitContainer1.Location = new System.Drawing.Point(0, 0);
            this.splitContainer1.Name = "splitContainer1";
            // 
            // splitContainer1.Panel1
            // 
            this.splitContainer1.Panel1.Controls.Add(this.applyChangesButton);
            this.splitContainer1.Panel1.Controls.Add(this.propertyListBox1);
            this.splitContainer1.Panel1.Controls.Add(this.classPickComboBox);
            // 
            // splitContainer1.Panel2
            // 
            this.splitContainer1.Panel2.Controls.Add(this.propertyGrid1);
            this.splitContainer1.Size = new System.Drawing.Size(883, 578);
            this.splitContainer1.SplitterDistance = 293;
            this.splitContainer1.TabIndex = 4;
            // 
            // ClassDescriptionForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(883, 578);
            this.Controls.Add(this.splitContainer1);
            this.Name = "ClassDescriptionForm";
            this.Text = "Class Description Form";
            this.splitContainer1.Panel1.ResumeLayout(false);
            this.splitContainer1.Panel2.ResumeLayout(false);
            this.splitContainer1.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.PropertyGrid propertyGrid1;
        private System.Windows.Forms.Button applyChangesButton;
        private System.Windows.Forms.ListBox propertyListBox1;
        private System.Windows.Forms.ComboBox classPickComboBox;
        private System.Windows.Forms.SplitContainer splitContainer1;
    }
}