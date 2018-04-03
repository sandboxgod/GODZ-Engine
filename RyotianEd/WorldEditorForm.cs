using System;
using System.Collections;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using GodzGlue;

namespace RyotianEd
{
    public partial class WorldEditorForm : Form, TabPanelListener
    {
        public static GodzGlue.Vector3 zero = new GodzGlue.Vector3(0, 0, 0);
        MainForm1 mMainForm;
        Hashtable actorTreeMap = new Hashtable();
        List<Entity> modifiedActorList = new List<Entity>();
        bool isCharacterMovementActive = false;
        GizmoAxis mHighlightedAxis;
        Point mOldPoint = new Point(-1,-1);
        Camera mSceneCamera;

        public WorldEditorForm()
        {
            InitializeComponent();
        }

        void TabPanelListener.OnTick()
        {
        }

        void TabPanelListener.FormClosing()
        {
            if (modifiedActorList.Count > 0)
            {
                DialogResult result = MessageBox.Show("You have modified actors and not submitted the changes. Do you wish to submit them now?");
                if (result == DialogResult.OK)
                {
                    foreach (Entity e in modifiedActorList)
                    {
                        UpdateResult sqlresult = Editor.UpdateEntity(e.getLayer().getName(), e);

                        if (!sqlresult.result)
                        {
                            MessageBox.Show(sqlresult.error);
                        }
                    }

                    modifiedActorList.Clear();
                }
            }
        }

        void TabPanelListener.PackageLoaded(GodzGlue.Package package)
        {
            AddPackage(package);
        }

        void TabPanelListener.OnSave()
        {
        }

        void TabPanelListener.OnLostFocus()
        {
        }

        void TabPanelListener.OnGainedFocus()
        {
        }

        void TabPanelListener.OnObjectAdded(GodzGlue.ObjectBase obj)
        {
        }

        void TabPanelListener.OnKeyDown(object sender, KeyEventArgs e)
        {
            if (mSceneCamera != null)
            {
                // Move the scene camera
                Vector3 pos = mSceneCamera.getLocation();
                switch (e.KeyCode)
                {
                    case Keys.Up:
                        {
                            Vector3 dir = mSceneCamera.getForward();
                            pos += dir * 5.0f;
                        }
                        break;
                    case Keys.Down:
                        {
                            Vector3 dir = mSceneCamera.getForward();
                            pos += dir * -5.0f;
                        }
                        break;
                    case Keys.Right:
                        {
                            Vector3 dir = mSceneCamera.getRight();
                            pos += dir * 5.0f;
                        }
                        break;
                    case Keys.Left:
                        {
                            Vector3 dir = mSceneCamera.getRight();
                            pos += dir * -5.0f;
                        }
                        break;
                }

                mSceneCamera.setLocation(ref pos);
            }
        }

        public void setTabData(TabPanelData data, MainForm1 mainForm)
        {
            mMainForm = mainForm;

            data.mViewportId = pictureBox1.Handle;
            data.entityOptionMenuStrip = contextMenuStrip1;

            WorldEditor.buildLayersTree(sectorsTreeView1, "World", data, layerContextMenuStrip, layerContextSubMenuStrip);

            pictureBox1.MouseClick += new System.Windows.Forms.MouseEventHandler(this.pictureBox1_MouseClick);
            pictureBox1.MouseDown += new System.Windows.Forms.MouseEventHandler(mainForm.pictureBox1_MouseDown);
            pictureBox1.MouseUp += new System.Windows.Forms.MouseEventHandler(mainForm.pictureBox1_MouseUp);
            pictureBox1.MouseMove += new System.Windows.Forms.MouseEventHandler(mainForm.pictureBox1_MouseMove);
            pictureBox1.MouseMove += new System.Windows.Forms.MouseEventHandler(this.pictureBox1_MouseMove);            
            pictureBox1.MouseWheel += new System.Windows.Forms.MouseEventHandler(mainForm.pictureBox1_MouseWheel);

            buildPackageTree();
        }

        private void buildPackageTree()
        {
            List<Package> packages = new List<Package>();
            GodzUtil.getPackages(packages);

            foreach (Package p in packages)
            {
                AddPackage(p);
            }

            packageTreeView1.ExpandAll();
        }

        //Adds a Package to the Package tree control
        private void AddPackage(Package p)
        {
            UInt32 count = 0;

            String packageName = "<Package>";

            UInt32 nameHash = p.GetName();
            packageName = Editor.GetHashString(nameHash);

            TreeNode pnode = new TreeNode(packageName);

            UInt32 num = p.GetNumObjects();
            for (UInt32 i = 0; i < num; i++)
            {
                ObjectBase placeable = p.GetObjectAt(i);
                if (placeable.getClass().isPlaceable()
                    || (placeable.IsA("Mesh") && placeable.IsAObject())
                    )
                {
                    count++;

                    String objName = placeable.getClass().ClassName;

                    if (placeable.IsA("Mesh"))
                    {
                        //check for the mesh name
                        nameHash = placeable.getObjectName();
                        String temp = Editor.GetHashString(nameHash);
                        if (temp != null)
                        {
                            objName = temp;
                        }
                    }

                    TreeNode n = new TreeNode(objName);
                    pnode.Nodes.Add(n);
                    n.Tag = placeable;
                    n.ContextMenuStrip = contextMenuStrip1;
                }
            }

            if (count > 0)
            {
                //Add to the package root node
                packageTreeView1.Nodes.Add(pnode);
                pnode.ExpandAll();
            }
        }

        public void setViewport()
        {
            GodzUtil.AddViewport(pictureBox1.Handle);
        }

        private void submitChanges()
        {
            TabPanelData data = mMainForm.getTabPanel();
            DatabaseObjectInfo inf = DatabaseObjectRegistry.get(data.mSelectedEntity);

            if (inf.locked)
            {
                //Update the database entry for this item
                Editor.UpdateEntity(data.mSelectedEntity.getLayer().getName(), data.mSelectedEntity);

                //remove from the modified actors list
                modifiedActorList.Remove(data.mSelectedEntity);
            }
            else
            {
                MessageBox.Show("Changes cannot be submitted for an unlocked object");
            }
        }

        private String getEntityName(GodzGlue.Entity e)
        {
            UInt32 hash = e.getObjectName();
            String text = Editor.GetHashString(hash);
            if (text == null)
            {
                text = "<" + e.getClass().ClassName + ">";
            }

            return text;
        }

        private void AddActorToTree(GodzGlue.Entity actor, TreeNode parent)
        {
            String text = getEntityName(actor);
            TreeNode n = new System.Windows.Forms.TreeNode(text);
            parent.Nodes.Add(n);
            n.Tag = actor; //store the related entity
            n.ContextMenuStrip = actorContextMenuStrip1;

            actorTreeMap.Add(actor, n);
        }

        //add all the actors for the active layer
        private void buildActorTree(TabPanelData data)
        {
            actorsTreeView1.Nodes.Clear();
            actorTreeMap.Clear();

            TreeNode treeNode2 = new System.Windows.Forms.TreeNode("Entities");
            uint num = data.mActiveLayer.getNumActors();
            for (uint i = 0; i < num; i++)
            {
                GodzGlue.Entity e = data.mActiveLayer.getActor(i);

                //Only add placeable entities
                if (e.getClass().isPlaceable())
                {
                    AddActorToTree(e, treeNode2);
                }
            }

            actorsTreeView1.Nodes.Add(treeNode2);
            actorsTreeView1.ExpandAll();
        }

        private void selectEntity(GodzGlue.Entity ent, TabPanelData data)
        {
            data.mSelectedEntity = ent;
            Object proxyObj = Editor.GetNewObjectProxy(ent);
            propertyGrid1.SelectedObject = proxyObj;
            DatabaseObjectInfo inf = DatabaseObjectRegistry.get(ent);

            //disable property editing unless this object is checked out
            propertyGrid1.Enabled = inf.locked;
            data.mWorld.selectEntity(ent);

            if (inf.locked)
            {
                //if we have it checked out update the tooltip
                lockForEditingToolStripMenuItem.Text = "Checkin";
            }
            else
            {
                lockForEditingToolStripMenuItem.Text = "Checkout";
            }
        }

        private void onEntityRenamed(GodzGlue.Entity entity)
        {
            TreeNode n = (TreeNode)actorTreeMap[entity];
            if (n != null)
            {
                n.Text = getEntityName(entity);
            }
        }

        private void addLayerToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if (!Editor.IsConnectedToDatabase())
            {
                WorkOfflineConfirmationForm workOffline = new WorkOfflineConfirmationForm();
                if (workOffline.ShowDialog() == DialogResult.Cancel)
                {
                    return;
                }
            }

            NewLayer dlg = new NewLayer();
            DialogResult result = dlg.ShowDialog();
            if (result != DialogResult.OK)
            {
                return;
            }

            TabPanelData data = mMainForm.getTabPanel();

            data.mActiveLayer = data.mWorld.addLayer(dlg.sectorName);
            Package layerPackage = GodzUtil.FindPackage(dlg.sectorName);
            if( layerPackage == null)
                layerPackage = GodzUtil.AddPackage(dlg.sectorName, PackageType.PackageType_GDZ);

            data.mActiveLayer.setPackage(layerPackage);
            data.mPrimaryCamera = data.mActiveLayer.spawnCamera(ref zero, ref zero);
            data.mPrimaryCamera.GrabFocus(); //make this camera primary
            mSceneCamera = data.mPrimaryCamera;

            buildActorTree(data);

            WorldEditor.addLayerToTree(data.mActiveLayer, sectorsTreeView1, data, layerContextSubMenuStrip, sectorsTreeView1.Nodes[0]);
        }

        private void setActiveLayer(Layer layer)
        {
            TabPanelData data = mMainForm.getTabPanel();
            data.mActiveLayer = layer;

            //Change Tab name to the Active Layer
            String layerName = Editor.GetHashString(layer.getName());
            if (layerName != null)
            {
                mMainForm.mActiveTabPage.Text = layerName;
            }

            //if it's a brand new layer; we acquire everything from SQL Server
            if (layer.getNumActors() == 0)
            {
                data.mPrimaryCamera = data.mActiveLayer.spawnCamera(ref zero, ref zero);
                data.mPrimaryCamera.GrabFocus(); //make this camera primary

                Editor.LoadEntities(data);

                layer.loadNavigationMesh(layerName);
            }
            else
            {
                //restore the previous layer
                data.mPrimaryCamera = layer.getPrimaryCamera();
                data.mPrimaryCamera.GrabFocus(); //make this camera primary
            }

            mSceneCamera = data.mPrimaryCamera;

            //add all the newly loaded packages
            packageTreeView1.Nodes.Clear();
            buildPackageTree();

            buildActorTree(data);

            sectorsTreeView1.ExpandAll();
        }

        private void openSectorToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if (Editor.IsConnectedToDatabase())
            {
                TabPanelData data = mMainForm.getTabPanel();
                OpenLayerForm form = new OpenLayerForm(data);
                if (form.ShowDialog() == DialogResult.OK)
                {
                    //Make it the active layer if one hasn't been set
                    if (data.mActiveLayer == null)
                    {
                        setActiveLayer(form.mLayer);
                    }

                    //Append the Layer to the current "World"
                    WorldEditor.addLayerToTree(form.mLayer, sectorsTreeView1, data, layerContextSubMenuStrip, sectorsTreeView1.Nodes[0]);
                }

                form.Dispose();
            }
        }

        private void addToWorldToolStripMenuItem_Click(object sender, EventArgs e)
        {
            TabPanelData data = mMainForm.getTabPanel();

            if (data.mActiveLayer == null)
            {
                MessageBox.Show("Cannot add an entity without an active Sector. Please add one to the Level");
                return;
            }

            if (packageTreeView1.SelectedNode == null || packageTreeView1.SelectedNode.Tag == null)
            {
                //They must have selected a package node...
                return;
            }

            ObjectBase selectedNode = (ObjectBase)packageTreeView1.SelectedNode.Tag;

            //TODO: need to somehow cache off a global placement position
            GodzGlue.Vector3 cameraPos = data.mPrimaryCamera.getLocation();

            GodzGlue.Vector3 fdir = data.mPrimaryCamera.getForward();

            //y+ up
            fdir.z += 100.0f;
            cameraPos += fdir;

            GodzGlue.Entity newActor = null;

            if (selectedNode.IsA("Mesh"))
            {
                //build entity then set Mesh
                newActor = data.mActiveLayer.spawnActor("WEntity", ref cameraPos, ref zero);

                //Get mesh from the package
                Mesh m = (Mesh)selectedNode;
                newActor.setMesh(m);
            }
            else if (selectedNode.IsA("WSunLight"))
            {
                if (data.mSun != null)
                {
                    //display error, can only have 1 Sun
                    MessageBox.Show("You can only have 1 sunlight in the scene");
                }
                else
                {
                    data.mSun = (GodzGlue.SunLight)data.mActiveLayer.spawnActor("WSunLight", ref cameraPos, ref zero);
                    data.mSun.setSunLight();
                    newActor = data.mSun;
                }
            }
            else
            {
                ClassBase gc = selectedNode.getClass();
                newActor = data.mActiveLayer.spawnActor(gc.getObjectName(), ref cameraPos, ref zero);
            }

            //set package....
            newActor.setPackage(data.mActiveLayer.getPackage());

            //Add to the Database
            Editor.AddEntity(data.mActiveLayer.getName(), newActor);

            //tick the actors so they send events over to renderer
            GodzUtil.RunMainPass();

            //Add the entity to the 'Actors' list
            AddActorToTree(newActor, actorsTreeView1.Nodes[0]);
        }

        //We handle the mouse click here locally since it's specific to world editor only
        private void pictureBox1_MouseClick(object sender, MouseEventArgs e)
        {
            TabPanelData data = mMainForm.getTabPanel();
            if (!mMainForm.mCameraInputActive && e.Button == MouseButtons.Left && data.mActiveLayer != null)
            {
                Point p = new Point();
                p.X = e.X;
                p.Y = e.Y;

                GodzGlue.Entity ent = data.mActiveLayer.pickObjectAtPoint(ref p);
                if (ent != null)
                {
                    selectEntity(ent, data);
                }
                else
                {
                    propertyGrid1.SelectedObject = null;
                }
            }
        }

        public void pictureBox1_MouseMove(object sender, MouseEventArgs e)
        {
            if (isCharacterMovementActive && e.Button != MouseButtons.Right)
            {
                TabPanelData data = mMainForm.getTabPanel();
                DatabaseObjectInfo inf = DatabaseObjectRegistry.get(data.mSelectedEntity);

                if (data.mSelectedEntity != null && inf.locked)
                {
                    if (e.Button == MouseButtons.Left)
                    {
                        //move the selected character along the preferred axis
                        if (mHighlightedAxis != GizmoAxis.GizmoAxis_None)
                        {
                            if (mOldPoint.X == -1)
                            {
                                mOldPoint.X = e.X;
                                mOldPoint.Y = e.Y;
                            }

                            float dx = e.X - mOldPoint.X;
                            float dy = e.Y - mOldPoint.Y;

                            //move character along the gizmo
                            float scale = 1;
                            Vector3 axis = new Vector3(0, 0, 0);
                            switch (mHighlightedAxis)
                            {
                                default:
                                    {
                                    }
                                    break;
                                case GizmoAxis.GizmoAxis_X:
                                    {
                                        axis = new Vector3(1, 0, 0);
                                        axis = axis * dx;
                                    }
                                    break;
                                case GizmoAxis.GizmoAxis_Y:
                                    {
                                        axis = new Vector3(0, -1, 0);
                                        axis = axis * dy;
                                    }
                                    break;
                                case GizmoAxis.GizmoAxis_Z:
                                    {
                                        axis = new Vector3(0, 0, 1);
                                        axis = axis * dx;
                                    }
                                    break;
                            } //switch

                            axis = axis * scale;
                            Vector3 pos = data.mSelectedEntity.getLocation();
                            pos = pos + axis;
                            data.mSelectedEntity.setLocation(ref pos);

                            //add this actor to the modified list
                            if (!modifiedActorList.Contains(data.mSelectedEntity))
                            {
                                modifiedActorList.Add(data.mSelectedEntity);
                            }

                            GodzUtil.RunMainPass();

                            //cache mouse pos
                            mOldPoint.X = e.X;
                            mOldPoint.Y = e.Y;
                        }
                    }
                    else
                    {
                        //reset cached mousepoint
                        mOldPoint.X = -1;
                        mOldPoint.Y = -1;

                        //highlight the active axis the character will be moved along....
                        Point p = new Point();
                        p.X = e.X;
                        p.Y = e.Y;

                        mHighlightedAxis = data.mWorld.pickAxis(ref p);

                        //tick the actors so they send events over to renderer
                        GodzUtil.RunMainPass();
                    } //else
                }
            }
            else
            {
                //reset
                mOldPoint.X = -1;
                mOldPoint.Y = -1;
            }
        }

        private void packageTreeView1_NodeMouseClick(object sender, TreeNodeMouseClickEventArgs e)
        {
        }

        private void actorsTreeView1_NodeMouseClick(object sender, TreeNodeMouseClickEventArgs e)
        {
            TreeNode n = e.Node;
            GodzGlue.Entity ent = (GodzGlue.Entity)n.Tag;
            if (ent != null)
            {
                TabPanelData data = mMainForm.getTabPanel();
                selectEntity(ent, data);
            }
        }

        private void pictureBox1_Click(object sender, EventArgs e)
        {

        }

        private void propertyGrid1_PropertyValueChanged(object s, PropertyValueChangedEventArgs e)
        {
            TabPanelData data = mMainForm.getTabPanel();

            //We should go update all the Entity names on the tree just incase one of the
            //properties was the name
            onEntityRenamed(data.mSelectedEntity);
        }

        private void moveToolStripButton1_CheckStateChanged(object sender, EventArgs e)
        {
            //determine if we need to activate/deactivate move mode
            ToolStripButton button = (ToolStripButton)sender;
            isCharacterMovementActive = button.Checked;

            if (!isCharacterMovementActive)
            {
                TabPanelData data = mMainForm.getTabPanel();
                data.mWorld.selectAxis(GizmoAxis.GizmoAxis_None);
            }
        }

        private void removeActorToolStripMenuItem_Click(object sender, EventArgs e)
        {
            DialogResult result = MessageBox.Show("Are sure you wish to remove this actor?", "Remove Actor Confirmation", MessageBoxButtons.YesNo );
            if (result == DialogResult.Yes)
            {
                //User wishes to remove the selected entity....
                TabPanelData data = mMainForm.getTabPanel();
                DatabaseObjectInfo inf = DatabaseObjectRegistry.get(data.mSelectedEntity);

                //1. remove this entity from the database
                if (Editor.IsConnectedToDatabase())
                {
                    if (inf.locked && Editor.RemoveEntity(data.mActiveLayer.getName(), data.mSelectedEntity))
                    {
                        MessageBox.Show("Entity has been successfully removed from the Database");
                    }
                    else
                    {
                        MessageBox.Show("Entity could not be removed from the Database");
                        return;
                    }
                }

                //2. remove the node associated with this guy....
                TreeNode n = (TreeNode)actorTreeMap[data.mSelectedEntity];
                if (n != null)
                {
                    actorsTreeView1.Nodes.Remove(n);
                }

                //3. deref the hashstring(s) that belong to this entity
                List<GodzGlue.ClassPropertyInfo> cpList = new List<GodzGlue.ClassPropertyInfo>();
                data.mSelectedEntity.getPropertyValues(cpList);

                foreach (ClassPropertyInfo cp in cpList)
                {
                    if (cp.mPropertyType == GodzGlue.ClassPropertyType.PropertyType_HashCode)
                    {
                        uint hash = cp.mObjectHash;
                        Editor.RemoveHash(hash);
                    }
                }

                //4. clean the actorTreeMap / modified actor list
                actorTreeMap.Remove(data.mSelectedEntity);
                modifiedActorList.Remove(data.mSelectedEntity);

                //Finally, remove this entity from the world
                data.mSelectedEntity.removeFromWorld();
                data.mSelectedEntity = null;

                //Deselect the former object
                propertyGrid1.SelectedObject = null;

                //turn off the axis too
                data.mWorld.selectAxis(GizmoAxis.GizmoAxis_None);

                GodzUtil.RunMainPass();
            }
        }

        private void zoomToActorToolStripMenuItem_Click(object sender, EventArgs e)
        {
            TabPanelData data = mMainForm.getTabPanel();
            data.mPrimaryCamera.moveToActor(data.mSelectedEntity);

            GodzUtil.RunMainPass();
        }

        private void submitChangesToolStripMenuItem_Click(object sender, EventArgs e)
        {
            submitChanges();
        }

        private void WorldEditorForm_FormClosing(object sender, FormClosingEventArgs e)
        {

        }

        private void refreshToolStripMenuItem_Click(object sender, EventArgs e)
        {
            TabPanelData data = mMainForm.getTabPanel();

            Editor.RefreshEntity(data.mSelectedEntity, data.mSelectedEntity.getLayer().getName());
            GodzUtil.RunMainPass();
        }

        private void lockForEditingToolStripMenuItem_Click(object sender, EventArgs e)
        {
            TabPanelData data = mMainForm.getTabPanel();
            DatabaseObjectInfo inf = DatabaseObjectRegistry.get(data.mSelectedEntity);

            if (inf.locked)
            {
                //they trying to checkin changes...
                submitChanges();

                if (Editor.UnlockEntity(data.mSelectedEntity))
                {
                    propertyGrid1.Enabled = false;
                }
            }
            else
            {
                //grab latest from the server then lock it
                Editor.RefreshEntity(data.mSelectedEntity, data.mActiveLayer.getName());
                if (Editor.LockEntity(data.mSelectedEntity))
                {
                    propertyGrid1.Enabled = true;
                }
            }
        }

        private void clearSelection(TabPanelData data)
        {
            //clear out the previous selection
            data.mWorld.selectAxis(GizmoAxis.GizmoAxis_None);
            mHighlightedAxis = GizmoAxis.GizmoAxis_None;

            if (data.mSelectedEntity != null)
            {
                data.mWorld.deselectEntity(data.mSelectedEntity);
                data.mSelectedEntity = null;
            }

            propertyGrid1.SelectedObject = null;
        }

        //Makes a Layer active
        private void makeActiveToolStripMenuItem_Click(object sender, EventArgs e)
        {
            TabPanelData data = mMainForm.getTabPanel();
            TreeNode node = sectorsTreeView1.SelectedNode;
            Layer activeLayer = (Layer)node.Tag;
            if (activeLayer != null && data.mActiveLayer != activeLayer)
            {
                setActiveLayer(activeLayer);

                WorldEditor.buildLayersTree(sectorsTreeView1, "World", data, layerContextMenuStrip, layerContextSubMenuStrip);

                //re-expand the tree
                sectorsTreeView1.ExpandAll();

                clearSelection(data);
            }
        }

        //Unloads a Layer...
        private void unloadToolStripMenuItem_Click(object sender, EventArgs e)
        {

        }

        //Removes the Layer from the database
        private void removeLayerToolStripMenuItem_Click(object sender, EventArgs e)
        {

        }

        private void pictureBox1_SizeChanged(object sender, EventArgs e)
        {
            
        }

        private void WorldEditorForm_Load(object sender, EventArgs e)
        {

        }

        private void rotateToolStripButton1_CheckStateChanged(object sender, EventArgs e)
        {

        }

        private void navMeshToolStripButton1_Click(object sender, EventArgs e)
        {
            //generates a navigation mesh for the level....
            TabPanelData data = mMainForm.getTabPanel();

            if (data.mActiveLayer != null)
            {
                string layerName = Editor.GetHashString(data.mActiveLayer.getName());
                data.mActiveLayer.buildNavigationMesh();
                data.mActiveLayer.saveNavigationMesh(layerName);
            }
        }

        private void onExportSector(object sender, EventArgs e)
        {
            TreeNode node = sectorsTreeView1.SelectedNode;
            Layer selectedLayer = (Layer)node.Tag;
            //String layerName = Editor.GetHashString(selectedLayer.getName());
            //selectedLayer.SaveToRelativeLocation(layerName);

            //Let's export it fresh from the Database, this way our camera won't get saved out
            //get the navigation mesh and export that out to the package
            ObjectBase navMeshInfo = selectedLayer.getNavMeshInfo();
            List<ObjectBase> list = new List<ObjectBase>();
            if ( navMeshInfo != null )
                list.Add(navMeshInfo);

            Editor.ExportPackage(selectedLayer.getName(), ResourceType.ResourceType_Scene,ref list);
        }
    }
}
