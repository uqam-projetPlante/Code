using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace V5EvalKitGUI
{
    public partial class connectForm : Form
    {
        public connectForm(string[] devList)
        {
            InitializeComponent();
            boxDeviceList.Items.AddRange(devList);
        }

        public string getSelectedText()
        {
            return boxDeviceList.Text;
        }

        public int getSelectedIndex()
        {
            return boxDeviceList.SelectedIndex;
        }

        public RadioType nodeType()
        {
            if (radMaster.Checked)
                return RadioType.bidiMaster;
            if (radSlave.Checked)
                return RadioType.bidiSlave;
            if (radRXnode.Checked)
                return RadioType.RXonly;
            return RadioType.TXonly;
        }

        private void radiobtn_CheckedChanged(object sender, EventArgs e)
        {
            if (boxDeviceList.SelectedIndex >= 0)
                btnOK.Enabled = true;
        }

        private void boxDeviceList_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (radTXnode.Checked || radRXnode.Checked || radSlave.Checked || radMaster.Checked)
                btnOK.Enabled = true;
        }
    }
}
