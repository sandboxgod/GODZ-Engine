using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Data.SqlClient;
using GodzGlue;

namespace RyotianEd
{
    public partial class OpenLayerForm : Form
    {
        public GodzGlue.Layer mLayer;
        TabPanelData mData;

        public OpenLayerForm(TabPanelData data)
        {
            InitializeComponent();

            mData = data;
            List<uint> hashList = new List<uint>();

            //Load all the available sectors...
            try
            {
                SqlDataReader myReader = null;
                SqlCommand myCommand = new SqlCommand("select * from Layers", Editor.sqlConnection);
                myReader = myCommand.ExecuteReader();
            
                while (myReader.Read())
                {
                    Int64 db_hash = (Int64)myReader["HashCode"];

                    uint hash = (uint)db_hash;
                    hashList.Add(hash);
                }

                myReader.Close();
            }
            catch (Exception e)
            {
                Console.WriteLine(e.ToString());
            }

            foreach (uint hash in hashList)
            {
                String name = Editor.GetHashString(hash);
                sectorsComboBox1.Items.Add(name);
            }
        }

        private void confirmSectorButton1_Click(object sender, EventArgs e)
        {
            //Load the Layer based on the selection...
            Object selectedItem = sectorsComboBox1.SelectedItem;
            int index = sectorsComboBox1.SelectedIndex;
            String layerName = (String)selectedItem;
            if (layerName != null)
            {
                mLayer = mData.mWorld.addLayer(layerName);

                Package layerPackage = GodzUtil.FindPackage(layerName);
                if (layerPackage == null)
                {
                    layerPackage = GodzUtil.AddPackage(layerName, PackageType.PackageType_GDZ);
                }

                mLayer.setPackage(layerPackage);

                DialogResult = DialogResult.OK;
                Close();
            }
        }
    }
}
