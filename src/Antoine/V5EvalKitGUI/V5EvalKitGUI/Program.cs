using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.IO;
using LibUsbDotNet;

namespace V5EvalKitGUI
{
    public enum RadioType : byte
    {
        TXonly = 0,
        RXonly = 1,
        bidiSlave = 2,
        bidiMaster = 3
    }

    public static class nodeSettings
    {
        public static UInt16 TXaddr;
        public static UInt16 RXaddr;
        public static bool PPM;
        public static bool BER_LFSR;
        public static UInt16 BER_PktSize;
        public static UInt16 BER_PktRate;
        public static UInt16 BER_GUIperiod;
        public static uint BER_AvgSpan;
        public static uint BER_AvgType;
        public static uint BER_AvgHL;
        public static bool Sen_ACK;
        public static UInt16 Sen_PktRate;
        public static bool Sen_EnTemp;
        public static bool Sen_EnRH;
        public static bool Sen_EnLight;
        public static bool Sen_EnAccel;
        public static int Sen_Tres;
        public static int Sen_RHres;
        public static bool Sen_LongLightInt;

        public static byte[] configPkt = new byte[12];
    }

    public static class logParams
    {
        public static string fileNameAndPath;
        public static bool appendFile;
        public static bool appendTimeToName;
        public static bool putTimeAxis;
    }

    static class Program
    {
        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        [STAThread]
        static void Main()
        {
            IniFile settingsFile = new IniFile("settings.ini");

            if (settingsFile.KeyExists("TXaddr", "nodeSettings"))
                nodeSettings.TXaddr = Convert.ToUInt16(settingsFile.Read("TXaddr", "nodeSettings"), 16);
            else
                nodeSettings.TXaddr = 0xADDA;

            if (settingsFile.KeyExists("RXaddr", "nodeSettings"))
                nodeSettings.RXaddr = Convert.ToUInt16(settingsFile.Read("RXaddr", "nodeSettings"), 16);
            else
                nodeSettings.RXaddr = 0xADDB;

            if (settingsFile.KeyExists("PPM", "nodeSettings"))
                nodeSettings.PPM = settingsFile.Read("PPM", "nodeSettings").Equals("True", StringComparison.OrdinalIgnoreCase);
            else
                nodeSettings.PPM = false;

            if (settingsFile.KeyExists("BER_LFSR", "nodeSettings"))
                nodeSettings.BER_LFSR = settingsFile.Read("BER_LFSR", "nodeSettings").Equals("True", StringComparison.OrdinalIgnoreCase);
            else
                nodeSettings.BER_LFSR = true;

            if (settingsFile.KeyExists("BER_PktSize", "nodeSettings"))
                nodeSettings.BER_PktSize = Convert.ToUInt16(settingsFile.Read("BER_PktSize", "nodeSettings"));
            else
                nodeSettings.BER_PktSize = 32;

            if (settingsFile.KeyExists("BER_PktRate", "nodeSettings"))
                nodeSettings.BER_PktRate = Convert.ToUInt16(settingsFile.Read("BER_PktRate", "nodeSettings"));
            else
                nodeSettings.BER_PktRate = 100;

            if (settingsFile.KeyExists("BER_GUIperiod", "nodeSettings"))
                nodeSettings.BER_GUIperiod = Convert.ToUInt16(settingsFile.Read("BER_GUIperiod", "nodeSettings"));
            else
                nodeSettings.BER_GUIperiod = 1000;

            if (settingsFile.KeyExists("BER_AvgSpan", "nodeSettings"))
                nodeSettings.BER_AvgSpan = Convert.ToUInt32(settingsFile.Read("BER_AvgSpan", "nodeSettings"));
            else
                nodeSettings.BER_AvgSpan = 15000;

            if (settingsFile.KeyExists("BER_AvgType", "nodeSettings"))
                nodeSettings.BER_AvgType = Convert.ToUInt32(settingsFile.Read("BER_AvgType", "nodeSettings"));
            else
                nodeSettings.BER_AvgType = 0;

            if (settingsFile.KeyExists("BER_AvgHL", "nodeSettings"))
                nodeSettings.BER_AvgHL = Convert.ToUInt32(settingsFile.Read("BER_AvgHL", "nodeSettings"));
            else
                nodeSettings.BER_AvgHL = 6000;

            if (settingsFile.KeyExists("Sen_ACK", "nodeSettings"))
                nodeSettings.Sen_ACK = settingsFile.Read("Sen_ACK", "nodeSettings").Equals("True", StringComparison.OrdinalIgnoreCase);
            else
                nodeSettings.Sen_ACK = true;

            if (settingsFile.KeyExists("Sen_PktRate", "nodeSettings"))
                nodeSettings.Sen_PktRate = Convert.ToUInt16(settingsFile.Read("Sen_PktRate", "nodeSettings"));
            else
                nodeSettings.Sen_PktRate = 2000;

            if (settingsFile.KeyExists("Sen_EnTemp", "nodeSettings"))
                nodeSettings.Sen_EnTemp = settingsFile.Read("Sen_EnTemp", "nodeSettings").Equals("True", StringComparison.OrdinalIgnoreCase);
            else
                nodeSettings.Sen_EnTemp = true;

            if (settingsFile.KeyExists("Sen_EnRH", "nodeSettings"))
                nodeSettings.Sen_EnRH = settingsFile.Read("Sen_EnRH", "nodeSettings").Equals("True", StringComparison.OrdinalIgnoreCase);
            else
                nodeSettings.Sen_EnRH = true;

            if (settingsFile.KeyExists("Sen_EnLight", "nodeSettings"))
                nodeSettings.Sen_EnLight = settingsFile.Read("Sen_EnLight", "nodeSettings").Equals("True", StringComparison.OrdinalIgnoreCase);
            else
                nodeSettings.Sen_EnLight = true;

            if (settingsFile.KeyExists("Sen_EnAccel", "nodeSettings"))
                nodeSettings.Sen_EnAccel = settingsFile.Read("Sen_EnAccel", "nodeSettings").Equals("True", StringComparison.OrdinalIgnoreCase);
            else
                nodeSettings.Sen_EnAccel = true;

            if (settingsFile.KeyExists("Sen_Tres", "nodeSettings"))
                nodeSettings.Sen_Tres = Convert.ToInt32(settingsFile.Read("Sen_Tres", "nodeSettings")); // could be adapted to direct num of bits entry
            else
                nodeSettings.Sen_Tres = 0;

            if (settingsFile.KeyExists("Sen_RHres", "nodeSettings"))
                nodeSettings.Sen_RHres = Convert.ToInt32(settingsFile.Read("Sen_RHres", "nodeSettings")); // could be adapted to direct num of bits entry
            else
                nodeSettings.Sen_RHres = 0;

            if (settingsFile.KeyExists("Sen_LongLightInt", "nodeSettings"))
                nodeSettings.Sen_LongLightInt = settingsFile.Read("Sen_LongLightInt", "nodeSettings").Equals("True", StringComparison.OrdinalIgnoreCase);
            else
                nodeSettings.Sen_LongLightInt = true;

            if (settingsFile.KeyExists("fileNameAndPath", "logParams"))
                logParams.fileNameAndPath = settingsFile.Read("fileNameAndPath", "logParams");
            else
                logParams.fileNameAndPath = Directory.GetCurrentDirectory() + Path.DirectorySeparatorChar + "data.csv";

            if (settingsFile.KeyExists("appendFile", "logParams"))
                logParams.appendFile = settingsFile.Read("appendFile", "logParams").Equals("True", StringComparison.OrdinalIgnoreCase);
            else
                logParams.appendFile = false;

            if (settingsFile.KeyExists("appendTimeToName", "logParams"))
                logParams.appendTimeToName = settingsFile.Read("appendTimeToName", "logParams").Equals("True", StringComparison.OrdinalIgnoreCase);
            else
                logParams.appendTimeToName = true;

            if (settingsFile.KeyExists("putTimeAxis", "logParams"))
                logParams.putTimeAxis = settingsFile.Read("putTimeAxis", "logParams").Equals("True", StringComparison.OrdinalIgnoreCase);
            else
                logParams.putTimeAxis = false;

            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);
            try
            {
                Application.Run(new MainForm());
            }
            finally
            {
                UsbDevice.Exit();
            }
        }
    }
}
