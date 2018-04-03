using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Data.SqlClient;
using GodzGlue;

namespace RyotianEd
{
    #region DatabaseObject
    public class DatabaseObjectInfo
    {
        public Guid objID = Guid.Empty;
        public bool locked = false;
        public String dbUser = null;
        public int revision = 0;
        public bool modified = false;
        public bool isDeleted = false;
    }

    public class DatabaseObject
    {
        public uint packageHash;
        public uint classhash;
        public Guid objID;
        public String xmlData;
        public bool locked;
        public String dbUser;
        public int revision;

        public void importData(SqlDataReader myReader, ApplicationSettings applicationSettings)
        {
            importData(myReader, applicationSettings, true);
        }

        public void importData(SqlDataReader myReader, ApplicationSettings applicationSettings, bool importGUID)
        {
            if (importGUID)
            {
                objID = (Guid)myReader["GUID"];
            }

            Int64 objPackageHash = (Int64)myReader["PackageHashCode"];
            Int64 tempclasshash = (Int64)myReader["ClassHashCode"];
            xmlData = (String)myReader["Properties"];
            Object lockedUser = myReader["Locked"];
            revision = (int)myReader["Revision"];

            packageHash = (uint)objPackageHash;
            classhash = (uint)tempclasshash;

            if (!(lockedUser is DBNull))
            {
                String owner = (String)lockedUser;
                locked = (owner.Equals(applicationSettings.DatabaseOptions.user));
                dbUser = owner;
            }
        }

        public ObjectBase newInstance()
        {
            Package p = GodzUtil.FindPackage(packageHash);
            if (p == null)
            {
                string text = Editor.GetHashString(packageHash);

                //If Package is still null, attempt to instance it...
                p = GodzUtil.AddPackage(text, PackageType.PackageType_GDZ);
            }

            return newInstance(p);
        }

        public ObjectBase newInstance(Package p)
        {
            ClassBase godzClass = ClassBase.findClass(classhash);
            if (godzClass != null)
            {
                //set package
                ObjectBase obj = godzClass.newInstance();

                if (p != null)
                {
                    obj.setPackage(p);

                    DatabaseObjectInfo info = new DatabaseObjectInfo();
                    info.dbUser = dbUser;
                    info.locked = locked;
                    info.objID = objID;
                    info.revision = revision;
                    DatabaseObjectRegistry.Items[obj] = info;

                    obj.Id = objID;

                    Editor.ImportXMLData(obj, xmlData);
                    Editor.ImportObjectReferences(obj, p);

                    return obj;
                }
            }

            return null;
        }
    }
    #endregion

    #region Registry
    public class DatabaseObjectRegistry
    {
        public static Hashtable Items {get; set;}

        public static DatabaseObjectInfo get(ObjectBase obj)
        {
            Object temp = Items[obj];
            if (temp != null)
            {
                return (DatabaseObjectInfo)temp;
            }

            return null;
        }
    }
    #endregion
}
