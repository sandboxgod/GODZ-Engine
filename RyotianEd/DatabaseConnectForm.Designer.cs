namespace RyotianEd
{
    partial class DatabaseConnectForm
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            this.label1 = new System.Windows.Forms.Label();
            this.bindingSource1 = new System.Windows.Forms.BindingSource(this.components);
            this.hostTextBox1 = new System.Windows.Forms.TextBox();
            this.label2 = new System.Windows.Forms.Label();
            this.userTextBox2 = new System.Windows.Forms.TextBox();
            this.label3 = new System.Windows.Forms.Label();
            this.passwordTextBox3 = new System.Windows.Forms.TextBox();
            this.dbConnectButton1 = new System.Windows.Forms.Button();
            ((System.ComponentModel.ISupportInitialize)(this.bindingSource1)).BeginInit();
            this.SuspendLayout();
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label1.Location = new System.Drawing.Point(0, 9);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(47, 20);
            this.label1.TabIndex = 0;
            this.label1.Text = "Host:";
            // 
            // hostTextBox1
            // 
            this.hostTextBox1.Location = new System.Drawing.Point(104, 9);
            this.hostTextBox1.Name = "hostTextBox1";
            this.hostTextBox1.Size = new System.Drawing.Size(333, 20);
            this.hostTextBox1.TabIndex = 1;
            this.hostTextBox1.Text = ".\\SQLEXPRESS";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label2.Location = new System.Drawing.Point(0, 46);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(47, 20);
            this.label2.TabIndex = 2;
            this.label2.Text = "User:";
            // 
            // userTextBox2
            // 
            this.userTextBox2.Location = new System.Drawing.Point(104, 46);
            this.userTextBox2.Name = "userTextBox2";
            this.userTextBox2.Size = new System.Drawing.Size(332, 20);
            this.userTextBox2.TabIndex = 3;
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label3.Location = new System.Drawing.Point(0, 95);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(82, 20);
            this.label3.TabIndex = 4;
            this.label3.Text = "Password:";
            // 
            // passwordTextBox3
            // 
            this.passwordTextBox3.Location = new System.Drawing.Point(104, 97);
            this.passwordTextBox3.Name = "passwordTextBox3";
            this.passwordTextBox3.PasswordChar = '*';
            this.passwordTextBox3.Size = new System.Drawing.Size(332, 20);
            this.passwordTextBox3.TabIndex = 5;
            // 
            // dbConnectButton1
            // 
            this.dbConnectButton1.Location = new System.Drawing.Point(172, 149);
            this.dbConnectButton1.Name = "dbConnectButton1";
            this.dbConnectButton1.Size = new System.Drawing.Size(172, 31);
            this.dbConnectButton1.TabIndex = 6;
            this.dbConnectButton1.Text = "Connect";
            this.dbConnectButton1.UseVisualStyleBackColor = true;
            this.dbConnectButton1.Click += new System.EventHandler(this.dbButton1_Click);
            // 
            // DatabaseConnectForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(489, 192);
            this.Controls.Add(this.dbConnectButton1);
            this.Controls.Add(this.passwordTextBox3);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.userTextBox2);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.hostTextBox1);
            this.Controls.Add(this.label1);
            this.Name = "DatabaseConnectForm";
            this.Text = "Database Connection Properties";
            ((System.ComponentModel.ISupportInitialize)(this.bindingSource1)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.BindingSource bindingSource1;
        private System.Windows.Forms.TextBox hostTextBox1;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.TextBox userTextBox2;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.TextBox passwordTextBox3;
        private System.Windows.Forms.Button dbConnectButton1;

    }
}