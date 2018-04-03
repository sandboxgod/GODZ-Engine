using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace RyotianEd
{
    /*
     * Info associated with a TabPage
     */
    public class TabPanelData
    {
        public void setViewport(IntPtr view)
        {
            mViewportId = view;
        }

        //viewport HWND
        public IntPtr mViewportId;     

        //Level*
        public GodzGlue.World mWorld;                //World object where all entities are placed
        public GodzGlue.Layer mActiveLayer;         //current layer the user has selected thats active

        public GodzGlue.Camera mPrimaryCamera;       //primary camera for this scene
        public ContextMenuStrip entityOptionMenuStrip; //stores add to world options, etc

        public GodzGlue.SunLight mSun;                  //sun light; should only be 1 in the scene
        public GodzGlue.Entity mSelectedEntity;      //last entity user selected
        public TabPanelListener mListener;
    }

    public interface TabPanelListener
    {
        void FormClosing();
        void PackageLoaded(GodzGlue.Package package);
        void OnSave();
        void OnLostFocus();
        void OnGainedFocus();
        void OnTick();
        void OnKeyDown(object sender, KeyEventArgs e);

        //invoked when a new object is added to a Package
        void OnObjectAdded(GodzGlue.ObjectBase obj);
    }
}
