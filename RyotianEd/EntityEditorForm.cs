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
    public partial class EntityEditorForm : Form, TabPanelListener
    {
        MainForm1 mMainForm;
        public static GodzGlue.Vector3 zero = new GodzGlue.Vector3(0, 0, 0);
        List<Package> packages = new List<Package>();
        List<AnimationSequence> animSequences = new List<AnimationSequence>();
        List<Material> materials = null;
        Entity previewActor = null;
        Package selectedPackage = null;
        TreeNode selPackageNode = null;
        TreeNode mActiveNode = null;
        Mesh selectedMesh = null;
        Hashtable meshMap = new Hashtable();
        Hashtable packageDatabaseMap = new Hashtable();
        uint selectedAnimationIndex = 0;
        Camera mSceneCamera;

        public EntityEditorForm()
        {
            InitializeComponent();
        }

        void TabPanelListener.FormClosing()
        {
        }

        void TabPanelListener.OnTick()
        {
            if (previewActor != null)
            {
                float t = previewActor.getTime();
                int frameNumber = (int)(t);
                frameLabel.Text = "Frame: " + frameNumber;

                // add rotation
                Vector3 up = new Vector3(0, 1, 0);
                previewActor.rotate(up, 0.01f); // rotate along Y axis
            }
        }

        void TabPanelListener.PackageLoaded(GodzGlue.Package package)
        {
            if (package.Type == PackageType.PackageType_RESOURCE)
            {
                TreeNode node = AddPackage(package);
                selectedPackage = package;

                if (node != null)
                {
                    setActivePackageNode(node);
                }
            }
        }

        void TabPanelListener.OnGainedFocus()
        {
            // Since we have no Sun, we cannot allow deferred rendering...
            GodzUtil.EnableDeferredRendering(false);
        }

        void TabPanelListener.OnLostFocus()
        {
            GodzUtil.EnableDeferredRendering(true);
        }

        void TabPanelListener.OnSave()
        {
            if (selectedPackage != null)
            {
                UInt32 hash = selectedPackage.GetName();
                String pname = Editor.GetHashString(hash);
                if (pname == null)
                {
                    //TODO: add a path that allows them to enter a new package name?
                    MessageBox.Show("Cannot find this package in the database! Cannot SAVE...");
                    return;
                }

                //build animation table for all the exported meshes
                UInt32 numObjs = selectedPackage.GetNumObjects();
                for (UInt32 j = 0; j < numObjs; j++)
                {
                    ObjectBase obj = selectedPackage.GetObjectAt(j);
                    if (obj is Mesh)
                    {
                        Mesh mesh = (Mesh)obj;
                        mesh.buildAnims();
                    }
                }

                String packagePath = GodzUtil.getFullPathToPackage(pname, ResourceType.ResourceType_Character);
                GodzUtil.savePackage(packagePath, selectedPackage);
            }
        }

        void TabPanelListener.OnObjectAdded(GodzGlue.ObjectBase obj)
        {
        }

        void TabPanelListener.OnKeyDown(object sender, KeyEventArgs e)
        {
            // Move the scene camera
            Vector3 pos = mSceneCamera.getLocation();
            switch (e.KeyCode)
            {
                case Keys.Up:
                    {
                        Vector3 up = mSceneCamera.getUp();
                        pos += up * -5.0f;
                    }
                    break;
                case Keys.Down:
                    {
                        Vector3 up = mSceneCamera.getUp();
                        pos += up * 5.0f;
                    }
                    break;
                case Keys.Right:
                    {
                        Vector3 dir = mSceneCamera.getRight();
                        pos += dir * -5.0f;
                    }
                    break;
                case Keys.Left:
                    {
                        Vector3 dir = mSceneCamera.getRight();
                        pos += dir * 5.0f;
                    }
                    break;
            }

            mSceneCamera.setLocation(ref pos);
        }

        public void setTabData(TabPanelData data, MainForm1 mainForm)
        {
            mMainForm = mainForm;

            data.mViewportId = pictureBox1.Handle;
        
            pictureBox1.MouseDown += new System.Windows.Forms.MouseEventHandler(mainForm.pictureBox1_MouseDown);
            pictureBox1.MouseUp += new System.Windows.Forms.MouseEventHandler(mainForm.pictureBox1_MouseUp);
            pictureBox1.MouseMove += new System.Windows.Forms.MouseEventHandler(mainForm.pictureBox1_MouseMove);
            pictureBox1.MouseWheel += new System.Windows.Forms.MouseEventHandler(mainForm.pictureBox1_MouseWheel);

            buildAnimationTree();

            categoryComboBox.Items.Clear();
            categoryComboBox.Sorted = false;
            List<String> sequences = new List<string>();
            GodzUtil.getConfigSection(sequences, "Sequences");

            foreach (String text in sequences)
            {
                categoryComboBox.Items.Add(text);
            }
        }

        public void setViewport()
        {
            GodzUtil.AddViewport(pictureBox1.Handle);
            GodzUtil.SetBackgroundColor(pictureBox1.Handle, 162, 162, 162);

            TabPanelData data = mMainForm.getTabPanel();
            data.mActiveLayer = data.mWorld.addLayer("Default");
            data.mPrimaryCamera = data.mActiveLayer.spawnCamera(ref zero, ref zero);
            data.mPrimaryCamera.GrabFocus(); //make this camera primary
            mSceneCamera = data.mPrimaryCamera;

            data.mSun = (GodzGlue.SunLight)data.mActiveLayer.spawnActor("WSunLight", ref zero, ref zero);         

            Vector3 sunspot = new Vector3(0.08508922f, -0.4238535f, 0.9017249f);
            data.mSun.setLookAt(sunspot);

            // Hide the sunlight icon in this scene
            data.mSun.hideEditorMaterial();
            data.mSun.setSunLight();
        }

        private void buildAnimationTree()
        {
            //build the Objects tab -- add all packages, models, then animations under the tree
            GodzUtil.getPackages(packages);

            int numPkgs = packages.Count;
            for (int i = 0; i < numPkgs; i++)
            {
                Package p = packages[i];
                //if (p.GetPackageType() == PackageType.PackageType_RESOURCE)
                    AddPackage(p);
            }

            animationsTreeView.ExpandAll();
        }

        private void setupMaterials()
        {
            materials = new List<Material>();
            selectedMesh.getMaterials(materials);
            materialsComboBox.Items.Clear();

            for (int i = 0; i < materials.Count; i++)
            {
                UInt32 hash = materials[i].getObjectName();
                String text = Editor.GetHashString(hash);
                if (text == null)
                {
                    text = "Material #" + i;
                }

                materialsComboBox.Items.Add(text);
            }
        }

        private TreeNode AddPackage(Package p)
        {
            if (p.Type == PackageType.PackageType_RESOURCE)
            {
                UInt32 nameHash = p.GetName();
                String pname = Editor.GetHashString(nameHash);
                if (pname == null)
                {
                    pname = "<Package>";
                }

                System.Windows.Forms.TreeNode packageNode = new TreeNode(pname);
                animationsTreeView.Nodes.Add(packageNode);
                packageNode.Tag = p;

                //Add all the models to this guy....
                UInt32 numObjs = p.GetNumObjects();
                for (UInt32 j = 0; j < numObjs; j++)
                {
                    ObjectBase obj = p.GetObjectAt(j);
                    if (obj is Mesh)
                    {
                        //assign GUIDs if the mesh needs one
                        Mesh m = (Mesh)obj;
                        if (m.Id == Guid.Empty)
                        {
                            m.Id = Guid.NewGuid();
                        }

                        AddObjectToTree(obj, packageNode);
                    }
                }

                packageNode.ExpandAll();

                return packageNode;
            }

            return null;
        }

        private void AddObjectToTree(ObjectBase obj, TreeNode packageNode)
        {
            String objName;
            UInt32 nameHash = obj.getObjectName();
            objName = Editor.GetHashString(nameHash);

            if (objName == null)
            {
                objName = "<blank>";
            }

            System.Windows.Forms.TreeNode objectNode = new TreeNode(objName);
            packageNode.Nodes.Add(objectNode);
            objectNode.Tag = obj;
            objectNode.ContextMenuStrip = entityContextMenuStrip1;

            //get a list of animations from this mesh
            Mesh mesh = (Mesh)obj;
            meshMap[mesh] = objectNode;

            animSequences.Clear();
            mesh.getAnimations(animSequences);
            UInt32 count = 0;
            foreach (AnimationSequence sequence in animSequences)
            {
                TreeNode animnode = new TreeNode(sequence.name);
                objectNode.Nodes.Add(animnode);
                animnode.Tag = count; //store sequence # for lookup
                count++;
            }
        }

        private void setActivePackageNode(TreeNode node)
        {
            if (selPackageNode != null)
            {
                selPackageNode.ForeColor = Color.Black;
            }

            node.ForeColor = Color.Blue;
            selPackageNode = node;
        }

        private void importGSAToolStripButton1_Click(object sender, EventArgs e)
        {
            if (selectedPackage == null)
            {
                MessageBox.Show("You must select a package first");
                return;
            }

            //First open a Package dialog....
            DialogResult res = gsaOpenFileDialog1.ShowDialog();

            if (res == DialogResult.OK)
            {
                if (gsaOpenFileDialog1.FilterIndex == 1)
                {
                    // FBX selected...
                    List<String> stringNames = new List<string>();
                    List<ObjectBase> sceneObjects = new List<ObjectBase>();

                    GodzUtil.ImportFBXMesh(gsaOpenFileDialog1.FileName, selectedPackage, stringNames, sceneObjects);

                    //add strings to the database
                    foreach (String text in stringNames)
                    {
                        uint hash = GodzUtil.GetHashCode(text);
                        Editor.AddHash(hash, text, false);
                    }

                    foreach (ObjectBase sceneObject in sceneObjects)
                    {
                        if (sceneObject is Mesh)
                        {
                            AddObjectToTree(sceneObject, selPackageNode);
                            mMainForm.onNewObjectAdded(sceneObject);
                        }
                    }
                }
                else
                {
                    List<String> stringNames = new List<string>();
                    Mesh m = GodzUtil.ImportMesh(gsaOpenFileDialog1.FileName, selectedPackage, stringNames);
                    if (m != null)
                    {
                        m.setPackage(selectedPackage);

                        //Add Mesh to the Database
                        //Editor.AddEntity(selectedPackage.GetName(), m);

                        //add strings to the database
                        foreach (String text in stringNames)
                        {
                            uint hash = GodzUtil.GetHashCode(text);
                            Editor.AddHash(hash, text, false);
                        }

                        AddObjectToTree(m, selPackageNode);
                        mMainForm.onNewObjectAdded(m);
                    }
                }
            }
        }

        private void importAnimToolStripButton1_Click(object sender, EventArgs e)
        {
            if (selectedMesh != null)
            {
                DialogResult res = gsaOpenFileDialog1.ShowDialog();

                if (res == DialogResult.OK)
                {
                    String file = gsaOpenFileDialog1.FileName;
                    List<String> stringNames = new List<string>();
                    if (GodzUtil.AppendGSA(file, selectedMesh, stringNames))
                    {
                        //add strings to the database
                        foreach (String text in stringNames)
                        {
                            uint hash = GodzUtil.GetHashCode(text);
                            Editor.AddHash(hash, text, false);
                        }

                        //find the mesh node and repopulate...
                        TreeNode node = meshMap[selectedMesh] as TreeNode;
                        if (node != null)
                        {
                            TreeNode parent = node.Parent;
                            animationsTreeView.Nodes.Remove(node);
                            meshMap.Remove(selectedMesh);
                            AddObjectToTree(selectedMesh, parent);
                        }
                    }
                }
            }
        }

        private void gsaOpenFileDialog1_FileOk(object sender, CancelEventArgs e)
        {
            System.Windows.Forms.OpenFileDialog dlg = (OpenFileDialog)sender;
        }

        private void newPackageToolStripButton1_Click(object sender, EventArgs e)
        {

        }

        private void animationsTreeView_NodeMouseClick(object sender, TreeNodeMouseClickEventArgs e)
        {
            TabPanelData data = mMainForm.getTabPanel();

            Object tag = e.Node.Tag;
            mActiveNode = e.Node;
            if (tag != null)
            {
                if (tag is Mesh)
                {
                    if (previewActor == null)
                    {
                        previewActor = data.mActiveLayer.spawnActor("WEntity", ref zero, ref zero);
                        previewActor.setName("PreviewActor");
                    }

                    Mesh mesh = (Mesh)tag;
                    selectedMesh = mesh;
                    selectedPackage = (Package)e.Node.Parent.Tag;
                    setActivePackageNode(e.Node.Parent);
                    previewActor.setMesh(mesh);
                    tabControl1.TabPages[0].Text = e.Node.Text;

                    data.mPrimaryCamera.moveToActor(previewActor);
                    //Vector3 lookAtPoint = previewActor.getLocation();
                    //data.mPrimaryCamera.setLookAtPoint(ref lookAtPoint);

                    Object proxy = Editor.GetNewObjectProxy(mesh);
                    propertyGrid1.SelectedObject = proxy;

                    CollisionType collType = mesh.getCollisionType();
                    collisionTypeComboBox1.SelectedIndex = (int)collType;

                    setupMaterials();
                } //tag mesh
                else if (tag is Package)
                {
                    selectedPackage = (Package)tag;
                    selectedMesh = null;
                    setActivePackageNode(e.Node);
                }
                else if ((tag is UInt32) && previewActor != null)
                {
                    //Animation selected...
                    UInt32 id = (UInt32)tag;
                    previewActor.freeze(false);
                    previewActor.playAnimation(id);
                    selectedAnimationIndex = id;
                    int noneIndex = 0;
                    bool hasFound = false;

                    //Select our entry from the category
                    UInt32 categoryHash = previewActor.getAnimationCategory(id);
                    for (int i = 0; i < categoryComboBox.Items.Count; i++)
                    {
                        string text = (string)categoryComboBox.Items[i];

                        if (text.Equals("None"))
                        {
                            noneIndex = i;
                        }

                        uint phash = GodzUtil.GetHashCode(text);
                        if (phash == categoryHash)
                        {
                            categoryComboBox.SelectedIndex = i;
                            hasFound = true;
                            break;
                        }
                    }

                    if (!hasFound)
                    {
                        categoryComboBox.SelectedIndex = noneIndex;
                    }

                    //set active package
                    TreeNode temp = e.Node.Parent;
                    while (temp != null)
                    {
                        if (temp.Tag is Package)
                        {
                            selectedPackage = (Package)temp.Tag;
                            setActivePackageNode(temp);
                            break;
                        }

                        temp = temp.Parent;
                    }
                }

                GodzUtil.RunMainPass();
            }
        }

        private void propertyGrid1_PropertyValueChanged(object s, PropertyValueChangedEventArgs e)
        {
            if (previewActor != null)
            {
                previewActor.OnPropertiesUpdated();
            }
        }

        private void collisionTypeComboBox1_SelectedIndexChanged(object sender, EventArgs e)
        {
            //I don't like using this event because whenever we change the combo from code, this event fires again....
        }

        private void collisionTypeComboBox1_SelectionChangeCommitted(object sender, EventArgs e)
        {
            if (selectedMesh != null)
            {
                int opt = collisionTypeComboBox1.SelectedIndex;
                CollisionType ctype = (CollisionType)opt;
                selectedMesh.setCollisionType(ctype);
            }
        }

        private void selectMaterialButton_Click(object sender, EventArgs e)
        {
            if (selectedMesh != null && materials != null)
            {
                int index = materialsComboBox.SelectedIndex;

                if (index > -1)
                {
                    //hopefully its the correct materials index.....
                    selectedMesh.setVisible(materials[index]);

                    //resubmit the mesh since visibility changed (NOTE: need better way to submit visibility changes)
                    previewActor.setMesh(selectedMesh);
                }
            }
        }

        private void showAllMaterialsButton_Click(object sender, EventArgs e)
        {
            if (selectedMesh != null)
            {
                selectedMesh.setVisible(true);

                //resubmit the mesh since visibility changed (NOTE: need better way to submit visibility changes)
                previewActor.setMesh(selectedMesh);
            }
        }

        private void categoryButton_Click(object sender, EventArgs e)
        {
            string text = categoryComboBox.Text;
            uint hash = GodzUtil.GetHashCode(text);
            previewActor.setAnimationCategory(selectedAnimationIndex, hash);
        }

        private void removeNodeToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if (mActiveNode != null)
            {
                if (mActiveNode.Tag is Mesh)
                {
                    Mesh m = (Mesh)mActiveNode.Tag;
                    Package p = m.getPackage();

                    //should drop ref on the name as well...
                    Editor.RemoveHash(m.getObjectName());

                    //also remove the default instance....
                    MeshInstance defaultMeshInstance = m.getDefaultInstance();
                    if (defaultMeshInstance != null)
                    {
                        p.RemoveObject(defaultMeshInstance);
                    }

                    //drop the selected material from the package
                    p.RemoveObject(m);

                    //Remove the Mesh from the database....
                    //Editor.RemoveEntity(p.GetName(), m);

                    animationsTreeView.Nodes.Remove(mActiveNode);
                    mActiveNode = null;
                    selectedMesh = null;
                }
            }
        }

        private void stopbutton_Click(object sender, EventArgs e)
        {
            if (previewActor != null)
            {
                previewActor.freeze(true);
            }
        }

        private void advanceButton_Click(object sender, EventArgs e)
        {
            if (previewActor != null)
            {
                previewActor.advanceAnimation(0.1f);
            }
        }

        private void reverseButton_Click(object sender, EventArgs e)
        {
            if (previewActor != null)
            {
                previewActor.advanceAnimation(-0.1f);
            }
        }

        private void trackBar1_Scroll(object sender, EventArgs e)
        {
            if (previewActor != null)
            {
                previewActor.freeze(true);
                float value = trackBar1.Value;
                float t = value / (float)trackBar1.Maximum;
                previewActor.setTime(t);
            }
        }

        private void submitToolStripMenuItem_Click(object sender, EventArgs e)
        {
            //submit the selected mesh
            //Editor.UpdateEntity(selectedMesh.getPackage().GetName(), selectedMesh);
        }
    }
}
