namespace V5EvalKitGUI
{
    partial class SettingsForm
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(SettingsForm));
            this.tabControl1 = new System.Windows.Forms.TabControl();
            this.tabBER = new System.Windows.Forms.TabPage();
            this.lblBytes = new System.Windows.Forms.Label();
            this.lblPktSize = new System.Windows.Forms.Label();
            this.lblPktRate = new System.Windows.Forms.Label();
            this.lblPktSec = new System.Windows.Forms.Label();
            this.numPktRate = new System.Windows.Forms.NumericUpDown();
            this.numPktSize = new System.Windows.Forms.NumericUpDown();
            this.lblRateSec = new System.Windows.Forms.Label();
            this.numSampleRate = new System.Windows.Forms.NumericUpDown();
            this.lblSampleRate = new System.Windows.Forms.Label();
            this.lblAvg = new System.Windows.Forms.Label();
            this.lblAvgSec = new System.Windows.Forms.Label();
            this.numAvgSpan = new System.Windows.Forms.NumericUpDown();
            this.grpAvgWeight = new System.Windows.Forms.GroupBox();
            this.radFlatAvg = new System.Windows.Forms.RadioButton();
            this.lblHLsec = new System.Windows.Forms.Label();
            this.radTriangleAvg = new System.Windows.Forms.RadioButton();
            this.lblHalfLife = new System.Windows.Forms.Label();
            this.radHalfLifeAvg = new System.Windows.Forms.RadioButton();
            this.numHalfLife = new System.Windows.Forms.NumericUpDown();
            this.tabAudio = new System.Windows.Forms.TabPage();
            this.tabFile = new System.Windows.Forms.TabPage();
            this.tabSensor = new System.Windows.Forms.TabPage();
            this.label1 = new System.Windows.Forms.Label();
            this.lblSensRateSec = new System.Windows.Forms.Label();
            this.numSenseRate = new System.Windows.Forms.NumericUpDown();
            this.chkAck = new System.Windows.Forms.CheckBox();
            this.lblLightInt = new System.Windows.Forms.Label();
            this.lblRHres = new System.Windows.Forms.Label();
            this.lblTres = new System.Windows.Forms.Label();
            this.cboxTres = new System.Windows.Forms.ComboBox();
            this.cboxRHres = new System.Windows.Forms.ComboBox();
            this.cboxLightInt = new System.Windows.Forms.ComboBox();
            this.lblSensorList = new System.Windows.Forms.Label();
            this.chkListSensors = new System.Windows.Forms.CheckedListBox();
            this.btnOK = new System.Windows.Forms.Button();
            this.btnCancel = new System.Windows.Forms.Button();
            this.btnApply = new System.Windows.Forms.Button();
            this.btnSave = new System.Windows.Forms.Button();
            this.btnLoad = new System.Windows.Forms.Button();
            this.txtTXaddr = new System.Windows.Forms.TextBox();
            this.lblTXaddr = new System.Windows.Forms.Label();
            this.chkPPM = new System.Windows.Forms.CheckBox();
            this.lblRXaddr = new System.Windows.Forms.Label();
            this.txtRXaddr = new System.Windows.Forms.TextBox();
            this.lblWarning = new System.Windows.Forms.Label();
            this.chkLFSR = new System.Windows.Forms.CheckBox();
            this.tabControl1.SuspendLayout();
            this.tabBER.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.numPktRate)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.numPktSize)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.numSampleRate)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.numAvgSpan)).BeginInit();
            this.grpAvgWeight.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.numHalfLife)).BeginInit();
            this.tabSensor.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.numSenseRate)).BeginInit();
            this.SuspendLayout();
            // 
            // tabControl1
            // 
            this.tabControl1.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.tabControl1.Controls.Add(this.tabBER);
            this.tabControl1.Controls.Add(this.tabAudio);
            this.tabControl1.Controls.Add(this.tabFile);
            this.tabControl1.Controls.Add(this.tabSensor);
            this.tabControl1.Location = new System.Drawing.Point(12, 64);
            this.tabControl1.Name = "tabControl1";
            this.tabControl1.SelectedIndex = 0;
            this.tabControl1.Size = new System.Drawing.Size(450, 265);
            this.tabControl1.TabIndex = 0;
            // 
            // tabBER
            // 
            this.tabBER.Controls.Add(this.chkLFSR);
            this.tabBER.Controls.Add(this.lblBytes);
            this.tabBER.Controls.Add(this.lblPktSize);
            this.tabBER.Controls.Add(this.lblPktRate);
            this.tabBER.Controls.Add(this.lblPktSec);
            this.tabBER.Controls.Add(this.numPktRate);
            this.tabBER.Controls.Add(this.numPktSize);
            this.tabBER.Controls.Add(this.lblRateSec);
            this.tabBER.Controls.Add(this.numSampleRate);
            this.tabBER.Controls.Add(this.lblSampleRate);
            this.tabBER.Controls.Add(this.lblAvg);
            this.tabBER.Controls.Add(this.lblAvgSec);
            this.tabBER.Controls.Add(this.numAvgSpan);
            this.tabBER.Controls.Add(this.grpAvgWeight);
            this.tabBER.Location = new System.Drawing.Point(4, 22);
            this.tabBER.Name = "tabBER";
            this.tabBER.Padding = new System.Windows.Forms.Padding(3);
            this.tabBER.Size = new System.Drawing.Size(442, 239);
            this.tabBER.TabIndex = 0;
            this.tabBER.Text = "BER Measurement";
            this.tabBER.UseVisualStyleBackColor = true;
            // 
            // lblBytes
            // 
            this.lblBytes.AutoSize = true;
            this.lblBytes.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.lblBytes.Location = new System.Drawing.Point(185, 11);
            this.lblBytes.Name = "lblBytes";
            this.lblBytes.Size = new System.Drawing.Size(37, 13);
            this.lblBytes.TabIndex = 17;
            this.lblBytes.Text = "bytes";
            // 
            // lblPktSize
            // 
            this.lblPktSize.AutoSize = true;
            this.lblPktSize.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.lblPktSize.Location = new System.Drawing.Point(9, 11);
            this.lblPktSize.Name = "lblPktSize";
            this.lblPktSize.Size = new System.Drawing.Size(77, 13);
            this.lblPktSize.TabIndex = 16;
            this.lblPktSize.Text = "Packet size:";
            // 
            // lblPktRate
            // 
            this.lblPktRate.AutoSize = true;
            this.lblPktRate.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.lblPktRate.Location = new System.Drawing.Point(9, 37);
            this.lblPktRate.Name = "lblPktRate";
            this.lblPktRate.Size = new System.Drawing.Size(93, 13);
            this.lblPktRate.TabIndex = 15;
            this.lblPktRate.Text = "1 Packet every";
            // 
            // lblPktSec
            // 
            this.lblPktSec.AutoSize = true;
            this.lblPktSec.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.lblPktSec.Location = new System.Drawing.Point(185, 37);
            this.lblPktSec.Name = "lblPktSec";
            this.lblPktSec.Size = new System.Drawing.Size(54, 13);
            this.lblPktSec.TabIndex = 14;
            this.lblPktSec.Text = "seconds";
            // 
            // numPktRate
            // 
            this.numPktRate.DecimalPlaces = 4;
            this.numPktRate.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.numPktRate.Increment = new decimal(new int[] {
            1,
            0,
            0,
            131072});
            this.numPktRate.Location = new System.Drawing.Point(115, 35);
            this.numPktRate.Maximum = new decimal(new int[] {
            6,
            0,
            0,
            0});
            this.numPktRate.Minimum = new decimal(new int[] {
            1,
            0,
            0,
            262144});
            this.numPktRate.Name = "numPktRate";
            this.numPktRate.Size = new System.Drawing.Size(64, 20);
            this.numPktRate.TabIndex = 13;
            this.numPktRate.Value = new decimal(new int[] {
            2,
            0,
            0,
            0});
            // 
            // numPktSize
            // 
            this.numPktSize.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.numPktSize.Location = new System.Drawing.Point(115, 9);
            this.numPktSize.Maximum = new decimal(new int[] {
            256,
            0,
            0,
            0});
            this.numPktSize.Minimum = new decimal(new int[] {
            1,
            0,
            0,
            0});
            this.numPktSize.Name = "numPktSize";
            this.numPktSize.Size = new System.Drawing.Size(64, 20);
            this.numPktSize.TabIndex = 12;
            this.numPktSize.Value = new decimal(new int[] {
            64,
            0,
            0,
            0});
            // 
            // lblRateSec
            // 
            this.lblRateSec.AutoSize = true;
            this.lblRateSec.Location = new System.Drawing.Point(185, 63);
            this.lblRateSec.Name = "lblRateSec";
            this.lblRateSec.Size = new System.Drawing.Size(47, 13);
            this.lblRateSec.TabIndex = 10;
            this.lblRateSec.Text = "seconds";
            // 
            // numSampleRate
            // 
            this.numSampleRate.DecimalPlaces = 3;
            this.numSampleRate.Increment = new decimal(new int[] {
            5,
            0,
            0,
            131072});
            this.numSampleRate.Location = new System.Drawing.Point(115, 61);
            this.numSampleRate.Maximum = new decimal(new int[] {
            6,
            0,
            0,
            0});
            this.numSampleRate.Minimum = new decimal(new int[] {
            1,
            0,
            0,
            65536});
            this.numSampleRate.Name = "numSampleRate";
            this.numSampleRate.Size = new System.Drawing.Size(64, 20);
            this.numSampleRate.TabIndex = 11;
            this.numSampleRate.Value = new decimal(new int[] {
            1,
            0,
            0,
            0});
            // 
            // lblSampleRate
            // 
            this.lblSampleRate.AutoSize = true;
            this.lblSampleRate.Location = new System.Drawing.Point(9, 63);
            this.lblSampleRate.Name = "lblSampleRate";
            this.lblSampleRate.Size = new System.Drawing.Size(100, 13);
            this.lblSampleRate.TabIndex = 9;
            this.lblSampleRate.Text = "GUI sample period :";
            // 
            // lblAvg
            // 
            this.lblAvg.AutoSize = true;
            this.lblAvg.Location = new System.Drawing.Point(9, 89);
            this.lblAvg.Name = "lblAvg";
            this.lblAvg.Size = new System.Drawing.Size(71, 13);
            this.lblAvg.TabIndex = 8;
            this.lblAvg.Text = "Average over";
            // 
            // lblAvgSec
            // 
            this.lblAvgSec.AutoSize = true;
            this.lblAvgSec.Location = new System.Drawing.Point(185, 89);
            this.lblAvgSec.Name = "lblAvgSec";
            this.lblAvgSec.Size = new System.Drawing.Size(47, 13);
            this.lblAvgSec.TabIndex = 6;
            this.lblAvgSec.Text = "seconds";
            // 
            // numAvgSpan
            // 
            this.numAvgSpan.DecimalPlaces = 3;
            this.numAvgSpan.Increment = new decimal(new int[] {
            1,
            0,
            0,
            65536});
            this.numAvgSpan.Location = new System.Drawing.Point(115, 87);
            this.numAvgSpan.Maximum = new decimal(new int[] {
            900000,
            0,
            0,
            196608});
            this.numAvgSpan.Name = "numAvgSpan";
            this.numAvgSpan.Size = new System.Drawing.Size(64, 20);
            this.numAvgSpan.TabIndex = 7;
            this.numAvgSpan.Value = new decimal(new int[] {
            900000,
            0,
            0,
            196608});
            // 
            // grpAvgWeight
            // 
            this.grpAvgWeight.Controls.Add(this.radFlatAvg);
            this.grpAvgWeight.Controls.Add(this.lblHLsec);
            this.grpAvgWeight.Controls.Add(this.radTriangleAvg);
            this.grpAvgWeight.Controls.Add(this.lblHalfLife);
            this.grpAvgWeight.Controls.Add(this.radHalfLifeAvg);
            this.grpAvgWeight.Controls.Add(this.numHalfLife);
            this.grpAvgWeight.Location = new System.Drawing.Point(6, 113);
            this.grpAvgWeight.Name = "grpAvgWeight";
            this.grpAvgWeight.Size = new System.Drawing.Size(234, 114);
            this.grpAvgWeight.TabIndex = 6;
            this.grpAvgWeight.TabStop = false;
            this.grpAvgWeight.Text = "Averaging Weight";
            // 
            // radFlatAvg
            // 
            this.radFlatAvg.AutoSize = true;
            this.radFlatAvg.Checked = true;
            this.radFlatAvg.Location = new System.Drawing.Point(6, 19);
            this.radFlatAvg.Name = "radFlatAvg";
            this.radFlatAvg.Size = new System.Drawing.Size(61, 17);
            this.radFlatAvg.TabIndex = 0;
            this.radFlatAvg.TabStop = true;
            this.radFlatAvg.Text = "Uniform";
            this.radFlatAvg.UseVisualStyleBackColor = true;
            this.radFlatAvg.CheckedChanged += new System.EventHandler(this.radFlatAvg_CheckedChanged);
            // 
            // lblHLsec
            // 
            this.lblHLsec.AutoSize = true;
            this.lblHLsec.Location = new System.Drawing.Point(179, 90);
            this.lblHLsec.Name = "lblHLsec";
            this.lblHLsec.Size = new System.Drawing.Size(47, 13);
            this.lblHLsec.TabIndex = 5;
            this.lblHLsec.Text = "seconds";
            // 
            // radTriangleAvg
            // 
            this.radTriangleAvg.AutoSize = true;
            this.radTriangleAvg.Location = new System.Drawing.Point(6, 42);
            this.radTriangleAvg.Name = "radTriangleAvg";
            this.radTriangleAvg.Size = new System.Drawing.Size(72, 17);
            this.radTriangleAvg.TabIndex = 1;
            this.radTriangleAvg.Text = "Triangular";
            this.radTriangleAvg.UseVisualStyleBackColor = true;
            this.radTriangleAvg.CheckedChanged += new System.EventHandler(this.radTriangleAvg_CheckedChanged);
            // 
            // lblHalfLife
            // 
            this.lblHalfLife.AutoSize = true;
            this.lblHalfLife.Location = new System.Drawing.Point(3, 90);
            this.lblHalfLife.Name = "lblHalfLife";
            this.lblHalfLife.Size = new System.Drawing.Size(49, 13);
            this.lblHalfLife.TabIndex = 4;
            this.lblHalfLife.Text = "Half-Life:";
            // 
            // radHalfLifeAvg
            // 
            this.radHalfLifeAvg.AutoSize = true;
            this.radHalfLifeAvg.Location = new System.Drawing.Point(6, 65);
            this.radHalfLifeAvg.Name = "radHalfLifeAvg";
            this.radHalfLifeAvg.Size = new System.Drawing.Size(128, 17);
            this.radHalfLifeAvg.TabIndex = 2;
            this.radHalfLifeAvg.Text = "Exponential (Half-Life)";
            this.radHalfLifeAvg.UseVisualStyleBackColor = true;
            this.radHalfLifeAvg.CheckedChanged += new System.EventHandler(this.radHalfLifeAvg_CheckedChanged);
            // 
            // numHalfLife
            // 
            this.numHalfLife.DecimalPlaces = 3;
            this.numHalfLife.Enabled = false;
            this.numHalfLife.Increment = new decimal(new int[] {
            1,
            0,
            0,
            65536});
            this.numHalfLife.Location = new System.Drawing.Point(109, 88);
            this.numHalfLife.Maximum = new decimal(new int[] {
            60,
            0,
            0,
            0});
            this.numHalfLife.Name = "numHalfLife";
            this.numHalfLife.Size = new System.Drawing.Size(64, 20);
            this.numHalfLife.TabIndex = 3;
            this.numHalfLife.Value = new decimal(new int[] {
            6,
            0,
            0,
            0});
            // 
            // tabAudio
            // 
            this.tabAudio.Location = new System.Drawing.Point(4, 22);
            this.tabAudio.Name = "tabAudio";
            this.tabAudio.Padding = new System.Windows.Forms.Padding(3);
            this.tabAudio.Size = new System.Drawing.Size(442, 239);
            this.tabAudio.TabIndex = 1;
            this.tabAudio.Text = "Audio Streaming";
            this.tabAudio.UseVisualStyleBackColor = true;
            // 
            // tabFile
            // 
            this.tabFile.Location = new System.Drawing.Point(4, 22);
            this.tabFile.Name = "tabFile";
            this.tabFile.Size = new System.Drawing.Size(442, 239);
            this.tabFile.TabIndex = 2;
            this.tabFile.Text = "File Transfer";
            this.tabFile.UseVisualStyleBackColor = true;
            // 
            // tabSensor
            // 
            this.tabSensor.Controls.Add(this.label1);
            this.tabSensor.Controls.Add(this.lblSensRateSec);
            this.tabSensor.Controls.Add(this.numSenseRate);
            this.tabSensor.Controls.Add(this.chkAck);
            this.tabSensor.Controls.Add(this.lblLightInt);
            this.tabSensor.Controls.Add(this.lblRHres);
            this.tabSensor.Controls.Add(this.lblTres);
            this.tabSensor.Controls.Add(this.cboxTres);
            this.tabSensor.Controls.Add(this.cboxRHres);
            this.tabSensor.Controls.Add(this.cboxLightInt);
            this.tabSensor.Controls.Add(this.lblSensorList);
            this.tabSensor.Controls.Add(this.chkListSensors);
            this.tabSensor.Location = new System.Drawing.Point(4, 22);
            this.tabSensor.Name = "tabSensor";
            this.tabSensor.Size = new System.Drawing.Size(442, 239);
            this.tabSensor.TabIndex = 3;
            this.tabSensor.Text = "Sensor Measurement";
            this.tabSensor.UseVisualStyleBackColor = true;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label1.Location = new System.Drawing.Point(26, 84);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(79, 13);
            this.label1.TabIndex = 18;
            this.label1.Text = "1 Packet every";
            // 
            // lblSensRateSec
            // 
            this.lblSensRateSec.AutoSize = true;
            this.lblSensRateSec.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.lblSensRateSec.Location = new System.Drawing.Point(181, 84);
            this.lblSensRateSec.Name = "lblSensRateSec";
            this.lblSensRateSec.Size = new System.Drawing.Size(47, 13);
            this.lblSensRateSec.TabIndex = 17;
            this.lblSensRateSec.Text = "seconds";
            // 
            // numSenseRate
            // 
            this.numSenseRate.DecimalPlaces = 2;
            this.numSenseRate.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.numSenseRate.Increment = new decimal(new int[] {
            1,
            0,
            0,
            65536});
            this.numSenseRate.Location = new System.Drawing.Point(111, 82);
            this.numSenseRate.Maximum = new decimal(new int[] {
            60,
            0,
            0,
            0});
            this.numSenseRate.Minimum = new decimal(new int[] {
            1,
            0,
            0,
            131072});
            this.numSenseRate.Name = "numSenseRate";
            this.numSenseRate.Size = new System.Drawing.Size(64, 20);
            this.numSenseRate.TabIndex = 16;
            this.numSenseRate.Value = new decimal(new int[] {
            2,
            0,
            0,
            0});
            // 
            // chkAck
            // 
            this.chkAck.AutoSize = true;
            this.chkAck.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.chkAck.Location = new System.Drawing.Point(29, 32);
            this.chkAck.Name = "chkAck";
            this.chkAck.Size = new System.Drawing.Size(196, 17);
            this.chkAck.TabIndex = 9;
            this.chkAck.Text = "Use acknowledgment packets";
            this.chkAck.UseVisualStyleBackColor = true;
            // 
            // lblLightInt
            // 
            this.lblLightInt.AutoSize = true;
            this.lblLightInt.Location = new System.Drawing.Point(229, 193);
            this.lblLightInt.Name = "lblLightInt";
            this.lblLightInt.Size = new System.Drawing.Size(107, 13);
            this.lblLightInt.TabIndex = 7;
            this.lblLightInt.Text = "Light integration time:";
            // 
            // lblRHres
            // 
            this.lblRHres.AutoSize = true;
            this.lblRHres.Location = new System.Drawing.Point(238, 166);
            this.lblRHres.Name = "lblRHres";
            this.lblRHres.Size = new System.Drawing.Size(98, 13);
            this.lblRHres.TabIndex = 6;
            this.lblRHres.Text = "Humidity resolution:";
            // 
            // lblTres
            // 
            this.lblTres.AutoSize = true;
            this.lblTres.Location = new System.Drawing.Point(218, 139);
            this.lblTres.Name = "lblTres";
            this.lblTres.Size = new System.Drawing.Size(118, 13);
            this.lblTres.TabIndex = 5;
            this.lblTres.Text = "Temperature resolution:";
            // 
            // cboxTres
            // 
            this.cboxTres.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cboxTres.FormattingEnabled = true;
            this.cboxTres.Items.AddRange(new object[] {
            "14 bits",
            "13 bits",
            "12 bits",
            "11 bits"});
            this.cboxTres.Location = new System.Drawing.Point(342, 136);
            this.cboxTres.Name = "cboxTres";
            this.cboxTres.Size = new System.Drawing.Size(64, 21);
            this.cboxTres.TabIndex = 4;
            // 
            // cboxRHres
            // 
            this.cboxRHres.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cboxRHres.FormattingEnabled = true;
            this.cboxRHres.Items.AddRange(new object[] {
            "12 bits",
            "11 bits",
            "10 bits",
            "8 bits"});
            this.cboxRHres.Location = new System.Drawing.Point(342, 163);
            this.cboxRHres.Name = "cboxRHres";
            this.cboxRHres.Size = new System.Drawing.Size(64, 21);
            this.cboxRHres.TabIndex = 3;
            // 
            // cboxLightInt
            // 
            this.cboxLightInt.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cboxLightInt.FormattingEnabled = true;
            this.cboxLightInt.Items.AddRange(new object[] {
            "800 ms",
            "100 ms"});
            this.cboxLightInt.Location = new System.Drawing.Point(342, 190);
            this.cboxLightInt.Name = "cboxLightInt";
            this.cboxLightInt.Size = new System.Drawing.Size(64, 21);
            this.cboxLightInt.TabIndex = 2;
            // 
            // lblSensorList
            // 
            this.lblSensorList.AutoSize = true;
            this.lblSensorList.Location = new System.Drawing.Point(26, 131);
            this.lblSensorList.Name = "lblSensorList";
            this.lblSensorList.Size = new System.Drawing.Size(74, 13);
            this.lblSensorList.TabIndex = 1;
            this.lblSensorList.Text = "Sensors used:";
            // 
            // chkListSensors
            // 
            this.chkListSensors.CheckOnClick = true;
            this.chkListSensors.FormattingEnabled = true;
            this.chkListSensors.Items.AddRange(new object[] {
            "Temperature",
            "Relative humidity",
            "Light intensity",
            "Acceleration"});
            this.chkListSensors.Location = new System.Drawing.Point(29, 147);
            this.chkListSensors.Name = "chkListSensors";
            this.chkListSensors.Size = new System.Drawing.Size(108, 64);
            this.chkListSensors.TabIndex = 0;
            this.chkListSensors.ThreeDCheckBoxes = true;
            // 
            // btnOK
            // 
            this.btnOK.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btnOK.DialogResult = System.Windows.Forms.DialogResult.OK;
            this.btnOK.Location = new System.Drawing.Point(225, 335);
            this.btnOK.Name = "btnOK";
            this.btnOK.Size = new System.Drawing.Size(75, 23);
            this.btnOK.TabIndex = 1;
            this.btnOK.Text = "OK";
            this.btnOK.UseMnemonic = false;
            this.btnOK.UseVisualStyleBackColor = true;
            this.btnOK.Click += new System.EventHandler(this.btnOK_Click);
            // 
            // btnCancel
            // 
            this.btnCancel.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btnCancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.btnCancel.Location = new System.Drawing.Point(306, 335);
            this.btnCancel.Name = "btnCancel";
            this.btnCancel.Size = new System.Drawing.Size(75, 23);
            this.btnCancel.TabIndex = 2;
            this.btnCancel.Text = "Cancel";
            this.btnCancel.UseMnemonic = false;
            this.btnCancel.UseVisualStyleBackColor = true;
            this.btnCancel.Click += new System.EventHandler(this.btnCancel_Click);
            // 
            // btnApply
            // 
            this.btnApply.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btnApply.Location = new System.Drawing.Point(387, 335);
            this.btnApply.Name = "btnApply";
            this.btnApply.Size = new System.Drawing.Size(75, 23);
            this.btnApply.TabIndex = 3;
            this.btnApply.Text = "Apply";
            this.btnApply.UseMnemonic = false;
            this.btnApply.UseVisualStyleBackColor = true;
            this.btnApply.Click += new System.EventHandler(this.btnApply_Click);
            // 
            // btnSave
            // 
            this.btnSave.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.btnSave.Location = new System.Drawing.Point(12, 335);
            this.btnSave.Name = "btnSave";
            this.btnSave.Size = new System.Drawing.Size(75, 23);
            this.btnSave.TabIndex = 4;
            this.btnSave.Text = "Save";
            this.btnSave.UseVisualStyleBackColor = true;
            this.btnSave.Click += new System.EventHandler(this.btnSave_Click);
            // 
            // btnLoad
            // 
            this.btnLoad.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.btnLoad.Location = new System.Drawing.Point(93, 335);
            this.btnLoad.Name = "btnLoad";
            this.btnLoad.Size = new System.Drawing.Size(75, 23);
            this.btnLoad.TabIndex = 5;
            this.btnLoad.Text = "Load";
            this.btnLoad.UseVisualStyleBackColor = true;
            this.btnLoad.Click += new System.EventHandler(this.btnLoad_Click);
            // 
            // txtTXaddr
            // 
            this.txtTXaddr.CharacterCasing = System.Windows.Forms.CharacterCasing.Upper;
            this.txtTXaddr.Location = new System.Drawing.Point(139, 38);
            this.txtTXaddr.MaxLength = 4;
            this.txtTXaddr.Name = "txtTXaddr";
            this.txtTXaddr.Size = new System.Drawing.Size(37, 20);
            this.txtTXaddr.TabIndex = 6;
            this.txtTXaddr.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.addr_KeyPress);
            this.txtTXaddr.LostFocus += new System.EventHandler(this.txtTXaddr_LostFocus);
            // 
            // lblTXaddr
            // 
            this.lblTXaddr.AutoSize = true;
            this.lblTXaddr.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.lblTXaddr.Location = new System.Drawing.Point(12, 41);
            this.lblTXaddr.Name = "lblTXaddr";
            this.lblTXaddr.Size = new System.Drawing.Size(121, 13);
            this.lblTXaddr.TabIndex = 7;
            this.lblTXaddr.Text = "TX Spark address (hex):";
            // 
            // chkPPM
            // 
            this.chkPPM.AutoSize = true;
            this.chkPPM.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.chkPPM.Location = new System.Drawing.Point(410, 40);
            this.chkPPM.Name = "chkPPM";
            this.chkPPM.Size = new System.Drawing.Size(52, 17);
            this.chkPPM.TabIndex = 8;
            this.chkPPM.Text = "PPM";
            this.chkPPM.UseVisualStyleBackColor = true;
            // 
            // lblRXaddr
            // 
            this.lblRXaddr.AutoSize = true;
            this.lblRXaddr.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.lblRXaddr.Location = new System.Drawing.Point(197, 41);
            this.lblRXaddr.Name = "lblRXaddr";
            this.lblRXaddr.Size = new System.Drawing.Size(145, 13);
            this.lblRXaddr.TabIndex = 10;
            this.lblRXaddr.Text = "RX Spark address (hex):";
            // 
            // txtRXaddr
            // 
            this.txtRXaddr.CharacterCasing = System.Windows.Forms.CharacterCasing.Upper;
            this.txtRXaddr.Location = new System.Drawing.Point(348, 38);
            this.txtRXaddr.MaxLength = 4;
            this.txtRXaddr.Name = "txtRXaddr";
            this.txtRXaddr.Size = new System.Drawing.Size(37, 20);
            this.txtRXaddr.TabIndex = 9;
            this.txtRXaddr.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.addr_KeyPress);
            this.txtRXaddr.LostFocus += new System.EventHandler(this.txtRXaddr_LostFocus);
            // 
            // lblWarning
            // 
            this.lblWarning.AutoSize = true;
            this.lblWarning.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.lblWarning.Location = new System.Drawing.Point(12, 9);
            this.lblWarning.Name = "lblWarning";
            this.lblWarning.Size = new System.Drawing.Size(410, 26);
            this.lblWarning.TabIndex = 11;
            this.lblWarning.Text = "WARNING: Local and remote node configurations must be compatible :\r\nOptions writt" +
    "en in bold must be matched between TX and RX nodes.";
            // 
            // chkLFSR
            // 
            this.chkLFSR.AutoSize = true;
            this.chkLFSR.Checked = true;
            this.chkLFSR.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chkLFSR.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.chkLFSR.Location = new System.Drawing.Point(331, 12);
            this.chkLFSR.Name = "chkLFSR";
            this.chkLFSR.Size = new System.Drawing.Size(105, 17);
            this.chkLFSR.TabIndex = 18;
            this.chkLFSR.Text = "bit randomizer";
            this.chkLFSR.UseVisualStyleBackColor = true;
            // 
            // SettingsForm
            // 
            this.AcceptButton = this.btnOK;
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.CancelButton = this.btnCancel;
            this.ClientSize = new System.Drawing.Size(474, 370);
            this.Controls.Add(this.lblWarning);
            this.Controls.Add(this.lblRXaddr);
            this.Controls.Add(this.txtRXaddr);
            this.Controls.Add(this.chkPPM);
            this.Controls.Add(this.lblTXaddr);
            this.Controls.Add(this.txtTXaddr);
            this.Controls.Add(this.btnLoad);
            this.Controls.Add(this.btnSave);
            this.Controls.Add(this.btnApply);
            this.Controls.Add(this.btnCancel);
            this.Controls.Add(this.btnOK);
            this.Controls.Add(this.tabControl1);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.MaximizeBox = false;
            this.Name = "SettingsForm";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "Spark Radio Configuration Settings";
            this.tabControl1.ResumeLayout(false);
            this.tabBER.ResumeLayout(false);
            this.tabBER.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.numPktRate)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.numPktSize)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.numSampleRate)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.numAvgSpan)).EndInit();
            this.grpAvgWeight.ResumeLayout(false);
            this.grpAvgWeight.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.numHalfLife)).EndInit();
            this.tabSensor.ResumeLayout(false);
            this.tabSensor.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.numSenseRate)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.TabControl tabControl1;
        private System.Windows.Forms.TabPage tabBER;
        private System.Windows.Forms.TabPage tabAudio;
        private System.Windows.Forms.Button btnOK;
        private System.Windows.Forms.Button btnCancel;
        private System.Windows.Forms.Button btnApply;
        private System.Windows.Forms.TabPage tabFile;
        private System.Windows.Forms.TabPage tabSensor;
        private System.Windows.Forms.Button btnSave;
        private System.Windows.Forms.Button btnLoad;
        private System.Windows.Forms.TextBox txtTXaddr;
        private System.Windows.Forms.Label lblTXaddr;
        private System.Windows.Forms.CheckBox chkPPM;
        private System.Windows.Forms.Label lblRXaddr;
        private System.Windows.Forms.TextBox txtRXaddr;
        private System.Windows.Forms.Label lblWarning;
        private System.Windows.Forms.RadioButton radFlatAvg;
        private System.Windows.Forms.RadioButton radTriangleAvg;
        private System.Windows.Forms.RadioButton radHalfLifeAvg;
        private System.Windows.Forms.Label lblHalfLife;
        private System.Windows.Forms.NumericUpDown numHalfLife;
        private System.Windows.Forms.Label lblHLsec;
        private System.Windows.Forms.GroupBox grpAvgWeight;
        private System.Windows.Forms.NumericUpDown numAvgSpan;
        private System.Windows.Forms.Label lblAvgSec;
        private System.Windows.Forms.Label lblAvg;
        private System.Windows.Forms.Label lblSampleRate;
        private System.Windows.Forms.Label lblRateSec;
        private System.Windows.Forms.NumericUpDown numSampleRate;
        private System.Windows.Forms.NumericUpDown numPktSize;
        private System.Windows.Forms.NumericUpDown numPktRate;
        private System.Windows.Forms.Label lblPktSec;
        private System.Windows.Forms.Label lblPktRate;
        private System.Windows.Forms.Label lblPktSize;
        private System.Windows.Forms.Label lblBytes;
        private System.Windows.Forms.Label lblSensorList;
        private System.Windows.Forms.CheckedListBox chkListSensors;
        private System.Windows.Forms.ComboBox cboxLightInt;
        private System.Windows.Forms.ComboBox cboxRHres;
        private System.Windows.Forms.ComboBox cboxTres;
        private System.Windows.Forms.Label lblRHres;
        private System.Windows.Forms.Label lblTres;
        private System.Windows.Forms.Label lblLightInt;
        private System.Windows.Forms.CheckBox chkAck;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label lblSensRateSec;
        private System.Windows.Forms.NumericUpDown numSenseRate;
        private System.Windows.Forms.CheckBox chkLFSR;
    }
}