using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Runtime.InteropServices;

namespace RyotianEd
{
    /*
     * Utility functions for the World editor tab
     */
    public class WorldEditor
    {
        public static void addLayerToTree(GodzGlue.Layer layer, TreeView treeView, TabPanelData data, ContextMenuStrip childNodeMenu, TreeNode levelNode)
        {
            System.Windows.Forms.TreeNode treeNode2 = null;

            String nodeName = Editor.GetHashString(layer.getName());

            if (nodeName != null)
            {
                treeNode2 = new System.Windows.Forms.TreeNode(nodeName);
            }
            else
            {
                //Error- Editor never submitted the sector name to the DB...
                return;
            }

            levelNode.Nodes.Add(treeNode2);
            treeNode2.ContextMenuStrip = childNodeMenu;
            treeNode2.Checked = true;
            treeNode2.Tag = layer;

            if (layer.getName() == data.mActiveLayer.getName())
            {
                //assign this layer a special color....
                treeNode2.ForeColor = System.Drawing.Color.DarkBlue;
            }
        }

        //Builds a Tree that contains information about a Layer and child layers
        public static void buildLayersTree(TreeView treeView, String worldName, TabPanelData data, ContextMenuStrip menuStrip, ContextMenuStrip childNodeMenu)
        {
            treeView.Nodes.Clear();

            //add the root level node
            System.Windows.Forms.TreeNode levelNode = new System.Windows.Forms.TreeNode(worldName);
            levelNode.Checked = true;
            
            //Menu -> Add Layer option
            levelNode.ContextMenuStrip = menuStrip;
            treeView.Nodes.Add(levelNode);

            //now add all the child regions...
            List<GodzGlue.Layer> layers = new List<GodzGlue.Layer>();
            data.mWorld.getLayers(layers);

            //now go through the database; find the string name for the layer
            foreach (GodzGlue.Layer layer in layers)
            {
                addLayerToTree(layer, treeView, data, childNodeMenu, levelNode);
            }
        }
    }
}
