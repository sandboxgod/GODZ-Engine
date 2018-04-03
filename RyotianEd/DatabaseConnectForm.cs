using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Data.SqlClient;

namespace RyotianEd
{
    public partial class DatabaseConnectForm : Form
    {
        ToolStripStatusLabel mLabel;

        public DatabaseConnectForm(ToolStripStatusLabel statusStrip)
        {
            InitializeComponent();

            mLabel = statusStrip;
        }

        private void dbButton1_Click(object sender, EventArgs e)
        {
            //serialize database connection
            Editor.applicationSettings.DatabaseOptions = new DatabaseSettings();
            DatabaseSettings setting = Editor.applicationSettings.DatabaseOptions;
            setting.server = hostTextBox1.Text;
            setting.user = this.userTextBox2.Text;
            setting.password = passwordTextBox3.Text;

            if (DBConnectHelper.connect(setting))
            {
                String status = "Connected to " + hostTextBox1.Text;
                mLabel.Text = status;

                //serialize application setting
                Editor.SerializeToXml(Editor.applicationSettings, typeof(ApplicationSettings));
                DialogResult = DialogResult.OK;
                Close();
            }
        }
    }

    public class DBConnectHelper
    {
        public static bool connect(DatabaseSettings settings)
        {
            String connectionString;

            connectionString = "user id=" + settings.user + ";";
            connectionString += "password=" + settings.password + ";";
            connectionString += "database=GODZ;";
            connectionString += "server=" + settings.server + ";";

            SqlConnection myConnection = new SqlConnection(connectionString);
            try
            {
                myConnection.Open();
                Editor.sqlConnection = myConnection;
                return true;
            }
            catch (Exception fail)
            {
                Console.WriteLine(fail.ToString());
            }

            return false;
        }
    }
}
