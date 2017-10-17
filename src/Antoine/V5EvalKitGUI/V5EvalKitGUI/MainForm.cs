using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.IO;

namespace V5EvalKitGUI
{
    public partial class MainForm : Form
    {
        public static Color[] defaultColors = { Color.Red, Color.Green, Color.Blue, Color.Yellow, Color.Cyan, Color.Magenta };

        public float DpiXratio, DpiYratio;
        public int nodePanelHeight;

        private RadioType radio;
        public byte[] currentConfig = new byte[12] { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
        private confViewForm confViewer = null;
        private SettingsForm setForm = null;
        private StreamWriter logFile = null;
        public USBmanager USBdev = null;
        private Thread USBthread = null;

        private double maxBER, minBER, zoom;
        private DateTime lastSampleTime = DateTime.MaxValue;
        private float[] currentPktDist = new float[32];
        private List<BERsample> sampleList = new List<BERsample>();
        private List<nodePanel> nodes = new List<nodePanel>();

        private System.Threading.Timer pollTimer;

        private struct BERsample
        {
            public DateTime sampleTime;
            public double BER, avg;
            public byte RSSI, RNSI, LinkMargin;
            public UInt16 pktSent;
            public UInt16[] pktErrorDist;

            public BERsample(DateTime time)
            {
                sampleTime = time;
                BER = avg = 0.0;
                RSSI = RNSI = LinkMargin = 0;
                pktSent = 0;
                pktErrorDist = new UInt16[32];
            }
        }

        public MainForm()
        {
            InitializeComponent();
            chkListPlotsSel.SetItemChecked(0, true);
            chkListPlotsSel.SetItemChecked(1, true);
            chkListPlotsSel.SetItemChecked(2, true);
            chkListPlotsSel.SetItemChecked(6, true);
            chkListPlotsSel.ItemCheck += chkListPlotsSel_ItemCheck;
            splitContainer1.Panel1.SizeChanged += adjustPanel;
            splitContainer2.Panel1.SizeChanged += adjustPanel;
            splitContainer3.Panel1.SizeChanged += adjustPanel;
            splitContainer4.Panel1.SizeChanged += adjustPanel;
            splitContainer5.Panel1.SizeChanged += adjustPanel;
            splitContainer6.Panel1.SizeChanged += adjustPanel;
            splitContainer7.Panel1.SizeChanged += adjustPanel;

            typeof(Panel).InvokeMember("DoubleBuffered", System.Reflection.BindingFlags.SetProperty
                                                       | System.Reflection.BindingFlags.Instance
                                                       | System.Reflection.BindingFlags.NonPublic,
                                                       null, splitContainer1.Panel1, new object[] { true });
            typeof(Panel).InvokeMember("DoubleBuffered", System.Reflection.BindingFlags.SetProperty
                                                       | System.Reflection.BindingFlags.Instance
                                                       | System.Reflection.BindingFlags.NonPublic,
                                                       null, splitContainer2.Panel1, new object[] { true });
            typeof(Panel).InvokeMember("DoubleBuffered", System.Reflection.BindingFlags.SetProperty
                                                       | System.Reflection.BindingFlags.Instance
                                                       | System.Reflection.BindingFlags.NonPublic,
                                                       null, splitContainer3.Panel1, new object[] { true });
            typeof(Panel).InvokeMember("DoubleBuffered", System.Reflection.BindingFlags.SetProperty
                                                       | System.Reflection.BindingFlags.Instance
                                                       | System.Reflection.BindingFlags.NonPublic,
                                                       null, splitContainer4.Panel1, new object[] { true });
            typeof(Panel).InvokeMember("DoubleBuffered", System.Reflection.BindingFlags.SetProperty
                                                       | System.Reflection.BindingFlags.Instance
                                                       | System.Reflection.BindingFlags.NonPublic,
                                                       null, splitContainer5.Panel1, new object[] { true });
            typeof(Panel).InvokeMember("DoubleBuffered", System.Reflection.BindingFlags.SetProperty
                                                       | System.Reflection.BindingFlags.Instance
                                                       | System.Reflection.BindingFlags.NonPublic,
                                                       null, splitContainer6.Panel1, new object[] { true });
            typeof(Panel).InvokeMember("DoubleBuffered", System.Reflection.BindingFlags.SetProperty
                                                       | System.Reflection.BindingFlags.Instance
                                                       | System.Reflection.BindingFlags.NonPublic,
                                                       null, splitContainer7.Panel1, new object[] { true });
            typeof(Panel).InvokeMember("DoubleBuffered", System.Reflection.BindingFlags.SetProperty
                                                       | System.Reflection.BindingFlags.Instance
                                                       | System.Reflection.BindingFlags.NonPublic,
                                                       null, BERgraphpanel, new object[] { true });

            using (Graphics g = CreateGraphics())
            {
                DpiXratio = g.DpiX / 96.0f;
                DpiYratio = g.DpiY / 96.0f;
                nodePanelHeight = (int)(72.0f * DpiYratio);
            }

            zoom = Math.Pow(10.0, (double)sliderZoom.Value / 30.0); // make constant

            pollTimer = new System.Threading.Timer(pollTimer_Tick);

            /* Fixing bullshit */
            SensPanel.VisibleChanged += manualAdjustments;
        }

        /* Fixing bullshit */
        private void manualAdjustments(object sender, EventArgs e)
        {
            btnHidePlot1.Location = new Point(splitContainer1.Width - (int)(4.0f * DpiYratio) - btnHidePlot1.Margin.Right - btnHidePlot1.Width, btnHidePlot1.Location.Y);
            btnHidePlot2.Location = new Point(splitContainer2.Width - (int)(4.0f * DpiYratio) - btnHidePlot2.Margin.Right - btnHidePlot2.Width, btnHidePlot2.Location.Y);
            btnHidePlot3.Location = new Point(splitContainer3.Width - (int)(4.0f * DpiYratio) - btnHidePlot3.Margin.Right - btnHidePlot3.Width, btnHidePlot3.Location.Y);
            btnHidePlot4.Location = new Point(splitContainer4.Width - (int)(4.0f * DpiYratio) - btnHidePlot4.Margin.Right - btnHidePlot4.Width, btnHidePlot4.Location.Y);
            btnHidePlot5.Location = new Point(splitContainer5.Width - (int)(4.0f * DpiYratio) - btnHidePlot5.Margin.Right - btnHidePlot5.Width, btnHidePlot5.Location.Y);
            btnHidePlot6.Location = new Point(splitContainer6.Width - (int)(4.0f * DpiYratio) - btnHidePlot6.Margin.Right - btnHidePlot6.Width, btnHidePlot6.Location.Y);
            btnHidePlot7.Location = new Point(splitContainer7.Width - (int)(4.0f * DpiYratio) - btnHidePlot7.Margin.Right - btnHidePlot7.Width, btnHidePlot7.Location.Y);
            splitContainer4.Panel1Collapsed = true;
            splitContainer5.Panel1Collapsed = true;
            splitContainer6.Panel1Collapsed = true;
            adjustPanel();
            SensPanel.VisibleChanged -= manualAdjustments;
        }

        public void setLightUnsynced()
        {
            picRsync.Visible = true;
            picYsync.Visible = false;
            picGsync.Visible = false;
            lblSync.Text = "Unsynced";
        }

        public void setLightSyncing()
        {
            picRsync.Visible = false;
            picYsync.Visible = true;
            picGsync.Visible = false;
            lblSync.Text = "Syncing";
        }

        public void setLightSynced()
        {
            picRsync.Visible = false;
            picYsync.Visible = false;
            picGsync.Visible = true;
            lblSync.Text = "Synced";
        }

        public void processConfig()
        {
            if (confViewer is confViewForm && !confViewer.IsDisposed)
                confViewer.updateState(currentConfig);
            if ((currentConfig[0] & 0x80) == 0)
                btnStart.Enabled = true; // currentConfig.SequenceEqual(nodeSettings.configPkt);
            else
            {
                comboBox1.SelectedIndex = currentConfig[0] & 0x03;
                btnStart.Enabled = true;
                GUIstart();
                currentConfig[0] &= 0x7F;
            }
        }

        public void addBERsample(DateTime sampleTime, byte[] numbers)
        {
            if (comboBox1.Enabled || (comboBox1.SelectedIndex != 0) || (radio == RadioType.TXonly))
                return;
            Func<double, double> getWeight;
            double weight, totalWeight = 1.0;
            uint numErrors, numReceived = 0;
            double[] pktOccurences = new double[32];
            TimeSpan timeSpan;
            BERsample sample = new BERsample(sampleTime);

            if (BitConverter.IsLittleEndian)
            {
                numbers = numbers.Reverse().ToArray();
                numErrors = BitConverter.ToUInt32(numbers, 69);
                sample.RSSI = numbers[68];
                sample.RNSI = numbers[67];
                sample.LinkMargin = numbers[66];
                sample.pktSent = BitConverter.ToUInt16(numbers, 64);
                for (int i = 0; i < 32; ++i)
                    sample.pktErrorDist[i] = BitConverter.ToUInt16(numbers, 2 * (31 - i));
            }
            else
            {
                numErrors = BitConverter.ToUInt32(numbers, 0);
                sample.RSSI = numbers[4];
                sample.RNSI = numbers[5];
                sample.LinkMargin = numbers[6];
                sample.pktSent = BitConverter.ToUInt16(numbers, 7);
                for (int i = 0; i < 32; ++i)
                    sample.pktErrorDist[i] = BitConverter.ToUInt16(numbers, 2 * i + 9);
            }
            for (int i = 0; i < 31; ++i)
                numReceived += sample.pktErrorDist[i];

            if (numReceived <= 0)
            {
                txtRSSI.Text = "??";
                txtRNSI.Text = "??";
                txtMargin.Text = "?? dB";
                txtBERavg.Text = "???";
                txtSample.Text = "???";
                txtSent.Text = sample.pktSent.ToString();
                txtReceived.Text = "0";
                txtMissing.Text = sample.pktErrorDist[31].ToString();
                return;
            }

            switch (nodeSettings.BER_AvgType)
            {
                case 0:
                    getWeight = t => 1.0;
                    break;
                case 1:
                    getWeight = t => 1.0 - t / (double)nodeSettings.BER_AvgSpan;
                    break;
                case 2:
                    getWeight = t => Math.Pow(0.5, t / (double)nodeSettings.BER_AvgHL);
                    break;
                default:
                    throw new Exception();
            }

            lastSampleTime = sampleTime;
            sample.avg = sample.BER = (double)numErrors / (double)(8 * nodeSettings.BER_PktSize * numReceived);
            for (int i = sampleList.Count - 1; i >= 0; --i)
            {
                timeSpan = sampleTime - sampleList[i].sampleTime;
                if (timeSpan.TotalMilliseconds > nodeSettings.BER_AvgSpan)
                    break;
                weight = getWeight(timeSpan.TotalMilliseconds);
                totalWeight += weight;
                sample.avg += weight * sampleList[i].BER;
                for (int j = 0; j < 32; ++j)
                    pktOccurences[j] += weight * sampleList[i].pktErrorDist[j];
            }
            sample.avg /= totalWeight;

            totalWeight = 0;
            for (int j = 0; j < 32; ++j)
            {
                pktOccurences[j] += sample.pktErrorDist[j];
                totalWeight += pktOccurences[j];
            }
            for (int j = 0; j < 32; ++j)
                currentPktDist[j] = (float)(pktOccurences[j] / totalWeight);

            sampleList.Add(sample);
            BERgraphpanel.Invalidate(true);

            maxBER = ((maxBER > sample.BER) ? maxBER : sample.BER);
            minBER = ((minBER < sample.BER) ? minBER : sample.BER);

            txtRSSI.Text = sample.RSSI.ToString();
            txtRNSI.Text = sample.RNSI.ToString();
            txtMargin.Text = sample.LinkMargin.ToString() + " dB";
            txtBERavg.Text = sample.avg.ToString("0.000e-0");
            txtSample.Text = sampleList.Count.ToString();
            txtSent.Text = sample.pktSent.ToString();
            txtReceived.Text = numReceived.ToString();
            txtMissing.Text = sample.pktErrorDist[31].ToString();

            exportMenuItem.Enabled = true;

            if (!btnLog.Enabled)
            {
                if (logParams.putTimeAxis)
                    logFile.Write(sample.sampleTime.Ticks.ToString("F0") + ',');
                logFile.Write(sample.BER.ToString("0.000000e+0") + ',' + sample.avg.ToString("0.000000e+0")
                    + ',' + sample.RSSI.ToString("00") + ',' + sample.RNSI.ToString("00")
                    + ',' + sample.LinkMargin.ToString("00") + ',' + sample.pktSent.ToString("00000"));
                for (int i = 0; i < 32; ++i)
                    logFile.Write(',' + sample.pktErrorDist[i].ToString("00000"));
                logFile.WriteLine();
            }
        }

        public void addSensSample(DateTime time, int enabledSensors, byte[] numbers)
        {
            if (comboBox1.Enabled || (comboBox1.SelectedIndex != 3) || (radio == RadioType.TXonly))
                return;
            SensorSample sample;
            UInt16 addr, temp, RH;
            Int16 accelX, accelY, accelZ;
            int light, index;

            if (BitConverter.IsLittleEndian)
            {
                numbers = numbers.Reverse().ToArray();
                addr  = BitConverter.ToUInt16(numbers, 12);
                temp  = BitConverter.ToUInt16(numbers, 10);
                RH    = BitConverter.ToUInt16(numbers, 8);
                light = (int)BitConverter.ToUInt16(numbers, 6);
                accelX = BitConverter.ToInt16(numbers, 4);
                accelY = BitConverter.ToInt16(numbers, 2);
                accelZ = BitConverter.ToInt16(numbers, 0);
            }
            else
            {
                addr  = BitConverter.ToUInt16(numbers, 0);
                temp  = BitConverter.ToUInt16(numbers, 2);
                RH    = BitConverter.ToUInt16(numbers, 4);
                light = (int)BitConverter.ToUInt16(numbers, 6);
                accelX = BitConverter.ToInt16(numbers, 8);
                accelY = BitConverter.ToInt16(numbers, 10);
                accelZ = BitConverter.ToInt16(numbers, 12);
            }
            sample.sampleTime = time;
            sample.temp = (float)(175.72 * (double)temp / 65536.0 - 46.85);
            sample.RH = Math.Max(0.0f, Math.Min(100.0f, (float)(125.0 * (double)RH / 65536.0 - 6.0)));
            sample.light = 0.01f * (float)((0x00000001 << (light >> 12)) * (light & 0x00000FFF)); // maybe need to be verified... (signed to unsigned conversion ?)
            sample.accelX = (float)accelX / 4096.0f;
            sample.accelY = (float)accelY / 4096.0f;
            sample.accelZ = (float)accelZ / 4096.0f;
            sample.accelMag = (float)Math.Sqrt(Math.Pow((double)sample.accelX, 2.0) + Math.Pow((double)sample.accelY, 2.0) + Math.Pow((double)sample.accelZ, 2.0));

            index = nodes.FindIndex(n => n.address == addr);
            if (index < 0)
            {
                Color c = Color.White;
                for (int i = 0; i < defaultColors.Length; ++i)
                    if (!nodes.Exists(n => n.BackColor == defaultColors[i]))
                    {
                        c = defaultColors[i];
                        break;
                    }
                index = nodes.Count;
                nodes.Add(new nodePanel(this, index, addr, c, enabledSensors));
                adjustPanel();
                txtNumNode.Text = nodes.Count.ToString();
            }
            else
            {
                if (nodes[index].displaySignal(1))
                    splitContainer1.Panel1.Invalidate(true);
                if (nodes[index].displaySignal(2))
                    splitContainer2.Panel1.Invalidate(true);
                if (nodes[index].displaySignal(3))
                    splitContainer3.Panel1.Invalidate(true);
                if (nodes[index].displaySignal(4))
                    splitContainer4.Panel1.Invalidate(true);
                if (nodes[index].displaySignal(5))
                    splitContainer5.Panel1.Invalidate(true);
                if (nodes[index].displaySignal(6))
                    splitContainer6.Panel1.Invalidate(true);
                if (nodes[index].displaySignal(7))
                    splitContainer7.Panel1.Invalidate(true);
            }
            nodes[index].sampleList.Add(sample);

            exportMenuItem.Enabled = true;
            
            if (!btnLog.Enabled)
            {
                if (logParams.putTimeAxis)
                    logFile.Write(((double)sample.sampleTime.Ticks / 10000000.0).ToString("0.000") + ',');
                logFile.WriteLine(addr.ToString("F0") + ','
                    + sample.temp.ToString("00.000000") + ','
                    + sample.RH.ToString("000.000000") + ','
                    + sample.light.ToString("0.000000e+0") + ','
                    + sample.accelX.ToString("0.000000") + ','
                    + sample.accelY.ToString("0.000000") + ','
                    + sample.accelZ.ToString("0.000000") + ','
                    + sample.accelMag.ToString("00.000000"));
            }
        }

        private void pollTimer_Tick(object argument)
        {
            USBdev.send(new byte[] { (byte)USBhdr_PC.reqStat }, 250);
            USBdev.send(new byte[] { (byte)USBhdr_PC.rstStat }, 250); // this will give horrific result at higher sample rates...
        }

        private void btnHidePlot1_Click(object sender, EventArgs e)
        {
            chkListPlotsSel.SetItemChecked(0, false);
        }

        private void btnHidePlot2_Click(object sender, EventArgs e)
        {
            chkListPlotsSel.SetItemChecked(1, false);
        }

        private void btnHidePlot3_Click(object sender, EventArgs e)
        {
            chkListPlotsSel.SetItemChecked(2, false);
        }

        private void btnHidePlot4_Click(object sender, EventArgs e)
        {
            chkListPlotsSel.SetItemChecked(3, false);
        }

        private void btnHidePlot5_Click(object sender, EventArgs e)
        {
            chkListPlotsSel.SetItemChecked(4, false);
        }

        private void btnHidePlot6_Click(object sender, EventArgs e)
        {
            chkListPlotsSel.SetItemChecked(5, false);
        }

        private void btnHidePlot7_Click(object sender, EventArgs e)
        {
            chkListPlotsSel.SetItemChecked(6, false);
        }

        private void chkListPlotsSel_ItemCheck(object sender, ItemCheckEventArgs e)
        {
            switch (e.Index)
            {
                case 0:
                    splitContainer1.Panel1Collapsed = (e.NewValue != CheckState.Checked);
                    break;
                case 1:
                    splitContainer2.Panel1Collapsed = (e.NewValue != CheckState.Checked);
                    break;
                case 2:
                    splitContainer3.Panel1Collapsed = (e.NewValue != CheckState.Checked);
                    break;
                case 3:
                    splitContainer4.Panel1Collapsed = (e.NewValue != CheckState.Checked);
                    break;
                case 4:
                    splitContainer5.Panel1Collapsed = (e.NewValue != CheckState.Checked);
                    break;
                case 5:
                    splitContainer6.Panel1Collapsed = (e.NewValue != CheckState.Checked);
                    break;
                case 6:
                    splitContainer7.Panel1Collapsed = (e.NewValue != CheckState.Checked);
                    break;
                default:
                    throw new Exception();
            }
            adjustPanel();
        }

        private void adjustPanel(object sender = null, EventArgs e = null)
        {
            int h = nodePanelHeight * (nodes.Count > 0 ? nodes.Count : 1) + (int)(4.0f * DpiYratio);
            if (!splitContainer1.Panel1Collapsed)
                h += splitContainer1.SplitterDistance + splitContainer1.SplitterWidth;
            if (!splitContainer2.Panel1Collapsed)
                h += splitContainer2.SplitterDistance + splitContainer2.SplitterWidth;
            if (!splitContainer3.Panel1Collapsed)
                h += splitContainer3.SplitterDistance + splitContainer3.SplitterWidth;
            if (!splitContainer4.Panel1Collapsed)
                h += splitContainer4.SplitterDistance + splitContainer4.SplitterWidth;
            if (!splitContainer5.Panel1Collapsed)
                h += splitContainer5.SplitterDistance + splitContainer5.SplitterWidth;
            if (!splitContainer6.Panel1Collapsed)
                h += splitContainer6.SplitterDistance + splitContainer6.SplitterWidth;
            if (!splitContainer7.Panel1Collapsed)
                h += splitContainer7.SplitterDistance + splitContainer7.SplitterWidth;
            splitContainer1.Height = h;
        }

        public void deleteNode(int index)
        {
            nodes[index].delete();
            nodes.RemoveAt(index);
            for (; index < nodes.Count; ++index)
                nodes[index].updateSlot(index);
            adjustPanel();
            txtNumNode.Text = nodes.Count.ToString();
        }

        private void sliderZoom_Scroll(object sender, EventArgs e)
        {
            BERgraphpanel.Invalidate(true);
            splitContainer1.Panel1.Invalidate(true);
            splitContainer2.Panel1.Invalidate(true);
            splitContainer3.Panel1.Invalidate(true);
            splitContainer4.Panel1.Invalidate(true);
            splitContainer5.Panel1.Invalidate(true);
            splitContainer6.Panel1.Invalidate(true);
            splitContainer7.Panel1.Invalidate(true);
            if (comboBox1.SelectedIndex == 0)
                sliderZoom4.Value = sliderZoom.Value;
            else
                sliderZoom.Value = sliderZoom4.Value;
            zoom = 0.5 * Math.Pow(10.0, (double)sliderZoom.Value / 30.0);
        }

        private void BERgraph_Resize(object sender, EventArgs e)
        {
            BERgraphpanel.Invalidate(true); // is that necessary ?
        }

        private void btnClear_Click(object sender, EventArgs e)
        {
            exportMenuItem.Enabled = false;

            maxBER = 0.0;
            minBER = 1.0;

            txtRSSI.Text = string.Empty;
            txtRNSI.Text = string.Empty;
            txtMargin.Text = string.Empty;
            txtBERavg.Text = string.Empty;
            txtSample.Text = string.Empty;
            txtSent.Text = string.Empty;
            txtReceived.Text = string.Empty;
            txtMissing.Text = string.Empty;

            for (int i = 0; i < 32; ++i)
                currentPktDist[i] = 0.0f;

            sampleList.Clear();
            BERgraphpanel.Invalidate(true);
        }

        private void btnClearAll_Click(object sender, EventArgs e)
        {
            exportMenuItem.Enabled = false;

            splitContainer1.Panel1.Invalidate(true);
            splitContainer2.Panel1.Invalidate(true);
            splitContainer3.Panel1.Invalidate(true);
            splitContainer4.Panel1.Invalidate(true);
            splitContainer5.Panel1.Invalidate(true);
            splitContainer6.Panel1.Invalidate(true);
            splitContainer7.Panel1.Invalidate(true);
            if (nodes.Count == 0)
                return;
            foreach (nodePanel node in nodes)
                node.delete();
            nodes.Clear();
            adjustPanel();
            txtNumNode.Text = "0";
        }

        private void startRecording()
        {
            try
            {
                if (logParams.appendTimeToName)
                    logFile = new StreamWriter(Path.GetDirectoryName(logParams.fileNameAndPath)
                        + Path.DirectorySeparatorChar
                        + Path.GetFileNameWithoutExtension(logParams.fileNameAndPath)
                        + DateTime.Now.ToString("-ddMMMyy-HH;mm;ss.ff")
                        + Path.GetExtension(logParams.fileNameAndPath), logParams.appendFile);
                else
                    logFile = new StreamWriter(logParams.fileNameAndPath, logParams.appendFile);
                btnLog.Enabled = false;
            }
            catch (Exception)
            {
                btnRecord.FlatStyle = FlatStyle.Standard;
                stopRecording();
                MessageBox.Show("Error: Could not create/open data logging file " + logParams.fileNameAndPath + ".",
                    "Recording File", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
        }

        private void stopRecording()
        {
            btnLog.Enabled = true;
            if (logFile is StreamWriter)
            {
                logFile.Close();
                logFile.Dispose();
            }
        }

        private void setHelpText()
        {
            if (!((USBthread is Thread) && USBthread.IsAlive))
            {
                lblHelp.Text = "No Spark device connected.\r\nUse \"File -> Connect to node\" to establish a connecti" +
                    "on with your Spark radio and configure it.";
                return;
            }
            switch (radio)
            {
                case RadioType.TXonly:
                    lblHelp.Text = "Spark radio TRANSMITTER " + USBdev.nodeLabel + " connected.\r\n"
                        + "(Related data acquisition is performed only on the terminal to which the receiver is connected.)";
                    break;
                case RadioType.RXonly:
                    lblHelp.Text = "Spark radio RECEIVER " + USBdev.nodeLabel + " connected.\r\n"
                        + "Its corresponding transmitter(s) should already be in operation before pressing \"Start RX\".";
                    break;
                case RadioType.bidiSlave:
                    lblHelp.Text = "Spark radio bidirectional SLAVE " + USBdev.nodeLabel + " connected.\r\n"
                        + "Its corresponding master should already be in operation before pressing \"Start RX\".";
                    break;
                case RadioType.bidiMaster:
                    lblHelp.Text = "Spark radio bidirectional MASTER " + USBdev.nodeLabel + " connected.\r\n"
                        + "Its corresponding slave(s) should already be in operation before pressing \"Start RX\".";
                    break;
                default:
                    throw new Exception();
            }
        }

        public void updateConfigPkt()
        {
            BERgraphpanel.Invalidate(true);
            splitContainer1.Panel1.Invalidate(true);
            splitContainer2.Panel1.Invalidate(true);
            splitContainer3.Panel1.Invalidate(true);
            splitContainer4.Panel1.Invalidate(true);
            splitContainer5.Panel1.Invalidate(true);
            splitContainer6.Panel1.Invalidate(true);
            splitContainer7.Panel1.Invalidate(true);
            if (!comboBox1.Enabled)
            {
                if (comboBox1.SelectedIndex == 0)
                    pollTimer.Change(nodeSettings.BER_GUIperiod, nodeSettings.BER_GUIperiod);
                return;
            }
            nodeSettings.configPkt[0] = (byte)((nodeSettings.BER_LFSR ? 0x20 : 0x00) | (nodeSettings.PPM ? 0x10 : 0x00) | ((byte)radio << 2) | comboBox1.SelectedIndex);
            nodeSettings.configPkt[1] = (byte)(nodeSettings.TXaddr >> 8);
            nodeSettings.configPkt[2] = (byte)nodeSettings.TXaddr;
            nodeSettings.configPkt[3] = (byte)(nodeSettings.RXaddr >> 8);
            nodeSettings.configPkt[4] = (byte)nodeSettings.RXaddr;
            nodeSettings.configPkt[5] = (byte)nodeSettings.BER_PktSize;
            nodeSettings.configPkt[6] = (byte)(nodeSettings.BER_PktRate >> 8);
            nodeSettings.configPkt[7] = (byte)nodeSettings.BER_PktRate;
            nodeSettings.configPkt[8] = (byte)(nodeSettings.Sen_PktRate >> 8);
            nodeSettings.configPkt[9] = (byte)nodeSettings.Sen_PktRate;
            nodeSettings.configPkt[10] = (byte)((nodeSettings.Sen_EnTemp ? 0x80 : 0) | (nodeSettings.Sen_EnRH ? 0x40 : 0)
                                             | (nodeSettings.Sen_EnLight ? 0x20 : 0) | (nodeSettings.Sen_EnAccel ? 0x10 : 0)
                                                      | (nodeSettings.Sen_Tres << 2) | nodeSettings.Sen_RHres);
            nodeSettings.configPkt[11] = (byte)((nodeSettings.Sen_ACK ? 2 : 0) | (nodeSettings.Sen_LongLightInt ? 1 : 0));
            btnStart.Enabled = currentConfig.SequenceEqual(nodeSettings.configPkt);
        }

        public void importConfig(object sender, EventArgs e)
        {
            nodeSettings.Sen_ACK = (currentConfig[11] & 0x02) != 0;
            nodeSettings.Sen_PktRate = (UInt16)(currentConfig[8] * 0x0100 + currentConfig[9]);
            nodeSettings.Sen_EnTemp = (currentConfig[10] & 0x80) != 0;
            nodeSettings.Sen_EnRH = (currentConfig[10] & 0x40) != 0;
            nodeSettings.Sen_EnLight = (currentConfig[10] & 0x20) != 0;
            nodeSettings.Sen_EnAccel = (currentConfig[10] & 0x10) != 0;
            nodeSettings.Sen_Tres = ((currentConfig[10] >> 2) & 0x03);
            nodeSettings.Sen_RHres = (currentConfig[10] & 0x03);
            nodeSettings.Sen_LongLightInt = (currentConfig[11] & 0x01) != 0;
            nodeSettings.BER_PktRate = (UInt16)(currentConfig[6] * 0x0100 + currentConfig[7]);
            nodeSettings.BER_PktSize = currentConfig[5];
            nodeSettings.RXaddr = (UInt16)(currentConfig[3] * 0x0100 + currentConfig[4]);
            nodeSettings.TXaddr = (UInt16)(currentConfig[1] * 0x0100 + currentConfig[2]);
            nodeSettings.BER_LFSR = (currentConfig[0] & 0x20) != 0;
            nodeSettings.PPM = (currentConfig[0] & 0x10) != 0;
            radio = (RadioType)((currentConfig[0] & 0x0C) >> 2);
            comboBox1.SelectedIndex = (currentConfig[0] & 0x03);
            comboBox1_SelectedIndexChanged(sender, e);

            if ((setForm is SettingsForm) && !setForm.IsDisposed)
            {
                setForm.loadState();
                setForm.setTab(comboBox1.SelectedIndex);
            }
            setHelpText();
        }

        private void comboBox1_SelectedIndexChanged(object sender, EventArgs e)
        {
            switch (comboBox1.SelectedIndex)
            {
                case 0:
                    BERpanel.Visible = true;
                    AudioPanel.Visible = false;
                    FilePanel.Visible = false;
                    SensPanel.Visible = false;
                    exportMenuItem.Enabled = (sampleList.Count > 0);
                    break;
                case 1:
                    BERpanel.Visible = false;
                    AudioPanel.Visible = true;
                    FilePanel.Visible = false;
                    SensPanel.Visible = false;
                    exportMenuItem.Enabled = false; // will have to change when fully implemented
                    break;
                case 2:
                    BERpanel.Visible = false;
                    AudioPanel.Visible = false;
                    FilePanel.Visible = true;
                    SensPanel.Visible = false;
                    exportMenuItem.Enabled = false; // will have to change when fully implemented
                    break;
                case 3:
                    BERpanel.Visible = false;
                    AudioPanel.Visible = false;
                    FilePanel.Visible = false;
                    SensPanel.Visible = true;
                    exportMenuItem.Enabled = (nodes.Count > 0);
                    break;
            }
            btnConfigNode.Enabled = ((USBthread is Thread) && USBthread.IsAlive);
            updateConfigPkt();
        }

        private void btnSettings_Click(object sender, EventArgs e)
        {
            if (!(setForm is SettingsForm) || setForm.IsDisposed)
            {
                setForm = new SettingsForm(this);
                setForm.Show();
            }
            else
                setForm.BringToFront();
            if (comboBox1.SelectedIndex >= 0)
                setForm.setTab(comboBox1.SelectedIndex);
        }

        private void btnConfigNode_Click(object sender, EventArgs e)
        {
            byte[] packet = new byte[1 + nodeSettings.configPkt.Length];
            packet[0] = (byte)USBhdr_PC.wrConf;
            System.Buffer.BlockCopy(nodeSettings.configPkt, 0, packet, 1, nodeSettings.configPkt.Length);
            USBdev.send(packet, 200);
        }

        private void GUIstart()
        {
            switch (comboBox1.SelectedIndex)
            {
              case 0:
                pollTimer.Change(nodeSettings.BER_GUIperiod, nodeSettings.BER_GUIperiod);
                break;
              case 3:
                pollTimer.Change(100, 100);
                break;
            }
            btnStart.Text = "Stop Radio";
//            connectMenuItem.Enabled = false;
//            disconnMenuItem.Enabled = false;
            btnConfigNode.Enabled = false;
            comboBox1.Enabled = false;
            if (btnRecord.FlatStyle == FlatStyle.Flat)
                startRecording();
        }

        private void GUIstop()
        {
            pollTimer.Change(Timeout.Infinite, Timeout.Infinite);
            btnStart.Text = "Start Radio";
//            connectMenuItem.Enabled = true;
//            disconnMenuItem.Enabled = true;
            btnConfigNode.Enabled = true;
            comboBox1.Enabled = true;
            if (btnRecord.FlatStyle == FlatStyle.Flat)
                stopRecording();
            updateConfigPkt();
        }

        private void btnStart_Click(object sender, EventArgs e)
        {
            if (comboBox1.Enabled)
            {
                GUIstart();
                USBdev.send(new byte[] { (byte)USBhdr_PC.start }, 80);
            }
            else
            {
                GUIstop();
                USBdev.send(new byte[] { (byte)USBhdr_PC.stop }, 200);
            }
        }

        private void btnRecord_Click(object sender, EventArgs e)
        {
            if (btnRecord.FlatStyle == FlatStyle.Flat)
            {
                btnRecord.FlatStyle = FlatStyle.Standard;
                if (!comboBox1.Enabled)
                    stopRecording();
            }
            else
            {
                btnRecord.FlatStyle = FlatStyle.Flat;
                if (!comboBox1.Enabled)
                    startRecording();
            }
        }

        private void btnLog_Click(object sender, EventArgs e)
        {
            LoggingForm logForm = new LoggingForm();
            logForm.ShowDialog();
        }

        private void connectMenuItem_Click(object sender, EventArgs e)
        {
            List<string> devListStr = USBmanager.getAvailableDevices();
            if (devListStr.Count <= 0)
            {
                if ((USBthread is Thread) && USBthread.IsAlive)
                    MessageBox.Show("No other Spark radio node detected: The one available is already ready to use.",
                        "USB Device Connection", MessageBoxButtons.OK, MessageBoxIcon.Information);
                else
                    MessageBox.Show("No Spark radio node identified from available USB devices." +
                                    "\r\n\r\nPlease connect one of your Spark Radio nodes to this" +
                                    "\r\ncomputer to configure it or use it for data collection.",
                        "USB Device Connection", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                return;
            }
            connectForm connectDialog = new connectForm(devListStr.ToArray());
            if (connectDialog.ShowDialog() == DialogResult.OK)
            {
                USBmanager newUSBdev = new USBmanager(this, connectDialog.getSelectedIndex(), connectDialog.getSelectedText());
                Thread newUSBthread = new Thread(newUSBdev.USBthreadLoop);
                try
                {
                    newUSBthread.Start();
                }
                catch (Exception)
                {
                    newUSBdev.runLoop = false;
                    newUSBthread.Join(1000);
                    MessageBox.Show("Error: Could not establish USB endpoint connection with Spark radio " + newUSBdev.nodeLabel + ".",
                        "Serial Port Connection", MessageBoxButtons.OK, MessageBoxIcon.Error);
                    return;
                }
                if (USBdev is USBmanager)
                    USBdev.runLoop = false;
                if (USBthread is Thread)
                    USBthread.Join(1000);
                USBthread = newUSBthread;
                USBdev = newUSBdev;
                if (!comboBox1.Enabled)
                    GUIstop();
                USBdev.send(new byte[] { (byte)'v', (byte)'0' }, 500);
                USBdev.send(new byte[] { (byte)USBhdr_PC.reqConf }, 500);
                radio = connectDialog.nodeType();
                setHelpText();
                updateConfigPkt();
                btnConfigNode.Enabled = (comboBox1.SelectedIndex >= 0);
                disconnMenuItem.Enabled = true;
                confViewMenuItem.Enabled = true;
                btnClear_Click(sender, e);
                btnClearAll_Click(sender, e);
            }
        }

        private void disconnMenuItem_Click(object sender, EventArgs e)
        {
            if (USBdev is USBmanager)
                USBdev.runLoop = false;
            if (USBthread is Thread)
                USBthread.Join(1000);
            USBthread = null;
            USBdev = null;
            if (!comboBox1.Enabled)
                GUIstop();
            setHelpText();
            btnStart.Enabled = false;
            btnConfigNode.Enabled = false;
            confViewMenuItem.Enabled = false;
            disconnMenuItem.Enabled = false;
            if (confViewer is confViewForm)
                confViewer.Close();
            BERgraphpanel.Invalidate(true);
            splitContainer1.Panel1.Invalidate(true);
            splitContainer2.Panel1.Invalidate(true);
            splitContainer3.Panel1.Invalidate(true);
            splitContainer4.Panel1.Invalidate(true);
            splitContainer5.Panel1.Invalidate(true);
            splitContainer6.Panel1.Invalidate(true);
            splitContainer7.Panel1.Invalidate(true);
        }

        private void exportMenuItem_Click(object sender, EventArgs e)
        {
            SaveFileDialog FileSaveDialog = new SaveFileDialog();

            FileSaveDialog.Filter = "Comma Separated Values files (*.csv)|*.csv|all files (*.*)|*";
            if (FileSaveDialog.ShowDialog() == DialogResult.OK)
                using (StreamWriter saveFile = new StreamWriter(FileSaveDialog.FileName))
                {
                    if (comboBox1.SelectedIndex == 0)
                    {
                        saveFile.WriteLine("Milliseconds,BER,averageBER");
                        for (int i = 0; i < sampleList.Count; ++i)
                        {
                            saveFile.Write(sampleList[i].sampleTime.ToString("F0")
                                + ',' + sampleList[i].BER.ToString("0.0#####e+0")
                                + ',' + sampleList[i].avg.ToString("0.0#####e+0"));
                            for (int j = 0; j < 32; ++j)
                                saveFile.Write(',' + sampleList[i].pktErrorDist[j].ToString("F0"));
                            saveFile.WriteLine();
                        }
                    }
                    else
                    {
                        // sensor node data recording...
                    }
                }
        }

        private void exitMenuItem_Click(object sender, EventArgs e)
        {
            if (USBdev is USBmanager)
                USBdev.runLoop = false;
            if (USBthread is Thread)
                USBthread.Join(1000);
            Application.Exit();
        }

        private void aboutMenuItem_Click(object sender, EventArgs e)
        {
            AboutForm aboutWin = new AboutForm();
            aboutWin.ShowDialog();
        }

        private void confViewMenuItem_Click(object sender, EventArgs e)
        {
            USBdev.send(new byte[] { (byte)USBhdr_PC.reqConf }, 200);
            if (!(confViewer is confViewForm) || confViewer.IsDisposed)
            {
                confViewer = new confViewForm(this);
                confViewer.Show();
            }
            else
                confViewer.BringToFront();
            confViewer.waitingState();
        }

/*        protected override void WndProc(ref Message m)
        {
            switch (m.Msg)
            {
                case WM_DEVICECHANGE:
                    break;
            }
            base.WndProc(ref m);
        }*/
    }
}
