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
    public partial class SettingsForm : Form
    {
        private MainForm mainForm;

        public void loadState()
        {
            txtTXaddr.Text = nodeSettings.TXaddr.ToString("X4");
            txtRXaddr.Text = nodeSettings.RXaddr.ToString("X4");
            chkPPM.Checked = nodeSettings.PPM;
            chkLFSR.Checked = nodeSettings.BER_LFSR;
            numPktSize.Value = nodeSettings.BER_PktSize;
            numPktRate.Value = (decimal)(nodeSettings.BER_PktRate / 10000.0);
            chkAck.Checked = nodeSettings.Sen_ACK;
            numSenseRate.Value = (decimal)(nodeSettings.Sen_PktRate / 1000.0);
            chkListSensors.SetItemChecked(0, nodeSettings.Sen_EnTemp);
            chkListSensors.SetItemChecked(1, nodeSettings.Sen_EnRH);
            chkListSensors.SetItemChecked(2, nodeSettings.Sen_EnLight);
            chkListSensors.SetItemChecked(3, nodeSettings.Sen_EnAccel);
            cboxTres.SelectedIndex = nodeSettings.Sen_Tres;
            cboxRHres.SelectedIndex = nodeSettings.Sen_RHres;
            cboxLightInt.SelectedIndex = (nodeSettings.Sen_LongLightInt ? 0 : 1);
        }

        private void loadStateAux()
        {
            numSampleRate.Value = (decimal)(nodeSettings.BER_GUIperiod / 1000.0);
            numAvgSpan.Value = (decimal)(nodeSettings.BER_AvgSpan / 1000.0);
            radFlatAvg.Checked = (nodeSettings.BER_AvgType == 0);
            radTriangleAvg.Checked = (nodeSettings.BER_AvgType == 1);
            radHalfLifeAvg.Checked = (nodeSettings.BER_AvgType == 2);
            numHalfLife.Enabled = radHalfLifeAvg.Checked;
            numHalfLife.Value = (decimal)(nodeSettings.BER_AvgHL / 1000.0);
        }

        public SettingsForm(MainForm parent)
        {
            mainForm = parent;
            InitializeComponent();
            loadState();
            loadStateAux();
        }

        public void setTab(int index)
        {
            tabControl1.SelectedIndex = index;
        }

        private void addr_KeyPress(object sender, KeyPressEventArgs e)
        {
            if (e.KeyChar == 0x08 || e.KeyChar == 0x7F || char.IsDigit(e.KeyChar))
                return;
            char key = char.ToUpper(e.KeyChar);
            if (key < 'A' || key > 'F')
                e.Handled = true;
        }

        private void txtTXaddr_LostFocus(object sender, EventArgs e)
        {
            if (txtTXaddr.TextLength == 0)
            {
                MessageBox.Show("You must specify a link layer address for the transmitter.",
                    "Transmitter's address", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                txtTXaddr.Undo();
                txtTXaddr.Focus();
            }
        }

        private void txtRXaddr_LostFocus(object sender, EventArgs e)
        {
            if (txtRXaddr.TextLength == 0)
            {
                MessageBox.Show("You must specify a link layer address for the receiver.",
                    "Receiver's address", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                txtRXaddr.Undo();
                txtRXaddr.Focus();
            }
        }

        private void radFlatAvg_CheckedChanged(object sender, EventArgs e)
        {
            numHalfLife.Enabled = false;
        }

        private void radTriangleAvg_CheckedChanged(object sender, EventArgs e)
        {
            numHalfLife.Enabled = false;
        }

        private void radHalfLifeAvg_CheckedChanged(object sender, EventArgs e)
        {
            numHalfLife.Enabled = true;
        }

        private void saveToDrive(string filename)
        {
            IniFile configFile = new IniFile(filename);
            configFile.Write("TXaddr", txtTXaddr.Text, "nodeSettings");
            configFile.Write("RXaddr", txtRXaddr.Text, "nodeSettings");
            configFile.Write("PPM", chkPPM.Checked.ToString(), "nodeSettings");
            configFile.Write("BER_PktSize", numPktSize.Value.ToString("F0"), "nodeSettings");
            configFile.Write("BER_PktRate", (numPktRate.Value * 10000).ToString("F0"), "nodeSettings");
            configFile.Write("BER_GUIperiod", (numSampleRate.Value * 1000).ToString("F0"), "nodeSettings");
            configFile.Write("BER_AvgSpan", (numAvgSpan.Value * 1000).ToString("F0"), "nodeSettings");
            configFile.Write("BER_AvgHL", (numHalfLife.Value * 1000).ToString("F0"), "nodeSettings");
            if (radHalfLifeAvg.Checked)
                configFile.Write("BER_AvgType", "2", "nodeSettings");
            else if (radTriangleAvg.Checked)
                configFile.Write("BER_AvgType", "1", "nodeSettings");
            else
                configFile.Write("BER_AvgType", "0", "nodeSettings");
            configFile.Write("Sen_ACK", chkAck.Checked.ToString(), "nodeSettings");
            configFile.Write("Sen_PktRate", (numSenseRate.Value * 1000).ToString("F0"), "nodeSettings");
            configFile.Write("Sen_EnTemp", chkListSensors.GetItemChecked(0).ToString(), "nodeSettings");
            configFile.Write("Sen_EnRH", chkListSensors.GetItemChecked(1).ToString(), "nodeSettings");
            configFile.Write("Sen_EnLight", chkListSensors.GetItemChecked(2).ToString(), "nodeSettings");
            configFile.Write("Sen_EnAccel", chkListSensors.GetItemChecked(3).ToString(), "nodeSettings");
            configFile.Write("Sen_Tres", cboxTres.SelectedIndex.ToString("F0"), "nodeSettings");
            configFile.Write("Sen_RHres", cboxRHres.SelectedIndex.ToString("F0"), "nodeSettings");
            configFile.Write("Sen_LongLightInt", (cboxLightInt.SelectedIndex == 0).ToString(), "nodeSettings");
        }

        private void btnApply_Click(object sender, EventArgs e)
        {
            nodeSettings.TXaddr = Convert.ToUInt16(txtTXaddr.Text, 16);
            nodeSettings.RXaddr = Convert.ToUInt16(txtRXaddr.Text, 16);
            nodeSettings.PPM = chkPPM.Checked;
            nodeSettings.BER_LFSR = chkLFSR.Checked;
            nodeSettings.BER_PktSize = (UInt16)numPktSize.Value;
            nodeSettings.BER_PktRate = (UInt16)(numPktRate.Value * 10000);
            nodeSettings.BER_GUIperiod = (UInt16)(numSampleRate.Value * 1000);
            nodeSettings.BER_AvgSpan = (uint)(numAvgSpan.Value * 1000);
            nodeSettings.BER_AvgHL = (uint)(numHalfLife.Value * 1000);
            if (radHalfLifeAvg.Checked)
                nodeSettings.BER_AvgType = 2;
            else if (radTriangleAvg.Checked)
                nodeSettings.BER_AvgType = 1;
            else
                nodeSettings.BER_AvgType = 0;
            nodeSettings.Sen_ACK = chkAck.Checked;
            nodeSettings.Sen_PktRate = (UInt16)(numSenseRate.Value * 1000);
            nodeSettings.Sen_EnTemp = chkListSensors.GetItemChecked(0);
            nodeSettings.Sen_EnRH = chkListSensors.GetItemChecked(1);
            nodeSettings.Sen_EnLight = chkListSensors.GetItemChecked(2);
            nodeSettings.Sen_EnAccel = chkListSensors.GetItemChecked(3);
            nodeSettings.Sen_Tres = cboxTres.SelectedIndex;
            nodeSettings.Sen_RHres = cboxRHres.SelectedIndex;
            nodeSettings.Sen_LongLightInt = (cboxLightInt.SelectedIndex == 0);
            mainForm.updateConfigPkt();
            saveToDrive("settings.ini");
        }

        private void btnCancel_Click(object sender, EventArgs e)
        {
            Close();
        }

        private void btnOK_Click(object sender, EventArgs e)
        {
            btnApply_Click(sender, e);
            btnCancel_Click(sender, e);
        }

        private void btnLoad_Click(object sender, EventArgs e)
        {
            OpenFileDialog FileLoadDialog = new OpenFileDialog();

            FileLoadDialog.Filter = "configuration files (*.ini)|*.ini|All files (*.*)|*.*";
            if (FileLoadDialog.ShowDialog() == DialogResult.OK)
            {
                try
                {
                    IniFile configFile = new IniFile(FileLoadDialog.FileName);
                    txtTXaddr.Text = Convert.ToUInt16(configFile.Read("TXaddr", "nodeSettings"), 16).ToString("X4");
                    txtRXaddr.Text = Convert.ToUInt16(configFile.Read("RXaddr", "nodeSettings"), 16).ToString("X4");
                    chkPPM.Checked = configFile.Read("PPM", "nodeSettings").Equals("True", StringComparison.OrdinalIgnoreCase);
                    numPktSize.Value = Convert.ToDecimal(configFile.Read("BER_PktSize", "nodeSettings"));
                    numPktRate.Value = Convert.ToDecimal(configFile.Read("BER_PktRate", "nodeSettings")) / (decimal)10000;
                    numSampleRate.Value = Convert.ToDecimal(configFile.Read("BER_GUIperiod", "nodeSettings")) / (decimal)1000;
                    numAvgSpan.Value = Convert.ToDecimal(configFile.Read("BER_AvgSpan", "nodeSettings")) / (decimal)1000;
                    radFlatAvg.Checked = configFile.Read("BER_AvgType", "nodeSettings").Equals("0");
                    radTriangleAvg.Checked = configFile.Read("BER_AvgType", "nodeSettings").Equals("1");
                    radHalfLifeAvg.Checked = configFile.Read("BER_AvgType", "nodeSettings").Equals("2");
                    numHalfLife.Enabled = radHalfLifeAvg.Checked;
                    numHalfLife.Value = Convert.ToDecimal(configFile.Read("BER_AvgHL", "nodeSettings")) / (decimal)1000;
                    chkAck.Checked = configFile.Read("Sen_ACK", "nodeSettings").Equals("True", StringComparison.OrdinalIgnoreCase);
                    numSenseRate.Value = Convert.ToDecimal(configFile.Read("Sen_PktRate", "nodeSettings")) / (decimal)1000;
                    chkListSensors.SetItemChecked(0, configFile.Read("Sen_EnTemp", "nodeSettings").Equals("True", StringComparison.OrdinalIgnoreCase));
                    chkListSensors.SetItemChecked(1, configFile.Read("Sen_EnRH", "nodeSettings").Equals("True", StringComparison.OrdinalIgnoreCase));
                    chkListSensors.SetItemChecked(2, configFile.Read("Sen_EnLight", "nodeSettings").Equals("True", StringComparison.OrdinalIgnoreCase));
                    chkListSensors.SetItemChecked(3, configFile.Read("Sen_EnAccel", "nodeSettings").Equals("True", StringComparison.OrdinalIgnoreCase));
                    cboxTres.SelectedIndex = Convert.ToInt32(configFile.Read("Sen_Tres", "nodeSettings"));
                    cboxRHres.SelectedIndex = Convert.ToInt32(configFile.Read("Sen_RHres", "nodeSettings"));
                    cboxLightInt.SelectedIndex = (configFile.Read("Sen_LongLightInt", "nodeSettings").Equals("True", StringComparison.OrdinalIgnoreCase) ? 0 : 1);
                }
                catch (Exception)
                {
                    MessageBox.Show("Error: This configuration file appears to be corrupted thus its content will be ignored.",
                        "Configuration file loading", MessageBoxButtons.OK, MessageBoxIcon.Error);
                    loadState();
                    loadStateAux();
                }
            }
        }

        private void btnSave_Click(object sender, EventArgs e)
        {
            SaveFileDialog FileSaveDialog = new SaveFileDialog();

            FileSaveDialog.Filter = "configuration files (*.ini)|*.ini";
            if (FileSaveDialog.ShowDialog() == DialogResult.OK)
                saveToDrive(FileSaveDialog.FileName);
        }
    }
}
