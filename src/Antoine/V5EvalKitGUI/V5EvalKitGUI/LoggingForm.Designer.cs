namespace V5EvalKitGUI
{
    partial class LoggingForm
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(LoggingForm));
            this.btnOK = new System.Windows.Forms.Button();
            this.btnCancel = new System.Windows.Forms.Button();
            this.textBox1 = new System.Windows.Forms.TextBox();
            this.btnDir = new System.Windows.Forms.Button();
            this.chkAppendTime = new System.Windows.Forms.CheckBox();
            this.chkTimeData = new System.Windows.Forms.CheckBox();
            this.grpExistingFile = new System.Windows.Forms.GroupBox();
            this.radAppend = new System.Windows.Forms.RadioButton();
            this.radOverwrite = new System.Windows.Forms.RadioButton();
            this.lblFile = new System.Windows.Forms.Label();
            this.grpExistingFile.SuspendLayout();
            this.SuspendLayout();
            // 
            // btnOK
            // 
            this.btnOK.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btnOK.DialogResult = System.Windows.Forms.DialogResult.OK;
            this.btnOK.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.btnOK.Location = new System.Drawing.Point(286, 91);
            this.btnOK.Name = "btnOK";
            this.btnOK.Size = new System.Drawing.Size(75, 23);
            this.btnOK.TabIndex = 0;
            this.btnOK.Text = "OK";
            this.btnOK.UseVisualStyleBackColor = true;
            this.btnOK.Click += new System.EventHandler(this.btnOK_Click);
            // 
            // btnCancel
            // 
            this.btnCancel.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btnCancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.btnCancel.Location = new System.Drawing.Point(367, 91);
            this.btnCancel.Name = "btnCancel";
            this.btnCancel.Size = new System.Drawing.Size(75, 23);
            this.btnCancel.TabIndex = 1;
            this.btnCancel.Text = "Cancel";
            this.btnCancel.UseVisualStyleBackColor = true;
            // 
            // textBox1
            // 
            this.textBox1.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.textBox1.Location = new System.Drawing.Point(44, 15);
            this.textBox1.Name = "textBox1";
            this.textBox1.Size = new System.Drawing.Size(369, 20);
            this.textBox1.TabIndex = 2;
            this.textBox1.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.textBox1_KeyPress);
            this.textBox1.LostFocus += new System.EventHandler(this.textBox1_LostFocus);
            // 
            // btnDir
            // 
            this.btnDir.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.btnDir.BackColor = System.Drawing.SystemColors.Control;
            this.btnDir.BackgroundImage = ((System.Drawing.Image)(resources.GetObject("btnDir.BackgroundImage")));
            this.btnDir.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Zoom;
            this.btnDir.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.btnDir.Location = new System.Drawing.Point(419, 12);
            this.btnDir.Name = "btnDir";
            this.btnDir.Size = new System.Drawing.Size(23, 23);
            this.btnDir.TabIndex = 3;
            this.btnDir.TextAlign = System.Drawing.ContentAlignment.BottomCenter;
            this.btnDir.UseVisualStyleBackColor = false;
            this.btnDir.Click += new System.EventHandler(this.btnDir_Click);
            // 
            // chkAppendTime
            // 
            this.chkAppendTime.AutoSize = true;
            this.chkAppendTime.Location = new System.Drawing.Point(152, 45);
            this.chkAppendTime.Name = "chkAppendTime";
            this.chkAppendTime.Size = new System.Drawing.Size(139, 17);
            this.chkAppendTime.TabIndex = 4;
            this.chkAppendTime.Text = "Append time to filename";
            this.chkAppendTime.UseVisualStyleBackColor = true;
            // 
            // chkTimeData
            // 
            this.chkTimeData.AutoSize = true;
            this.chkTimeData.Location = new System.Drawing.Point(152, 68);
            this.chkTimeData.Name = "chkTimeData";
            this.chkTimeData.Size = new System.Drawing.Size(139, 17);
            this.chkTimeData.TabIndex = 6;
            this.chkTimeData.Text = "Add sample time to data";
            this.chkTimeData.UseVisualStyleBackColor = true;
            // 
            // grpExistingFile
            // 
            this.grpExistingFile.Controls.Add(this.radAppend);
            this.grpExistingFile.Controls.Add(this.radOverwrite);
            this.grpExistingFile.Location = new System.Drawing.Point(12, 41);
            this.grpExistingFile.Name = "grpExistingFile";
            this.grpExistingFile.Size = new System.Drawing.Size(134, 65);
            this.grpExistingFile.TabIndex = 7;
            this.grpExistingFile.TabStop = false;
            this.grpExistingFile.Text = "Existing Files";
            // 
            // radAppend
            // 
            this.radAppend.AutoSize = true;
            this.radAppend.Location = new System.Drawing.Point(6, 42);
            this.radAppend.Name = "radAppend";
            this.radAppend.Size = new System.Drawing.Size(123, 17);
            this.radAppend.TabIndex = 8;
            this.radAppend.TabStop = true;
            this.radAppend.Text = "Append at end of file";
            this.radAppend.UseVisualStyleBackColor = true;
            // 
            // radOverwrite
            // 
            this.radOverwrite.AutoSize = true;
            this.radOverwrite.Checked = true;
            this.radOverwrite.Location = new System.Drawing.Point(6, 19);
            this.radOverwrite.Name = "radOverwrite";
            this.radOverwrite.Size = new System.Drawing.Size(86, 17);
            this.radOverwrite.TabIndex = 8;
            this.radOverwrite.TabStop = true;
            this.radOverwrite.Text = "Overwrite file";
            this.radOverwrite.UseVisualStyleBackColor = true;
            // 
            // lblFile
            // 
            this.lblFile.AutoSize = true;
            this.lblFile.Location = new System.Drawing.Point(12, 18);
            this.lblFile.Name = "lblFile";
            this.lblFile.Size = new System.Drawing.Size(26, 13);
            this.lblFile.TabIndex = 8;
            this.lblFile.Text = "File:";
            // 
            // LoggingForm
            // 
            this.AcceptButton = this.btnOK;
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.CancelButton = this.btnCancel;
            this.ClientSize = new System.Drawing.Size(454, 126);
            this.Controls.Add(this.lblFile);
            this.Controls.Add(this.grpExistingFile);
            this.Controls.Add(this.chkTimeData);
            this.Controls.Add(this.chkAppendTime);
            this.Controls.Add(this.btnDir);
            this.Controls.Add(this.textBox1);
            this.Controls.Add(this.btnCancel);
            this.Controls.Add(this.btnOK);
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.MaximizeBox = false;
            this.MinimizeBox = false;
//            this.MinimumSize = new System.Drawing.Size(320 + Size.Width - ClientSize.Width, 126 + Size.Height - ClientSize.Height);
            this.Name = "LoggingForm";
            this.ShowInTaskbar = false;
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
            this.Text = "Data Logging Options";
            this.grpExistingFile.ResumeLayout(false);
            this.grpExistingFile.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button btnOK;
        private System.Windows.Forms.Button btnCancel;
        private System.Windows.Forms.TextBox textBox1;
        private System.Windows.Forms.Button btnDir;
        private System.Windows.Forms.CheckBox chkAppendTime;
        private System.Windows.Forms.CheckBox chkTimeData;
        private System.Windows.Forms.GroupBox grpExistingFile;
        private System.Windows.Forms.RadioButton radOverwrite;
        private System.Windows.Forms.RadioButton radAppend;
        private System.Windows.Forms.Label lblFile;
    }
}