namespace V5EvalKitGUI
{
    partial class connectForm
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(connectForm));
            this.btnCancel = new System.Windows.Forms.Button();
            this.btnOK = new System.Windows.Forms.Button();
            this.boxDeviceList = new System.Windows.Forms.ComboBox();
            this.lblNode = new System.Windows.Forms.Label();
            this.radTXnode = new System.Windows.Forms.RadioButton();
            this.radRXnode = new System.Windows.Forms.RadioButton();
            this.radMaster = new System.Windows.Forms.RadioButton();
            this.radSlave = new System.Windows.Forms.RadioButton();
            this.grpNodeType = new System.Windows.Forms.GroupBox();
            this.grpNodeType.SuspendLayout();
            this.SuspendLayout();
            // 
            // btnCancel
            // 
            this.btnCancel.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btnCancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.btnCancel.Location = new System.Drawing.Point(195, 111);
            this.btnCancel.Name = "btnCancel";
            this.btnCancel.Size = new System.Drawing.Size(75, 23);
            this.btnCancel.TabIndex = 0;
            this.btnCancel.Text = "Cancel";
            this.btnCancel.UseVisualStyleBackColor = true;
            // 
            // btnOK
            // 
            this.btnOK.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btnOK.DialogResult = System.Windows.Forms.DialogResult.OK;
            this.btnOK.Enabled = false;
            this.btnOK.Location = new System.Drawing.Point(114, 111);
            this.btnOK.Name = "btnOK";
            this.btnOK.Size = new System.Drawing.Size(75, 23);
            this.btnOK.TabIndex = 1;
            this.btnOK.Text = "OK";
            this.btnOK.UseVisualStyleBackColor = true;
            // 
            // boxDeviceList
            // 
            this.boxDeviceList.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.boxDeviceList.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.boxDeviceList.FormattingEnabled = true;
            this.boxDeviceList.Location = new System.Drawing.Point(54, 83);
            this.boxDeviceList.Name = "boxDeviceList";
            this.boxDeviceList.Size = new System.Drawing.Size(216, 21);
            this.boxDeviceList.TabIndex = 2;
            this.boxDeviceList.SelectedIndexChanged += new System.EventHandler(this.boxDeviceList_SelectedIndexChanged);
            // 
            // lblNode
            // 
            this.lblNode.AutoSize = true;
            this.lblNode.Location = new System.Drawing.Point(12, 86);
            this.lblNode.Name = "lblNode";
            this.lblNode.Size = new System.Drawing.Size(36, 13);
            this.lblNode.TabIndex = 3;
            this.lblNode.Text = "Node:";
            // 
            // radTXnode
            // 
            this.radTXnode.AutoSize = true;
            this.radTXnode.Location = new System.Drawing.Point(6, 19);
            this.radTXnode.Name = "radTXnode";
            this.radTXnode.Size = new System.Drawing.Size(110, 17);
            this.radTXnode.TabIndex = 4;
            this.radTXnode.TabStop = true;
            this.radTXnode.Text = "As transmitter only";
            this.radTXnode.UseVisualStyleBackColor = true;
            this.radTXnode.CheckedChanged += new System.EventHandler(this.radiobtn_CheckedChanged);
            // 
            // radRXnode
            // 
            this.radRXnode.AutoSize = true;
            this.radRXnode.Location = new System.Drawing.Point(6, 42);
            this.radRXnode.Name = "radRXnode";
            this.radRXnode.Size = new System.Drawing.Size(100, 17);
            this.radRXnode.TabIndex = 5;
            this.radRXnode.TabStop = true;
            this.radRXnode.Text = "As receiver only";
            this.radRXnode.UseVisualStyleBackColor = true;
            this.radRXnode.CheckedChanged += new System.EventHandler(this.radiobtn_CheckedChanged);
            // 
            // radMaster
            // 
            this.radMaster.AutoSize = true;
            this.radMaster.Location = new System.Drawing.Point(122, 42);
            this.radMaster.Name = "radMaster";
            this.radMaster.Size = new System.Drawing.Size(130, 17);
            this.radMaster.TabIndex = 7;
            this.radMaster.TabStop = true;
            this.radMaster.Text = "As bidirectional master";
            this.radMaster.UseVisualStyleBackColor = true;
            this.radMaster.CheckedChanged += new System.EventHandler(this.radiobtn_CheckedChanged);
            // 
            // radSlave
            // 
            this.radSlave.AutoSize = true;
            this.radSlave.Location = new System.Drawing.Point(122, 19);
            this.radSlave.Name = "radSlave";
            this.radSlave.Size = new System.Drawing.Size(124, 17);
            this.radSlave.TabIndex = 6;
            this.radSlave.TabStop = true;
            this.radSlave.Text = "As bidirectional slave";
            this.radSlave.UseVisualStyleBackColor = true;
            this.radSlave.CheckedChanged += new System.EventHandler(this.radiobtn_CheckedChanged);
            // 
            // grpNodeType
            // 
            this.grpNodeType.Controls.Add(this.radTXnode);
            this.grpNodeType.Controls.Add(this.radRXnode);
            this.grpNodeType.Controls.Add(this.radSlave);
            this.grpNodeType.Controls.Add(this.radMaster);
            this.grpNodeType.Location = new System.Drawing.Point(12, 12);
            this.grpNodeType.Name = "grpNodeType";
            this.grpNodeType.Size = new System.Drawing.Size(258, 65);
            this.grpNodeType.TabIndex = 8;
            this.grpNodeType.TabStop = false;
            this.grpNodeType.Text = "Define node...";
            // 
            // connectForm
            // 
            this.AcceptButton = this.btnOK;
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.CancelButton = this.btnCancel;
            this.ClientSize = new System.Drawing.Size(282, 146);
            this.Controls.Add(this.grpNodeType);
            this.Controls.Add(this.lblNode);
            this.Controls.Add(this.boxDeviceList);
            this.Controls.Add(this.btnOK);
            this.Controls.Add(this.btnCancel);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "connectForm";
            this.ShowInTaskbar = false;
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
            this.Text = "Connection throught USB";
            this.grpNodeType.ResumeLayout(false);
            this.grpNodeType.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button btnCancel;
        private System.Windows.Forms.Button btnOK;
        private System.Windows.Forms.ComboBox boxDeviceList;
        private System.Windows.Forms.Label lblNode;
        private System.Windows.Forms.RadioButton radTXnode;
        private System.Windows.Forms.RadioButton radRXnode;
        private System.Windows.Forms.RadioButton radMaster;
        private System.Windows.Forms.RadioButton radSlave;
        private System.Windows.Forms.GroupBox grpNodeType;
    }
}