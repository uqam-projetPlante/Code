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
    public partial class confViewForm : Form
    {
        private MainForm mainForm;

        public confViewForm(MainForm parent)
        {
            mainForm = parent;
            InitializeComponent();
        }

        public void waitingState()
        {
            label1.Text = "Polling for Spark radio configuration...";
            listView1.Enabled = false;
            listView1.Items[0].SubItems[1].Text = "???";
            listView1.Items[1].SubItems[1].Text = "???";
            listView1.Items[2].SubItems[1].Text = "???";
            listView1.Items[3].SubItems[1].Text = "???";
            listView1.Items[4].SubItems[1].Text = "???";
            listView1.Items[5].SubItems[1].Text = "??? bytes";
            listView1.Items[6].SubItems[1].Text = "??? packets/s";
            listView1.Items[7].SubItems[1].Text = "???";
            listView1.Items[8].SubItems[1].Text = "??? packets/s";
            listView1.Items[9].SubItems[1].Text = "???";
            listView1.Items[10].SubItems[1].Text = "???";
            listView1.Items[11].SubItems[1].Text = "???";
            listView1.Items[12].SubItems[1].Text = "???";
            listView1.Items[13].SubItems[1].Text = "??? bits";
            listView1.Items[14].SubItems[1].Text = "??? bits";
            listView1.Items[15].SubItems[1].Text = "??? milliseconds";
        }

        public void updateState(byte[] newConfig)
        {
            string TXaddr = (newConfig[1] * 0x0100 + newConfig[2]).ToString("X4");
            string RXaddr = (newConfig[3] * 0x0100 + newConfig[4]).ToString("X4");

            switch (newConfig[0] & 0x03)
            {
              case 0:
                listView1.Items[1].SubItems[1].Text = "BER meas.";
                break;
              case 1:
                listView1.Items[1].SubItems[1].Text = "Audio Stream";
                break;
              case 2:
                listView1.Items[1].SubItems[1].Text = "File transfer";
                break;
              case 3:
                listView1.Items[1].SubItems[1].Text = "Sensor Meas.";
                break;
            }
            switch (newConfig[0] & 0x0C)
            {
              case 0:
                listView1.Items[0].SubItems[1].Text = "Transmitter";
                listView1.Items[3].SubItems[1].Text = TXaddr;
                listView1.Items[4].SubItems[1].Text = RXaddr;
                break;
              case 4:
                listView1.Items[0].SubItems[1].Text = "Receiver";
                listView1.Items[3].SubItems[1].Text = RXaddr;
                listView1.Items[4].SubItems[1].Text = TXaddr;
                break;
              case 8:
                listView1.Items[0].SubItems[1].Text = "Bidi Slave";
                listView1.Items[3].SubItems[1].Text = TXaddr;
                listView1.Items[4].SubItems[1].Text = RXaddr;
                break;
              case 12:
                listView1.Items[0].SubItems[1].Text = "Bidi Master";
                listView1.Items[3].SubItems[1].Text = RXaddr;
                listView1.Items[4].SubItems[1].Text = TXaddr;
                break;
            }
            listView1.Items[2].SubItems[1].Text = ((newConfig[0] & 0x10) != 0 ? "PPM" : "OOK");
            listView1.Items[5].SubItems[1].Text = newConfig[5] + " bytes";
            listView1.Items[6].SubItems[1].Text = (10000.0 / (newConfig[6] * 0x0100 + newConfig[7])).ToString("F") + " packets/s";
            listView1.Items[7].SubItems[1].Text = ((newConfig[11] & 0x02) != 0 ? "Enabled" : "Disabled");
            listView1.Items[8].SubItems[1].Text = (1000.0 / (newConfig[8] * 0x0100 + newConfig[9])).ToString("F") + " packets/s";
            listView1.Items[9].SubItems[1].Text = ((newConfig[10] & 0x80) != 0 ? "Enabled" : "Disabled");
            listView1.Items[10].SubItems[1].Text = ((newConfig[10] & 0x40) != 0 ? "Enabled" : "Disabled");
            listView1.Items[11].SubItems[1].Text = ((newConfig[10] & 0x20) != 0 ? "Enabled" : "Disabled");
            listView1.Items[12].SubItems[1].Text = ((newConfig[10] & 0x10) != 0 ? "Enabled" : "Disabled");
            switch (newConfig[10] & 0x0C)
            {
              case 0:
                listView1.Items[13].SubItems[1].Text = "14 bits";
                break;
              case 4:
                listView1.Items[13].SubItems[1].Text = "13 bits";
                break;
              case 8:
                listView1.Items[13].SubItems[1].Text = "12 bits";
                break;
              case 12:
                listView1.Items[13].SubItems[1].Text = "11 bits";
                break;
            }
            switch (newConfig[10] & 0x03)
            {
              case 0:
                listView1.Items[14].SubItems[1].Text = "12 bits";
                break;
              case 1:
                listView1.Items[14].SubItems[1].Text = "11 bits";
                break;
              case 2:
                listView1.Items[14].SubItems[1].Text = "10 bits";
                break;
              case 3:
                listView1.Items[14].SubItems[1].Text = "8 bits";
                break;
            }
            listView1.Items[15].SubItems[1].Text = ((newConfig[11] & 0x01) != 0 ? "800 milliseconds" : "100 milliseconds");
            listView1.Enabled = true;
            label1.Text = "Listing Spark radio configuration parameters of\r\n" + mainForm.USBdev.nodeLabel + '.';
        }

        private void btnSavetoNVM_Click(object sender, EventArgs e)
        {
            mainForm.USBdev.send(new byte[] { (byte)USBhdr_PC.toNVM }, 200);
            waitingState();
        }

        private void btnImport_Click(object sender, EventArgs e)
        {
            mainForm.importConfig(sender, e);
        }

        private void btnClose_Click(object sender, EventArgs e)
        {
            Close();
        }
    }
}
