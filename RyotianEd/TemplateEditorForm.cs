using System;
using System.Collections;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Diagnostics;
using GodzGlue;


namespace RyotianEd
{
    public partial class TemplateEditorForm : Form, TabPanelListener
    {
        public Hashtable mPackageMap = new Hashtable();
        public Hashtable objectMap = new Hashtable();

        ObjectBase mTemplate = null;
        TreeNode currentNode = null;

        Package mSelectedPackage = null;
        TreeNode mPackageNode = null;
        TreeNode mTemplateNode = null;
        PropertyGridValue mValue = null;

        class PackageNode
        {
            public TreeNode node;
            public PackageInfo info;
        }

        public TemplateEditorForm()
        {
            InitializeComponent();

            buildTree();
        }

        void reset()
        {
            //rebuild the package tree
            treeView1.Nodes.Clear();

            mPackageMap.Clear();
            mTemplate = null;
            mSelectedPackage = null;
            mPackageNode = null;
            mTemplateNode = null;
            mValue = null;
            currentNode = null;

            buildTree();
        }

        void TabPanelListener.OnTick()
        {
        }

        void TabPanelListener.FormClosing()
        {
            //TODO: Check with user to verify if they want to save changes...
        }

        void TabPanelListener.PackageLoaded(GodzGlue.Package package)
        {
        }

        void TabPanelListener.OnKeyDown(object sender, KeyEventArgs e)
        {
        }

        void TabPanelListener.OnSave()
        {
            if (mSelectedPackage != null)
            {
                UInt32 hash = mSelectedPackage.GetName();
                String pname = Editor.GetHashString(hash);
                if (pname == null)
                {
                    //TODO: add a path that allows them to enter a new package name?
                    MessageBox.Show("Cannot find this package in the database! Cannot SAVE...");
                    return;
                }

                String packagePath = GodzUtil.getFullPathToPackage(pname, ResourceType.ResourceType_Template);
                GodzUtil.savePackage(packagePath, mSelectedPackage);
            }
        }

        void TabPanelListener.OnLostFocus()
        {
        }

        void TabPanelListener.OnGainedFocus()
        {
            //rebuild the tree and all the objects. This way if a ClassDescription changes, it
            //will show up
            reset();
        }

        void TabPanelListener.OnObjectAdded(GodzGlue.ObjectBase obj)
        {
        }

        private void saveCurrentPackage()
        {
            if (mSelectedPackage != null)
            {
                UInt32 hash = mSelectedPackage.GetName();
                String pname = Editor.GetHashString(hash);
                if (pname == null)
                {
                    //TODO: add a path that allows them to enter a new package name?
                    MessageBox.Show("Cannot find this package in the database! Cannot SAVE...");
                    return;
                }

                Editor.ExportPackage(hash, ResourceType.ResourceType_Template);
            }
            else
            {
                MessageBox.Show("Could not save; unknown package");
            }
        }

        private void buildTree()
        {
            //reset
            treeView1.Nodes.Clear();

            System.Windows.Forms.TreeNode packageRoot = new TreeNode("Packages");
            treeView1.Nodes.Add(packageRoot);

            //retrieve all objects from DB
            List<DatabaseObject> list = new List<DatabaseObject>();
            Editor.LoadObjects(ref list, 0);

            //get all packages...
            List<PackageInfo> packs = new List<PackageInfo>();
            Editor.GetPackages(ref packs);

            foreach (PackageInfo p in packs)
            {
                AddPackage(p);
            }


            foreach (DatabaseObject data in list)
            {
                ClassBase godzClass = ClassBase.findClass(data.classhash);
                if (godzClass != null && godzClass.IsA("ObjectTemplate"))
                {
                    //see if we already added the package
                    Object temp = mPackageMap[data.packageHash];

                    if (temp != null)
                    {
                        PackageNode pnode = (PackageNode)temp;
                        TreeNode parent_node = pnode.node;
                        AddTemplateToTree(parent_node, data);
                    }
                }
            }

            treeView1.ExpandAll();
        }

        private void AddTemplateToTree(TreeNode packageNode, ObjectBase obj)
        {
            String objName;
            uint objnameHash = obj.getObjectName();
            objName = Editor.GetHashString(objnameHash);


            if (objName == null)
            {
                objName = obj.getClass().ClassName;
            }

            System.Windows.Forms.TreeNode objectNode = new TreeNode(objName);
            packageNode.Nodes.Add(objectNode);
            objectNode.Tag = obj;
            objectNode.ContextMenuStrip = templateNodeContextMenuStrip1;
            objectMap[obj] = objectNode;

            //mTemplateNode = objectNode;

            //Add on properties....
            addSubProperties(obj, objectNode);
        }

        private void AddTemplateToTree(TreeNode packageNode, DatabaseObject obj)
        {
            String objName = Editor.GetPropertyFromXML("Name", obj.xmlData); //obj.getObjectName();
            

            if (objName == null)
            {
                ClassBase gc = ClassBase.findClass(obj.classhash);
                objName = gc.ClassName;
            }

            System.Windows.Forms.TreeNode objectNode = new TreeNode(objName);
            packageNode.Nodes.Add(objectNode);
            objectNode.Tag = obj;
            objectNode.ContextMenuStrip = blankTemplateContextMenuStrip1;
            //objectMap[obj] = objectNode;

            //mTemplateNode = objectNode;
        }

        private void addSubObject(ObjectBase objbase, TreeNode parentnode)
        {
            uint hash = objbase.getObjectName();
            string text;

            if (hash == 0)
            {
                ClassBase godzClass = objbase.getClass();
                text = godzClass.ClassName;
            }
            else
                text = Editor.GetHashString(hash);

            System.Windows.Forms.TreeNode subobjnode = new TreeNode(text);
            subobjnode.Tag = objbase;
            objectMap[objbase] = subobjnode;
            
            parentnode.Nodes.Add(subobjnode);

            addSubProperties(objbase, subobjnode);
        }

        private void addSubProperties(ObjectBase obj, TreeNode node)
        {
            List<ClassPropertyInfo> cpList = new List<ClassPropertyInfo>();
            obj.getPropertyValues(cpList);

            foreach (ClassPropertyInfo cp in cpList)
            {
                //check to see if the property had a container...
                PropertyArray container = cp.mArray;
                if (container != null && container.getElementType() == ClassPropertyType.PropertyType_Object)
                {
                    System.Windows.Forms.TreeNode prop = new TreeNode(cp.mName);

                    PropertyClassSelection propSelection = new PropertyClassSelection();
                    propSelection.cp = cp;
                    propSelection.obj = obj;
                    propSelection.treeNode = prop;
                    propSelection.parentNode = node;
                    prop.Tag = propSelection;
                    node.Nodes.Add(prop);

                    //tell treenode to allow new elements to be added
                    prop.ContextMenuStrip = objectListContextMenuStrip;

                    uint len = container.size();
                    for (uint i = 0; i < len; i++)
                    {
                        PropertyValue pv = container[i];
                        addSubObject(pv.getObject(), prop);
                    }
                }
            }
        }

        private TreeNode AddPackage(PackageInfo p)
        {
            UInt32 nameHash = p.hash;

            //make sure we don't already have the package listed...
            if (mPackageMap[nameHash] == null && p.type == PackageType.PackageType_RESOURCE)
            {
                String pname = Editor.GetHashString(nameHash);
                if (pname == null)
                {
                    pname = "<Package>";
                }

                System.Windows.Forms.TreeNode packageTreeNode = new TreeNode(pname);
                
                PackageNode node = new PackageNode();
                node.node = packageTreeNode;
                node.info = p;
                mPackageMap[nameHash] = node;

                treeView1.Nodes[0].Nodes.Add(packageTreeNode);
                packageTreeNode.Tag = p;
                packageTreeNode.ContextMenuStrip = templateContextMenuStrip1;

                // Add all templates contained within package
                packageTreeNode.ExpandAll();

                return packageTreeNode;
            }

            return null;
        }

        private void pictureBox1_Click(object sender, EventArgs e)
        {

        }

        private void addTemplateToolStripMenuItem_Click(object sender, EventArgs e)
        {
            // Ask the user to select the type of Template
            ClassBase godzClass = ClassBase.findClass("ObjectTemplate");
            ClassSelectionForm form = new ClassSelectionForm(godzClass);
            if (form.ShowDialog() == DialogResult.OK)
            {
                //open up the template editor for this type...
                mTemplate = form.mSelectedType.newInstance();
                mTemplate.setPackage(mSelectedPackage);

                //Assign this object a package...
                mTemplate.setPackage(mSelectedPackage);

                object proxy = Editor.GetNewObjectProxy(mTemplate);
                propertyGrid1.SelectedObject = proxy;

                AddTemplateToTree(mPackageNode, mTemplate);

                Editor.AddEntity(mSelectedPackage.GetName(), mTemplate);
            }
        }

        private void treeView1_NodeMouseClick(object sender, TreeNodeMouseClickEventArgs e)
        {
            currentNode = e.Node;
            if (e.Node.Tag is PackageInfo)
            {
                mPackageNode = e.Node;
                PackageInfo pinfo = (PackageInfo)e.Node.Tag;

                // Find the package...
                Package p = GodzUtil.FindPackage(pinfo.hash);
                if (p != null)
                {
                    mSelectedPackage = p;
                }
                else
                {
                    // Create the package. 
                    // Note/Bug: if the pack exists somewhere on the
                    // hard drive and the user later loads that one
                    // it will lose this object.
                    mSelectedPackage = GodzUtil.AddPackage(pinfo.name, pinfo.type);
                }

                mTemplate = null;
                mValue = null;
                propertyGrid1.SelectedObject = null;
            }
            else if (e.Node.Tag is ObjectBase)
            {
                // Stores context information
                PropertyContextData contextData = null;
                ClassBase templateClass = ClassBase.findClass("ObjectTemplate");

                ObjectBase obj = (ObjectBase)e.Node.Tag;
                if (obj.IsA(templateClass))
                {
                    mTemplate = (ObjectBase)e.Node.Tag;
                    mTemplateNode = e.Node;

                    mSelectedPackage = mTemplate.getPackage();
                }
                else
                {
                    mSelectedPackage = obj.getPackage();

                    //Set the 'Template' parent object as the context. This way
                    //the property knows about it's parent Object
                    TreeNode tnode = e.Node.Parent;
                    while (tnode != null)
                    {
                        Object temp = tnode.Tag;
                        if (temp is ObjectBase)
                        {
                            ObjectBase tempBase = (ObjectBase)temp;
                            if (tempBase.IsA(templateClass))
                            {
                                contextData = new PropertyContextData();
                                contextData.context1 = tempBase;
                                break;
                            }
                        }

                        tnode = tnode.Parent;
                    }
                }

                //Debug.Assert(mSelectedPackage != null);
                object proxy = Editor.GetNewObjectProxy(obj);
                ObjectBaseProxy objProxy = (ObjectBaseProxy)proxy;
                objProxy.contextData = contextData;
                propertyGrid1.SelectedObject = proxy;

                mValue = null;
            }
            else if (e.Node.Tag is PropertyClassSelection)
            {
                PropertyClassSelection propertySelection = (PropertyClassSelection)e.Node.Tag;
                mSelectedPackage = propertySelection.obj.getPackage();
                mValue = new PropertyGridValue();

                //refresh the property
                propertySelection.cp = propertySelection.obj.getPropertyValue(propertySelection.cp.mName);

                if (propertySelection.cp.mPropertyType == ClassPropertyType.PropertyType_HashCode)
                {
                    mValue.value = Editor.GetHashString(propertySelection.cp.mObjectHash);
                }
                else
                    mValue.value = propertySelection.cp.mText;

                mValue.mPropertySelection = propertySelection;
                propertyGrid1.SelectedObject = mValue;
            }
        }

        private void propertyGrid1_PropertyValueChanged(object s, PropertyValueChangedEventArgs e)
        {
            if (mValue != null)
            {
                string text = (string)e.ChangedItem.Value;
                Editor.OnClassPropertyChanged(mValue.mPropertySelection.obj, mValue.mPropertySelection.cp, text, e);

                //get latest value
                mValue.mPropertySelection.cp = mValue.mPropertySelection.obj.getPropertyValue(mValue.mPropertySelection.cp.mName);

                if (mValue.mPropertySelection.cp.mName.Equals("Name"))
                    mValue.mPropertySelection.parentNode.Text = text;
            }
            else if (mTemplate != null)
            {
                //Editor.HandlePropertyChanged(mTemplate, e);

                //update template node name just incase....
                uint hash = mTemplate.getObjectName();
                string text = Editor.GetHashString(hash);
                if (text != null)
                {
                    mTemplateNode.Text = text;
                }
            }
            else if (currentNode != null && currentNode.Tag != null && currentNode.Tag is ObjectBase)
            {
                ObjectBase obj = (ObjectBase)currentNode.Tag;
                ClassPropertyInfo cp = obj.getPropertyValue("Name");
                if (cp != null)
                {
                    TreeNode treenode = (TreeNode)objectMap[obj];
                    treenode.Text = Editor.GetHashString(cp.mObjectHash);
                }
            }
        }

        private void saveToolStripMenuItem_Click(object sender, EventArgs e)
        {
            //TODO: get all the templates owned by this package and submit them -OR-
            //at least display a warning all local changes should be submit before EXPORT
            saveCurrentPackage();
        }

        private void refreshToolStripMenuItem_Click(object sender, EventArgs e)
        {
            UpdateResult sqlResult = Editor.RefreshEntity(mTemplate, mSelectedPackage.GetName());

            if (!sqlResult.result)
            {
                MessageBox.Show(sqlResult.error);
            }
            else
            {
                //refresh the display incase they pulled down something different...
                object proxy = Editor.GetNewObjectProxy(mTemplate);
                propertyGrid1.SelectedObject = proxy;
            }
        }

        private bool submit()
        {
            UpdateResult sqlresult;

            if (mValue != null)
            {
                sqlresult = Editor.UpdateEntity(mValue.mPropertySelection.obj.getPackage().GetName(), mValue.mPropertySelection.obj);
            }
            else if (mTemplate != null)
                sqlresult = Editor.UpdateEntity(mSelectedPackage.GetName(), mTemplate);
            else
                return true;

            if (!sqlresult.result)
            {
                //failed to submit changes....
                MessageBox.Show(sqlresult.error);
            }

            return sqlresult.result;
        }

        private void submitToolStripMenuItem_Click(object sender, EventArgs e)
        {
            submit();
        }

        private void removeToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if ( mValue != null && Editor.RemoveEntity(mSelectedPackage.GetName(), mValue.mPropertySelection.obj) )
            {
                treeView1.Nodes.Remove(mValue.mPropertySelection.treeNode);
            }
            else if (Editor.RemoveEntity(mSelectedPackage.GetName(), mTemplate))
            {
                //drop template from the tree.....
                treeView1.Nodes.Remove(mTemplateNode);
            }
        }

        private void refreshToolStripMenuItem1_Click(object sender, EventArgs e)
        {
            reset();
        }

        // PropertyArray list --> Add
        private void addToolStripMenuItem_Click(object sender, EventArgs e)
        {
            ClassBase godzClass = mValue.mPropertySelection.cp.mClassType;

            //check to see if there are any subclasses....
            List<ClassBase> list = new List<ClassBase>();
            godzClass.getSubClasses(list);

            ObjectBase newObj = null;

            if (list.Count == 0)
            {
                // Make sure its not abstract before we instance it.... 
                // Otherwise, just do nothing
                if (!godzClass.isAbstract())
                {
                    newObj = godzClass.newInstance();
                    newObj.setPackage(mSelectedPackage);
                }
            }
            else
            {
                ClassSelectionForm form = new ClassSelectionForm(godzClass);
                if (form.ShowDialog() == DialogResult.OK)
                {
                    newObj = form.mSelectedType.newInstance();
                    newObj.setPackage(mSelectedPackage);
                }
            }

            if (newObj != null)
            {
                PropertyArray vector = mValue.mPropertySelection.cp.mArray;
                PropertyObject po = new PropertyObject(newObj);
                vector.add(po);

                Editor.AddObjectReference(newObj.getPackage().GetName(), mValue.mPropertySelection.cp.mPropertyNameHash, newObj, mValue.mPropertySelection.obj);

                //Now add the node to the tree....
                addSubObject(newObj, mValue.mPropertySelection.treeNode);
            }
        }

        // PropertyArray list --> Remove
        private void removeToolStripMenuItem1_Click(object sender, EventArgs e)
        {
            ObjectBase obj = mValue.mPropertySelection.obj;

            PropertyArray vector = mValue.mPropertySelection.cp.mArray;
            PropertyObject po = new PropertyObject(obj);
            vector.remove(po);
        }

        private void loadToolStripMenuItem_Click(object sender, EventArgs e)
        {
            DatabaseObject data = (DatabaseObject)currentNode.Tag;
            ObjectBase obj = data.newInstance();

            currentNode.Tag = obj;
            currentNode.ContextMenuStrip = templateNodeContextMenuStrip1;
            objectMap[obj] = currentNode;

            mTemplateNode = currentNode;

            //Add on properties....
            addSubProperties(obj, currentNode);

            currentNode.ExpandAll();
        }
    }
}
