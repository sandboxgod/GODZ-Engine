namespace RyotianEd
{
    partial class WorldEditorForm
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(WorldEditorForm));
            this.layerContextMenuStrip = new System.Windows.Forms.ContextMenuStrip(this.components);
            this.addLayerToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.openSectorToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.layerContextSubMenuStrip = new System.Windows.Forms.ContextMenuStrip(this.components);
            this.makeActiveToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.unloadToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.removeLayerToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.contextMenuStrip1 = new System.Windows.Forms.ContextMenuStrip(this.components);
            this.addToWorldToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.actorContextMenuStrip1 = new System.Windows.Forms.ContextMenuStrip(this.components);
            this.lockForEditingToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.submitChangesToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.refreshToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.zoomToActorToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.removeActorToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.tabControl1 = new System.Windows.Forms.TabControl();
            this.tabPage1 = new System.Windows.Forms.TabPage();
            this.packageTreeView1 = new System.Windows.Forms.TreeView();
            this.tabPage3 = new System.Windows.Forms.TabPage();
            this.sectorsTreeView1 = new System.Windows.Forms.TreeView();
            this.tabPage4 = new System.Windows.Forms.TabPage();
            this.actorsTreeView1 = new System.Windows.Forms.TreeView();
            this.toolStrip1 = new System.Windows.Forms.ToolStrip();
            this.moveToolStripButton1 = new System.Windows.Forms.ToolStripButton();
            this.rotateToolStripButton1 = new System.Windows.Forms.ToolStripButton();
            this.navMeshToolStripButton1 = new System.Windows.Forms.ToolStripButton();
            this.propertyGrid1 = new System.Windows.Forms.PropertyGrid();
            this.pictureBox1 = new System.Windows.Forms.PictureBox();
            this.exportToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.layerContextMenuStrip.SuspendLayout();
            this.layerContextSubMenuStrip.SuspendLayout();
            this.contextMenuStrip1.SuspendLayout();
            this.actorContextMenuStrip1.SuspendLayout();
            this.tabControl1.SuspendLayout();
            this.tabPage1.SuspendLayout();
            this.tabPage3.SuspendLayout();
            this.tabPage4.SuspendLayout();
            this.toolStrip1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).BeginInit();
            this.SuspendLayout();
            // 
            // layerContextMenuStrip
            // 
            this.layerContextMenuStrip.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.addLayerToolStripMenuItem,
            this.openSectorToolStripMenuItem});
            this.layerContextMenuStrip.Name = "layerContextMenuStrip";
            this.layerContextMenuStrip.Size = new System.Drawing.Size(140, 48);
            // 
            // addLayerToolStripMenuItem
            // 
            this.addLayerToolStripMenuItem.Name = "addLayerToolStripMenuItem";
            this.addLayerToolStripMenuItem.Size = new System.Drawing.Size(139, 22);
            this.addLayerToolStripMenuItem.Text = "Add Sector";
            this.addLayerToolStripMenuItem.Click += new System.EventHandler(this.addLayerToolStripMenuItem_Click);
            // 
            // openSectorToolStripMenuItem
            // 
            this.openSectorToolStripMenuItem.Name = "openSectorToolStripMenuItem";
            this.openSectorToolStripMenuItem.Size = new System.Drawing.Size(139, 22);
            this.openSectorToolStripMenuItem.Text = "Open Sector";
            this.openSectorToolStripMenuItem.Click += new System.EventHandler(this.openSectorToolStripMenuItem_Click);
            // 
            // layerContextSubMenuStrip
            // 
            this.layerContextSubMenuStrip.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.makeActiveToolStripMenuItem,
            this.unloadToolStripMenuItem,
            this.removeLayerToolStripMenuItem,
            this.exportToolStripMenuItem});
            this.layerContextSubMenuStrip.Name = "layerContextSubMenuStrip";
            this.layerContextSubMenuStrip.Size = new System.Drawing.Size(154, 114);
            // 
            // makeActiveToolStripMenuItem
            // 
            this.makeActiveToolStripMenuItem.Name = "makeActiveToolStripMenuItem";
            this.makeActiveToolStripMenuItem.Size = new System.Drawing.Size(153, 22);
            this.makeActiveToolStripMenuItem.Text = "Make Active";
            this.makeActiveToolStripMenuItem.Click += new System.EventHandler(this.makeActiveToolStripMenuItem_Click);
            // 
            // unloadToolStripMenuItem
            // 
            this.unloadToolStripMenuItem.Name = "unloadToolStripMenuItem";
            this.unloadToolStripMenuItem.Size = new System.Drawing.Size(153, 22);
            this.unloadToolStripMenuItem.Text = "Unload";
            this.unloadToolStripMenuItem.ToolTipText = "Unloads this Layer";
            this.unloadToolStripMenuItem.Click += new System.EventHandler(this.unloadToolStripMenuItem_Click);
            // 
            // removeLayerToolStripMenuItem
            // 
            this.removeLayerToolStripMenuItem.Name = "removeLayerToolStripMenuItem";
            this.removeLayerToolStripMenuItem.Size = new System.Drawing.Size(153, 22);
            this.removeLayerToolStripMenuItem.Text = "Remove Sector";
            this.removeLayerToolStripMenuItem.ToolTipText = "Permenantly removes this Layer from the Database";
            this.removeLayerToolStripMenuItem.Click += new System.EventHandler(this.removeLayerToolStripMenuItem_Click);
            // 
            // contextMenuStrip1
            // 
            this.contextMenuStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.addToWorldToolStripMenuItem});
            this.contextMenuStrip1.Name = "contextMenuStrip1";
            this.contextMenuStrip1.Size = new System.Drawing.Size(146, 26);
            // 
            // addToWorldToolStripMenuItem
            // 
            this.addToWorldToolStripMenuItem.Name = "addToWorldToolStripMenuItem";
            this.addToWorldToolStripMenuItem.Size = new System.Drawing.Size(145, 22);
            this.addToWorldToolStripMenuItem.Text = "Add to World";
            this.addToWorldToolStripMenuItem.Click += new System.EventHandler(this.addToWorldToolStripMenuItem_Click);
            // 
            // actorContextMenuStrip1
            // 
            this.actorContextMenuStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.lockForEditingToolStripMenuItem,
            this.submitChangesToolStripMenuItem,
            this.refreshToolStripMenuItem,
            this.zoomToActorToolStripMenuItem,
            this.removeActorToolStripMenuItem});
            this.actorContextMenuStrip1.Name = "actorContextMenuStrip1";
            this.actorContextMenuStrip1.Size = new System.Drawing.Size(205, 114);
            // 
            // lockForEditingToolStripMenuItem
            // 
            this.lockForEditingToolStripMenuItem.Name = "lockForEditingToolStripMenuItem";
            this.lockForEditingToolStripMenuItem.Size = new System.Drawing.Size(204, 22);
            this.lockForEditingToolStripMenuItem.Text = "<placeholder checkout>";
            this.lockForEditingToolStripMenuItem.ToolTipText = "Exclusive Lock / Unlock this resource";
            this.lockForEditingToolStripMenuItem.Click += new System.EventHandler(this.lockForEditingToolStripMenuItem_Click);
            // 
            // submitChangesToolStripMenuItem
            // 
            this.submitChangesToolStripMenuItem.Name = "submitChangesToolStripMenuItem";
            this.submitChangesToolStripMenuItem.Size = new System.Drawing.Size(204, 22);
            this.submitChangesToolStripMenuItem.Text = "Submit Changes";
            this.submitChangesToolStripMenuItem.ToolTipText = "Submit changes to the database";
            this.submitChangesToolStripMenuItem.Click += new System.EventHandler(this.submitChangesToolStripMenuItem_Click);
            // 
            // refreshToolStripMenuItem
            // 
            this.refreshToolStripMenuItem.Name = "refreshToolStripMenuItem";
            this.refreshToolStripMenuItem.Size = new System.Drawing.Size(204, 22);
            this.refreshToolStripMenuItem.Text = "Refresh";
            this.refreshToolStripMenuItem.ToolTipText = "Refresh data from the database";
            this.refreshToolStripMenuItem.Click += new System.EventHandler(this.refreshToolStripMenuItem_Click);
            // 
            // zoomToActorToolStripMenuItem
            // 
            this.zoomToActorToolStripMenuItem.Name = "zoomToActorToolStripMenuItem";
            this.zoomToActorToolStripMenuItem.Size = new System.Drawing.Size(204, 22);
            this.zoomToActorToolStripMenuItem.Text = "Zoom To Actor";
            this.zoomToActorToolStripMenuItem.Click += new System.EventHandler(this.zoomToActorToolStripMenuItem_Click);
            // 
            // removeActorToolStripMenuItem
            // 
            this.removeActorToolStripMenuItem.Name = "removeActorToolStripMenuItem";
            this.removeActorToolStripMenuItem.Size = new System.Drawing.Size(204, 22);
            this.removeActorToolStripMenuItem.Text = "Remove Actor";
            this.removeActorToolStripMenuItem.Click += new System.EventHandler(this.removeActorToolStripMenuItem_Click);
            // 
            // tabControl1
            // 
            this.tabControl1.Controls.Add(this.tabPage1);
            this.tabControl1.Controls.Add(this.tabPage3);
            this.tabControl1.Controls.Add(this.tabPage4);
            this.tabControl1.Dock = System.Windows.Forms.DockStyle.Left;
            this.tabControl1.Location = new System.Drawing.Point(0, 0);
            this.tabControl1.Name = "tabControl1";
            this.tabControl1.SelectedIndex = 0;
            this.tabControl1.Size = new System.Drawing.Size(236, 518);
            this.tabControl1.TabIndex = 4;
            // 
            // tabPage1
            // 
            this.tabPage1.Controls.Add(this.packageTreeView1);
            this.tabPage1.Location = new System.Drawing.Point(4, 22);
            this.tabPage1.Name = "tabPage1";
            this.tabPage1.Padding = new System.Windows.Forms.Padding(3);
            this.tabPage1.Size = new System.Drawing.Size(228, 492);
            this.tabPage1.TabIndex = 0;
            this.tabPage1.Text = "Packages";
            this.tabPage1.UseVisualStyleBackColor = true;
            // 
            // packageTreeView1
            // 
            this.packageTreeView1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.packageTreeView1.Location = new System.Drawing.Point(3, 3);
            this.packageTreeView1.Name = "packageTreeView1";
            this.packageTreeView1.Size = new System.Drawing.Size(222, 486);
            this.packageTreeView1.TabIndex = 0;
            // 
            // tabPage3
            // 
            this.tabPage3.Controls.Add(this.sectorsTreeView1);
            this.tabPage3.Location = new System.Drawing.Point(4, 22);
            this.tabPage3.Name = "tabPage3";
            this.tabPage3.Size = new System.Drawing.Size(228, 492);
            this.tabPage3.TabIndex = 2;
            this.tabPage3.Text = "Sectors";
            this.tabPage3.UseVisualStyleBackColor = true;
            // 
            // sectorsTreeView1
            // 
            this.sectorsTreeView1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.sectorsTreeView1.Location = new System.Drawing.Point(0, 0);
            this.sectorsTreeView1.Name = "sectorsTreeView1";
            this.sectorsTreeView1.Size = new System.Drawing.Size(228, 492);
            this.sectorsTreeView1.TabIndex = 0;
            // 
            // tabPage4
            // 
            this.tabPage4.Controls.Add(this.actorsTreeView1);
            this.tabPage4.Location = new System.Drawing.Point(4, 22);
            this.tabPage4.Name = "tabPage4";
            this.tabPage4.Size = new System.Drawing.Size(228, 492);
            this.tabPage4.TabIndex = 3;
            this.tabPage4.Text = "Actors";
            this.tabPage4.UseVisualStyleBackColor = true;
            // 
            // actorsTreeView1
            // 
            this.actorsTreeView1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.actorsTreeView1.Location = new System.Drawing.Point(0, 0);
            this.actorsTreeView1.Name = "actorsTreeView1";
            this.actorsTreeView1.Size = new System.Drawing.Size(228, 492);
            this.actorsTreeView1.TabIndex = 0;
            this.actorsTreeView1.NodeMouseClick += new System.Windows.Forms.TreeNodeMouseClickEventHandler(this.actorsTreeView1_NodeMouseClick);
            // 
            // toolStrip1
            // 
            this.toolStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.moveToolStripButton1,
            this.rotateToolStripButton1,
            this.navMeshToolStripButton1});
            this.toolStrip1.Location = new System.Drawing.Point(236, 0);
            this.toolStrip1.Name = "toolStrip1";
            this.toolStrip1.Size = new System.Drawing.Size(383, 25);
            this.toolStrip1.TabIndex = 5;
            this.toolStrip1.Text = "toolStrip1";
            // 
            // moveToolStripButton1
            // 
            this.moveToolStripButton1.CheckOnClick = true;
            this.moveToolStripButton1.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.moveToolStripButton1.Image = ((System.Drawing.Image)(resources.GetObject("moveToolStripButton1.Image")));
            this.moveToolStripButton1.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.moveToolStripButton1.Name = "moveToolStripButton1";
            this.moveToolStripButton1.Size = new System.Drawing.Size(23, 22);
            this.moveToolStripButton1.Text = "Move";
            this.moveToolStripButton1.CheckStateChanged += new System.EventHandler(this.moveToolStripButton1_CheckStateChanged);
            // 
            // rotateToolStripButton1
            // 
            this.rotateToolStripButton1.CheckOnClick = true;
            this.rotateToolStripButton1.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.rotateToolStripButton1.Image = ((System.Drawing.Image)(resources.GetObject("rotateToolStripButton1.Image")));
            this.rotateToolStripButton1.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.rotateToolStripButton1.Name = "rotateToolStripButton1";
            this.rotateToolStripButton1.Size = new System.Drawing.Size(23, 22);
            this.rotateToolStripButton1.Text = "Rotate";
            this.rotateToolStripButton1.CheckStateChanged += new System.EventHandler(this.rotateToolStripButton1_CheckStateChanged);
            // 
            // navMeshToolStripButton1
            // 
            this.navMeshToolStripButton1.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.navMeshToolStripButton1.Image = ((System.Drawing.Image)(resources.GetObject("navMeshToolStripButton1.Image")));
            this.navMeshToolStripButton1.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.navMeshToolStripButton1.Name = "navMeshToolStripButton1";
            this.navMeshToolStripButton1.Size = new System.Drawing.Size(23, 22);
            this.navMeshToolStripButton1.Text = "NavMesh Generator";
            this.navMeshToolStripButton1.Click += new System.EventHandler(this.navMeshToolStripButton1_Click);
            // 
            // propertyGrid1
            // 
            this.propertyGrid1.Dock = System.Windows.Forms.DockStyle.Right;
            this.propertyGrid1.Location = new System.Drawing.Point(619, 0);
            this.propertyGrid1.Name = "propertyGrid1";
            this.propertyGrid1.Size = new System.Drawing.Size(265, 518);
            this.propertyGrid1.TabIndex = 6;
            this.propertyGrid1.PropertyValueChanged += new System.Windows.Forms.PropertyValueChangedEventHandler(this.propertyGrid1_PropertyValueChanged);
            // 
            // pictureBox1
            // 
            this.pictureBox1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.pictureBox1.Image = ((System.Drawing.Image)(resources.GetObject("pictureBox1.Image")));
            this.pictureBox1.Location = new System.Drawing.Point(236, 25);
            this.pictureBox1.Name = "pictureBox1";
            this.pictureBox1.Size = new System.Drawing.Size(383, 493);
            this.pictureBox1.TabIndex = 7;
            this.pictureBox1.TabStop = false;
            this.pictureBox1.MouseMove += new System.Windows.Forms.MouseEventHandler(this.pictureBox1_MouseMove);
            this.pictureBox1.MouseClick += new System.Windows.Forms.MouseEventHandler(this.pictureBox1_MouseClick);
            // 
            // exportToolStripMenuItem
            // 
            this.exportToolStripMenuItem.Name = "exportToolStripMenuItem";
            this.exportToolStripMenuItem.Size = new System.Drawing.Size(153, 22);
            this.exportToolStripMenuItem.Text = "Export";
            this.exportToolStripMenuItem.Click += new System.EventHandler(this.onExportSector);
            // 
            // WorldEditorForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(884, 518);
            this.Controls.Add(this.pictureBox1);
            this.Controls.Add(this.toolStrip1);
            this.Controls.Add(this.tabControl1);
            this.Controls.Add(this.propertyGrid1);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.None;
            this.Name = "WorldEditorForm";
            this.Text = "WorldEditorForm";
            this.Load += new System.EventHandler(this.WorldEditorForm_Load);
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.WorldEditorForm_FormClosing);
            this.layerContextMenuStrip.ResumeLayout(false);
            this.layerContextSubMenuStrip.ResumeLayout(false);
            this.contextMenuStrip1.ResumeLayout(false);
            this.actorContextMenuStrip1.ResumeLayout(false);
            this.tabControl1.ResumeLayout(false);
            this.tabPage1.ResumeLayout(false);
            this.tabPage3.ResumeLayout(false);
            this.tabPage4.ResumeLayout(false);
            this.toolStrip1.ResumeLayout(false);
            this.toolStrip1.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.ContextMenuStrip layerContextMenuStrip;
        private System.Windows.Forms.ToolStripMenuItem addLayerToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem openSectorToolStripMenuItem;
        private System.Windows.Forms.ContextMenuStrip layerContextSubMenuStrip;
        private System.Windows.Forms.ToolStripMenuItem makeActiveToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem removeLayerToolStripMenuItem;
        private System.Windows.Forms.ContextMenuStrip contextMenuStrip1;
        private System.Windows.Forms.ToolStripMenuItem addToWorldToolStripMenuItem;
        private System.Windows.Forms.ContextMenuStrip actorContextMenuStrip1;
        private System.Windows.Forms.ToolStripMenuItem submitChangesToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem zoomToActorToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem removeActorToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem refreshToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem lockForEditingToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem unloadToolStripMenuItem;
        private System.Windows.Forms.TabControl tabControl1;
        private System.Windows.Forms.TabPage tabPage1;
        private System.Windows.Forms.TreeView packageTreeView1;
        private System.Windows.Forms.TabPage tabPage3;
        private System.Windows.Forms.TreeView sectorsTreeView1;
        private System.Windows.Forms.TabPage tabPage4;
        private System.Windows.Forms.TreeView actorsTreeView1;
        private System.Windows.Forms.ToolStrip toolStrip1;
        private System.Windows.Forms.ToolStripButton moveToolStripButton1;
        private System.Windows.Forms.PropertyGrid propertyGrid1;
        private System.Windows.Forms.PictureBox pictureBox1;
        private System.Windows.Forms.ToolStripButton rotateToolStripButton1;
        private System.Windows.Forms.ToolStripButton navMeshToolStripButton1;
        private System.Windows.Forms.ToolStripMenuItem exportToolStripMenuItem;
    }
}