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
    public partial class MaterialBrowserForm : Form, TabPanelListener
    {
        MainForm1 mMainForm;
        public static GodzGlue.Vector3 zero = new GodzGlue.Vector3(0, 0, 0);
        List<Package> packages = new List<Package>();
        List<Material> materials = new List<Material>();
        List<ShaderInfo> shaders = null;
        Material mMaterial = null;
        TreeNode mActiveNode = null;
        TreeNode mPackageNode = null;
        public Hashtable mMap = new Hashtable();
        Package selectedPackage = null;

        public MaterialBrowserForm()
        {
            InitializeComponent();
        }

        void TabPanelListener.FormClosing()
        {
        }

        void TabPanelListener.OnGainedFocus()
        {
            // Turn off deferred rendering so user can see their skins
            GodzUtil.EnableDeferredRendering(false);
        }

        void TabPanelListener.OnLostFocus()
        {
            //remove all of our skins from the display
            GodzUtil.ClearTextures();

            GodzUtil.EnableDeferredRendering(true);
        }

        void TabPanelListener.OnTick()
        {
        }

        void TabPanelListener.OnKeyDown(object sender, KeyEventArgs e)
        {
        }

        void TabPanelListener.PackageLoaded(GodzGlue.Package package)
        {
            AddPackage(package);
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

                String packagePath = GodzUtil.getFullPathToPackage(pname, ResourceType.ResourceType_Texture );
                GodzUtil.savePackage(packagePath, selectedPackage);
            }
            else
            {
                MessageBox.Show("Could not save; unknown package");
            }
        }

        void TabPanelListener.OnObjectAdded(GodzGlue.ObjectBase obj)
        {
            if (obj is Mesh)
            {
                Mesh m = (Mesh)obj;

                //see if the materials this Mesh has was already added. if not, add it...
                List<Material> mats = new List<Material>();
                m.getMaterials(mats);

                foreach (Material mat in mats)
                {
                    bool foundPackage = false;

                    //do we have the package?
                    foreach (Package p in packages)
                    {
                        if (mat.getPackage().Equals(p))
                        {
                            //found!
                            foundPackage = true;
                            break;
                        }
                    }

                    if (!foundPackage)
                    {
                        //add the package...
                        AddPackage(mat.getPackage());
                    }
                    else
                    {
                        //we already have the package but maybe not this material...
                        Material foundMat = materials.Find(
                            delegate(Material bk)
                            {
                                return bk == mat;
                            }
                        );

                        if (foundMat == null)
                        {
                            //need to add this material to the tree
                            object treeobj = mMap[mat.getPackage().GetName()];
                            if (treeobj != null)
                            {
                                TreeNode t = (TreeNode)treeobj;
                                AddMaterialToTree(t, mat, 0);
                            }
                        }
                    }
                }
            }
        }

        public void setTabData(TabPanelData data, MainForm1 mainForm)
        {
            mMainForm = mainForm;

            data.mViewportId = pictureBox1.Handle;
            buildMaterialsTree();
        }

        public void setViewport()
        {
            GodzUtil.AddViewport(pictureBox1.Handle);

            TabPanelData data = mMainForm.getTabPanel();
            data.mActiveLayer = data.mWorld.addLayer("Default");
            data.mPrimaryCamera = data.mActiveLayer.spawnCamera(ref zero, ref zero);
            data.mPrimaryCamera.GrabFocus(); //make this camera primary            
        }

        private void buildMaterialsTree()
        {
            //build the Objects tab -- add all packages, models, then animations under the tree
            GodzUtil.getPackages(packages);

            int numPkgs = packages.Count;
            for (int i = 0; i < numPkgs; i++)
            {
                Package p = packages[i];
                AddPackage(p);
            }

            treeView1.ExpandAll();
        }

        private void AddMaterialToTree(TreeNode parentnode, Material m, int matcount)
        {
            materials.Add(m);

            if (m.Id == Guid.Empty)
            {
                m.Id = Guid.NewGuid();
            }

            String objName;
            uint nameHash = m.getObjectName();
            objName = Editor.GetHashString(nameHash);

            if (objName == null)
            {
                objName = "Material #" + matcount;
            }

            System.Windows.Forms.TreeNode objectNode = new TreeNode(objName);
            parentnode.Nodes.Add(objectNode);
            objectNode.Tag = m;
            objectNode.ContextMenuStrip = materialNodeContextMenuStrip1;
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
                mMap[nameHash] = packageNode;
                treeView1.Nodes.Add(packageNode);
                packageNode.Tag = p;
                packageNode.ContextMenuStrip = materialContextMenuStrip;

                int matcount = 0;

                //Add all the materials
                UInt32 numObjs = p.GetNumObjects();
                for (UInt32 j = 0; j < numObjs; j++)
                {
                    ObjectBase obj = p.GetObjectAt(j);
                    if (obj is Material)
                    {
                        Material m = (Material)obj;
                        AddMaterialToTree(packageNode, m, matcount);

                        matcount++;
                    }
                }

                packageNode.ExpandAll();

                return packageNode;
            }

            return null;
        }

        private void treeView1_NodeMouseClick(object sender, TreeNodeMouseClickEventArgs e)
        {
            Object tag = e.Node.Tag;
            if (tag != null)
            {
                if ( (tag is Material) && e.Button == MouseButtons.Left)
                {
                    mMaterial = (Material)tag;

                    if (mPackageNode != null)
                    {
                        mPackageNode.ForeColor = Color.Black;
                    }

                    mActiveNode = e.Node;

                    //highlight this node's package!
                    e.Node.Parent.ForeColor = Color.Blue;
                    selectedPackage = (Package)e.Node.Parent.Tag;
                    mPackageNode = e.Node.Parent;

                    GodzUtil.ClearTextures();
                    GodzUtil.DisplayTexture(mMaterial, pictureBox1.Width, pictureBox1.Height);

                    Object proxy = Editor.GetNewObjectProxy(mMaterial);
                    propertyGrid1.SelectedObject = proxy;

                    List<MaterialParameter> parameters = new List<MaterialParameter>();
                    mMaterial.getParameters(parameters);

                    parameterComboBox1.Text = "";
                    parameterComboBox1.Items.Clear();

                    foreach (MaterialParameter mp in parameters)
                    {
                        String objName = Editor.GetHashString(mp.getName());
                        if (objName != null)
                        {
                            parameterComboBox1.Items.Add(objName);
                        }
                        else
                        {
                            parameterComboBox1.Items.Add("<Unknown>");
                        }
                    }

                    UInt32 shaderhash = mMaterial.getShaderName();

                    //shaders
                    shaders = new List<ShaderInfo>();
                    GodzUtil.GetShaders(shaders);

                    shaderComboBox1.Items.Clear();
                    foreach (ShaderInfo sinfo in shaders)
                    {
                        shaderComboBox1.Items.Add(sinfo);

                        if (sinfo.id == shaderhash)
                        {
                            shaderComboBox1.SelectedItem = sinfo;
                        }
                    }
                }
                else if (tag is Package)
                {
                    selectedPackage = (Package)tag;

                    if (mPackageNode != null)
                    {
                        mPackageNode.ForeColor = Color.Black;
                    }

                    mPackageNode = e.Node;
                    mPackageNode.ForeColor = Color.Blue;

                    GodzUtil.ClearTextures();
                    GodzUtil.DisplayTextures(selectedPackage, pictureBox1.Width, pictureBox1.Height);
                }
            }
        }

        private void parameterComboBox1_TextUpdate(object sender, EventArgs e)
        {

        }

        private void parameterComboBox1_TextChanged(object sender, EventArgs e)
        {

        }

        private void parameterComboBox1_SelectedValueChanged(object sender, EventArgs e)
        {

        }

        private void parameterComboBox1_KeyDown(object sender, KeyEventArgs e)
        {
        }

        private void parameterComboBox1_SelectedIndexChanged(object sender, EventArgs e)
        {
            //get the parameter type
            String text = parameterComboBox1.Text;
            uint paramName = GodzUtil.GetHashCode(text);
            MaterialParameter mp = mMaterial.getParameter(paramName);
            if (mp != null)
            {
                parameterTextBox.Text = "";

                //select the type
                MaterialParameterType ty = mp.getType();
                if (ty == MaterialParameterType.MaterialParameter_Texture)
                {
                    parameterTypeComboBox.SelectedIndex = 0;
                }
                else
                {
                    parameterTypeComboBox.SelectedIndex = 1;

                    //set float value....
                    String text2 = "";
                    uint num = mp.getNumFloats();
                    for (uint i = 0; i < num; i++)
                    {
                        text2 += mp.getFloat(i).ToString();
                        if (i < num - 1)
                        {
                            text2 += ",";
                        }
                    }

                    parameterTextBox.Text = text2;
                }
            }
        }

        private void pickTextureButton_Click(object sender, EventArgs e)
        {
            //get the parameter type
            String text = parameterComboBox1.Text;
            uint paramName = GodzUtil.GetHashCode(text);
            MaterialParameter mp = mMaterial.getParameter(paramName);
            if (mp != null)
            {
                MaterialParameterType ty = mp.getType();
                if (ty == MaterialParameterType.MaterialParameter_Texture)
                {
                    textureOpenFileDialog.ShowDialog();
                }
                else
                {
                    mp.clearValues();
                    String value = parameterTextBox.Text;

                    // Split string on comma - tokenize
                    string[] words = value.Split(',');
                    foreach (string word in words)
                    {
                        float v = float.Parse(word);
                        mp.addValue(v);
                    }

                    mp.submitValues();
                }
            }
        }

        private void textureOpenFileDialog_FileOk(object sender, CancelEventArgs e)
        {
            System.Windows.Forms.OpenFileDialog dlg = (OpenFileDialog)sender;

            String filename = dlg.FileName;
            String text = parameterComboBox1.Text;
            uint paramName = GodzUtil.GetHashCode(text);
            MaterialParameter mp = mMaterial.getParameter(paramName);
            if (mp == null)
            {
                MessageBox.Show("Could not find the parameter!");
                return;
            }

            //next, import the texture
            bool isLoaded = mp.loadTexture(mMaterial.getPackage() , filename);
            if (!isLoaded)
            {
                MessageBox.Show("Could not load the texture!");
                return;
            }

            //populate the textbox
            parameterTextBox.Text = filename;
        }

        private void addParamButton_Click(object sender, EventArgs e)
        {
            AddParameterForm dlg = new AddParameterForm();
            if (dlg.ShowDialog() == DialogResult.OK)
            {
                parameterComboBox1.Items.Add(dlg.parameterName);
                parameterComboBox1.SelectedIndex = parameterComboBox1.Items.Count - 1;

                //add a new parameter
                String text = parameterComboBox1.Text;

                //get hash code....
                uint hash = GodzUtil.GetHashCode(text);
                Editor.AddHash(hash, text, false);

                //TODO: get the type & add the proper paramType.... For now we know its a texture....
                if (parameterComboBox1.SelectedIndex == 0)
                {
                    mMaterial.addTextureParameter(hash);
                }
                else
                {
                    mMaterial.addFloatParameter(hash);
                }
            }
        }

        private void importToolStripMenuItem_Click_1(object sender, EventArgs e)
        {
            if (importOpenFileDialog.ShowDialog() == DialogResult.OK)
            {
                Material m = GodzUtil.ImportMaterial(importOpenFileDialog.FileName, selectedPackage);
                Package p = m.getPackage();
                object obj = mMap[p.GetName()];
                if (obj != null)
                {
                    TreeNode tn = (TreeNode)obj;

                    int matcount = 0;
                    uint num = p.GetNumObjects();
                    for (uint i = 0; i < num; i++)
                    {
                        if (p.GetObjectAt(i) is Material)
                        {
                            matcount++;
                        }
                    }

                    AddMaterialToTree(tn, m, matcount);
                }
            }
        }

        private void updateButton1_Click(object sender, EventArgs e)
        {
            //set the shader for this material
            if (mMaterial != null)
            {
                Object obj = shaderComboBox1.SelectedItem;
                ShaderInfo sinfo = (ShaderInfo)obj;
                bool wasSet = mMaterial.setShader(sinfo.id);
                if (!wasSet)
                {
                    MessageBox.Show("Shader was not set on the Material...");
                }
            }
        }

        private void removeParameterButton_Click(object sender, EventArgs e)
        {
            String text = parameterComboBox1.Text;
            uint paramName = GodzUtil.GetHashCode(text);
            if (mMaterial != null)
            {
                mMaterial.removeParameter(paramName);

                //reset
                parameterComboBox1.Items.RemoveAt(parameterComboBox1.SelectedIndex);
            }
        }

        private void propertyGrid1_PropertyValueChanged(object s, PropertyValueChangedEventArgs e)
        {
        }

        private void removeNodeToolStripMenuItem_Click(object sender, EventArgs e)
        {
            Package p = mMaterial.getPackage();

            //should drop ref on the name as well...
            Editor.RemoveHash(mMaterial.getObjectName());

            materials.Remove(mMaterial);

            //drop the selected material from the package
            p.RemoveObject(mMaterial);

            treeView1.Nodes.Remove(mActiveNode);
            mMaterial = null;
            mActiveNode = null;
        }
    }
}
