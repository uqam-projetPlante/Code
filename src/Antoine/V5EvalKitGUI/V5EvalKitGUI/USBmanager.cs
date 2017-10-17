using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Forms;
using LibUsbDotNet;
using LibUsbDotNet.Main;
using LibUsbDotNet.WinUsb;

namespace V5EvalKitGUI
{
    public enum USBhdr_PC : byte
    {
        reqConf = (byte)'i',
        wrConf = (byte)'w',
        toNVM = (byte)'\'', // PLEASE CHANGE
        start = (byte)'+',
        stop = (byte)'-',
        reqStat = (byte)'~',
        rstStat = (byte)'r'
    }

    public enum USBhdr_node
    {
        unsynced = 0,
        syncing = 1,
        synced = 2,
        config = 3,
        BERsamp = 4,
        audioPk = 5,
        filePk = 6,
        sensors = 7
    }

    public class USBmanager
    {
        private static List<WinUsbRegistry> allDevices;

        private MainForm owner;
        private WinUsbRegistry deviceReg;
        private volatile byte[] TXdata;
        private volatile bool dataToSend;
        public volatile bool runLoop;
        public string nodeLabel;

        public USBmanager(MainForm _owner, int deviceRegIndex = 0, string _nodeLabel = "")
        {
            owner = _owner;
            deviceReg = allDevices[deviceRegIndex];
            nodeLabel = _nodeLabel;
            dataToSend = false;
        }

        public void send(byte[] data, uint timeout)
        {
            uint waited = 0;
            while (dataToSend)
            {
                //if (waited++ > timeout)
                  //  throw new TimeoutException("Timeout Error: USBmgmt.USBthreadLoop() haven't freed the USB transmission buffer in time.");
                Thread.Sleep(1);
            }
            TXdata = data;
            dataToSend = true;
        }

        public void USBthreadLoop()
        {
            UsbDevice device;
            UsbEndpointWriter writer;
            UsbEndpointReader reader;
            DateTime arrivalTime;
            byte[] RXdata = new byte[4096];
            int bytesExchanged;

            runLoop = true;
            if (!deviceReg.Open(out device))
                throw new Exception("Cannot open chosen device !");
            writer = device.OpenEndpointWriter(WriteEndpointID.Ep01);
            reader = device.OpenEndpointReader(ReadEndpointID.Ep01);
            do
            {
                if (dataToSend)
                {
                    writer.Write(TXdata, 360, out bytesExchanged);
                    dataToSend = false;
                }
                reader.Read(RXdata, 200, out bytesExchanged);
                arrivalTime = DateTime.UtcNow;
                if (bytesExchanged > 0)
                {
                    byte[] bytes;
                    int header, sensorsMask, RXoffset = 0;
                    while (RXoffset < bytesExchanged)
                    {
                        header = RXdata[RXoffset++];
                        sensorsMask = header & 0x000000F0;
                        switch ((USBhdr_node)(header & 0x0000000F))
                        {
                          case USBhdr_node.unsynced:
                            owner.BeginInvoke((MethodInvoker)owner.setLightUnsynced);
                            break;
                          case USBhdr_node.syncing:
                            owner.BeginInvoke((MethodInvoker)owner.setLightSyncing);
                            break;
                          case USBhdr_node.synced:
                            owner.BeginInvoke((MethodInvoker)owner.setLightSynced);
                            break;
                          case USBhdr_node.config:
                            System.Buffer.BlockCopy(RXdata, RXoffset, owner.currentConfig, 0, 12);
                            owner.BeginInvoke((MethodInvoker)owner.processConfig);
                            RXoffset += 12;
                            break;
                          case USBhdr_node.BERsamp:
                            bytes = new byte[73];
                            System.Buffer.BlockCopy(RXdata, RXoffset, bytes, 1, 72);
                            bytes[0] = 0;
                            owner.BeginInvoke((Action<DateTime,byte[]>)owner.addBERsample, arrivalTime, bytes);
                            arrivalTime.AddMilliseconds(1.0);
                            RXoffset += 72;
                            break;
                          case USBhdr_node.audioPk:
                            break;
                          case USBhdr_node.filePk:
                            break;
                          case USBhdr_node.sensors:
                            bytes = new byte[14];
                            System.Buffer.BlockCopy(RXdata, RXoffset, bytes, 0, 14);
                            owner.BeginInvoke((Action<DateTime,int,byte[]>)owner.addSensSample, arrivalTime, sensorsMask, bytes);
                            arrivalTime.AddMilliseconds(1.0);
                            RXoffset += 14;
                            break;
                          default:
                            MessageBox.Show("An unrecognized USB transaction was just received.",
                                "Unknown USB transaction", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                            RXoffset = bytesExchanged;
                            break;
                        }
                    }
                }
            }
            while (runLoop);
            device.Close();
        }

        public static List<string> getAvailableDevices()
        {
            List<string> devListStr = new List<string>();
            string S_N;
            UsbDevice CurrentDevice;
            WinUsbRegistry.GetWinUsbRegistryList(new Guid("a5dcbf10-6530-11d2-901f-00c04fb951ed"), out allDevices);
            allDevices.RemoveAll(dev => (dev.Vid != 0x03EB) || (dev.Pid != 0x2040));
            int i = 0;
            while (i < allDevices.Count)
            {
                if (allDevices[i].Open(out CurrentDevice))
                {
                    CurrentDevice.GetString(out S_N, 0x0000, 0x03);
                    devListStr.Add("Eval. node v1.0 [S/N:" + S_N + "]");
                    CurrentDevice.Close();
                    ++i;
                }
                else
                    allDevices.RemoveAt(i);
            }
            return devListStr;
        }
    }
}
