using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.ComponentModel;
using System.Data.SqlClient;
using System.Drawing.Design;
using System.Windows.Forms;
using GodzGlue;

namespace RyotianEd
{
    // Note: This Enum is saved to the Database so the order shouldn't change
    public enum GodzClassPropertyType
    {
        Default = 0,
        PackageEditor = 1,
        AnimationTriggerEditor = 2,
        AnimationDescEditor = 3,
        MeshSelector = 4,
        AnimationNameEditor = 5,            //animation name list
        ProjectileEditor,
    };

    public class GodzClassProperty
    {
        public uint property;
        public string Description { get; set; }
        public GodzClassPropertyType Type { get; set; }
    }

    public class GodzClassInfo
    {
        public List<GodzClassProperty> cpList = new List<GodzClassProperty>();

        public GodzClassProperty findProperty(uint hash)
        {
            foreach (GodzClassProperty cp in cpList)
            {
                if (cp.property == hash)
                {
                    return cp;
                }
            }

            return null;
        }
    }

    public class GodzClassDescriptionRegistry
    {
        // Stores a private class definition for each Class
        private static Hashtable mClassMap = new Hashtable();

        public static void put(uint classHash, GodzClassInfo cp)
        {
            mClassMap[classHash] = cp;
        }

        public static GodzClassInfo get(uint classHash)
        {
            GodzClassInfo cp = new GodzClassInfo();

            // constructs a class info that has all the properties from this class
            // with it's parents
            ClassBase temp = ClassBase.findClass(classHash);
            while (temp != null)
            {
                //find the GodzClassInfo for this class....
                GodzClassInfo classDesc = (GodzClassInfo)mClassMap[temp.getObjectName()];
                if (classDesc != null)
                {
                    // append the properties
                    int num = classDesc.cpList.Count;
                    for (int i = 0; i < num; i++)
                    {
                        cp.cpList.Add(classDesc.cpList[i]);
                    }
                }

                temp = temp.getSuperClass();
            }

            return cp;
        }

        public static void init()
        {
            //connects to DB
            if (Editor.sqlConnection != null)
            {
                string selectString = "SELECT [ClassHash],[PropertyHash],[Description],[Editor] FROM ClassPropertyDescription";

                try
                {
                    SqlCommand myCommand = new SqlCommand(selectString, Editor.sqlConnection);
                    SqlDataReader myReader = myCommand.ExecuteReader();

                    while (myReader.Read())
                    {
                        GodzClassProperty cp = new GodzClassProperty();

                        Int64 tempclass = (Int64)myReader["ClassHash"];
                        uint classhash = (uint)tempclass;

                        if (mClassMap[classhash] == null)
                        {
                            GodzClassInfo gclass = new GodzClassInfo();
                            mClassMap[classhash] = gclass;
                        }

                        Int64 temp = (Int64)myReader["PropertyHash"];
                        cp.property = (uint)temp;
                        cp.Description = (String)myReader["Description"];
                        object type = myReader["Editor"];

                        if (type != null)
                        {
                            cp.Type = (GodzClassPropertyType)type;
                        }
                        else
                        {
                            cp.Type = GodzClassPropertyType.Default;
                        }

                        GodzClassInfo godzClass = (GodzClassInfo)mClassMap[classhash];
                        godzClass.cpList.Add(cp);
                    }

                    myReader.Close();
                }
                catch (System.Data.SqlClient.SqlException exception)
                {
                    Console.WriteLine(exception.ToString());
                }
            }
        }
    }
}
