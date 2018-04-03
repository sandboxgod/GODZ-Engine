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
    public partial class NewLayer : Form
    {
        public String sectorName;

        public NewLayer()
        {
            InitializeComponent();
        }

        private void newSectorButton1_Click(object sender, EventArgs e)
        {
            sectorName = sectorNameTextBox1.Text;

            //get hash
            UInt32 hash = GodzGlue.GodzUtil.GetHashCode(sectorName);

            if (Editor.IsConnectedToDatabase())
            {
                //check database for name collisions (for Layers)....
                if (Editor.IsUniqueHash(hash, "Layers") )
                {
                    if (!Editor.IsPackageNameTaken(hash))
                    {
                        //ok, add the new Layer; it's ok if someone else has the same name as long as it wasnt a layer
                        if (Editor.AddHash(hash, sectorName, true))
                        {
                            if (Editor.AddPackage(hash, "Data", PackageType.PackageType_GDZ))
                            {
                                Editor.AddLayer(hash);
                                DialogResult = DialogResult.OK;
                            }
                            else
                            {
                                MessageBox.Show("Was not able to add the package of the Layer to the Database");
                                DialogResult = DialogResult.Cancel;
                            }
                        }
                        else
                        {
                            MessageBox.Show("Invalid Layer Name. Might be due to a hash name collision. Try a different name");
                            DialogResult = DialogResult.Cancel;
                        }
                    }
                    else
                    {
                        MessageBox.Show("Invalid PackageName, this name is already declared");
                        DialogResult = DialogResult.Cancel;
                    }

                    Close();
                }
                else
                {
                    MessageBox.Show("The name was already used.");
                }
            }
            else // not connected
            {
                Editor.AddHash(hash, sectorName, true);
                Editor.AddLayer(hash);

                DialogResult = DialogResult.OK;
                Close();
            }
        }
    }
}
