namespace RyotianEd
{
    partial class EntityEditorForm
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(EntityEditorForm));
            this.tabControl1 = new System.Windows.Forms.TabControl();
            this.animationsTabPage1 = new System.Windows.Forms.TabPage();
            this.animationsTreeView = new System.Windows.Forms.TreeView();
            this.pictureBox1 = new System.Windows.Forms.PictureBox();
            this.toolStrip1 = new System.Windows.Forms.ToolStrip();
            this.importGSAToolStripButton1 = new System.Windows.Forms.ToolStripButton();
            this.importAnimToolStripButton1 = new System.Windows.Forms.ToolStripButton();
            this.gsaOpenFileDialog1 = new System.Windows.Forms.OpenFileDialog();
            this.panel1 = new System.Windows.Forms.Panel();
            this.showAllMaterialsButton = new System.Windows.Forms.Button();
            this.categoryButton = new System.Windows.Forms.Button();
            this.categoryComboBox = new System.Windows.Forms.ComboBox();
            this.label3 = new System.Windows.Forms.Label();
            this.selectMaterialButton = new System.Windows.Forms.Button();
            this.materialsComboBox = new System.Windows.Forms.ComboBox();
            this.label2 = new System.Windows.Forms.Label();
            this.label1 = new System.Windows.Forms.Label();
            this.collisionTypeComboBox1 = new System.Windows.Forms.ComboBox();
            this.entityContextMenuStrip1 = new System.Windows.Forms.ContextMenuStrip(this.components);
            this.removeNodeToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.bottom_panel = new System.Windows.Forms.Panel();
            this.trackBar1 = new System.Windows.Forms.TrackBar();
            this.panel_child_frame_group = new System.Windows.Forms.Panel();
            this.stopbutton = new System.Windows.Forms.Button();
            this.frameLabel = new System.Windows.Forms.Label();
            this.propertyGrid1 = new System.Windows.Forms.PropertyGrid();
            this.tabControl1.SuspendLayout();
            this.animationsTabPage1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).BeginInit();
            this.toolStrip1.SuspendLayout();
            this.panel1.SuspendLayout();
            this.entityContextMenuStrip1.SuspendLayout();
            this.bottom_panel.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.trackBar1)).BeginInit();
            this.panel_child_frame_group.SuspendLayout();
            this.SuspendLayout();
            // 
            // tabControl1
            // 
            this.tabControl1.Controls.Add(this.animationsTabPage1);
            this.tabControl1.Dock = System.Windows.Forms.DockStyle.Left;
            this.tabControl1.Location = new System.Drawing.Point(0, 0);
            this.tabControl1.Name = "tabControl1";
            this.tabControl1.SelectedIndex = 0;
            this.tabControl1.Size = new System.Drawing.Size(204, 451);
            this.tabControl1.TabIndex = 0;
            // 
            // animationsTabPage1
            // 
            this.animationsTabPage1.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.animationsTabPage1.Controls.Add(this.animationsTreeView);
            this.animationsTabPage1.Location = new System.Drawing.Point(4, 22);
            this.animationsTabPage1.Name = "animationsTabPage1";
            this.animationsTabPage1.Padding = new System.Windows.Forms.Padding(3);
            this.animationsTabPage1.Size = new System.Drawing.Size(196, 425);
            this.animationsTabPage1.TabIndex = 0;
            this.animationsTabPage1.Text = "Objects";
            this.animationsTabPage1.UseVisualStyleBackColor = true;
            // 
            // animationsTreeView
            // 
            this.animationsTreeView.Dock = System.Windows.Forms.DockStyle.Fill;
            this.animationsTreeView.Location = new System.Drawing.Point(3, 3);
            this.animationsTreeView.Name = "animationsTreeView";
            this.animationsTreeView.Size = new System.Drawing.Size(186, 415);
            this.animationsTreeView.TabIndex = 0;
            this.animationsTreeView.NodeMouseClick += new System.Windows.Forms.TreeNodeMouseClickEventHandler(this.animationsTreeView_NodeMouseClick);
            // 
            // pictureBox1
            // 
            this.pictureBox1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.pictureBox1.Location = new System.Drawing.Point(204, 25);
            this.pictureBox1.Name = "pictureBox1";
            this.pictureBox1.Size = new System.Drawing.Size(747, 426);
            this.pictureBox1.TabIndex = 1;
            this.pictureBox1.TabStop = false;
            // 
            // toolStrip1
            // 
            this.toolStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.importGSAToolStripButton1,
            this.importAnimToolStripButton1});
            this.toolStrip1.Location = new System.Drawing.Point(204, 0);
            this.toolStrip1.Name = "toolStrip1";
            this.toolStrip1.Size = new System.Drawing.Size(747, 25);
            this.toolStrip1.TabIndex = 3;
            this.toolStrip1.Text = "toolStrip1";
            // 
            // importGSAToolStripButton1
            // 
            this.importGSAToolStripButton1.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Text;
            this.importGSAToolStripButton1.Image = ((System.Drawing.Image)(resources.GetObject("importGSAToolStripButton1.Image")));
            this.importGSAToolStripButton1.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.importGSAToolStripButton1.Name = "importGSAToolStripButton1";
            this.importGSAToolStripButton1.Size = new System.Drawing.Size(84, 22);
            this.importGSAToolStripButton1.Text = "Import Model";
            this.importGSAToolStripButton1.Click += new System.EventHandler(this.importGSAToolStripButton1_Click);
            // 
            // importAnimToolStripButton1
            // 
            this.importAnimToolStripButton1.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Text;
            this.importAnimToolStripButton1.Image = ((System.Drawing.Image)(resources.GetObject("importAnimToolStripButton1.Image")));
            this.importAnimToolStripButton1.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.importAnimToolStripButton1.Name = "importAnimToolStripButton1";
            this.importAnimToolStripButton1.Size = new System.Drawing.Size(79, 22);
            this.importAnimToolStripButton1.Text = "Import Anim";
            this.importAnimToolStripButton1.Click += new System.EventHandler(this.importAnimToolStripButton1_Click);
            // 
            // gsaOpenFileDialog1
            // 
            this.gsaOpenFileDialog1.DefaultExt = "fbx";
            this.gsaOpenFileDialog1.FileName = "openFileDialog1";
            this.gsaOpenFileDialog1.Filter = "FBX|*.fbx|Model files|*.gsa";
            this.gsaOpenFileDialog1.FileOk += new System.ComponentModel.CancelEventHandler(this.gsaOpenFileDialog1_FileOk);
            // 
            // panel1
            // 
            this.panel1.Controls.Add(this.showAllMaterialsButton);
            this.panel1.Controls.Add(this.categoryButton);
            this.panel1.Controls.Add(this.categoryComboBox);
            this.panel1.Controls.Add(this.label3);
            this.panel1.Controls.Add(this.selectMaterialButton);
            this.panel1.Controls.Add(this.materialsComboBox);
            this.panel1.Controls.Add(this.label2);
            this.panel1.Controls.Add(this.label1);
            this.panel1.Controls.Add(this.collisionTypeComboBox1);
            this.panel1.Dock = System.Windows.Forms.DockStyle.Right;
            this.panel1.Location = new System.Drawing.Point(785, 25);
            this.panel1.Name = "panel1";
            this.panel1.Size = new System.Drawing.Size(166, 426);
            this.panel1.TabIndex = 4;
            // 
            // showAllMaterialsButton
            // 
            this.showAllMaterialsButton.Location = new System.Drawing.Point(4, 164);
            this.showAllMaterialsButton.Name = "showAllMaterialsButton";
            this.showAllMaterialsButton.Size = new System.Drawing.Size(155, 27);
            this.showAllMaterialsButton.TabIndex = 8;
            this.showAllMaterialsButton.Text = "Show All Materials";
            this.showAllMaterialsButton.UseVisualStyleBackColor = true;
            this.showAllMaterialsButton.Click += new System.EventHandler(this.showAllMaterialsButton_Click);
            // 
            // categoryButton
            // 
            this.categoryButton.Location = new System.Drawing.Point(3, 291);
            this.categoryButton.Name = "categoryButton";
            this.categoryButton.Size = new System.Drawing.Size(151, 22);
            this.categoryButton.TabIndex = 7;
            this.categoryButton.Text = "Update";
            this.categoryButton.UseVisualStyleBackColor = true;
            this.categoryButton.Click += new System.EventHandler(this.categoryButton_Click);
            // 
            // categoryComboBox
            // 
            this.categoryComboBox.FormattingEnabled = true;
            this.categoryComboBox.Location = new System.Drawing.Point(3, 252);
            this.categoryComboBox.MaxDropDownItems = 99;
            this.categoryComboBox.Name = "categoryComboBox";
            this.categoryComboBox.Size = new System.Drawing.Size(152, 21);
            this.categoryComboBox.TabIndex = 6;
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(3, 226);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(52, 13);
            this.label3.TabIndex = 5;
            this.label3.Text = "Category:";
            // 
            // selectMaterialButton
            // 
            this.selectMaterialButton.Location = new System.Drawing.Point(3, 131);
            this.selectMaterialButton.Name = "selectMaterialButton";
            this.selectMaterialButton.Size = new System.Drawing.Size(155, 27);
            this.selectMaterialButton.TabIndex = 4;
            this.selectMaterialButton.Text = "Select Material";
            this.selectMaterialButton.UseVisualStyleBackColor = true;
            this.selectMaterialButton.Click += new System.EventHandler(this.selectMaterialButton_Click);
            // 
            // materialsComboBox
            // 
            this.materialsComboBox.FormattingEnabled = true;
            this.materialsComboBox.Location = new System.Drawing.Point(4, 87);
            this.materialsComboBox.Name = "materialsComboBox";
            this.materialsComboBox.Size = new System.Drawing.Size(158, 21);
            this.materialsComboBox.TabIndex = 3;
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(3, 62);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(49, 13);
            this.label2.TabIndex = 2;
            this.label2.Text = "Materials";
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(3, 4);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(72, 13);
            this.label1.TabIndex = 1;
            this.label1.Text = "Collision Type";
            // 
            // collisionTypeComboBox1
            // 
            this.collisionTypeComboBox1.FormattingEnabled = true;
            this.collisionTypeComboBox1.Items.AddRange(new object[] {
            "Box",
            "AABB Tree"});
            this.collisionTypeComboBox1.Location = new System.Drawing.Point(3, 26);
            this.collisionTypeComboBox1.Name = "collisionTypeComboBox1";
            this.collisionTypeComboBox1.Size = new System.Drawing.Size(160, 21);
            this.collisionTypeComboBox1.TabIndex = 0;
            this.collisionTypeComboBox1.SelectedIndexChanged += new System.EventHandler(this.collisionTypeComboBox1_SelectedIndexChanged);
            this.collisionTypeComboBox1.SelectionChangeCommitted += new System.EventHandler(this.collisionTypeComboBox1_SelectionChangeCommitted);
            // 
            // entityContextMenuStrip1
            // 
            this.entityContextMenuStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.removeNodeToolStripMenuItem});
            this.entityContextMenuStrip1.Name = "entityContextMenuStrip1";
            this.entityContextMenuStrip1.Size = new System.Drawing.Size(118, 26);
            // 
            // removeNodeToolStripMenuItem
            // 
            this.removeNodeToolStripMenuItem.Name = "removeNodeToolStripMenuItem";
            this.removeNodeToolStripMenuItem.Size = new System.Drawing.Size(117, 22);
            this.removeNodeToolStripMenuItem.Text = "Remove";
            this.removeNodeToolStripMenuItem.Click += new System.EventHandler(this.removeNodeToolStripMenuItem_Click);
            // 
            // bottom_panel
            // 
            this.bottom_panel.AutoSize = true;
            this.bottom_panel.Controls.Add(this.trackBar1);
            this.bottom_panel.Controls.Add(this.panel_child_frame_group);
            this.bottom_panel.Controls.Add(this.propertyGrid1);
            this.bottom_panel.Dock = System.Windows.Forms.DockStyle.Bottom;
            this.bottom_panel.Location = new System.Drawing.Point(0, 451);
            this.bottom_panel.Name = "bottom_panel";
            this.bottom_panel.Size = new System.Drawing.Size(951, 244);
            this.bottom_panel.TabIndex = 6;
            // 
            // trackBar1
            // 
            this.trackBar1.Dock = System.Windows.Forms.DockStyle.Top;
            this.trackBar1.Location = new System.Drawing.Point(0, 32);
            this.trackBar1.Maximum = 100;
            this.trackBar1.Name = "trackBar1";
            this.trackBar1.Size = new System.Drawing.Size(951, 45);
            this.trackBar1.TabIndex = 9;
            this.trackBar1.Scroll += new System.EventHandler(this.trackBar1_Scroll);
            // 
            // panel_child_frame_group
            // 
            this.panel_child_frame_group.Controls.Add(this.stopbutton);
            this.panel_child_frame_group.Controls.Add(this.frameLabel);
            this.panel_child_frame_group.Dock = System.Windows.Forms.DockStyle.Top;
            this.panel_child_frame_group.Location = new System.Drawing.Point(0, 0);
            this.panel_child_frame_group.Name = "panel_child_frame_group";
            this.panel_child_frame_group.Size = new System.Drawing.Size(951, 32);
            this.panel_child_frame_group.TabIndex = 10;
            // 
            // stopbutton
            // 
            this.stopbutton.Location = new System.Drawing.Point(193, 0);
            this.stopbutton.Name = "stopbutton";
            this.stopbutton.Size = new System.Drawing.Size(63, 23);
            this.stopbutton.TabIndex = 8;
            this.stopbutton.Text = "Stop";
            this.stopbutton.UseVisualStyleBackColor = true;
            this.stopbutton.Click += new System.EventHandler(this.stopbutton_Click);
            // 
            // frameLabel
            // 
            this.frameLabel.AutoSize = true;
            this.frameLabel.Location = new System.Drawing.Point(-2, 0);
            this.frameLabel.Name = "frameLabel";
            this.frameLabel.Size = new System.Drawing.Size(42, 13);
            this.frameLabel.TabIndex = 7;
            this.frameLabel.Text = "Frame: ";
            // 
            // propertyGrid1
            // 
            this.propertyGrid1.Dock = System.Windows.Forms.DockStyle.Bottom;
            this.propertyGrid1.Location = new System.Drawing.Point(0, 77);
            this.propertyGrid1.Name = "propertyGrid1";
            this.propertyGrid1.Size = new System.Drawing.Size(951, 167);
            this.propertyGrid1.TabIndex = 3;
            // 
            // EntityEditorForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(951, 695);
            this.Controls.Add(this.panel1);
            this.Controls.Add(this.pictureBox1);
            this.Controls.Add(this.toolStrip1);
            this.Controls.Add(this.tabControl1);
            this.Controls.Add(this.bottom_panel);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.None;
            this.Name = "EntityEditorForm";
            this.Text = "EntityEditorForm";
            this.tabControl1.ResumeLayout(false);
            this.animationsTabPage1.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).EndInit();
            this.toolStrip1.ResumeLayout(false);
            this.toolStrip1.PerformLayout();
            this.panel1.ResumeLayout(false);
            this.panel1.PerformLayout();
            this.entityContextMenuStrip1.ResumeLayout(false);
            this.bottom_panel.ResumeLayout(false);
            this.bottom_panel.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.trackBar1)).EndInit();
            this.panel_child_frame_group.ResumeLayout(false);
            this.panel_child_frame_group.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.TabControl tabControl1;
        private System.Windows.Forms.TabPage animationsTabPage1;
        private System.Windows.Forms.PictureBox pictureBox1;
        private System.Windows.Forms.ToolStrip toolStrip1;
        private System.Windows.Forms.ToolStripButton importGSAToolStripButton1;
        private System.Windows.Forms.OpenFileDialog gsaOpenFileDialog1;
        private System.Windows.Forms.TreeView animationsTreeView;
        private System.Windows.Forms.ToolStripButton importAnimToolStripButton1;
        private System.Windows.Forms.Panel panel1;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.ComboBox collisionTypeComboBox1;
        private System.Windows.Forms.ComboBox materialsComboBox;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Button selectMaterialButton;
        private System.Windows.Forms.ComboBox categoryComboBox;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Button categoryButton;
        private System.Windows.Forms.Button showAllMaterialsButton;
        private System.Windows.Forms.ContextMenuStrip entityContextMenuStrip1;
        private System.Windows.Forms.ToolStripMenuItem removeNodeToolStripMenuItem;
        private System.Windows.Forms.Panel bottom_panel;
        private System.Windows.Forms.PropertyGrid propertyGrid1;
        private System.Windows.Forms.Label frameLabel;
        private System.Windows.Forms.Button stopbutton;
        private System.Windows.Forms.TrackBar trackBar1;
        private System.Windows.Forms.Panel panel_child_frame_group;
    }
}