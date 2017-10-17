using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using LibUsbDotNet;
using LibUsbDotNet.Main;
using LibUsbDotNet.WinUsb;

namespace EvalKitDebug
{
    public class USBmanager
    {
        private static List<WinUsbRegistry> allDevices;

        private WinUsbRegistry deviceReg;
        private volatile byte[] TXdata;
        private volatile bool dataToSend;
        public volatile bool runLoop;

        public USBmanager(int deviceRegIndex = 0)
        {
            deviceReg = allDevices[deviceRegIndex];
            dataToSend = false;
        }

        public void send(byte[] data, uint timeout)
        {
            uint waited = 0;
            while (dataToSend)
            {
                if (waited++ > timeout)
                    throw new TimeoutException("Timeout Error: USBmanager.USBthreadLoop() haven't freed the USB transmission buffer in time.");
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
                    writer.Write(TXdata, 50, out bytesExchanged); // 360
                    dataToSend = false;
                }
                reader.Read(RXdata, 50, out bytesExchanged); //200 // REALLY garbage... this fucking driver I swear...
                if (bytesExchanged > 0)
                    Console.Write(Encoding.UTF8.GetString(RXdata, 0, bytesExchanged));
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
