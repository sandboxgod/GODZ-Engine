using System;
using System.Linq;
using System.Text;
using System.Data.SqlClient;
using System.Xml;
using System.Xml.Serialization;
using System.IO;
using System.Collections;
using System.Collections.Generic;
using System.Reflection;
using System.Reflection.Emit;
using System.Threading;
using System.Diagnostics;
using GodzGlue;

namespace RyotianEd
{
    public class DatabaseSettings
    {
        //database settings
        public string server { get; set; }
        public string user { get; set; }
        public string password { get; set; }
    }

    /*
     * Application settings
     */
    public class ApplicationSettings
    {
        public DatabaseSettings DatabaseOptions { get; set; }
    }

    public struct UpdateResult
    {
        public string error;
        public bool result;
    }

    public class ContainerData
    {
        public uint propertyHash;
        public Guid objectID;
        public bool isExternal;
    }

    public class PackageInfo
    {
        public uint hash;
        public string name;
        public PackageType type;
    }

    public class HashStringRecord
    {
        public uint hash;
        public string text;
    }

    /*
     * Stores global methods/data required by the Editor for all the Forms
     */
    public class Editor
    {
        public static SqlConnection sqlConnection;

        //used during offline mode to store hashes
        private static Hashtable mStringMap = new Hashtable();

        public static RyotianEd.ApplicationSettings applicationSettings { get; set; }

        public static void initEditor()
        {
            //Editor initialization goes here....
            DatabaseObjectRegistry.Items = new Hashtable();
        }

        /// <summary>
        /// Returns a new object that wraps the ObjectBase, used by PropertyGrid
        /// </summary>
        public static Object GetNewObjectProxy(GodzGlue.ObjectBase obj)
        {
            ObjectBaseProxy proxy = new ObjectBaseProxy(obj);

            List<GodzGlue.ClassPropertyInfo> cpList = new List<GodzGlue.ClassPropertyInfo>();
            obj.getPropertyValues(cpList);

            ClassBase gc = obj.getClass();
            GodzClassInfo classInfo = GodzClassDescriptionRegistry.get(gc.getObjectName());

            foreach (GodzGlue.ClassPropertyInfo cp in cpList)
            {
                bool isCustomProperty = true;

                if (classInfo != null)
                {
                    GodzClassProperty gp = classInfo.findProperty(cp.mPropertyNameHash);
                    if (gp != null)
                    {
                        switch (gp.Type)
                        {
                            case GodzClassPropertyType.PackageEditor:
                                {
                                    isCustomProperty = false;

                                    PackageProperty pp = new PackageProperty(cp.mName, cp.mPropertyNameHash, obj, false, true);
                                    proxy.Add(pp);
                                }
                                break;

                            case GodzClassPropertyType.MeshSelector:
                                {
                                    isCustomProperty = false;

                                    MeshProperty pp = new MeshProperty(cp.mName, cp.mPropertyNameHash, obj, false, true);
                                    proxy.Add(pp);
                                }
                                break;

                            case GodzClassPropertyType.AnimationNameEditor:
                                {
                                    isCustomProperty = false;

                                    AnimationNameProperty pp = new AnimationNameProperty(cp.mName, cp.mPropertyNameHash, obj, false, true);
                                    proxy.Add(pp);
                                }
                                break;
                            case GodzClassPropertyType.ProjectileEditor:
                                {
                                    isCustomProperty = false;

                                    ProjectileProperty pp = new ProjectileProperty(cp.mName, cp.mPropertyNameHash, obj, false, true);
                                    proxy.Add(pp);
                                }
                                break;
                        }
                    }
                }

                if (isCustomProperty)
                {
                    CustomProperty property = new CustomProperty(cp.mName, cp.mPropertyNameHash, obj, false, true);
                    proxy.Add(property);
                }
            }

            return proxy;
        }

        public static bool OnClassPropertyChanged(GodzGlue.ObjectBase obj, GodzGlue.ClassPropertyInfo cp, String text, System.Windows.Forms.PropertyValueChangedEventArgs e)
        {
            bool propertyRenamed = false;

            
            if (cp.mPropertyType == GodzGlue.ClassPropertyType.PropertyType_HashCode)
            {
                //deref the old string then addref for the new String in the HashTable...
                //1. Find the old hash in the HashTable....
                String oldStr = (String)e.OldValue;

                if (oldStr != null)
                {
                    uint oldhash = GodzGlue.GodzUtil.GetHashCode(oldStr);
                    Editor.RemoveHash(oldhash);
                }

                //2. Add the new String to the HashTable
                uint hash = GodzGlue.GodzUtil.GetHashCode(text);
                Editor.AddHash(hash, text, false);

                //track that a hashString has been renamed
                propertyRenamed = true;
            }

            //commit the change to the entity in memory
            //obj.setPropertyAt((UInt32)i, text);
            obj.setProperty(cp.mName, text);
            obj.OnPropertiesUpdated();

            //set the changed state on the object
            DatabaseObjectInfo info = DatabaseObjectRegistry.get(obj);
            if (info != null)
            {
                info.modified = true;
            }

            return propertyRenamed;
        }

        //Utility function that can be used to update a Godz object related to a 'proxy' object used within
        //a property editor. Returns true if Entity was renamed....
        public static bool HandlePropertyChanged(GodzGlue.ObjectBase obj, System.Windows.Forms.PropertyValueChangedEventArgs e)
        {
            //check to see if the entity was renamed; if so, we store the text to hash table
            List<GodzGlue.ClassPropertyInfo> cpList = new List<GodzGlue.ClassPropertyInfo>();
            obj.getPropertyValues(cpList);

            int count = cpList.Count;
            int i;
            bool found = false;

            for (i = 0; i < count; i++)
            {
                if (e.ChangedItem.Label.Equals(cpList[i].mName))
                {
                    found = true;
                    break;
                }
            }

            if (!found)
            {
                //TODO: Should toss an exception of some sort here
                return false;
            }

            Object value = e.ChangedItem.Value;
            String text = (String)value;

            GodzGlue.ClassPropertyInfo cp = cpList[i];
            return OnClassPropertyChanged(obj, cp, text, e);
        }

        public static bool IsConnectedToDatabase()
        {
            return sqlConnection != null;
        }

        public static void CloseSQLConnection()
        {
            if (sqlConnection != null)
            {
                try
                {
                    sqlConnection.Close();
                    sqlConnection = null;
                }
                catch (SqlException)
                {
                    //connection was never opened or something....
                }
            }
        }

        #region HashStrings
        //determines if the hash code is unique
        public static bool IsUniqueHash(UInt32 hash, String tableName)
        {
            if (sqlConnection != null)
            {
                try
                {
                    SqlDataReader myReader = null;
                    SqlCommand myCommand = new SqlCommand("select * from " + tableName, sqlConnection);
                    myReader = myCommand.ExecuteReader();

                    bool ret = true;

                    while (myReader.Read())
                    {
                        Int64 db_hash = (Int64)myReader["HashCode"];
                        if (hash == db_hash)
                        {
                            ret = false;
                            break;
                        }
                    }

                    myReader.Close();

                    //we didn't find the hash code so it's unique
                    return ret;
                }
                catch (System.Data.SqlClient.SqlException e)
                {
                    Console.WriteLine(e.ToString());
                }
            }
            else //load the local HString table file, check it....
            {
            }

            return false;
        }

        //get the name associated with the hash
        public static String GetHashString(UInt32 hash)
        {
            String text = null;

            if (sqlConnection != null)
            {
                try
                {
                    SqlDataReader myReader = null;
                    SqlCommand myCommand = new SqlCommand("select * from HashTable WHERE HashCode = " + hash, sqlConnection);
                    myReader = myCommand.ExecuteReader();

                    if (myReader.Read())
                    {
                        //found the hash string....
                        text = (String)myReader["Text"];
                    }

                    myReader.Close();
                }
                catch (System.Data.SqlClient.SqlException e)
                {
                    Console.WriteLine(e.ToString());
                }
            }
            else
            {
                //offline mode, we can only check the memory-only hash map
                text = (String)mStringMap[hash];
            }

            return text;
        }

        public static void RemoveHash(UInt32 hash)
        {
            if (sqlConnection != null)
            {
                Int32 refs = 0;
                bool found = false;

                //1. Get the # of refs for this HashCode
                try
                {
                    SqlDataReader myReader = null;
                    SqlCommand myCommand = new SqlCommand("select * from HashTable", sqlConnection);
                    myReader = myCommand.ExecuteReader();

                    while (myReader.Read())
                    {
                        Int64 db_hash = (Int64)myReader["HashCode"];
                        if (hash == db_hash)
                        {
                            //found the hash string....
                            refs = (Int32)myReader["RefCount"];
                            found = true;
                            break;
                        }
                    }

                    myReader.Close();
                }
                catch (SqlException e)
                {
                    Console.WriteLine(e.ToString());
                }

                if (!found)
                {
                    //record doesn't exist....
                    return;
                }

                //2. remove a ref count for the string....
                refs--;

                if (refs == 0)
                {
                    //3. remove the record for the hash code
                    try
                    {
                        String query = "WHERE HashCode = " + hash;
                        SqlCommand removeCommand = new SqlCommand("DELETE FROM HashTable " + query, sqlConnection);
                        removeCommand.ExecuteNonQuery();
                    }
                    catch (System.Data.SqlClient.SqlException e)
                    {
                        Console.WriteLine(e.ToString());
                    }
                }
                else
                {
                    //update the existing record
                    try
                    {
                        SqlCommand insertCommand = new SqlCommand("UPDATE HashTable Set RefCount = " + refs + " WHERE HashCode = " + hash, sqlConnection);
                        insertCommand.ExecuteNonQuery();
                    }
                    catch (SqlException e)
                    {
                        Console.WriteLine(e.ToString());
                    }
                }
            } // if sqlConnection
        }

        //Adds the string to the HString Manager...
        //IsRuntimeFlag = Set to true if this string needs to be available at runtime.
        //False is returned if a Hash name collision occurs (wrong hash for the text)
        public static bool AddHash(UInt32 hash, String text, bool IsRuntimeFlag)
        {
            //mStringMap.Add(hash, text);

            if (sqlConnection != null)
            {
                Int32 refs = 0;

                //1. Get the # of refs for this HashCode
                try
                {
                    SqlDataReader myReader = null;
                    SqlCommand myCommand = new SqlCommand("select * from HashTable", sqlConnection);
                    myReader = myCommand.ExecuteReader();

                    while (myReader.Read())
                    {
                        Int64 db_hash = (Int64)myReader["HashCode"];
                        if (hash == db_hash)
                        {
                            //found the hash string....
                            refs = (Int32)myReader["RefCount"];
                            break;
                        }
                    }

                    myReader.Close();
                }
                catch (SqlException e)
                {
                    Console.WriteLine(e.ToString());
                }

                //2. add a ref count for the new string we're adding....
                refs++;

                if (refs == 1)
                {
                    //INSERT
                    String value = "Values (" + hash + ", '" + text + "'," + refs + ", '" + IsRuntimeFlag + "')";

                    //3. add a new record for the hash code
                    try
                    {
                        SqlCommand insertCommand = new SqlCommand("INSERT INTO HashTable (HashCode, Text, RefCount, IsRuntimeString) " + value, sqlConnection);
                        insertCommand.ExecuteNonQuery();
                    }
                    catch (System.Data.SqlClient.SqlException e)
                    {
                        Console.WriteLine(e.ToString());
                    }
                }
                else
                {
                    //update the existing record
                    try
                    {
                        SqlCommand insertCommand = new SqlCommand("UPDATE HashTable Set RefCount = " + refs + " WHERE HashCode = " + hash + " AND Text = '" + text + "'", sqlConnection);
                        insertCommand.ExecuteNonQuery();
                    }
                    catch (SqlException e)
                    {
                        //if we've failed, perhaps we've encountered a hashcode collision
                        Console.WriteLine(e.ToString());
                        return false;
                    }
                }

            }

            return true;
        }

        public static void GetHashStrings(ref List<HashStringRecord> stringList, bool exportAllStrings)
        {
            if (sqlConnection != null)
            {
                try
                {
                    string cmd = "select * from HashTable";

                    if (!exportAllStrings)
                    {
                        cmd += " WHERE IsRuntimeString = 1";
                    }

                    SqlDataReader myReader = null;
                    SqlCommand myCommand = new SqlCommand(cmd, sqlConnection);
                    myReader = myCommand.ExecuteReader();

                    while (myReader.Read())
                    {
                        HashStringRecord record = new HashStringRecord();

                        //found the hash string....
                        record.text = (String)myReader["Text"];
                        Int64 db_hash = (Int64)myReader["HashCode"];
                        record.hash = (uint)db_hash;
                        stringList.Add(record);
                    }

                    myReader.Close();
                }
                catch (System.Data.SqlClient.SqlException e)
                {
                    Console.WriteLine(e.ToString());
                }
            }
        }

        public static void ExportStrings(bool exportAllStrings, bool isSaving)
        {
            List<HashStringRecord> hashList = new List<HashStringRecord>();
            Editor.GetHashStrings(ref hashList, exportAllStrings);

            HStringTable.Clear();
            foreach (HashStringRecord r in hashList)
            {
                HStringTable.AddString(r.text, r.hash);
            }

            if (isSaving)
            {
                HStringTable.Save();
            }
        }

        #endregion

        //Adds a new Layer
        public static void AddLayer(UInt32 hash)
        {
            if (sqlConnection != null)
            {
                try
                {
                    SqlCommand insertCommand = new SqlCommand("INSERT INTO Layers (HashCode) Values(" + hash + ")", sqlConnection);
                    insertCommand.ExecuteNonQuery();
                }
                catch (SqlException e)
                {
                    Console.WriteLine(e.ToString());
                }
            }
        }

        private static void SubmitNewObject(uint packageHash, GodzGlue.ObjectBase entity)
        {
            String xmlData = Editor.GetXML(entity);

            try
            {
                ClassBase gc = entity.getClass();
                UInt32 classHash = gc.getObjectName();

                //id of the new entity
                DatabaseObjectInfo info = new DatabaseObjectInfo();
                info.objID = Guid.NewGuid();
                DatabaseObjectRegistry.Items[entity] = info;
                entity.Id = info.objID;

                String value = "Values (" + packageHash + ", " + classHash + ", '" + xmlData + "','" + info.objID + "', 0, 'false')";
                SqlCommand insertCommand = new SqlCommand("INSERT INTO Objects (PackageHashCode, ClassHashCode, Properties, GUID, Revision, IsDeleted) " + value, sqlConnection);
                insertCommand.ExecuteNonQuery();
            }
            catch (SqlException e)
            {
                Console.WriteLine(e.ToString());
                return;
            }

            //now go into the properties and submit subobjects 
            //only if they belong to same package....
            List<ClassPropertyInfo> cpList = new List<ClassPropertyInfo>();
            entity.getPropertyValues(cpList);

            foreach (ClassPropertyInfo cp in cpList)
            {
                switch (cp.mPropertyType)
                {
                    case (ClassPropertyType.PropertyType_Object):
                    {
                        if (cp.mObject != null && cp.mObject.getPackage().GetName() == packageHash)
                        {
                            SubmitNewObject(cp.mObject.getPackage().GetName(), cp.mObject);
                        }
                    }
                    break;
                    case (ClassPropertyType.PropertyType_Array):
                    {
                        PropertyArray parray = cp.mArray;
                        if (parray != null)
                        {
                            uint num = parray.size();
                            for (uint i = 0; i < num; i++)
                            {
                                PropertyValue v = parray[i];
                                ObjectBase oo = v.getObject();
                                if (oo != null && oo.getPackage().GetName() == packageHash)
                                {
                                    SubmitNewObject(oo.getPackage().GetName(), oo);
                                }
                            }
                        }
                    }
                    break;
                    case (ClassPropertyType.PropertyType_Map):
                    {
                        PropertyMap pmap = cp.mMap;
                        if (pmap != null)
                        {
                            List<PropertyValue> values = new List<PropertyValue>();
                            pmap.getValues(values);

                            for (int i = 0; i < values.Count; i++)
                            {
                                PropertyValue v = values[i];
                                ObjectBase oo = v.getObject();
                                if (oo != null)
                                {
                                    //if (oo.getPackage() == null )
                                    //    oo.OnPropertiesUpdated();
                                    if (oo.getPackage().GetName() == packageHash)
                                    {
                                        SubmitNewObject(oo.getPackage().GetName(), oo);
                                    }
                                }
                            }
                        }
                    }
                    break;
                }
            }
        }


        public static void AddEntity(uint packageHash, GodzGlue.ObjectBase entity)
        {
            if (sqlConnection != null)
            {
                SubmitNewObject(packageHash, entity);
            }
        }

        /// <summary>
        /// Sets the Removed status on this Object in the database.
        /// </summary>
        public static bool UpdateDeleteStatus(uint packageHash, GodzGlue.ObjectBase entity, bool isDeleted)
        {
            DatabaseObjectInfo info = DatabaseObjectRegistry.get(entity);

            if (sqlConnection != null)
            {
                try
                {
                    string updateText = "UPDATE Objects Set IsDeleted = '"
                        + isDeleted + "' WHERE GUID = '" + info.objID + "' AND PackageHashCode = " + packageHash
                        + " AND Revision = " + info.revision
                        + " AND Locked IS NULL OR Locked = '" + applicationSettings.DatabaseOptions.user + "'";

                    SqlCommand insertCommand = new SqlCommand(updateText, sqlConnection);

                    insertCommand.ExecuteNonQuery();
                }
                catch (SqlException e)
                {
                    Console.WriteLine(e.ToString());
                    return false;
                }
            }

            //Tell the object it has been successfully deleted/restored....
            info.isDeleted = (isDeleted);

            return true;
        }

        public static bool RemoveEntity(uint packageHash, GodzGlue.ObjectBase entity)
        {
            return UpdateDeleteStatus(packageHash, entity, true);
        }

        public static UpdateResult UpdateEntity(uint packageHash, GodzGlue.ObjectBase entity)
        {
            DatabaseObjectInfo info = DatabaseObjectRegistry.get(entity);

            String xmlData = GetXML(entity);
            String user = applicationSettings.DatabaseOptions.user;
            int revision = info.revision;
            int new_revision = revision + 1;

            UpdateResult result;
            result.result = true;
            result.error = null;

            //number of rows are query modified
            int numRows = 0;

            if (sqlConnection != null)
            {
                try
                {
                    string updateText = "UPDATE Objects Set Properties = '"
                        + xmlData + "', Revision=" + new_revision + " WHERE GUID = '" + info.objID + "' AND PackageHashCode = " + packageHash
                        + " AND Revision = " + revision
                        + " AND Locked IS NULL OR Locked = '" + user + "'";

                    SqlCommand insertCommand = new SqlCommand(updateText, sqlConnection);

                    numRows = insertCommand.ExecuteNonQuery();
                }
                catch (SqlException e)
                {
                    Console.WriteLine(e.ToString());
                    result.result = false;
                    result.error = e.ToString();
                    return result;
                }
            }

            //reset the data modified flag now that the object has been submitted
            if (numRows > 0)
            {
                info.modified = false;
                info.revision = (new_revision);
            }
            else
            {
                result.result = false;
                result.error = "Unable to Submit Changes. You might need to sync";
            }

            //now update all the Objects that's encapsulated within the containers....
            List<ClassPropertyInfo> cpList = new List<ClassPropertyInfo>();
            entity.getPropertyValues(cpList);

            foreach (ClassPropertyInfo cp in cpList)
            {
                if (cp.mArray != null && cp.mArray.getElementType() == ClassPropertyType.PropertyType_Object)
                {
                    PropertyArray vector = cp.mArray;
                    for (uint i = 0; i < vector.size(); i++)
                    {
                        PropertyValue pv = vector[i];
                        ObjectBase obj = pv.getObject();
                        Debug.Assert(obj.getPackage() != null, "Unknown package for object");

                        if (obj.getPackage() != null)
                            UpdateEntity(obj.getPackage().GetName(), obj);
                    }
                }
                else if (cp.mMap != null)
                {
                    PropertyMap map = cp.mMap;
                    List<PropertyValue> pvalues = new List<PropertyValue>();
                    map.getValues(pvalues);

                    for (int i = 0; i < pvalues.Count; i++)
                    {
                        PropertyValue pv = pvalues[i];
                        ObjectBase obj = pv.getObject();
                        if (obj != null)
                        {
                            if (obj.getPackage() != null)
                                UpdateEntity(obj.getPackage().GetName(), obj);
                        }
                    }
                }
                else if (cp.mPropertyType == ClassPropertyType.PropertyType_Object)
                {
                    if (cp.mObject != null)
                    {
                        Debug.Assert(cp.mObject.getPackage() != null, "Unknown package for object");

                        if (cp.mObject.getPackage() != null)
                        {
                            //Check to see if this object really belongs in the Database.
                            //If it doesn't, then we don't add it to the Objects Table
                            DatabaseObjectInfo dbo = DatabaseObjectRegistry.get(cp.mObject);
                            bool isExternal = (dbo == null);

                            UpdateObjectReference(cp.mPropertyNameHash, cp.mObject, entity, isExternal);

                            if (!isExternal)
                            {
                                UpdateEntity(cp.mObject.getPackage().GetName(), cp.mObject);
                            }
                            else
                            {
                                bool readRecords = false;

                                //check to see if we inserted a record for this
                                try
                                {
                                    String selectString = "SELECT [PackageHash] FROM ExternalObjects WHERE GUID = '" + cp.mObject.Id + "'";

                                    SqlCommand myCommand = new SqlCommand(selectString, sqlConnection);
                                    SqlDataReader myReader = myCommand.ExecuteReader();
                                    readRecords = myReader.Read();
                                    myReader.Close();
                                }
                                catch (SqlException e)
                                {
                                    Console.WriteLine(e.ToString());
                                }

                                //Append a record to the ExternalObjects table so
                                //we know how to load this object later...
                                if (!readRecords)
                                {
                                    try
                                    {
                                        String value = "Values ('" + cp.mObject.Id.ToString() + "', " + cp.mObject.getPackage().GetName() + ", 0)";
                                        SqlCommand insertCommand = new SqlCommand("INSERT INTO ExternalObjects (GUID, PackageHash, Revision) " + value, sqlConnection);
                                        insertCommand.ExecuteNonQuery();
                                    }
                                    catch (SqlException e)
                                    {
                                        Console.WriteLine(e.ToString());

                                        result.result = false;
                                        result.error = "Unable to Submit subobject " + cp.mObject.Id.ToString();
                                    }
                                }
                            }
                        }
                    }
                    else
                    {
                        //check to see if we need to delete a ref
                        RemoveObjectReference(cp.mPropertyNameHash, entity);
                    }
                }
            }

            return result;
        }

        private static void ReadDBOwner(GodzGlue.ObjectBase godzObject, SqlDataReader myReader)
        {
            DatabaseObjectInfo info = DatabaseObjectRegistry.get(godzObject);

            Object lockedUser = myReader["Locked"];
            if (!(lockedUser is DBNull))
            {
                String owner = (String)lockedUser;
                info.dbUser = (owner);
            }
            else
            {
                info.dbUser = null;
            }

            info.isDeleted = (bool)myReader["IsDeleted"];
        }

        private static void ReadSQL(GodzGlue.ObjectBase godzObject, SqlDataReader myReader)
        {
            DatabaseObjectInfo info = DatabaseObjectRegistry.get(godzObject);

            String xmlData = (String)myReader["Properties"];
            ImportXMLData(godzObject, xmlData);

            info.revision = (int)myReader["Revision"];

            ReadDBOwner(godzObject, myReader);
        }

        //Syncs up the Entity to the Database
        public static UpdateResult RefreshEntity(GodzGlue.ObjectBase godzObject, uint packageHash)
        {
            DatabaseObjectInfo info = DatabaseObjectRegistry.get(godzObject);

            UpdateResult sqlresult;
            try
            {
                String selectString = "SELECT [Properties],[Locked],[Revision],[IsDeleted] FROM Objects Where PackageHashCode=" + packageHash + " AND GUID = '"
                    + info.objID + "'";

                SqlCommand myCommand = new SqlCommand(selectString, sqlConnection);
                SqlDataReader myReader = myCommand.ExecuteReader();

                String xmlData = null;
                int newRevision = 0;

                if (myReader.Read())
                {
                    if (info.modified)
                    {
                        //check for collisions...
                        ReadDBOwner(godzObject, myReader);

                        //get xml from server
                        xmlData = (String)myReader["Properties"];
                        newRevision = (int)myReader["Revision"];

                    }
                    else
                    {
                        ReadSQL(godzObject, myReader);
                    }
                }

                myReader.Close();

                if (info.modified)
                {
                    //create an object that stores the server data that we can use
                    //to detect differences...
                    ClassBase gc = godzObject.getClass();
                    ObjectBase compareObj = gc.newInstance();
                    ImportXMLData(compareObj, xmlData);

                    //now display a dialog that allows user to select what they want...
                    ResolveConflictForm conflictForm = new ResolveConflictForm(compareObj, godzObject);
                    if (conflictForm.ShowDialog() == System.Windows.Forms.DialogResult.OK)
                    {
                        //update revision number...
                        info.revision = (newRevision);

                        //turn off modified flag, since now we're synced to the DB...
                        info.modified = (false);
                    }
                }
            }
            catch (SqlException e)
            {
                Console.WriteLine(e.ToString());

                sqlresult.result = false;
                sqlresult.error = e.ToString();
                return sqlresult;
            }

            
            sqlresult.result = true;
            sqlresult.error = null;
            

            //Now Refresh all the Objects that's encapsulated within the containers....
            List<ClassPropertyInfo> cpList = new List<ClassPropertyInfo>();
            godzObject.getPropertyValues(cpList);

            foreach (ClassPropertyInfo cp in cpList)
            {
                if (cp.mArray != null && cp.mArray.getElementType() == ClassPropertyType.PropertyType_Object)
                {
                    PropertyArray vector = cp.mArray;
                    for (uint i = 0; i < vector.size(); i++)
                    {
                        PropertyValue pv = vector[i];
                        ObjectBase obj = pv.getObject();

                        //might get an error like record doesnt exist if another user
                        //deleted something...
                        sqlresult = RefreshEntity(obj, packageHash);

                        //TODO: Need a way to accumulate errors
                    }

                    //See if new entries were added....
                    AddMissingObjectsToContainer(cp.mPropertyNameHash, godzObject, vector);

                } //if (cp.mContainer != null)
            }

            return sqlresult;
        }

        //Loads all of the objects associated with the package. PackageHash = 0, will return ALL
        //objects from all packages in the Database
        public static void LoadObjects(ref List<DatabaseObject> list, uint packageHash)
        {
            try
            {
                String selectString;

                if (packageHash > 0)
                    selectString = "SELECT [PackageHashCode],[Properties],[ClassHashCode],[Locked],[GUID],[Revision] FROM Objects Where PackageHashCode=" + packageHash + " AND IsDeleted = 'FALSE'";
                else
                {
                    selectString = "SELECT [PackageHashCode],[Properties],[ClassHashCode],[Locked],[GUID],[Revision] FROM Objects WHERE IsDeleted = 'FALSE'";
                }

                SqlDataReader myReader = null;
                SqlCommand myCommand = new SqlCommand(selectString, sqlConnection);
                myReader = myCommand.ExecuteReader();

                while (myReader.Read())
                {
                    DatabaseObject obj = new DatabaseObject();
                    obj.importData(myReader, applicationSettings, true);
                    list.Add(obj);
                }

                myReader.Close();
            }
            catch (SqlException e)
            {
                Console.WriteLine(e.ToString());
            }
        }

        //Loads all of the objects associated with the package. PackageHash = 0, will return ALL
        //objects from all packages in the Database
        public static void LoadObjectsOfType(ref List<DatabaseObject> list, uint packageHash, ClassBase baseClass)
        {
            try
            {
                String selectString;
                String classSelectString = " ";

                List<ClassBase> subclasses = new List<ClassBase>();
                baseClass.getSubClasses(subclasses);

                subclasses.Add(baseClass);
                int count = 0;

                foreach (ClassBase gc in subclasses)
                {
                    if (count == 0)
                    {
                        classSelectString += " AND ClassHashCode = " + gc.getObjectName();
                    }
                    else
                        classSelectString += " OR ClassHashCode = " + gc.getObjectName();

                    count++;
                }

                if (packageHash > 0)
                    selectString = "SELECT [PackageHashCode],[ClassHashCode],[Properties],[Locked],[GUID],[Revision] FROM Objects Where PackageHashCode=" + packageHash + " AND IsDeleted = 'FALSE'" + classSelectString;
                else
                {
                    selectString = "SELECT [PackageHashCode],[ClassHashCode],[Properties],[Locked],[GUID],[Revision] FROM Objects WHERE IsDeleted = 'FALSE'" + classSelectString;
                }

                SqlDataReader myReader = null;
                SqlCommand myCommand = new SqlCommand(selectString, sqlConnection);
                myReader = myCommand.ExecuteReader();

                while (myReader.Read())
                {
                    DatabaseObject obj = new DatabaseObject();
                    obj.importData(myReader, applicationSettings, true);
                    list.Add(obj);
                }

                myReader.Close();
            }
            catch (SqlException e)
            {
                Console.WriteLine(e.ToString());
            }
        }

        /// <summary>
        /// Sorts through the list, checking to see if the objects are ref'ed.
        /// Child references will be pruned from the list. This function will
        /// return a list of Parent Database objects (children are culled)
        /// </summary>
        /// <param name="list"></param>
        public static void CullChildObjects(ref List<DatabaseObject> list)
        {
            foreach (DatabaseObject dbo1 in list)
            {
                //get object references for this guy
                List<Guid> childGuids = new List<Guid>();
                GetObjectReferences(ref childGuids, dbo1.objID);

                foreach (DatabaseObject dbo2 in list)
                {
                    if (childGuids.Contains(dbo2.objID))
                    {
                        list.Remove(dbo2);

                        //keep culling until no children are left
                        CullChildObjects(ref list);
                        return;
                    }
                }
            }
        }

        public static void LoadEntities(TabPanelData tabData)
        {
            if (sqlConnection != null)
            {
                List<DatabaseObject> entityList = new List<DatabaseObject>();

                //first gather list of entities then close connection so we can query the HashTable later...
                LoadObjects(ref entityList, tabData.mActiveLayer.getName());

                //go through the list.... Add the entities to the Layer
                foreach (DatabaseObject data in entityList)
                {
                    //spawn at default coordinates however this will be overwritten anyway when we import
                    //properties....
                    GodzGlue.Entity newActor = tabData.mActiveLayer.spawnActor(data.classhash);

                    //setup a DB info for this object
                    DatabaseObjectInfo info = new DatabaseObjectInfo();
                    info.objID = (data.objID);
                    info.locked = (data.locked);
                    info.dbUser = (data.dbUser);
                    info.revision = (data.revision);
                    DatabaseObjectRegistry.Items[newActor] = info;
                    newActor.Id = data.objID;

                    ImportXMLData(newActor, data.xmlData);
                    ImportObjectReferences(newActor, null);

                    if (newActor is GodzGlue.SunLight)
                    {
                        GodzGlue.SunLight sun = (GodzGlue.SunLight)newActor;
                        sun.setSunLight();
                        tabData.mSun = sun;
                    }
                } //foreach entity

                GodzUtil.RunMainPass();
            }
        }

        public static void ExportPackage(uint packageHash, ResourceType rt)
        {
            List<ObjectBase> list = new List<ObjectBase>();
            ExportPackage(packageHash, rt, ref list);
        }

        /// <summary>
        /// Exports a package from the Database
        /// </summary>
        /// <param name="packageHash"></param>
        /// <param name="externalList"></param>
        public static void ExportPackage(uint packageHash, ResourceType rt, ref List<ObjectBase> externalList)
        {
            List<DatabaseObject> list = new List<DatabaseObject>();
            LoadObjects(ref list, packageHash);

            //Cull encapsulated children. We only need to write out the root
            //parent objects because when we instance the parent, it will automatically
            //submit child objects
            CullChildObjects(ref list);

            //create a dummy package; this way we won't interfere with any other
            //open dialogs plus we know we got a clean package...
            string pname = GetHashString(packageHash);
            Package p = new Package(pname);
            foreach (DatabaseObject rootObject in list)
            {
                rootObject.newInstance(p);
            }

            foreach (ObjectBase oo in externalList)
            {
                p.Add(oo);
            }

            String packagePath = GodzUtil.getFullPathToPackage(pname, rt);
            GodzUtil.savePackage(packagePath, p);

            //drop all the dummy objects & reap this package
            p.Clear();
        }


        #region "LockEntity"
        public static bool LockEntity(GodzGlue.ObjectBase entity)
        {
            DatabaseObjectInfo info = DatabaseObjectRegistry.get(entity);

            if (sqlConnection != null)
            {
                String user = applicationSettings.DatabaseOptions.user;

                try
                {
                    SqlCommand insertCommand = new SqlCommand("UPDATE Objects Set Locked = '" + user + "' WHERE Locked IS NULL AND GUID = '" + info.objID + "'", sqlConnection);
                    insertCommand.ExecuteNonQuery();

                    info.locked = (true);
                    info.dbUser = (user);
                }
                catch (SqlException e)
                {
                    info.locked = (false);
                    Console.WriteLine(e.ToString());
                    return false;
                }
            }
            return true;
        }

        public static bool UnlockEntity(GodzGlue.Entity entity)
        {
            DatabaseObjectInfo info = DatabaseObjectRegistry.get(entity);

            if (sqlConnection != null)
            {
                try
                {
                    SqlCommand insertCommand = new SqlCommand("UPDATE Objects Set Locked = NULL WHERE GUID = '" + info.objID + "'", sqlConnection);
                    insertCommand.ExecuteNonQuery();

                    info.locked = (false);
                    info.dbUser = (null);
                }
                catch (SqlException e)
                {
                    Console.WriteLine(e.ToString());
                    return false;
                }
            }
            return true;
        }
        #endregion

        #region "Packages"
        /// <summary>
        /// Returns a list of all the Packages registered in the Database
        /// </summary>
        public static void GetPackages(ref List<PackageInfo> packages)
        {
            if (sqlConnection != null)
            {
                try
                {
                    SqlDataReader myReader = null;
                    SqlCommand myCommand = new SqlCommand("SELECT [HashCode],[Type] FROM [GODZ].[dbo].[Packages]", sqlConnection);
                    myReader = myCommand.ExecuteReader();

                    while (myReader.Read())
                    {
                        PackageInfo p = new PackageInfo();
                        Int64 hash = (Int64)myReader["HashCode"];
                        p.hash = (uint)hash;
                        p.type = (PackageType)myReader["Type"];
                        packages.Add(p);
                    }

                    myReader.Close();


                    //Now go lookup the actual package names....
                    for (int i = 0; i < packages.Count; i++)
                    {
                        packages[i].name = GetHashString(packages[i].hash);
                    }
                }
                catch (System.Data.SqlClient.SqlException e)
                {
                    Console.WriteLine(e.ToString());
                }
            }
        }

        public static bool IsPackageNameTaken(UInt32 hash)
        {
            if (sqlConnection != null)
            {
                try
                {
                    SqlDataReader myReader = null;
                    SqlCommand myCommand = new SqlCommand("SELECT [HashCode] FROM [GODZ].[dbo].[Packages] WHERE HashCode = " + hash, sqlConnection);
                    myReader = myCommand.ExecuteReader();

                    bool ret = false;

                    if (myReader.Read())
                    {
                        ret = true;
                    }

                    myReader.Close();

                    //we didn't find the hash code so it's unique
                    return ret;
                }
                catch (System.Data.SqlClient.SqlException e)
                {
                    Console.WriteLine(e.ToString());
                }
            }
            else //load the local HString table file, check it....
            {
            }

            return false;
        }

        public static bool AddPackage(UInt32 hash, String location, PackageType type)
        {
            if (sqlConnection != null)
            {
                try
                {
                    String value = "Values (" + hash + ", '" + location + "'," + (int)type + ")";
                    SqlCommand insertCommand = new SqlCommand("INSERT INTO Packages (HashCode, RelativeLocation, Type) " + value, sqlConnection);
                    insertCommand.ExecuteNonQuery();
                }
                catch (SqlException e)
                {
                    Console.WriteLine(e.ToString());
                    return false;
                }                
            }

            return true;
        }
        #endregion


        #region "Object XML"
        //Converts all the properties that belongs to this entity into XML
        public static String GetXML(GodzGlue.ObjectBase entity)
        {
            XmlDocument doc = new XmlDocument();
            XmlDeclaration dec = doc.CreateXmlDeclaration("1.0", null, null);
            doc.AppendChild(dec);// Create the root element
            XmlElement root = doc.CreateElement("Entity");
            doc.AppendChild(root);

            //compare properties to a default object... If it's different, then send to SQL server
            ClassBase gc = entity.getClass();
            GodzGlue.ObjectBase defaultObj = GodzGlue.ObjectBase.newInstance(gc.getObjectName());

            List<GodzGlue.ClassPropertyInfo> cpList = new List<GodzGlue.ClassPropertyInfo>();
            entity.getPropertyValues(cpList);

            List<GodzGlue.ClassPropertyInfo> defList = new List<GodzGlue.ClassPropertyInfo>();
            defaultObj.getPropertyValues(defList);

            int num = cpList.Count;
            for (int i = 0; i < num; i++)
            {
                // Don't write out Objects to XML Properties...
                if (cpList[i].mPropertyType != GodzGlue.ClassPropertyType.PropertyType_Object)
                {
                    bool isResource = (cpList[i].mPropertyType == GodzGlue.ClassPropertyType.PropertyType_Resource);
                    bool isHash = cpList[i].mPropertyType == GodzGlue.ClassPropertyType.PropertyType_HashCode;

                    if ((!isResource && !isHash && !cpList[i].mText.Equals(defList[i].mText))
                        || (isResource && cpList[i].mObjectHash != defList[i].mObjectHash)
                        || (isResource && cpList[i].mPackageHash != defList[i].mPackageHash)
                        || (isHash && cpList[i].mObjectHash != defList[i].mObjectHash)
                        )
                    {
                        GodzGlue.ClassPropertyInfo info = cpList[i];
                        XmlElement title = doc.CreateElement(info.mName);
                        if (isResource)
                        {
                            title.SetAttribute("Package", GetHashString(info.mPackageHash));
                            title.SetAttribute("ObjectHash", GetHashString(info.mObjectHash));
                        }

                        if (isHash)
                        {
                            title.InnerText = GetHashString(info.mObjectHash);
                        }
                        else
                        {
                            title.InnerText = info.mText;
                        }

                        root.AppendChild(title);
                    }
                }
            }

            return doc.DocumentElement.OuterXml.ToString();
        }

        public static String GetPropertyFromXML(String propertyName, String xmlData)
        {
            //parse the XML, set properties....
            StringReader reader = new StringReader(xmlData);
            XmlTextReader r = new XmlTextReader(reader);
            r.Read();
            XmlDocument doc = new XmlDocument();
            doc.Load(r);

            XmlNode entityNode = doc.FirstChild;
            int num = entityNode.ChildNodes.Count;
            for (int i = 0; i < num; i++)
            {
                XmlNode n = entityNode.ChildNodes.Item(i);
                if (n.NodeType == XmlNodeType.Element)
                {
                    String nodePropertyName = n.Name;

                    if (propertyName.Equals(nodePropertyName))
                    {
                        return n.InnerText;
                    }
                }
            }

            return null;
        }

        //Set properties via XML data
        public static void ImportXMLData(GodzGlue.ObjectBase newActor, String xmlData)
        {
            //parse the XML, set properties....
            StringReader reader = new StringReader(xmlData);
            XmlTextReader r = new XmlTextReader(reader);
            r.Read();
            XmlDocument doc = new XmlDocument();
            doc.Load(r);

            XmlNode entityNode = doc.FirstChild;
            int num = entityNode.ChildNodes.Count;
            for (int i = 0; i < num; i++)
            {
                XmlNode n = entityNode.ChildNodes.Item(i);
                if (n.NodeType == XmlNodeType.Element)
                {
                    String propertyName = n.Name;

                    GodzGlue.ClassPropertyInfo info = newActor.getPropertyValue(propertyName);

                    //The property was probably dropped from the object
                    if (info == null)
                        continue;

                    //check if it's an object or resource
                    if (info.mPropertyType == GodzGlue.ClassPropertyType.PropertyType_Object || info.mPropertyType == GodzGlue.ClassPropertyType.PropertyType_Resource)
                    {
                        XmlAttributeCollection xattr = n.Attributes;

                        XmlAttribute packageAttr = xattr["Package"];
                        String packageFile = packageAttr.InnerText;
                        uint phash = GodzUtil.GetHashCode(packageFile);

                        XmlAttribute objectAttr = xattr["ObjectHash"];
                        String objText = objectAttr.InnerText;
                        uint objHash = GodzUtil.GetHashCode(objText);

                        //This is an object, so we gotta set this property directly
                        GodzGlue.Package p = GodzGlue.GodzUtil.FindPackage(phash);
                        if (p == null)
                        {
                            //we gotta go load the package
                            if (packageFile != null)
                            {
                                p = GodzGlue.GodzUtil.LoadPackage(packageFile);
                            }
                        }

                        if (p != null)
                        {
                            if (info.mPropertyType == GodzGlue.ClassPropertyType.PropertyType_Object)
                            {
                                GodzGlue.ObjectBase obj = p.FindObject(objHash);
                                if (obj != null)
                                {
                                    newActor.setObject(propertyName, obj);
                                }
                            }
                            else
                            {
                                //resource...
                                GodzGlue.ResourceObject res = p.FindResourceObject(objHash);
                                newActor.setResource(propertyName, res);
                            }
                        }
                    }
                    else
                    {
                        String value = n.InnerText;
                        newActor.setProperty(propertyName, value);
                    }
                }
            }
        }
        #endregion

        #region "XML Serialization"
        public static void SerializeToXml(Object obj, Type objtype)
        {
            XmlSerializer serializer = new XmlSerializer(objtype);
            TextWriter textWriter = new StreamWriter(@"EditorSettings.xml");
            serializer.Serialize(textWriter, obj);
            textWriter.Close();
        }

        public static Object DeserializeFromXml(Type objtype)
        {
            try
            {
                XmlSerializer deserializer = new XmlSerializer(objtype);
                TextReader textReader = new StreamReader(@"EditorSettings.xml");
                Object obj = deserializer.Deserialize(textReader);
                textReader.Close();
                return obj;
            }
            catch (System.IO.FileNotFoundException)
            {
                return null;
            }
            catch (System.InvalidOperationException)
            {
                return null;
            }
        }
        #endregion

        #region "ObjectReferences"
        /// <summary>
        /// Adds or Updates an existing object reference
        /// </summary>
        /// <param name="packageHash"></param>
        /// <param name="propertyNameHash"></param>
        /// <param name="newObject"></param>
        /// <param name="parent"></param>
        public static void UpdateObjectReference(uint propertyNameHash, GodzGlue.ObjectBase newObject, ObjectBase parent, bool isExternal)
        {
            int rows = 0;

            //We assume the object already has an entry in the Objects table.
            //1. See if we already have a record to update
            try
            {
                String value = "UPDATE ObjectReferences Set ObjectGUID = '" + newObject.Id + "' WHERE PropertyHash = " + propertyNameHash + " AND OwnerGUID = '" + parent.Id + "'";
                SqlCommand insertCommand = new SqlCommand(value, sqlConnection);
                rows = insertCommand.ExecuteNonQuery();
            }
            catch (SqlException e)
            {
                Console.WriteLine(e.ToString());
            }

            //2. Add a new record since we lack an entry....
            if (rows == 0)
            {
                try
                {
                    String value = "Values ('" + parent.Id + "', " + propertyNameHash + ", '" + newObject.Id + "', '" + isExternal + "')";
                    SqlCommand insertCommand = new SqlCommand("INSERT INTO ObjectReferences (OwnerGUID, PropertyHash, ObjectGUID, IsExternal) " + value, sqlConnection);
                    insertCommand.ExecuteNonQuery();
                }
                catch (SqlException e)
                {
                    Console.WriteLine(e.ToString());
                }
            }
        }

        public static void AddObjectReference(uint packageHash, uint propertyNameHash, GodzGlue.ObjectBase newObject, ObjectBase parent)
        {
            SubmitNewObject(packageHash, newObject);

            DatabaseObjectInfo parentInfo = DatabaseObjectRegistry.get(parent);
            DatabaseObjectInfo info = DatabaseObjectRegistry.get(newObject);

            //also append a corresponding record to the Containers table
            try
            {
                String value = "Values ('" + parentInfo.objID + "', " + propertyNameHash + ", '" + info.objID + "','False', 0)";
                SqlCommand insertCommand = new SqlCommand("INSERT INTO ObjectReferences (OwnerGUID, PropertyHash, ObjectGUID, IsExternal, Revision) " + value, sqlConnection);
                insertCommand.ExecuteNonQuery();
            }
            catch (SqlException e)
            {
                Console.WriteLine(e.ToString());
            }
        }

        public static void RemoveObjectReference(uint propertyNameHash, ObjectBase containerOwner)
        {
            DatabaseObjectInfo info = DatabaseObjectRegistry.get(containerOwner);

            try
            {
                String selectString = "DELETE FROM ObjectReferences WHERE OwnerGUID = '" + info.objID + "' AND PropertyHash = " + propertyNameHash;
                SqlCommand insertCommand = new SqlCommand(selectString, sqlConnection);
                insertCommand.ExecuteNonQuery();
            }
            catch (SqlException e)
            {
                Console.WriteLine(e.ToString());
            }
        }

        public static void RemoveObjectReference(uint propertyNameHash, GodzGlue.ObjectBase newObject, ObjectBase containerOwner)
        {
            DatabaseObjectInfo ownerInfo = DatabaseObjectRegistry.get(containerOwner);
            DatabaseObjectInfo newInfo = DatabaseObjectRegistry.get(newObject);

            try
            {
                String selectString = "DELETE FROM ObjectReferences WHERE OwnerGUID = '" + ownerInfo.objID + "' AND PropertyHash = " + propertyNameHash + " AND ObjectGUID = '" + newInfo.objID + "'";
                SqlCommand insertCommand = new SqlCommand(selectString, sqlConnection);
                insertCommand.ExecuteNonQuery();
            }
            catch (SqlException e)
            {
                Console.WriteLine(e.ToString());
            }
        }

        // Updates an existing container by adding objects it's missing
        public static void AddMissingObjectsToContainer(uint propertyNameHash, ObjectBase parent, PropertyArray container)
        {
            DatabaseObjectInfo parentInfo = DatabaseObjectRegistry.get(parent);

            List<Guid> missingGuids = new List<Guid>();

            try
            {
                String selectString = "SELECT [ObjectGUID] FROM ObjectReferences WHERE OwnerGUID = '" + parentInfo.objID + "' AND PropertyHash = " + propertyNameHash;

                SqlDataReader myReader = null;
                SqlCommand myCommand = new SqlCommand(selectString, sqlConnection);
                myReader = myCommand.ExecuteReader();

                while (myReader.Read())
                {
                    ContainerData data = new ContainerData();

                    Guid id = (Guid)myReader["ObjectGUID"];

                    // Does the Container store this Database ID???
                    if (container.contains(id) == null)
                    {
                        //Add this guid to a list of missing ids
                        missingGuids.Add(id);
                    }
                }

                myReader.Close();
            }
            catch (SqlException e)
            {
                Console.WriteLine(e.ToString());
                return;
            }

            //Now go find those missing objects and append to this container....
            foreach (Guid guid in missingGuids)
            {
                List<DatabaseObject> dbObjs = new List<DatabaseObject>();
                try
                {
                    String selectString = "SELECT [PackageHashCode],[Properties],[ClassHashCode],[Locked],[Revision] FROM Objects WHERE GUID = '" + guid + "' AND IsDeleted='false'";

                    SqlDataReader myReader = null;
                    SqlCommand myCommand = new SqlCommand(selectString, sqlConnection);
                    myReader = myCommand.ExecuteReader();

                    

                    while (myReader.Read())
                    {
                        DatabaseObject obj = new DatabaseObject();

                        //Set the id on the Object so the new instance will have it
                        obj.objID = guid;
                        obj.importData(myReader, applicationSettings, false);

                        //cache the dbObjects since we will query the DB again...
                        dbObjs.Add(obj);
                    }

                    myReader.Close();
                }
                catch (SqlException e)
                {
                    Console.WriteLine(e.ToString());
                }

                for (int i = 0; i < dbObjs.Count; i++)
                {
                    ObjectBase newObject = dbObjs[i].newInstance();
                    PropertyObject po = new PropertyObject(newObject);
                    container.add(po);
                }
            }
        }

        /// <summary>
        /// Returns a list of child GUIDs
        /// </summary>
        /// <param name="guids"></param>
        /// <param name="owner"></param>
        public static void GetObjectReferences(ref List<Guid> guids, Guid owner)
        {
            try
            {
                String selectString = "SELECT [ObjectGUID] FROM ObjectReferences WHERE OwnerGUID = '" + owner + "'";

                SqlDataReader myReader = null;
                SqlCommand myCommand = new SqlCommand(selectString, sqlConnection);
                myReader = myCommand.ExecuteReader();

                while (myReader.Read())
                {
                    Guid id = (Guid)myReader["ObjectGUID"];

                    guids.Add(id);
                }

                myReader.Close();
            }
            catch (SqlException e)
            {
                Console.WriteLine(e.ToString());
                return;
            }
        }

        //Imports Containers / Object Refs for the Object
        public static void ImportObjectReferences(GodzGlue.ObjectBase godzObject, Package presetPackage)
        {
            DatabaseObjectInfo inf = DatabaseObjectRegistry.get(godzObject);
            List<ContainerData> containerObjects = new List<ContainerData>();

            try
            {
                String selectString = "SELECT [PropertyHash],[ObjectGUID],[IsExternal] FROM ObjectReferences WHERE OwnerGUID = '" + inf.objID + "'";

                SqlDataReader myReader = null;
                SqlCommand myCommand = new SqlCommand(selectString, sqlConnection);
                myReader = myCommand.ExecuteReader();

                while (myReader.Read())
                {
                    ContainerData data = new ContainerData();

                    data.propertyHash = (uint)((Int64)myReader["PropertyHash"]);
                    data.objectID = (Guid)myReader["ObjectGUID"];
                    Object temp = myReader["IsExternal"];
                    data.isExternal = (bool)temp;

                    containerObjects.Add(data);
                }

                myReader.Close();
            }
            catch (SqlException e)
            {
                Console.WriteLine(e.ToString());
                return;
            }

            if (containerObjects.Count > 0)
            {
                //Now we lookup the Object GUIDs and append them to the Container...
                List<ClassPropertyInfo> cpList = new List<ClassPropertyInfo>();
                godzObject.getPropertyValues(cpList);

                for (int i = 0; i < containerObjects.Count; i++)
                {
                    for (int j = 0; j < cpList.Count; j++)
                    {
                        if (containerObjects[i].propertyHash == cpList[j].mPropertyNameHash)
                        {
                            if (containerObjects[i].isExternal)
                            {
                                uint packHash = 0;

                                //load the package the external object belongs to.
                                //find the objects that belong to this container...
                                try
                                {
                                    String selectString = "SELECT [PackageHash] FROM ExternalObjects WHERE GUID = '" + containerObjects[i].objectID + "'";

                                    SqlDataReader myReader = null;
                                    SqlCommand myCommand = new SqlCommand(selectString, sqlConnection);
                                    myReader = myCommand.ExecuteReader();

                                    if (myReader.Read())
                                    {
                                        packHash = (uint)((Int64)myReader["PackageHash"]);
                                    }

                                    myReader.Close();
                                }
                                catch (SqlException e)
                                {
                                    Console.WriteLine(e.ToString());
                                }

                                if (packHash != 0)
                                {
                                    string pname = GetHashString(packHash);
                                    Package p = GodzUtil.LoadPackage(pname);
                                    ObjectBase newObj = p[containerObjects[i].objectID];
                                    if (newObj != null)
                                    {
                                        //We only handle Object types atm...
                                        Debug.Assert(cpList[j].mPropertyType == ClassPropertyType.PropertyType_Object);

                                        if (cpList[j].mPropertyType == ClassPropertyType.PropertyType_Object)
                                        {
                                            godzObject.setObject(cpList[j].mPropertyNameHash, newObj);
                                        }
                                    }
                                }
                            }
                            else
                            {
                                List<DatabaseObject> dbObjects = new List<DatabaseObject>();

                                //find the objects that belong to this container...
                                try
                                {
                                    String selectString = "SELECT [PackageHashCode],[Properties],[ClassHashCode],[Locked],[Revision] FROM Objects WHERE GUID = '" + containerObjects[i].objectID + "' AND IsDeleted = 'false'";

                                    SqlDataReader myReader = null;
                                    SqlCommand myCommand = new SqlCommand(selectString, sqlConnection);
                                    myReader = myCommand.ExecuteReader();

                                    while (myReader.Read())
                                    {
                                        DatabaseObject obj = new DatabaseObject();
                                        obj.objID = containerObjects[i].objectID;
                                        obj.importData(myReader, applicationSettings, false);
                                        dbObjects.Add(obj);
                                    }

                                    myReader.Close();
                                }
                                catch (SqlException e)
                                {
                                    Console.WriteLine(e.ToString());
                                }

                                //iterate through DB List, add to container...
                                for (int k = 0; k < dbObjects.Count; k++)
                                {
                                    //use the preferred package if possible
                                    Package temp = null;
                                    if (presetPackage != null && presetPackage.GetName() == dbObjects[k].packageHash)
                                        temp = presetPackage;

                                    ObjectBase newObj;

                                    if (temp != null)
                                    {
                                        newObj = dbObjects[k].newInstance(temp);
                                    }
                                    else
                                        newObj = dbObjects[k].newInstance();

                                    PropertyArray vector = cpList[j].mArray;
                                    if (vector != null)
                                    {
                                        PropertyObject po = new PropertyObject(newObj);
                                        vector.add(po);
                                    }
                                    else if (cpList[j].mPropertyType == ClassPropertyType.PropertyType_Object)
                                    {
                                        godzObject.setObject(cpList[j].mPropertyNameHash, newObj);
                                    }
                                }
                            } //else isExternal
                        }
                    }
                }
            }
        }
        #endregion
    }    //class Editor
}
