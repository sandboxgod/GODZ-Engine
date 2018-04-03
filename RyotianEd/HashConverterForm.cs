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
    public partial class HashConverterForm : Form
    {
        public HashConverterForm()
        {
            InitializeComponent();
        }

        private void button1_Click(object sender, EventArgs e)
        {
            uint hash = GodzUtil.GetHashCode(textBox1.Text);
            textBox1.Text = hash.ToString();
        }
    }
}
