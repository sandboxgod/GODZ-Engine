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
    public partial class ResolveConflictForm : Form
    {
        struct PropertyDiff
        {
            public ClassPropertyInfo serverProp;
            public ClassPropertyInfo localProp;
        };

        private ObjectBase mServerObject;
        private ObjectBase mLocalObject;
        int conflictNum = 0;

        List<PropertyDiff> diffs = new List<PropertyDiff>();
        List<ClassPropertyInfo> finalList = new List<ClassPropertyInfo>();


        public ResolveConflictForm(ObjectBase serverObject, ObjectBase localObject)
        {
            InitializeComponent();

            mServerObject = serverObject;
            mLocalObject = localObject;

            //note: if the server has a property that doesnt exist in local then
            //we will most likely end up stripping out that property. Since it doesnt
            //exist on our side. Might want to display a warning about possible
            //obsolete data or obsolete codebase, etc

            //todo: simply get all properties as text, build grid, allow user to select
            //which version of the data or change the text, then save the changes
            //to the property as text...
            List<ClassPropertyInfo> server_properties = new List<ClassPropertyInfo>();
            List<ClassPropertyInfo> local_properties = new List<ClassPropertyInfo>();
            mServerObject.getPropertyValues(server_properties);
            mLocalObject.getPropertyValues(local_properties);

            foreach (ClassPropertyInfo cp in server_properties)
            {
                foreach (ClassPropertyInfo local_cp in local_properties)
                {
                    if (cp.mPropertyNameHash == local_cp.mPropertyNameHash)
                    {
                        if (cp.mText != local_cp.mText)
                        {
                            PropertyDiff diff;
                            diff.serverProp = cp;
                            diff.localProp = local_cp;
                            diffs.Add(diff);
                        }

                        break;
                    }
                }
            }

            if (diffs.Count == 0)
            {
                //There are no changes, close....
                DialogResult = DialogResult.OK;
                Close();
                return;
            }

            resolveConflict();
        }

        private void resolveConflict()
        {
            string conflictText = "Conflicts " + (conflictNum+1) + "/" + diffs.Count;
            conflictLabel.Text = conflictText;

            localChangeTextBox.Text = diffs[conflictNum].localProp.mText;
            serverChangeTextBox.Text = diffs[conflictNum].serverProp.mText;
        }

        private void increaseConflictNum()
        {
            conflictNum++;
            if (conflictNum >= diffs.Count)
            {
                //Apply Changes....
                int num = finalList.Count;
                for (int i = 0; i < num; i++)
                {
                    mLocalObject.setProperty(finalList[i].mName, finalList[i].mText);
                }

                DialogResult = DialogResult.OK;
                Close();
            }
        }

        private void useLocalButton_Click(object sender, EventArgs e)
        {
            ClassPropertyInfo cp = diffs[conflictNum].localProp;
            finalList.Add(cp);
            increaseConflictNum();
        }

        private void useServerButton_Click(object sender, EventArgs e)
        {
            ClassPropertyInfo cp = diffs[conflictNum].serverProp;
            finalList.Add(cp);

            increaseConflictNum();
        }
    }
}
