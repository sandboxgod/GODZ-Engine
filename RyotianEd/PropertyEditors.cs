using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.ComponentModel;
using System.Windows.Forms;
using System.Windows.Forms.Design;
using System.Drawing.Design;
using GodzGlue;

namespace RyotianEd
{
    #region "PropertyGridValues"
    /// <summary>
    /// Stores user data that stores data for the property editor
    /// </summary>
    public class PropertyContextData
    {
        public ObjectBase context1;         //user-data
    }

    public class PropertyClassSelection
    {
        public ObjectBase obj;
        public ClassPropertyInfo cp;
        public TreeNode treeNode;
        public TreeNode parentNode;
    }

    public class PropertyGridValue
    {
        public string value { get; set; }
        public PropertyClassSelection mPropertySelection;
    }

    public class PropertyString
    {
        public string text { get; set; }
        public ObjectBase obj;

        public override string ToString()
        {
            return text;
        }
    }
    #endregion

    #region "DropDownList"

    /// http://peterblum.com/UITypeEditors/UITypeEditorsHome.aspx
    /// <remarks>
    /// BaseDropDownListTypeEditor can be subclassed to build UITypeEditors that show
    /// a drop down list in the Visual Studio.Net property editor.
    /// You only need to override the FillInList method.
    /// </remarks>
    public abstract class BaseDropDownListTypeEditor : UITypeEditor
    {
        // fEdSrc is created and nulled in EditValue. It is here only to allow the value
        // to be shared with the List_Click event handler
        private IWindowsFormsEditorService fEdSvc = null;
        //------------------------------------------------------------------------
        /// <summary>
        /// GetEditStyle must be overridden for any UITypeEditor.
        /// In this case, we are using a DropDown style.
        /// </summary>
        public override UITypeEditorEditStyle GetEditStyle(ITypeDescriptorContext pContext)
        {
            if (pContext != null && pContext.Instance != null)
            {
                return UITypeEditorEditStyle.DropDown;
            }
            return base.GetEditStyle(pContext);
        }  // GetEditStyle()

        //------------------------------------------------------------------------
        /// <summary>
        /// EditValue must be overridden from UITypeEditor to install a control that appears in the drop down.
        /// For UITypeEditorEditStyle.DropDown, here is the general procedure:
        /// 1. Get the Edit Service from pProvider.GetService(typeof(IWindowsFormsEditorService)).
        ///    It contains methods to run the DropDown and Forms interfaces.
        /// 2. Get a Window control instance that reflects the UI you want. In this case, its a ListBox.
        ///    If you wanted multiple controls, consider something like the Panel class and add controls to its Controls list.
        /// 3. We want the ListBox to close on a Click event much like boolean and enum dropdowns do.
        ///    So we set up a Click event handler that calls CloseDropDown on the Edit Service.
        /// 4. Add data to the ListBox.
        /// 5. Set the initial value of the list. pValue contains that value.
        /// 6. Let the Edit Service open and manage the DropDown interface.
        /// 7. Return the value from the list.
        /// </summary>
        public override object EditValue(ITypeDescriptorContext pContext, IServiceProvider pProvider, object pValue)
        {
            if (pContext != null
               && pContext.Instance != null
               && pProvider != null)
                try
                {
                    // get the editor service
                    fEdSvc = (IWindowsFormsEditorService)
                       pProvider.GetService(typeof(IWindowsFormsEditorService));

                    // create the control(s) we want for the UI
                    ListBox vListBox = new ListBox();
                    vListBox.Click += new EventHandler(List_Click);

                    // modify the list's properties including the Item list
                    FillInList(pContext, pProvider, vListBox);

                    // initialize the selection on the list
                    vListBox.SelectedItem = pValue;

                    // let the editor service place the list on screen and manage its events
                    fEdSvc.DropDownControl(vListBox);

                    // return the updated value;
                    return vListBox.SelectedItem;
                }  // try
                finally
                {
                    fEdSvc = null;
                }
            else
                return pValue;

        }  // EditValue

        /// <summary>
        /// List_Click is a click event handler for the ListBox. We want the have the list
        /// close when the user clicks, just like the Enum and Bool types do in their UITypeEditors.
        /// </summary>
        protected void List_Click(object pSender, EventArgs pArgs)
        {
            fEdSvc.CloseDropDown();
        }  // List_Click

        /// <summary>
        /// FillInList must be overriden to supply properties to the ListBox.
        /// It is essential to assign objects to the ListBox.Items collection.
        /// You can also customize other properties.
        /// Your data does not need to be strings. Window forms list boxes offer
        /// customizable drawing rules which you can use to represent any kind of object. 
        /// </summary>
        protected abstract void FillInList(ITypeDescriptorContext pContext, IServiceProvider pProvider, ListBox pListBox);
    }
    #endregion

    #region "ObjectBaseProxy"
    /// <summary>
    /// See http://www.codeproject.com/KB/tabs/Dynamic_Propertygrid.aspx
    /// </summary>
    internal class ObjectBaseProxy : CollectionBase, ICustomTypeDescriptor
    {
        private ObjectBase godzObject;
        public PropertyContextData contextData;

        public ObjectBaseProxy(ObjectBase obj)
        {
            godzObject = obj;
        }

        public ObjectBase getObject()
        {
            return godzObject;
        }

        /// <summary>
        /// Add CustomProperty to Collectionbase List
        /// </summary>
        /// <param name="Value"></param>
        public void Add(CustomProperty Value)
        {
            base.List.Add(Value);
        }

        /// <summary>
        /// Remove item from List
        /// </summary>
        /// <param name="Name"></param>
        public void Remove(string Name)
        {
            foreach (CustomProperty prop in base.List)
            {
                if (prop.Name == Name)
                {
                    base.List.Remove(prop);
                    return;
                }
            }
        }

        /// <summary>
        /// Indexer
        /// </summary>
        public CustomProperty this[int index]
        {
            get
            {
                return (CustomProperty)base.List[index];
            }
            set
            {
                base.List[index] = (CustomProperty)value;
            }
        }


        #region "TypeDescriptor Implementation"
        /// <summary>
        /// Get Class Name
        /// </summary>
        /// <returns>String</returns>
        public String GetClassName()
        {
            return TypeDescriptor.GetClassName(this, true);
        }

        /// <summary>
        /// GetAttributes
        /// </summary>
        /// <returns>AttributeCollection</returns>
        public AttributeCollection GetAttributes()
        {
            return TypeDescriptor.GetAttributes(this, true);
        }

        /// <summary>
        /// GetComponentName
        /// </summary>
        /// <returns>String</returns>
        public String GetComponentName()
        {
            return TypeDescriptor.GetComponentName(this, true);
        }

        /// <summary>
        /// GetConverter
        /// </summary>
        /// <returns>TypeConverter</returns>
        public TypeConverter GetConverter()
        {
            return TypeDescriptor.GetConverter(this, true);
        }

        /// <summary>
        /// GetDefaultEvent
        /// </summary>
        /// <returns>EventDescriptor</returns>
        public EventDescriptor GetDefaultEvent()
        {
            return TypeDescriptor.GetDefaultEvent(this, true);
        }

        /// <summary>
        /// GetDefaultProperty
        /// </summary>
        /// <returns>PropertyDescriptor</returns>
        public PropertyDescriptor GetDefaultProperty()
        {
            return TypeDescriptor.GetDefaultProperty(this, true);
        }

        /// <summary>
        /// GetEditor
        /// </summary>
        /// <param name="editorBaseType">editorBaseType</param>
        /// <returns>object</returns>
        public object GetEditor(Type editorBaseType)
        {
            return TypeDescriptor.GetEditor(this, editorBaseType, true);
        }

        public EventDescriptorCollection GetEvents(Attribute[] attributes)
        {
            return TypeDescriptor.GetEvents(this, attributes, true);
        }

        public EventDescriptorCollection GetEvents()
        {
            return TypeDescriptor.GetEvents(this, true);
        }

        public PropertyDescriptorCollection GetProperties(Attribute[] attributes)
        {
            PropertyDescriptor[] newProps = new PropertyDescriptor[this.Count];
            for (int i = 0; i < this.Count; i++)
            {

                CustomProperty prop = (CustomProperty)this[i];
                newProps[i] = new CustomPropertyDescriptor(ref prop, attributes);
            }

            return new PropertyDescriptorCollection(newProps);
        }

        public PropertyDescriptorCollection GetProperties()
        {

            return TypeDescriptor.GetProperties(this, true);

        }

        public object GetPropertyOwner(PropertyDescriptor pd)
        {
            return this;
        }
        #endregion
    }
    #endregion

    #region "Custom Properties"
    /// <summary>
    /// Custom property class 
    /// </summary>
    public class CustomProperty
    {
        private string sName = string.Empty;
        private bool bReadOnly = false;
        private bool bVisible = true;

        public ObjectBase godzObject = null;
        public uint propertyNameHash = 0;

        public CustomProperty(string sName, uint propertyHash, ObjectBase value, bool bReadOnly, bool bVisible)
        {
            this.sName = sName;
            this.godzObject = value;
            propertyNameHash = propertyHash;
            this.bReadOnly = bReadOnly;
            this.bVisible = bVisible;
        }

        protected void updateValue(Object value)
        {
            string text = value.ToString();

            ClassPropertyInfo cp = godzObject.getPropertyValue(propertyNameHash);
            if (cp.mPropertyType == GodzGlue.ClassPropertyType.PropertyType_HashCode)
            {
                //deref the old string then addref for the new String in the HashTable...
                //1. Find the old hash in the HashTable....
                if (cp.mObjectHash > 0)
                {
                    Editor.RemoveHash(cp.mObjectHash);
                }

                //2. Add the new String to the HashTable
                uint hash = GodzGlue.GodzUtil.GetHashCode(text);
                Editor.AddHash(hash, text, false);
            }

            godzObject.setProperty(propertyNameHash, text);
            DatabaseObjectInfo info = DatabaseObjectRegistry.get(godzObject);
            if (info != null)
            {
                info.modified = true;
            }
        }

        public bool ReadOnly
        {
            get
            {
                return bReadOnly;
            }
        }

        public string Name
        {
            get
            {
                return sName;
            }
        }

        public bool Visible
        {
            get
            {
                return bVisible;
            }
        }

        public virtual object Value
        {
            get
            {
                ClassPropertyInfo cp = godzObject.getPropertyValue(propertyNameHash);
                string text = cp.mText;

                if (cp.mPropertyType == ClassPropertyType.PropertyType_HashCode)
                {
                    string temp = Editor.GetHashString(cp.mObjectHash);

                    if (temp != null)
                    {
                        text = temp;
                    }
                }
                
                return text;
            }
            set
            {
                updateValue(value);
            }
        }
    }
    #endregion

    #region "MeshList"
    public class MeshDropDownListTypeEditor : BaseDropDownListTypeEditor
    {
        protected override void FillInList(ITypeDescriptorContext context, IServiceProvider provider, ListBox listBox)
        {
            //Note: ITypeDescriptorContext contains the Instance object

            //Look at all the currently loaded packages and see what Meshes
            //are available...
            List<Package> packages = new List<Package>();
            GodzUtil.getPackages(packages);

            foreach (Package p in packages)
            {
                if (p.Type == PackageType.PackageType_RESOURCE)
                {
                    uint size = p.Count;
                    for (uint i = 0; i < size; i++)
                    {
                        ObjectBase oo = p[i];
                        if (oo is Mesh)
                        {
                            Mesh m = (Mesh)oo;
                            uint hash = m.getObjectName();
                            string name = Editor.GetHashString(hash);

                            PropertyString s = new PropertyString();
                            s.text = name;
                            s.obj = m;

                            listBox.Items.Add(s);
                        }
                    }
                }
            }
        }
    }

    [Editor(typeof(MeshDropDownListTypeEditor), typeof(UITypeEditor))]
    public class MeshName
    {
        public string text { get; set; }

        public override string ToString()
        {
            return text;
        }
    }

    public class MeshProperty : CustomProperty
    {
        MeshName p = new MeshName();

        public MeshProperty(string sName, uint propertyHash, ObjectBase value, bool bReadOnly, bool bVisible)
            : base(sName, propertyHash, value, bReadOnly, bVisible)
        {
            // Initialize our MeshReference to the active value
            ClassPropertyInfo cp = godzObject.getPropertyValue(propertyHash);
            string temp = Editor.GetHashString(cp.mObjectHash);

            if (temp == null)
            {
                temp = "";
            }

            p.text = temp;
        }

        public override object Value
        {
            get
            {
                return p;
            }
            set
            {
                p.text = value.ToString();
                PropertyString s = (PropertyString)value;
                godzObject.setObject(propertyNameHash, s.obj);
            }
        }
    }
    #endregion

    #region "AnimationList"
    public class AnimationDropDownListTypeEditor : BaseDropDownListTypeEditor
    {
        protected override void FillInList(ITypeDescriptorContext context, IServiceProvider provider, ListBox listBox)
        {
            //Note: ITypeDescriptorContext contains the Instance object

            // find out which mesh/package it was....
            ObjectBaseProxy objProxy = (ObjectBaseProxy)context.Instance;
            PropertyContextData contextData = objProxy.contextData;

            ClassPropertyInfo packageProperty = contextData.context1.getPropertyValue("Model");
            ObjectBase oo = packageProperty.mObject;

            if (oo != null && oo is Mesh)
            {
                Mesh m = (Mesh)oo;

                List<String> animList = new List<string>();
                m.getAnimations(animList);

                foreach (string animName in animList)
                {
                    listBox.Items.Add(animName);
                }
            }
        }
    }

    [Editor(typeof(AnimationDropDownListTypeEditor), typeof(UITypeEditor))]
    public class AnimationName
    {
        public string text { get; set; }

        public override string ToString()
        {
            return text;
        }
    }


    public class AnimationNameProperty : CustomProperty
    {
        AnimationName p = new AnimationName();

        public AnimationNameProperty(string sName, uint propertyHash, ObjectBase value, bool bReadOnly, bool bVisible)
            : base(sName, propertyHash, value, bReadOnly, bVisible)
        {
            // Initialize to the active value
            ClassPropertyInfo cp = godzObject.getPropertyValue(propertyHash);
            p.text = Editor.GetHashString(cp.mObjectHash);
        }

        public override object Value
        {
            get
            {
                return p;
            }
            set
            {
                p.text = value.ToString();
                updateValue(value);
            }
        }
    }
    #endregion

    #region "PackageList"
    public class PackageDropDownListTypeEditor : BaseDropDownListTypeEditor
    {
        protected override void FillInList(ITypeDescriptorContext context, IServiceProvider provider, ListBox listBox)
        {
            //Note: ITypeDescriptorContext contains the Instance object

            //Get all the packages and add to this dropdown....
            List<PackageInfo> packages = new List<PackageInfo>();
            Editor.GetPackages(ref packages);

            for (int i = 0; i < packages.Count; i++)
            {
                listBox.Items.Add(packages[i].name);
            }
        }
    }

    [Editor(typeof(PackageDropDownListTypeEditor), typeof(UITypeEditor))]
    public class PackageName
    {
        public string text {get; set;}

        public override string ToString()
        {
            return text;
        }
    }

    public class PackageProperty : CustomProperty
    {
        PackageName p = new PackageName();

        public PackageProperty(string sName, uint propertyHash, ObjectBase value, bool bReadOnly, bool bVisible)
            : base(sName, propertyHash, value, bReadOnly, bVisible)
        {
            // Initialize our packageList to the active package value
            ClassPropertyInfo cp = godzObject.getPropertyValue(propertyHash);
            p.text = Editor.GetHashString(cp.mObjectHash);
        }

        public override object Value
        {
            get
            {
                return p;
            }
            set
            {
                p.text = value.ToString();
                updateValue(value);
            }
        }
    }
    #endregion

    #region "ProjectileList"
    public class ProjectileDropDownListTypeEditor : BaseDropDownListTypeEditor
    {
        protected override void FillInList(ITypeDescriptorContext context, IServiceProvider provider, ListBox listBox)
        {
            //Note: ITypeDescriptorContext contains the Instance object

            ClassBase objectTemplateClass = ClassBase.findClass("ProjectileTemplate");

            //Get all the projectiles and add to this dropdown....
            List<DatabaseObject> projectileTemplates = new List<DatabaseObject>();
            Editor.LoadObjectsOfType(ref projectileTemplates, 0, objectTemplateClass);

            for (int i = 0; i < projectileTemplates.Count; i++)
            {
                string name = Editor.GetPropertyFromXML("Name", projectileTemplates[i].xmlData);
                listBox.Items.Add(name);
            }
        }
    }

    [Editor(typeof(ProjectileDropDownListTypeEditor), typeof(UITypeEditor))]
    public class ProjectileName
    {
        public string text { get; set; }

        public override string ToString()
        {
            return text;
        }
    }

    public class ProjectileProperty : CustomProperty
    {
        ProjectileName p = new ProjectileName();

        public ProjectileProperty(string sName, uint propertyHash, ObjectBase value, bool bReadOnly, bool bVisible)
            : base(sName, propertyHash, value, bReadOnly, bVisible)
        {
            // Initialize the projectile name
            ClassPropertyInfo cp = godzObject.getPropertyValue(propertyHash);
            string temp = Editor.GetHashString(cp.mObjectHash);

            if (temp == null)
            {
                temp = "";
            }

            p.text = temp;
        }

        public override object Value
        {
            get
            {
                return p;
            }
            set
            {
                p.text = value.ToString();

                //set the Object reference
                ClassBase objectTemplateClass = ClassBase.findClass("ProjectileTemplate");

                //Get all the projectiles...
                List<DatabaseObject> projectileTemplates = new List<DatabaseObject>();
                Editor.LoadObjectsOfType(ref projectileTemplates, 0, objectTemplateClass);

                for (int i = 0; i < projectileTemplates.Count; i++)
                {
                    string name = Editor.GetPropertyFromXML("Name", projectileTemplates[i].xmlData);
                    if (name.Equals(p.text))
                    {
                        ObjectBase projectileTemplate = projectileTemplates[i].newInstance();
                        godzObject.setObject(propertyNameHash, projectileTemplate);

                        DatabaseObjectInfo info = DatabaseObjectRegistry.get(godzObject);
                        if (info != null)
                        {
                            info.modified = true;
                        }

                        break;
                    }
                }
            }
        }
    }
    #endregion


    #region "ClassList"
    public class ClassTypeDropDownListTypeEditor : BaseDropDownListTypeEditor
    {
        protected override void FillInList(ITypeDescriptorContext context, IServiceProvider provider, ListBox listBox)
        {
            //Note: ITypeDescriptorContext contains the Instance object

            List<ClassBase> classes = new List<ClassBase>();
            ClassBase.getAllClasses(classes);

            foreach (ClassBase godzClass in classes)
            {
                if (!godzClass.isAbstract())
                    listBox.Items.Add(godzClass.ClassName);
            }
        }
    }
    #endregion

    #region "Property Descriptor"
    /// <summary>
    /// Custom PropertyDescriptor
    /// </summary>
    public class CustomPropertyDescriptor : PropertyDescriptor
    {
        CustomProperty m_Property;
        public CustomPropertyDescriptor(ref CustomProperty myProperty, Attribute[] attrs)
            : base(myProperty.Name, attrs)
        {
            m_Property = myProperty;
        }

        public override bool CanResetValue(object component)
        {
            return false;
        }

        public override Type ComponentType
        {
            get
            {
                return null;
            }
        }

        public override object GetValue(object component)
        {
            return m_Property.Value;
        }

        public override string Description
        {
            get
            {
                return m_Property.Name;
            }
        }

        public override string Category
        {
            get
            {
                return string.Empty;
            }
        }

        public override string DisplayName
        {
            get
            {
                return m_Property.Name;
            }

        }



        public override bool IsReadOnly
        {
            get
            {
                return m_Property.ReadOnly;
            }
        }

        public override void ResetValue(object component)
        {
            //Have to implement
        }

        public override bool ShouldSerializeValue(object component)
        {
            return false;
        }

        public override void SetValue(object component, object value)
        {
            m_Property.Value = value;
        }

        public override Type PropertyType
        {
            get { return m_Property.Value.GetType(); }
        }
    }
#endregion
}
