using System;
using System.Collections.Generic;
using System.Drawing;
using System.Windows.Forms;

namespace V5EvalKitGUI
{
    public struct SensorSample
    {
        public DateTime sampleTime;
        public float temp, RH, light, accelX, accelY, accelZ, accelMag;
    }

    public class nodePanel : TableLayoutPanel
    {
        private static bool processingItemCheck = false;

        public bool temp_en, RH_en, light_en, accel_en;
        public List<SensorSample> sampleList;

        private CheckedListBox nodeChkList;
        private Button btnDelete, btnColor;
        private Label lblAddr;
        private TextBox txtAddr;
        private MainForm parent;
        private int index;
        public UInt16 address;

        public nodePanel(MainForm _parent, int _index, UInt16 addr, Color _color, int enabledSensors) : base()
        {
            parent = _parent;
            temp_en  = (enabledSensors & 0x00000080) != 0;
            RH_en    = (enabledSensors & 0x00000040) != 0;
            light_en = (enabledSensors & 0x00000020) != 0;
            accel_en = (enabledSensors & 0x00000010) != 0;

            sampleList = new List<SensorSample>();
            nodeChkList = new CheckedListBox();
            btnDelete = new Button();
            btnColor = new Button();
            lblAddr = new Label();
            txtAddr = new TextBox();

            Anchor = AnchorStyles.Left | AnchorStyles.Top | AnchorStyles.Right;
            BackColor = _color;
            CellBorderStyle = TableLayoutPanelCellBorderStyle.InsetDouble;
            ColumnCount = 1;
            ColumnStyles.Add(new ColumnStyle(SizeType.Percent, 50F));
            Name = "nodePanel";
            RowCount = 1;
            RowStyles.Add(new RowStyle(SizeType.Percent, 50F));
            Size = new Size(_parent.splitContainer7.Width - (int)(2.0f * parent.DpiXratio), parent.nodePanelHeight);
            TabIndex = 0;

            nodeChkList.BackColor = SystemColors.Window;
            nodeChkList.CheckOnClick = true;
            nodeChkList.FormattingEnabled = true;
            nodeChkList.Items.AddRange(new object[] { "Show/Hide all", "Temperature", "Relative humidity", "Light intensity",
                "Acceleration along X axis", "Acceleration along Y axis", "Acceleration along Z axis", "Magnitude of acceleration"});
            nodeChkList.MultiColumn = true;
            nodeChkList.Name = "nodeChkList";
            nodeChkList.Size = new Size((int)(288.0f * parent.DpiXratio), (int)(64.0f * parent.DpiYratio));
            nodeChkList.TabIndex = 0;
            nodeChkList.ThreeDCheckBoxes = true;
            nodeChkList.ColumnWidth = (int)(141.0f * parent.DpiXratio);

            btnDelete.Anchor = AnchorStyles.Top | AnchorStyles.Right;
            btnDelete.Name = "btnDelete";
            btnDelete.Size = new Size((int)(75.0f * parent.DpiXratio), (int)(23.0f * parent.DpiYratio));
            btnDelete.TabIndex = 1;
            btnDelete.Text = "Delete node";
            btnDelete.UseVisualStyleBackColor = true;

            btnColor.Anchor = AnchorStyles.Top | AnchorStyles.Right;
            btnColor.Font = new Font("Microsoft Sans Serif", 8.25F, FontStyle.Bold, GraphicsUnit.Point, (byte)0);
            btnColor.Name = "btnColor";
            btnColor.Size = new Size((int)(75.0f * parent.DpiXratio), (int)(23.0f * parent.DpiYratio));
            btnColor.TabIndex = 2;
            btnColor.Text = "Color";
            btnColor.UseVisualStyleBackColor = true;

            lblAddr.AutoSize = true;
            lblAddr.Name = "lblAddr";
            lblAddr.Size = new Size((int)(110.0f * parent.DpiXratio), (int)(13.0f * parent.DpiYratio));
            lblAddr.TabIndex = 3;
            lblAddr.Text = "Sensor node address:";

            txtAddr.BackColor = SystemColors.Window;
            txtAddr.Name = "txtAddr";
            txtAddr.ReadOnly = true;
            txtAddr.Size = new Size((int)(37.0f * parent.DpiXratio), (int)(20.0f * parent.DpiYratio));
            txtAddr.TabIndex = 4;
            txtAddr.Text = addr.ToString("X4");
            address = addr;

            updateSlot(_index);

            parent.splitContainer7.Panel2.SuspendLayout();
            parent.splitContainer7.Panel2.Controls.Add(nodeChkList);
            parent.splitContainer7.Panel2.Controls.Add(btnDelete);
            parent.splitContainer7.Panel2.Controls.Add(btnColor);
            parent.splitContainer7.Panel2.Controls.Add(lblAddr);
            parent.splitContainer7.Panel2.Controls.Add(txtAddr);
            parent.splitContainer7.Panel2.Controls.Add(this);
            parent.splitContainer7.Panel2.ResumeLayout(false);
            parent.splitContainer7.Panel2.PerformLayout();

            btnDelete.Click += new System.EventHandler((Action<object,EventArgs>)((sender, e) => parent.deleteNode(index) ));
            btnColor.Click += new System.EventHandler(btnColor_Click);
            nodeChkList.ItemCheck += new System.Windows.Forms.ItemCheckEventHandler(this.nodeChkList_ItemCheck);
            if (!temp_en)
                nodeChkList.SetItemCheckState(1, CheckState.Indeterminate);
            if (!RH_en)
                nodeChkList.SetItemCheckState(2, CheckState.Indeterminate);
            if (!light_en)
                nodeChkList.SetItemCheckState(3, CheckState.Indeterminate);
            if (!accel_en)
            {
                nodeChkList.SetItemCheckState(4, CheckState.Indeterminate);
                nodeChkList.SetItemCheckState(5, CheckState.Indeterminate);
                nodeChkList.SetItemCheckState(6, CheckState.Indeterminate);
                nodeChkList.SetItemCheckState(7, CheckState.Indeterminate);
            }
            nodeChkList.SetItemChecked(0, true);
        }
        
        public void delete()
        {
            updatePlots();

            parent.splitContainer7.Panel2.Controls.Remove(nodeChkList);
            parent.splitContainer7.Panel2.Controls.Remove(btnDelete);
            parent.splitContainer7.Panel2.Controls.Remove(btnColor);
            parent.splitContainer7.Panel2.Controls.Remove(lblAddr);
            parent.splitContainer7.Panel2.Controls.Remove(txtAddr);
            parent.splitContainer7.Panel2.Controls.Remove(this);

            sampleList.Clear();
            nodeChkList.Dispose();
            btnDelete.Dispose();
            btnColor.Dispose();
            lblAddr.Dispose();
            txtAddr.Dispose();
            base.Dispose();
        }

        private void nodeChkList_ItemCheck(object sender, ItemCheckEventArgs e)
        {
            if (processingItemCheck)
                return;
            parent.splitContainer1.Panel1.Invalidate(true);
            parent.splitContainer2.Panel1.Invalidate(true);
            parent.splitContainer3.Panel1.Invalidate(true);
            parent.splitContainer4.Panel1.Invalidate(true);
            parent.splitContainer5.Panel1.Invalidate(true);
            parent.splitContainer6.Panel1.Invalidate(true);
            parent.splitContainer7.Panel1.Invalidate(true);
            processingItemCheck = true;
            if (e.Index == 0)
            {
                bool check = (e.NewValue == CheckState.Checked);
                for (int i = 1; i < 8; ++i)
                    if (nodeChkList.GetItemCheckState(i) != CheckState.Indeterminate)
                        nodeChkList.SetItemChecked(i, check);
            }
            else
            {
                CheckState temp;
                if (e.CurrentValue == CheckState.Indeterminate)
                {
                    e.NewValue = CheckState.Indeterminate;
                    processingItemCheck = false;
                    return;
                }
                for (int i = 1; i < 8; ++i)
                {
                    temp = nodeChkList.GetItemCheckState(i);
                    if ((i != e.Index) && (temp != CheckState.Indeterminate) && (temp != e.NewValue))
                    {
                        nodeChkList.SetItemCheckState(0, CheckState.Indeterminate);
                        processingItemCheck = false;
                        return;
                    }
                }
                nodeChkList.SetItemCheckState(0, e.NewValue);
            }
            processingItemCheck = false;
        }

        private void updatePlots()
        {
            if (nodeChkList.GetItemChecked(1))
                parent.splitContainer1.Panel1.Invalidate(true);
            if (nodeChkList.GetItemChecked(2))
                parent.splitContainer2.Panel1.Invalidate(true);
            if (nodeChkList.GetItemChecked(3))
                parent.splitContainer3.Panel1.Invalidate(true);
            if (nodeChkList.GetItemChecked(4))
                parent.splitContainer4.Panel1.Invalidate(true);
            if (nodeChkList.GetItemChecked(5))
                parent.splitContainer5.Panel1.Invalidate(true);
            if (nodeChkList.GetItemChecked(6))
                parent.splitContainer6.Panel1.Invalidate(true);
            if (nodeChkList.GetItemChecked(7))
                parent.splitContainer7.Panel1.Invalidate(true);
        }

        private void btnColor_Click(object sender, EventArgs e)
        {
            ColorDialog colorSelector = new ColorDialog();
            colorSelector.Color = BackColor;
            if (colorSelector.ShowDialog() == DialogResult.OK)
            {
                BackColor = colorSelector.Color;
                updatePlots();
            }
        }

        public void updateSlot(int _index)
        {
            int w = (int)((float)parent.splitContainer7.Width - 81.0f * parent.DpiXratio);
            index = _index;
            Location = new Point(0, parent.nodePanelHeight * _index);
            nodeChkList.Location = new Point((int)(162.0f * parent.DpiXratio), (int)((float)(4 + 72 * _index) * parent.DpiYratio));
            btnDelete.Location = new Point(w, (int)((float)(4 + 72 * _index) * parent.DpiYratio));
            btnColor.Location = new Point(w, (int)((float)(45 + 72 * _index) * parent.DpiYratio));
            lblAddr.Location = new Point((int)(3.0f * parent.DpiXratio), (int)((float)(29 + 72 * _index) * parent.DpiYratio));
            txtAddr.Location = new Point((int)(119.0f * parent.DpiXratio), (int)((float)(26 + 72 * _index) * parent.DpiYratio));
        }

        public bool displaySignal(int i)
        {
            return nodeChkList.GetItemCheckState(i) == CheckState.Checked;
        }
    }
}
