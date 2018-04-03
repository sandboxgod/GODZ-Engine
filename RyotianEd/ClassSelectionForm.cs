using System;
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
    public partial class ClassSelectionForm : Form
    {
        ClassBase mClassType;
        public ClassBase mSelectedType;

        public ClassSelectionForm(ClassBase godzClass)
        {
            InitializeComponent();

            mClassType = godzClass;

            //initialize the tree with all subclasses of this type....
            List<ClassBase> list = new List<ClassBase>();
            mClassType.getSubClasses(list);

            //Add the class if its not abstract....
            if (!godzClass.isAbstract())
            {
                TreeNode n = new TreeNode(godzClass.ClassName);
                n.Tag = godzClass;
                treeView1.Nodes.Add(n);
            }

            foreach (ClassBase classRef in list)
            {
                TreeNode n = new TreeNode(classRef.ClassName);
                n.Tag = classRef;
                treeView1.Nodes.Add(n);
            }
        }

        private void treeView1_NodeMouseClick(object sender, TreeNodeMouseClickEventArgs e)
        {
            mSelectedType = (ClassBase)e.Node.Tag;

            DialogResult = DialogResult.OK;
            Close();
        }
    }
}
