namespace RyotianEd
{
    partial class MaterialBrowserForm
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
            this.components = new System.ComponentModel.Container();
            this.propertyGrid1 = new System.Windows.Forms.PropertyGrid();
            this.treeView1 = new System.Windows.Forms.TreeView();
            this.pictureBox1 = new System.Windows.Forms.PictureBox();
            this.panel1 = new System.Windows.Forms.Panel();
            this.updateButton1 = new System.Windows.Forms.Button();
            this.label1 = new System.Windows.Forms.Label();
            this.shaderComboBox1 = new System.Windows.Forms.ComboBox();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.removeParameterButton = new System.Windows.Forms.Button();
            this.addParamButton = new System.Windows.Forms.Button();
            this.pickTextureButton = new System.Windows.Forms.Button();
            this.parameterTextBox = new System.Windows.Forms.TextBox();
            this.parameterTypeComboBox = new System.Windows.Forms.ComboBox();
            this.parameterComboBox1 = new System.Windows.Forms.ComboBox();
            this.textureOpenFileDialog = new System.Windows.Forms.OpenFileDialog();
            this.materialContextMenuStrip = new System.Windows.Forms.ContextMenuStrip(this.components);
            this.importToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.importOpenFileDialog = new System.Windows.Forms.OpenFileDialog();
            this.materialNodeContextMenuStrip1 = new System.Windows.Forms.ContextMenuStrip(this.components);
            this.removeNodeToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).BeginInit();
            this.panel1.SuspendLayout();
            this.groupBox1.SuspendLayout();
            this.materialContextMenuStrip.SuspendLayout();
            this.materialNodeContextMenuStrip1.SuspendLayout();
            this.SuspendLayout();
            // 
            // propertyGrid1
            // 
            this.propertyGrid1.Dock = System.Windows.Forms.DockStyle.Bottom;
            this.propertyGrid1.Location = new System.Drawing.Point(0, 429);
            this.propertyGrid1.Name = "propertyGrid1";
            this.propertyGrid1.Size = new System.Drawing.Size(775, 148);
            this.propertyGrid1.TabIndex = 0;
            this.propertyGrid1.PropertyValueChanged += new System.Windows.Forms.PropertyValueChangedEventHandler(this.propertyGrid1_PropertyValueChanged);
            // 
            // treeView1
            // 
            this.treeView1.Dock = System.Windows.Forms.DockStyle.Left;
            this.treeView1.Location = new System.Drawing.Point(0, 0);
            this.treeView1.Name = "treeView1";
            this.treeView1.Size = new System.Drawing.Size(170, 429);
            this.treeView1.TabIndex = 1;
            this.treeView1.NodeMouseClick += new System.Windows.Forms.TreeNodeMouseClickEventHandler(this.treeView1_NodeMouseClick);
            // 
            // pictureBox1
            // 
            this.pictureBox1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.pictureBox1.Location = new System.Drawing.Point(170, 0);
            this.pictureBox1.Name = "pictureBox1";
            this.pictureBox1.Size = new System.Drawing.Size(605, 429);
            this.pictureBox1.TabIndex = 2;
            this.pictureBox1.TabStop = false;
            // 
            // panel1
            // 
            this.panel1.Controls.Add(this.updateButton1);
            this.panel1.Controls.Add(this.label1);
            this.panel1.Controls.Add(this.shaderComboBox1);
            this.panel1.Controls.Add(this.groupBox1);
            this.panel1.Dock = System.Windows.Forms.DockStyle.Right;
            this.panel1.Location = new System.Drawing.Point(637, 0);
            this.panel1.Name = "panel1";
            this.panel1.Size = new System.Drawing.Size(138, 429);
            this.panel1.TabIndex = 3;
            // 
            // updateButton1
            // 
            this.updateButton1.Location = new System.Drawing.Point(17, 76);
            this.updateButton1.Name = "updateButton1";
            this.updateButton1.Size = new System.Drawing.Size(102, 21);
            this.updateButton1.TabIndex = 6;
            this.updateButton1.Text = "Update";
            this.updateButton1.UseVisualStyleBackColor = true;
            this.updateButton1.Click += new System.EventHandler(this.updateButton1_Click);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(3, 33);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(41, 13);
            this.label1.TabIndex = 5;
            this.label1.Text = "Shader";
            // 
            // shaderComboBox1
            // 
            this.shaderComboBox1.FormattingEnabled = true;
            this.shaderComboBox1.Location = new System.Drawing.Point(5, 49);
            this.shaderComboBox1.Name = "shaderComboBox1";
            this.shaderComboBox1.Size = new System.Drawing.Size(130, 21);
            this.shaderComboBox1.TabIndex = 4;
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.removeParameterButton);
            this.groupBox1.Controls.Add(this.addParamButton);
            this.groupBox1.Controls.Add(this.pickTextureButton);
            this.groupBox1.Controls.Add(this.parameterTextBox);
            this.groupBox1.Controls.Add(this.parameterTypeComboBox);
            this.groupBox1.Controls.Add(this.parameterComboBox1);
            this.groupBox1.Location = new System.Drawing.Point(3, 111);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(133, 231);
            this.groupBox1.TabIndex = 3;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "Parameter";
            // 
            // removeParameterButton
            // 
            this.removeParameterButton.Location = new System.Drawing.Point(20, 207);
            this.removeParameterButton.Name = "removeParameterButton";
            this.removeParameterButton.Size = new System.Drawing.Size(95, 23);
            this.removeParameterButton.TabIndex = 5;
            this.removeParameterButton.Text = "Remove Parameter";
            this.removeParameterButton.UseVisualStyleBackColor = true;
            this.removeParameterButton.Click += new System.EventHandler(this.removeParameterButton_Click);
            // 
            // addParamButton
            // 
            this.addParamButton.Location = new System.Drawing.Point(14, 178);
            this.addParamButton.Name = "addParamButton";
            this.addParamButton.Size = new System.Drawing.Size(104, 23);
            this.addParamButton.TabIndex = 4;
            this.addParamButton.Text = "Add Parameter";
            this.addParamButton.UseVisualStyleBackColor = true;
            this.addParamButton.Click += new System.EventHandler(this.addParamButton_Click);
            // 
            // pickTextureButton
            // 
            this.pickTextureButton.Location = new System.Drawing.Point(14, 149);
            this.pickTextureButton.Name = "pickTextureButton";
            this.pickTextureButton.Size = new System.Drawing.Size(104, 23);
            this.pickTextureButton.TabIndex = 4;
            this.pickTextureButton.Text = "Update";
            this.pickTextureButton.UseVisualStyleBackColor = true;
            this.pickTextureButton.Click += new System.EventHandler(this.pickTextureButton_Click);
            // 
            // parameterTextBox
            // 
            this.parameterTextBox.Location = new System.Drawing.Point(9, 115);
            this.parameterTextBox.Name = "parameterTextBox";
            this.parameterTextBox.Size = new System.Drawing.Size(110, 20);
            this.parameterTextBox.TabIndex = 3;
            // 
            // parameterTypeComboBox
            // 
            this.parameterTypeComboBox.FormattingEnabled = true;
            this.parameterTypeComboBox.Items.AddRange(new object[] {
            "Texture",
            "Float"});
            this.parameterTypeComboBox.Location = new System.Drawing.Point(9, 72);
            this.parameterTypeComboBox.Name = "parameterTypeComboBox";
            this.parameterTypeComboBox.Size = new System.Drawing.Size(111, 21);
            this.parameterTypeComboBox.TabIndex = 2;
            // 
            // parameterComboBox1
            // 
            this.parameterComboBox1.FormattingEnabled = true;
            this.parameterComboBox1.Location = new System.Drawing.Point(7, 31);
            this.parameterComboBox1.Name = "parameterComboBox1";
            this.parameterComboBox1.Size = new System.Drawing.Size(113, 21);
            this.parameterComboBox1.TabIndex = 1;
            this.parameterComboBox1.SelectedIndexChanged += new System.EventHandler(this.parameterComboBox1_SelectedIndexChanged);
            this.parameterComboBox1.SelectedValueChanged += new System.EventHandler(this.parameterComboBox1_SelectedValueChanged);
            this.parameterComboBox1.KeyDown += new System.Windows.Forms.KeyEventHandler(this.parameterComboBox1_KeyDown);
            this.parameterComboBox1.TextUpdate += new System.EventHandler(this.parameterComboBox1_TextUpdate);
            this.parameterComboBox1.TextChanged += new System.EventHandler(this.parameterComboBox1_TextChanged);
            // 
            // textureOpenFileDialog
            // 
            this.textureOpenFileDialog.Filter = "jpg|*.jpg|bmp|*.bmp|tga|*.tga";
            this.textureOpenFileDialog.FileOk += new System.ComponentModel.CancelEventHandler(this.textureOpenFileDialog_FileOk);
            // 
            // materialContextMenuStrip
            // 
            this.materialContextMenuStrip.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.importToolStripMenuItem});
            this.materialContextMenuStrip.Name = "materialContextMenuStrip";
            this.materialContextMenuStrip.Size = new System.Drawing.Size(111, 26);
            // 
            // importToolStripMenuItem
            // 
            this.importToolStripMenuItem.Name = "importToolStripMenuItem";
            this.importToolStripMenuItem.Size = new System.Drawing.Size(110, 22);
            this.importToolStripMenuItem.Text = "Import";
            this.importToolStripMenuItem.Click += new System.EventHandler(this.importToolStripMenuItem_Click_1);
            // 
            // importOpenFileDialog
            // 
            this.importOpenFileDialog.Filter = "jpg|*.jpg|bmp|*.bmp|tga|*.tga";
            // 
            // materialNodeContextMenuStrip1
            // 
            this.materialNodeContextMenuStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.removeNodeToolStripMenuItem});
            this.materialNodeContextMenuStrip1.Name = "materialNodeContextMenuStrip1";
            this.materialNodeContextMenuStrip1.Size = new System.Drawing.Size(153, 48);
            // 
            // removeNodeToolStripMenuItem
            // 
            this.removeNodeToolStripMenuItem.Name = "removeNodeToolStripMenuItem";
            this.removeNodeToolStripMenuItem.Size = new System.Drawing.Size(152, 22);
            this.removeNodeToolStripMenuItem.Text = "Remove";
            this.removeNodeToolStripMenuItem.Click += new System.EventHandler(this.removeNodeToolStripMenuItem_Click);
            // 
            // MaterialBrowserForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(775, 577);
            this.Controls.Add(this.panel1);
            this.Controls.Add(this.pictureBox1);
            this.Controls.Add(this.treeView1);
            this.Controls.Add(this.propertyGrid1);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.None;
            this.Name = "MaterialBrowserForm";
            this.Text = "MaterialBrowserForm";
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).EndInit();
            this.panel1.ResumeLayout(false);
            this.panel1.PerformLayout();
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            this.materialContextMenuStrip.ResumeLayout(false);
            this.materialNodeContextMenuStrip1.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.PropertyGrid propertyGrid1;
        private System.Windows.Forms.TreeView treeView1;
        private System.Windows.Forms.PictureBox pictureBox1;
        private System.Windows.Forms.Panel panel1;
        private System.Windows.Forms.ComboBox parameterComboBox1;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.TextBox parameterTextBox;
        private System.Windows.Forms.ComboBox parameterTypeComboBox;
        private System.Windows.Forms.Button pickTextureButton;
        private System.Windows.Forms.OpenFileDialog textureOpenFileDialog;
        private System.Windows.Forms.Button addParamButton;
        private System.Windows.Forms.ContextMenuStrip materialContextMenuStrip;
        private System.Windows.Forms.ToolStripMenuItem importToolStripMenuItem;
        private System.Windows.Forms.OpenFileDialog importOpenFileDialog;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.ComboBox shaderComboBox1;
        private System.Windows.Forms.Button updateButton1;
        private System.Windows.Forms.Button removeParameterButton;
        private System.Windows.Forms.ContextMenuStrip materialNodeContextMenuStrip1;
        private System.Windows.Forms.ToolStripMenuItem removeNodeToolStripMenuItem;
    }
}