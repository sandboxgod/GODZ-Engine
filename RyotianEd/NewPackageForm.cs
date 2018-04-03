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
    public partial class NewPackageForm : Form
    {
        Package mPackage = null;
        PackageType mType = PackageType.PackageType_GDZ;

        public NewPackageForm()
        {
            InitializeComponent();
        }

        private void applyButton1_Click(object sender, EventArgs e)
        {
            //Make sure theres not another package with this name...
            String text = packageNameTextBox1.Text;
            UInt32 hash = GodzUtil.GetHashCode(text);
            if (!Editor.IsPackageNameTaken(hash))
            {
                if (!Editor.AddHash(hash, text, true))
                {
                    MessageBox.Show("Invalid PackageName. Might be due to a hash name collision. Try a different name");
                    DialogResult = DialogResult.Cancel;
                    Close();
                    return;
                }

                //go ahead and add the package to the default save location for now...
                if (!Editor.AddPackage(hash, "Data", mType))
                {
                    MessageBox.Show("Was not able to add the package to the Database");
                    DialogResult = DialogResult.Cancel;
                    Close();
                    return;
                }
            }

            mPackage = GodzUtil.AddPackage(text, mType);
            DialogResult = DialogResult.OK;
            Close();
        }

        public Package getPackage()
        {
            return mPackage;
        }

        private void cancelButton1_Click(object sender, EventArgs e)
        {
            DialogResult = DialogResult.Cancel;
            Close();
        }

        private void typeComboBox1_SelectedIndexChanged(object sender, EventArgs e)
        {
            switch (typeComboBox1.SelectedIndex)
            {
                case 0:
                    {
                        mType = PackageType.PackageType_GDZ;
                    }
                    break;
                case 1:
                    {
                        mType = PackageType.PackageType_RESOURCE;
                    }
                    break;
            }
        }
    }
}
