using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.IO;

namespace V5EvalKitGUI
{
    public partial class LoggingForm : Form
    {
        public LoggingForm()
        {
            InitializeComponent();

            textBox1.Text = logParams.fileNameAndPath;
            radOverwrite.Checked = !logParams.appendFile;
            radAppend.Checked = logParams.appendFile;
            chkAppendTime.Checked = logParams.appendTimeToName;
            chkTimeData.Checked = logParams.putTimeAxis;
        }

        private void textBox1_KeyPress(object sender, KeyPressEventArgs e)
        {
            if (e.KeyChar.ToString().LastIndexOfAny(Path.GetInvalidPathChars()) >= 0)
                e.Handled = (e.KeyChar != 0x08);
        }

        private void textBox1_LostFocus(object sender, EventArgs e)
        {
            textBox1.Text = textBox1.Text.Trim();
        }

        private void btnDir_Click(object sender, EventArgs e)
        {
            FolderBrowserDialog dirSelector = new FolderBrowserDialog();

            try
            {
                string dirPath = Path.GetDirectoryName(textBox1.Text);
                if (Directory.Exists(dirPath))
                    dirSelector.SelectedPath = dirPath;
                else
                    dirSelector.SelectedPath = Directory.GetCurrentDirectory();
            }
            catch (Exception)
            {
                dirSelector.SelectedPath = Directory.GetCurrentDirectory();
            }
            if (dirSelector.ShowDialog() == DialogResult.OK)
            {
                if (Path.GetFileNameWithoutExtension(textBox1.Text) != string.Empty)
                    textBox1.Text = dirSelector.SelectedPath + Path.DirectorySeparatorChar + Path.GetFileName(textBox1.Text);
                else
                    textBox1.Text = dirSelector.SelectedPath + Path.DirectorySeparatorChar + "data.csv";
            }
        }

        private void btnOK_Click(object sender, EventArgs e)
        {
            char[] separators = { Path.DirectorySeparatorChar, Path.AltDirectorySeparatorChar };
            int charIndex = textBox1.Text.LastIndexOfAny(separators);
            if ((textBox1.Text.Length == 0) || (textBox1.Text.Length == (charIndex + 1)))
                textBox1.Text += "data.csv";
            else
            {
                string filename = textBox1.Text.Substring(charIndex);
                if (!filename.Contains('.'))
                    textBox1.Text += ".csv";
                else if (filename.EndsWith("."))
                    textBox1.Text += "csv";
            }

            logParams.fileNameAndPath = textBox1.Text;
            logParams.appendFile = radAppend.Checked;
            logParams.appendTimeToName = chkAppendTime.Checked;
            logParams.putTimeAxis = chkTimeData.Checked;

            IniFile configFile = new IniFile("settings.ini");

            configFile.Write("fileNameAndPath", logParams.fileNameAndPath, "logParams");
            configFile.Write("appendFile", logParams.appendFile.ToString(), "logParams");
            configFile.Write("appendTimeToName", logParams.appendTimeToName.ToString(), "logParams");
            configFile.Write("putTimeAxis", logParams.putTimeAxis.ToString(), "logParams");
        }
    }
}
