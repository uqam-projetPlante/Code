using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using LibUsbDotNet;

namespace EvalKitDebug
{
    class Program
    {
        static void Main(string[] args)
        {
            string conInput;
            USBmanager USBdev = null;
            Thread USBthread = null;
            List<string> devList = USBmanager.getAvailableDevices();

            if (devList.Count == 0)
            {
                UsbDevice.Exit();
                Console.WriteLine("No Spark device detected.");
                Console.ReadKey();
                return;
            }
            USBdev = new USBmanager();
            USBthread = new Thread(USBdev.USBthreadLoop);
            try
            {
                USBthread.Start();
                Console.WriteLine("USB device opened...");
                while (true)
                {
                    conInput = Console.ReadLine();
                    if ((conInput.ToLower() == "quit") || (conInput.ToLower() == "exit"))
                        break;
                    if (conInput != string.Empty)
                        USBdev.send(Encoding.UTF8.GetBytes(conInput + '\n'), 50); // 200
                }
            }
            finally
            {
                USBdev.runLoop = false;
                USBthread.Join(1000);
                USBthread.Abort();
                UsbDevice.Exit();
            }
        }
    }
}
