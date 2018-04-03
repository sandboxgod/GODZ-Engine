using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Runtime.InteropServices;
using System.Collections;
using System.Reflection;
using GodzGlue;

namespace RyotianEd
{
    public partial class MainForm1 : Form
    {
        public Hashtable mTabMap = new Hashtable();
        public TabPage mActiveTabPage = null;
        public List<TabPage> mTabs = new List<TabPage>();
        List<TabPanelListener> mListeners = new List<TabPanelListener>();
        List<GodzGlue.PackageFuture> mFutures = new List<GodzGlue.PackageFuture>();
        List<Package> packages = new List<Package>();
        public static GodzGlue.Vector3 zero = new GodzGlue.Vector3(0, 0, 0);
        public String mDefaultWorldName = "World";
        public bool mCameraInputActive = false;
        private const int WM_ACTIVATEAPP = 0x001C;

        Point origin = new Point();
        System.Windows.Forms.Timer myTimer = null;
        List<String> mLoadedPackages = new List<String>();       //list of loaded packs
        private uint mEditorName = 0;
        Material mDefaultEntityMaterial = null;
        bool isAppActive;

        public MainForm1()
        {
            InitializeComponent();

            mEditorName = GodzUtil.GetHashCode("Editor");

            //We want for the form to display before attempting to connect to SQL. This way if we hit a timeout
            //or something, at least the user has the form to stare at
            connectToDB();

            Editor.initEditor();

            // Grab all the Hash strings and push them into the HStringTable. This way streaming files will work properly.
            // Without this step, loading in package depends will fail...
            Editor.ExportStrings(false, false);

            //make full screen
            Rectangle box = Screen.PrimaryScreen.Bounds;

            //hack so we can see status bar at bottom of the form. Cant set TopMost to true really since that
            //bugs out the form (make debug hard + dialogs not have focus)
            box.Height -= 50;
            SetBounds(0, 0, box.Width, box.Height);
            CenterToScreen();

            //directx-godzengine needs a dummy HWND.....
            GodzUtil.AddViewport(dummyPictureForDirectX.Handle);

            // Make single thread calls to renderer. As of visual studio 2012, running the renderer
            // asynchronously can lead to massive stalls in the Editor. So we have to perform
            // all of our draws in single threaded mode.
            GodzUtil.StartRenderThread(true, false);

            PackageFuture future = GodzUtil.StreamPackage("Editor");
            mFutures.Add(future);

            myTimer = new Timer();
            myTimer.Tick += new EventHandler(MainForm1_Timer);

            //Set to real low for per frame ticks. Run at 60 fps = 0.016
            myTimer.Interval = 1;
            myTimer.Start();

            //Now download all the Class Descriptions, etc
            GodzClassDescriptionRegistry.init();

            this.Focus();
            this.KeyPreview = true;
        }

        private void connectToDB()
        {
            //Connect to Database
            Editor.applicationSettings = (RyotianEd.ApplicationSettings)Editor.DeserializeFromXml(typeof(RyotianEd.ApplicationSettings));
            if (Editor.applicationSettings == null)
            {
                Editor.applicationSettings = new ApplicationSettings();
            }

            if (Editor.applicationSettings.DatabaseOptions != null && DBConnectHelper.connect(Editor.applicationSettings.DatabaseOptions))
            {
                toolStripStatusLabel1.Text = "Connected to " + Editor.applicationSettings.DatabaseOptions.server;
            }
        }

        protected override void WndProc(ref Message m)
        {
            switch (m.Msg)
            {
                // The WM_ACTIVATEAPP message occurs when the application 
                // becomes the active application or becomes inactive. 
                case WM_ACTIVATEAPP:
                    {
                        // The WParam value identifies what is occurring.
                        isAppActive = (((int)m.WParam != 0));

                        // Invalidate to invoke paint...
                        //this.Invalidate();
                    }
                    break;
            }

            base.WndProc( ref m);
        }


        // This is the method to run when the timer is raised.
        private void MainForm1_Timer(Object myObject, EventArgs myEventArgs)
        {
            //tick the game logic so events are dispersed
            //float elapsedTime = GodzUtil.RunMainPass();
            //Console.WriteLine("Elapsed Time " + elapsedTime);
            GodzUtil.RunMainPass();

            TabPanelData currTab = getTabPanel();
            if (currTab != null)
            {
                currTab.mListener.OnTick();
            }

            foreach (GodzGlue.PackageFuture pf in mFutures)
            {
                if (pf.isReady())
                {
                    GodzGlue.Package p = pf.getPackage();
                    packages.Add(p);

                    //iterate through all the trees and update...
                    int len = mTabs.Count;
                    for (int i = 0; i < len; i++)
                    {
                        TabPage tab = mTabs[i];
                        TabPanelData data = (TabPanelData)mTabMap[tab];
                        data.mListener.PackageLoaded(p);
                    }

                    //see if its the editor package
                    if (p.GetName() == mEditorName)
                    {
                        //now assign the editor icon to all Entity objects
                        uint entityName = GodzUtil.GetHashCode("WEntity");
                        ClassBase classBase = ClassBase.findClass(entityName);
                        if (classBase != null)
                        {
                            uint matName = GodzUtil.GetHashCode("icon");
                            mDefaultEntityMaterial = p.FindObject(matName) as Material;
                            if (mDefaultEntityMaterial != null)
                            {
                                classBase.setMaterial(mDefaultEntityMaterial);
                            }
                        }
                    }

                    mFutures.Remove(pf);
                    break; //list was modified while iterating...
                }
            }
        }

        public void centerCursor(PictureBox pictureBox)
        {
            Rectangle r = RectangleToScreen(pictureBox.ClientRectangle);
            origin.X = r.X + r.Width / 2;
            origin.Y = r.Y + r.Height / 2;
            Cursor.Position = origin;
        }



        private void splitContainer1_Panel1_Paint(object sender, PaintEventArgs e)
        {
        }

        private void worldToolStripMenuItem_Click(object sender, EventArgs e)
        {
            sendLostFocusEvent();

            //hide the DB access panel
            this.mainPanel1.SendToBack();
        
            TabPanelData data = new TabPanelData();
            data.mWorld = GodzUtil.AddLevel();

            mActiveTabPage = new TabPage(mDefaultWorldName);
            tabControl1.TabPages.Add(mActiveTabPage);

            WorldEditorForm form = new WorldEditorForm();
            mListeners.Add(form);
            data.mListener = form;
            form.TopLevel = false;
            form.Visible = true;
            form.Dock = DockStyle.Fill;

            form.Location = new System.Drawing.Point(0, 0);
            form.Name = "worldEditorForm";
            form.setTabData(data, this);

            mActiveTabPage.Controls.Add(form);

            tabControl1.ResumeLayout(false);
            mActiveTabPage.ResumeLayout(false);

            mTabMap.Add(mActiveTabPage, data);
            mTabs.Add(mActiveTabPage);
            tabControl1.SelectedTab = mActiveTabPage;

            form.setViewport();

            //GodzUtil.ResetSunLight();

            //tick the engine-- add the new camera, etc
            GodzUtil.RunMainPass();
            ((TabPanelListener)form).OnGainedFocus();
        }


        private void MainForm1_FormClosed_1(object sender, FormClosedEventArgs e)
        {
            foreach (TabPanelListener p in mListeners)
            {
                p.FormClosing();
            }

            mListeners.Clear();

            mDefaultEntityMaterial.Dispose();
            mDefaultEntityMaterial = null;

            GodzUtil.ExitRenderThread();
            GodzUtil.Close();

            Editor.CloseSQLConnection();
            myTimer.Stop();
            myTimer = null;
        }

        public void onNewObjectAdded(GodzGlue.ObjectBase obj)
        {
            //Notify all the listeners a new object was created
            foreach (TabPanelListener p in mListeners)
            {
                p.OnObjectAdded(obj);
            }
        }

        public TabPanelData getTabPanel()
        {
            if (mActiveTabPage != null)
                return (TabPanelData)mTabMap[mActiveTabPage];

            return null;
        }

        private void sendLostFocusEvent()
        {
            if (mActiveTabPage != null)
            {
                TabPanelData old_data = (TabPanelData)mTabMap[mActiveTabPage];
                if (old_data != null)
                {
                    old_data.mListener.OnLostFocus();
                }
            }
        }

        private void modelToolStripMenuItem_Click(object sender, EventArgs e)
        {
            openFileDialog1.ShowDialog();
        }

        private void openFileDialog1_FileOk(object sender, CancelEventArgs e)
        {
            System.Windows.Forms.OpenFileDialog dlg = (OpenFileDialog)sender;
            if (!mLoadedPackages.Contains(dlg.FileName))
            {
                mLoadedPackages.Add(dlg.FileName);
                mFutures.Add( GodzGlue.GodzUtil.StreamPackage(dlg.FileName) );
            }
        }

        private void levelToolStripMenuItem_Click(object sender, EventArgs e)
        {
            MessageBox.Show("Opening Levels has not been enabled yet...");
        }

        private void tabControl1_Selected(object sender, TabControlEventArgs e)
        {
            sendLostFocusEvent();

            mActiveTabPage = e.TabPage;
            TabPanelData data = (TabPanelData)mTabMap[mActiveTabPage];
            data.mListener.OnGainedFocus();

            //make sure the tab has a viewport....
            if (data.mViewportId.ToInt32() != 0)
            {
                GodzUtil.SetViewport(data.mViewportId);

                if (data.mPrimaryCamera != null)
                {
                    data.mPrimaryCamera.GrabFocus();

                    if (data.mSun != null)
                    {
                        //Inform the rendering pipe about the active sunlight
                        data.mSun.setSunLight();
                    }
                    else
                    {
                        //clear the renderer's active sunlight
                        GodzUtil.ResetSunLight();
                    }

                    //tick game engine so that it will inform renderer we've switched the primary camera
                    GodzUtil.RunMainPass();
                }
            }
        }

        private void MainForm1_Paint(object sender, PaintEventArgs e)
        {
        }

        public void pictureBox1_MouseDown(object sender, MouseEventArgs e)
        {
            if (e.Button == MouseButtons.Right)
            {
                mCameraInputActive = true;

                //Activate Camera input
                Cursor.Hide();

                //center the cursor
                PictureBox pictureBox = sender as PictureBox;
                centerCursor(pictureBox);

                //allows us to receive mouse wheel events
                pictureBox.Focus();
            }
        }

        public void pictureBox1_MouseUp(object sender, MouseEventArgs e)
        {
            if (e.Button == MouseButtons.Right)
            {
                mCameraInputActive = false;

                //Activate Camera input
                Cursor.Show();
            }
        }

        public void pictureBox1_MouseMove(object sender, MouseEventArgs e)
        {
            if (mCameraInputActive && e.Button == MouseButtons.Right)
            {
                TabPanelData data = (TabPanelData)mTabMap[mActiveTabPage];
                GodzGlue.Camera cam = data.mPrimaryCamera;

                PictureBox pictureBox = sender as PictureBox;
                if (pictureBox != null)
                {
                    cam.UpdateView(Cursor.Position.X, Cursor.Position.Y, origin.X, origin.Y, 0.1f);
                    centerCursor(pictureBox);
                }

                //tick the actors so they send events over to renderer
                GodzUtil.RunMainPass();
            }
        }

        public void pictureBox1_MouseWheel(object sender, MouseEventArgs e)
        {
            if (mCameraInputActive)
            {
                TabPanelData data = (TabPanelData)mTabMap[mActiveTabPage];
                GodzGlue.Camera cam = data.mPrimaryCamera;

                float mousedelta = (float)e.Delta / 120.0f;
                cam.MoveForward(mousedelta);

                //tick the actors so they send events over to renderer
                GodzUtil.RunMainPass();
            }
        }

        private void saveToolStripMenuItem_Click(object sender, EventArgs e)
        {
            TabPanelData data = (TabPanelData)mTabMap[mActiveTabPage];

            if (data.mListener != null)
            {
                data.mListener.OnSave();
            }
        }

        private void menuStrip1_ItemClicked(object sender, ToolStripItemClickedEventArgs e)
        {

        }

        private void characterToolStripMenuItem_Click(object sender, EventArgs e)
        {
            sendLostFocusEvent();

            //hide the DB access panel
            this.mainPanel1.SendToBack();

            TabPanelData data = new TabPanelData();
            data.mWorld = GodzUtil.AddLevel();

            mActiveTabPage = new TabPage("Entity");
            tabControl1.TabPages.Add(mActiveTabPage);

            EntityEditorForm form = new EntityEditorForm();
            mListeners.Add(form);
            data.mListener = form;
            form.TopLevel = false;
            form.Visible = true;
            form.Dock = DockStyle.Fill;

            form.Location = new System.Drawing.Point(0, 0);
            form.Name = "EntityEditorForm";
            form.setTabData(data, this);

            mActiveTabPage.Controls.Add(form);

            tabControl1.ResumeLayout(false);
            mActiveTabPage.ResumeLayout(false);

            mTabMap.Add(mActiveTabPage, data);
            mTabs.Add(mActiveTabPage);
            tabControl1.SelectedTab = mActiveTabPage;

            ((TabPanelListener)form).OnGainedFocus();

            form.setViewport();

            //tick the engine-- add the new camera, etc
            GodzUtil.RunMainPass();
        }

        private void databaseSettingsToolStripMenuItem_Click(object sender, EventArgs e)
        {
            DatabaseConnectForm db = new DatabaseConnectForm(toolStripStatusLabel1);
            db.Show();
        }

        private void layersExportToolStripMenuItem_Click(object sender, EventArgs e)
        {
            //TODO: Grab all sectors from the DB and export the layers
            MessageBox.Show("Sorry this feature is not implemented yet...");
        }

        private void pictureBox1_Click(object sender, EventArgs e)
        {

        }

        private void packageToolStripMenuItem_Click(object sender, EventArgs e)
        {
            NewPackageForm dlg = new NewPackageForm();
            if (dlg.ShowDialog() == DialogResult.OK)
            {
                GodzGlue.Package p = dlg.getPackage();

                //iterate through all the trees and update...
                int len = mTabs.Count;
                for (int i = 0; i < len; i++)
                {
                    TabPage tab = mTabs[i];
                    TabPanelData data = (TabPanelData)mTabMap[tab];
                    data.mListener.PackageLoaded(p);
                }
            }
        }

        private void MainForm1_Shown(object sender, EventArgs e)
        {

        }

        private void materialToolStripMenuItem_Click(object sender, EventArgs e)
        {
            sendLostFocusEvent();

            //hide the DB access panel
            this.mainPanel1.SendToBack();

            TabPanelData data = new TabPanelData();
            data.mWorld = GodzUtil.AddLevel();

            mActiveTabPage = new TabPage("Materials");
            tabControl1.TabPages.Add(mActiveTabPage);

            MaterialBrowserForm form = new MaterialBrowserForm();
            mListeners.Add(form);
            data.mListener = form;
            form.TopLevel = false;
            form.Visible = true;
            form.Dock = DockStyle.Fill;

            form.Location = new System.Drawing.Point(0, 0);
            form.Name = "MaterialBrowserForm";
            form.setTabData(data, this);

            mActiveTabPage.Controls.Add(form);

            tabControl1.ResumeLayout(false);
            mActiveTabPage.ResumeLayout(false);

            mTabMap.Add(mActiveTabPage, data);
            mTabs.Add(mActiveTabPage);
            tabControl1.SelectedTab = mActiveTabPage;

            GodzUtil.ResetSunLight();
            form.setViewport();
            ((TabPanelListener)form).OnGainedFocus();
        }

        private void hashConverterToolStripMenuItem_Click(object sender, EventArgs e)
        {
            HashConverterForm form = new HashConverterForm();
            form.ShowDialog();
        }

        private void templateEditorToolStripMenuItem1_Click(object sender, EventArgs e)
        {
            sendLostFocusEvent();

            //hide the DB access panel
            this.mainPanel1.SendToBack();

            TabPanelData data = new TabPanelData();
            data.mWorld = GodzUtil.AddLevel();

            mActiveTabPage = new TabPage("Templates");
            tabControl1.TabPages.Add(mActiveTabPage);

            TemplateEditorForm form = new TemplateEditorForm();
            mListeners.Add(form);
            data.mListener = form;
            form.TopLevel = false;
            form.Visible = true;
            form.Dock = DockStyle.Fill;

            form.Location = new System.Drawing.Point(0, 0);
            form.Name = "TemplateEditorForm";
            //form.setTabData(data, this);

            mActiveTabPage.Controls.Add(form);

            tabControl1.ResumeLayout(false);
            mActiveTabPage.ResumeLayout(false);

            mTabMap.Add(mActiveTabPage, data);
            mTabs.Add(mActiveTabPage);
            tabControl1.SelectedTab = mActiveTabPage;

            GodzUtil.ResetSunLight();
            //form.setViewport();
            ((TabPanelListener)form).OnGainedFocus();
        }

        private void classDescEditorToolStripMenuItem_Click(object sender, EventArgs e)
        {
            ClassDescriptionForm form = new ClassDescriptionForm();
            form.ShowDialog();
        }

        private void stringTableToolStripMenuItem_Click(object sender, EventArgs e)
        {
            Editor.ExportStrings(false, true);
        }

        private void debugStringTableToolStripMenuItem_Click(object sender, EventArgs e)
        {
            Editor.ExportStrings(true, true);
        }

        private void MainForm1_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyCode == Keys.Escape)
            {
                // Return focus to main form
                if (mActiveTabPage != null)
                {
                    mActiveTabPage.Focus();
                }
                return;
            }

            if (mActiveTabPage != null)
            {
                TabPanelData data = (TabPanelData)mTabMap[mActiveTabPage];

                if (data.mListener != null)
                {
                    data.mListener.OnKeyDown(sender, e);
                }
            }
        }
    }    
}
