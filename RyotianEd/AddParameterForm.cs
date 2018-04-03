using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace RyotianEd
{
    public partial class AddParameterForm : Form
    {
        public String parameterName;

        public AddParameterForm()
        {
            InitializeComponent();
        }

        private void button1_Click(object sender, EventArgs e)
        {
            parameterName = textBox1.Text;

            DialogResult = DialogResult.OK;
            Close();
        }
    }
}
