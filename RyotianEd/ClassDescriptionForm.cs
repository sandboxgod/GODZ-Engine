using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Data.SqlClient;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using GodzGlue;


namespace RyotianEd
{
    public partial class ClassDescriptionForm : Form
    {
        List<ClassBase> classes = new List<ClassBase>();
        List<ClassPropertyInfo> props = new List<ClassPropertyInfo>();
        ClassBase activeClass = null;
        GodzClassInfo classInfo = null;

        List<GodzClassProperty> dbProperties = null;
        
        public ClassDescriptionForm()
        {
            InitializeComponent();

            ClassBase.getAllClasses(classes);

            foreach (ClassBase gc in classes)
            {
                classPickComboBox.Items.Add(gc.ClassName);
            }
        }

        private void classPickComboBox_SelectedIndexChanged(object sender, EventArgs e)
        {
            //user selected a class...
            uint classhash = GodzUtil.GetHashCode((string)classPickComboBox.SelectedItem);
            activeClass = null;

            foreach (ClassBase tempClass in classes)
            {
                if (tempClass.getObjectName() == classhash)
                {
                    activeClass = tempClass;
                    break;
                }
            }

            //grab all the properties....
            props.Clear();
            activeClass.getPropertiesNonRecurse(props);

            propertyListBox1.Items.Clear();
            foreach (ClassPropertyInfo cp in props)
            {
                propertyListBox1.Items.Add(cp.mName);
            }

            //grab the documentation for this class from the database...
            classInfo = (GodzClassInfo)GodzClassDescriptionRegistry.get(activeClass.getObjectName());

            if (classInfo == null)
            {
                classInfo = new GodzClassInfo();
                GodzClassDescriptionRegistry.put(activeClass.getObjectName(), classInfo);
            }

            dbProperties = classInfo.cpList;
        }

        private void propertyListBox1_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (propertyListBox1.SelectedItem != null)
            {
                uint hash = props[propertyListBox1.SelectedIndex].mPropertyNameHash;

                foreach (GodzClassProperty cp in dbProperties)
                {
                    if (cp.property == hash)
                    {
                        propertyGrid1.SelectedObject = cp;
                        return;
                    }
                }

                //we never found an entry, so create one....
                GodzClassProperty newProperty = new GodzClassProperty();
                newProperty.property = hash;
                newProperty.Type = GodzClassPropertyType.Default;
                propertyGrid1.SelectedObject = newProperty;
                dbProperties.Add(newProperty);

                //update the class definition in memory as well....
                classInfo.cpList.Add(newProperty);

                //INSERT INTO ClassPropertyDescription Table....
                if (Editor.sqlConnection != null)
                {
                    try
                    {
                        String value = "Values (" + activeClass.getObjectName() + "," + hash + ",'" + newProperty.Description + "'," + (int)newProperty.Type + ")";
                        SqlCommand insertCommand = new SqlCommand("INSERT INTO ClassPropertyDescription (ClassHash, PropertyHash, Description, Editor) " + value, Editor.sqlConnection);
                        insertCommand.ExecuteNonQuery();
                    }
                    catch (SqlException sqlerror)
                    {
                        Console.WriteLine(sqlerror.ToString());
                    }
                }
            }
        }

        private void applyChangesButton_Click(object sender, EventArgs e)
        {
            //submit changes to DB
            if (Editor.sqlConnection != null)
            {
                foreach (GodzClassProperty cp in dbProperties)
                {
                    string updateText = "UPDATE ClassPropertyDescription Set Description='" + cp.Description 
                        + "',Editor="+ (int)cp.Type +"  WHERE ClassHash = " + activeClass.getObjectName() 
                        + " AND PropertyHash = " + cp.property;

                    try
                    {
                        SqlCommand insertCommand = new SqlCommand(updateText, Editor.sqlConnection);
                        insertCommand.ExecuteNonQuery();
                    }
                    catch (SqlException error)
                    {
                        Console.WriteLine(error.ToString());
                    }
                }
            }
        }
    }
}
