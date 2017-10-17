using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Drawing.Text;
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
        public static double[] log10of = { -0.5, 0.0, Math.Log10(2.0), Math.Log10(3.0), Math.Log10(4.0), Math.Log10(5.0), Math.Log10(6.0), Math.Log10(7.0), Math.Log10(8.0), Math.Log10(9.0) };
        public static string[] expUnicode = { "\u207B\u2076", "\u207B\u2075", "\u207B\u2074", "\u207B\u00B3", "\u207B\u00B2", "\u207B\u00B9", "\u2070", "\u00B9", "\u00B2", "\u00B3", "\u2074", "\u2075" };
        public static string TXmessage = "(Data collection can only be performed on the receiving end.)";

        private double selectXdiv(double zoomlevel)
        {
            if (zoomlevel >= 0.5)
                return 0.1;
            else if (zoomlevel >= 0.25)
                return 0.2;
            else if (zoomlevel >= 0.1)
                return 0.5;
            else if (zoomlevel >= 0.05)
                return 1.0;
            else if (zoomlevel >= 0.025)
                return 2.0;
            else if (zoomlevel >= 0.01)
                return 5.0;
            else if (zoomlevel >= 0.005)
                return 10.0;
            else if (zoomlevel >= 0.005 / 3.0)
                return 30.0;
            else if (zoomlevel >= 0.005 / 6.0)
                return 60.0;
            else if (zoomlevel >= 0.005 / 12.0)
                return 120.0;
            else if (zoomlevel >= 0.005 / 30.0)
                return 300.0;
            else if (zoomlevel >= 0.005 / 60.0)
                return 600.0;
            else if (zoomlevel >= 0.005 / 90.0)
                return 900.0;
            else if (zoomlevel >= 0.005 / 180.0)
                return 1800.0;
            else
                return 3600.0;
        }

        private double selectYdiv(double Yspan, int h)
        {
            double zoomlevel = Math.Max(h, 100) / Yspan;
            if (zoomlevel >= 5000.0)
                return 0.01;
            else if (zoomlevel >= 2500.0)
                return (Yspan < 0.04 ? 0.01 : 0.02);
            else if (zoomlevel >= 1000.0)
                return (Yspan < 0.1 ? 0.02 : 0.05);
            else if (zoomlevel >= 500.0)
                return (Yspan < 0.2 ? 0.05 : 0.1);
            else if (zoomlevel >= 250.0)
                return (Yspan < 0.4 ? 0.1 : 0.2);
            else if (zoomlevel >= 100.0)
                return (Yspan < 1.0 ? 0.2 : 0.5);
            else if (zoomlevel >= 50.0)
                return (Yspan < 2.0 ? 0.5 : 1.0);
            else if (zoomlevel >= 25.0)
                return (Yspan < 4.0 ? 1.0 : 2.0);
            else if (zoomlevel >= 10.0)
                return (Yspan < 10.0 ? 2.0 : 5.0);
            else if (zoomlevel >= 5.0)
                return (Yspan < 20.0 ? 5.0 : 10.0);
            else if (zoomlevel >= 2.0)
                return (Yspan < 40.0 ? 10.0 : 20.0);
            else
                return (Yspan < 100.0 ? 20.0 : 50.0);
        }

        private void OnPaintTempGraph(object sender, PaintEventArgs e)
        {
            if (!((USBthread is Thread) && USBthread.IsAlive))
                return;

            using (SolidBrush myBrush = new SolidBrush(Color.Black))
            using (StringFormat axisStrFormat = new StringFormat())
            using (Font axisFont = new Font("Arial", 156.0f / CurrentAutoScaleDimensions.Height))
            using (Pen axisLines = new Pen(Color.WhiteSmoke, 2.4f))
            using (Pen grid = new Pen(Color.DimGray, 0.95f))
            using (Pen signalPen = new Pen(Color.Black, 3.0f))
            {
                string str = "node temperature (in \u00B0C) over time (";
                int plotWidth = splitContainer1.Panel1.ClientSize.Width - 36;
                int plotHeight = splitContainer1.Panel1.ClientSize.Height - 20;
                DateTime leftBoundary, latestSample = DateTime.MaxValue;
                double Xzoom = zoom / (double)nodeSettings.Sen_PktRate;
                double Xdiv = 1000.0 * selectXdiv(Xzoom); // to be optimized
                double Xratio, pixelPerDiv = Xdiv * Xzoom; // to be optimized
                double topY = 130.0, botY = -50.0, Yspan = 180.0;
                float lastY = -99.9f, lastX, x, y, minY = -50.0f, maxY = 130.0f;
                double Ydiv, Yratio, Yrounding;

                if (nodes.Count > 0)
                {
                    latestSample = nodes.Max(n => (n.displaySignal(1) && n.sampleList.Count > 0) ? n.sampleList[n.sampleList.Count - 1].sampleTime : DateTime.MinValue);
                    if (latestSample == DateTime.MinValue)
                        latestSample = DateTime.MaxValue;
                    leftBoundary = latestSample - TimeSpan.FromMilliseconds(plotWidth / Xzoom);
                    minY = nodes.Min(n => (n.displaySignal(1) && n.sampleList.Count > 0) ? n.sampleList.Min(s => (s.sampleTime > leftBoundary ? s.temp : 130.0f)) : 130.0f); // perfs could be optimized
                    maxY = nodes.Max(n => (n.displaySignal(1) && n.sampleList.Count > 0) ? n.sampleList.Max(s => (s.sampleTime > leftBoundary ? s.temp : -50.0f)) : -50.0f); // perfs could be optimized
                    if (minY > maxY)
                    {
                        minY = -50.0f;
                        maxY = 130.0f;
                    }
                    Yspan = maxY - minY;
                    if (Yspan < 10.0)
                        Yrounding = 1.0;
                    else if (Yspan < 50.0)
                        Yrounding = 5.0;
                    else
                        Yrounding = 10.0;
                    topY = Yrounding * Math.Ceiling(maxY / Yrounding);
                    botY = Yrounding * Math.Floor(minY / Yrounding);
                    if (topY == botY)
                        botY -= 1.0;
                    Yspan = topY - botY;
                }
                Ydiv = selectYdiv(Yspan, plotHeight);
                Yratio = (double)plotHeight / Yspan;
                Xratio = Xzoom / 10000.0;

                axisStrFormat.LineAlignment = StringAlignment.Center;
                axisStrFormat.Alignment = StringAlignment.Far;
                grid.DashStyle = DashStyle.Dash;
                grid.DashCap = DashCap.Round;
                e.Graphics.TextRenderingHint = TextRenderingHint.AntiAlias;
                e.Graphics.SmoothingMode = SmoothingMode.AntiAlias;
                e.Graphics.FillRectangle(myBrush, -1, -1, splitContainer1.Panel1.ClientSize.Width, splitContainer1.Panel1.ClientSize.Height);
                myBrush.Color = Color.WhiteSmoke;
                if (radio == RadioType.TXonly)
                {
                    axisStrFormat.Alignment = StringAlignment.Center;
                    e.Graphics.DrawString(TXmessage, axisFont, myBrush, splitContainer1.Panel1.ClientSize.Width / 2.0f, splitContainer1.Panel1.ClientSize.Height / 2.0f, axisStrFormat);
                    return;
                }
                for (x = splitContainer1.Panel1.ClientSize.Width - (float)(pixelPerDiv * ((latestSample.ToBinary() / (long)10000) % (long)Xdiv) / Xdiv); x > 36.0f; x -= (float)pixelPerDiv)
                    e.Graphics.DrawLine(grid, x, 20.0f, x, splitContainer1.Panel1.ClientSize.Height);

                for (double yValue = Ydiv * Math.Floor(topY / Ydiv); yValue > botY; yValue -= Ydiv)
                {
                    y = (float)((topY - yValue) * Yratio + 20.0);
                    e.Graphics.DrawLine(grid, 36.0f, y, splitContainer1.Panel1.ClientSize.Width, y);
                    if (y - lastY >= 14.0)
                    {
                        e.Graphics.DrawString(yValue.ToString("0.#"), axisFont, myBrush, 36.0f, y, axisStrFormat);
                        lastY = y;
                    }
                }

                signalPen.StartCap = LineCap.RoundAnchor;
                signalPen.EndCap = LineCap.RoundAnchor;
                for (int i = nodes.Count - 1; i >= 0; --i)
                    if (nodes[i].displaySignal(1) && (nodes[i].sampleList.Count > 0))
                    {
                        lastX = (float)(splitContainer1.Panel1.ClientSize.Width - 1 - Xratio * (latestSample.ToBinary() - nodes[i].sampleList[nodes[i].sampleList.Count - 1].sampleTime.ToBinary()));
                        if (lastX >= 36.0f)
                        {
                            signalPen.Color = nodes[i].BackColor;
                            lastY = (float)((topY - nodes[i].sampleList[nodes[i].sampleList.Count - 1].temp) * Yratio + 20.0);
                            for (int j = nodes[i].sampleList.Count - 1; j >= 0; --j)
                            {
                                x = (float)(splitContainer1.Panel1.ClientSize.Width - Xratio * (latestSample.ToBinary() - nodes[i].sampleList[j].sampleTime.ToBinary()));
                                y = (float)((topY - nodes[i].sampleList[j].temp) * Yratio + 20.0);
                                if (x <= 36.0f)
                                {
                                    y = lastY + (y - lastY) * (lastX - 36.0f) / (lastX - x);
                                    signalPen.EndCap = LineCap.NoAnchor;
                                    e.Graphics.DrawLine(signalPen, lastX, lastY, 36.0f, y);
                                    signalPen.EndCap = LineCap.RoundAnchor;
                                    break;
                                }
                                e.Graphics.DrawLine(signalPen, lastX, lastY, x, y);
                                lastX = x;
                                lastY = y;
                            }
                        }
                    }

                axisLines.StartCap = LineCap.ArrowAnchor;
                myBrush.Color = Color.WhiteSmoke;
                e.Graphics.DrawLine(axisLines, 36.0f, 14.0f, 36.0f, splitContainer1.Panel1.ClientSize.Height);
                if (Xdiv > 100000.0)
                    str += (Xdiv / 60000.0) + " min./div)";
                else
                    str += (Xdiv / 1000.0) + " sec./div)";
                axisStrFormat.Alignment = StringAlignment.Center;
                e.Graphics.DrawString(str, axisFont, myBrush, splitContainer1.Panel1.ClientSize.Width / 2.0f, 10.0f, axisStrFormat);
            }
        }

        private void OnPaintRHgraph(object sender, PaintEventArgs e)
        {
            if (!((USBthread is Thread) && USBthread.IsAlive))
                return;

            using (SolidBrush myBrush = new SolidBrush(Color.Black))
            using (StringFormat axisStrFormat = new StringFormat())
            using (Font axisFont = new Font("Arial", 156.0f / CurrentAutoScaleDimensions.Height))
            using (Pen axisLines = new Pen(Color.WhiteSmoke, 2.4f))
            using (Pen grid = new Pen(Color.DimGray, 0.95f))
            using (Pen signalPen = new Pen(Color.Black, 3.0f))
            {
                string str = "node relative humidity (in %) over time (";
                int plotWidth = splitContainer2.Panel1.ClientSize.Width - 36;
                int plotHeight = splitContainer2.Panel1.ClientSize.Height - 20;
                DateTime leftBoundary, latestSample = DateTime.MaxValue;
                double Xzoom = zoom / (double)nodeSettings.Sen_PktRate;
                double Xdiv = 1000.0 * selectXdiv(Xzoom); // to be optimized
                double Xratio, pixelPerDiv = Xdiv * Xzoom; // to be optimized
                double topY = 100.0, botY = 0.0, Yspan = 100.0;
                float lastY = -99.9f, lastX, x, y, minY = 0.0f, maxY = 100.0f;
                double Ydiv, Yratio, Yrounding;

                if (nodes.Count > 0)
                {
                    latestSample = nodes.Max(n => (n.displaySignal(2) && n.sampleList.Count > 0) ? n.sampleList[n.sampleList.Count - 1].sampleTime : DateTime.MinValue);
                    if (latestSample == DateTime.MinValue)
                        latestSample = DateTime.MaxValue;
                    leftBoundary = latestSample - TimeSpan.FromMilliseconds(plotWidth / Xzoom);
                    minY = nodes.Min(n => (n.displaySignal(2) && n.sampleList.Count > 0) ? n.sampleList.Min(s => (s.sampleTime > leftBoundary ? s.RH : 100.0f)) : 100.0f); // perfs could be optimized
                    maxY = nodes.Max(n => (n.displaySignal(2) && n.sampleList.Count > 0) ? n.sampleList.Max(s => (s.sampleTime > leftBoundary ? s.RH : 0.0f)) : 0.0f); // perfs could be optimized
                    if (minY > maxY)
                    {
                        minY = 0.0f;
                        maxY = 100.0f;
                    }
                    Yspan = maxY - minY;
                    if (Yspan < 10.0)
                        Yrounding = 1.0;
                    else if (Yspan < 50.0)
                        Yrounding = 5.0;
                    else
                        Yrounding = 10.0;
                    topY = Yrounding * Math.Ceiling(maxY / Yrounding);
                    botY = Yrounding * Math.Floor(minY / Yrounding);
                    if (topY == botY)
                    {
                        if (topY < 1.0)
                            topY = 1.0;
                        else
                            botY -= 1.0;
                    }
                    Yspan = topY - botY;
                }
                Ydiv = selectYdiv(Yspan, plotHeight);
                Yratio = (double)plotHeight / Yspan;
                Xratio = Xzoom / 10000.0;

                axisStrFormat.LineAlignment = StringAlignment.Center;
                axisStrFormat.Alignment = StringAlignment.Far;
                grid.DashStyle = DashStyle.Dash;
                grid.DashCap = DashCap.Round;
                e.Graphics.TextRenderingHint = TextRenderingHint.AntiAlias;
                e.Graphics.SmoothingMode = SmoothingMode.AntiAlias;
                e.Graphics.FillRectangle(myBrush, -1, -1, splitContainer2.Panel1.ClientSize.Width, splitContainer2.Panel1.ClientSize.Height);
                myBrush.Color = Color.WhiteSmoke;
                if (radio == RadioType.TXonly)
                {
                    axisStrFormat.Alignment = StringAlignment.Center;
                    e.Graphics.DrawString(TXmessage, axisFont, myBrush, splitContainer2.Panel1.ClientSize.Width / 2.0f, splitContainer2.Panel1.ClientSize.Height / 2.0f, axisStrFormat);
                    return;
                }
                for (x = splitContainer2.Panel1.ClientSize.Width - (float)(pixelPerDiv * ((latestSample.ToBinary() / (long)10000) % (long)Xdiv) / Xdiv); x > 36.0f; x -= (float)pixelPerDiv)
                    e.Graphics.DrawLine(grid, x, 20.0f, x, splitContainer2.Panel1.ClientSize.Height);

                for (double yValue = Ydiv * Math.Floor(topY / Ydiv); yValue > botY; yValue -= Ydiv)
                {
                    y = (float)((topY - yValue) * Yratio + 20.0);
                    e.Graphics.DrawLine(grid, 36.0f, y, splitContainer2.Panel1.ClientSize.Width, y);
                    if (y - lastY >= 14.0)
                    {
                        e.Graphics.DrawString(yValue.ToString("0.#"), axisFont, myBrush, 36.0f, y, axisStrFormat);
                        lastY = y;
                    }
                }

                signalPen.StartCap = LineCap.RoundAnchor;
                signalPen.EndCap = LineCap.RoundAnchor;
                for (int i = nodes.Count - 1; i >= 0; --i)
                    if (nodes[i].displaySignal(2) && (nodes[i].sampleList.Count > 0))
                    {
                        lastX = (float)(splitContainer2.Panel1.ClientSize.Width - 1 - Xratio * (latestSample.ToBinary() - nodes[i].sampleList[nodes[i].sampleList.Count - 1].sampleTime.ToBinary()));
                        if (lastX >= 36.0f)
                        {
                            signalPen.Color = nodes[i].BackColor;
                            lastY = (float)((topY - nodes[i].sampleList[nodes[i].sampleList.Count - 1].RH) * Yratio + 20.0);
                            for (int j = nodes[i].sampleList.Count - 1; j >= 0; --j)
                            {
                                x = (float)(splitContainer2.Panel1.ClientSize.Width - Xratio * (latestSample.ToBinary() - nodes[i].sampleList[j].sampleTime.ToBinary()));
                                y = (float)((topY - nodes[i].sampleList[j].RH) * Yratio + 20.0);
                                if (x <= 36.0f)
                                {
                                    y = lastY + (y - lastY) * (lastX - 36.0f) / (lastX - x);
                                    signalPen.EndCap = LineCap.NoAnchor;
                                    e.Graphics.DrawLine(signalPen, lastX, lastY, 36.0f, y);
                                    signalPen.EndCap = LineCap.RoundAnchor;
                                    break;
                                }
                                e.Graphics.DrawLine(signalPen, lastX, lastY, x, y);
                                lastX = x;
                                lastY = y;
                            }
                        }
                    }

                axisLines.StartCap = LineCap.ArrowAnchor;
                myBrush.Color = Color.WhiteSmoke;
                e.Graphics.DrawLine(axisLines, 36.0f, 14.0f, 36.0f, splitContainer2.Panel1.ClientSize.Height);
                if (Xdiv > 100000.0)
                    str += (Xdiv / 60000.0) + " min./div)";
                else
                    str += (Xdiv / 1000.0) + " sec./div)";
                axisStrFormat.Alignment = StringAlignment.Center;
                e.Graphics.DrawString(str, axisFont, myBrush, splitContainer2.Panel1.ClientSize.Width / 2.0f, 10.0f, axisStrFormat);
            }
        }

        private void OnPaintLightGraph(object sender, PaintEventArgs e)
        {
            if (!((USBthread is Thread) && USBthread.IsAlive))
                return;

            using (SolidBrush myBrush = new SolidBrush(Color.Black))
            using (StringFormat axisStrFormat = new StringFormat())
            using (Font axisFont = new Font("Arial", 156.0f / CurrentAutoScaleDimensions.Height))
            using (Pen axisLines = new Pen(Color.WhiteSmoke, 2.4f))
            using (Pen grid = new Pen(Color.DimGray, 0.95f))
            using (Pen signalPen = new Pen(Color.Black, 3.0f))
            {
                string str = "light intensity on node (in lux) over time (";
                int plotWidth = splitContainer3.Panel1.ClientSize.Width - 36;
                int plotHeight = splitContainer3.Panel1.ClientSize.Height - 20;
                DateTime leftBoundary, latestSample = DateTime.MaxValue;
                double Xzoom = zoom / (double)nodeSettings.Sen_PktRate;
                double Xdiv = 1000.0 * selectXdiv(Xzoom); // to be optimized
                double Xratio, pixelPerDiv = Xdiv * Xzoom; // to be optimized
                double topY = 5.0, botY = -2.25, Yspan = 7.25;
                float lastY = -99.9f, lastX, x, y, minY = 0.01f, maxY = 90000.0f;
                double Yratio;
                bool showLogGrad;

                if (nodes.Count > 0)
                {
                    latestSample = nodes.Max(n => (n.displaySignal(3) && n.sampleList.Count > 0) ? n.sampleList[n.sampleList.Count - 1].sampleTime : DateTime.MinValue);
                    if (latestSample == DateTime.MinValue)
                        latestSample = DateTime.MaxValue;
                    leftBoundary = latestSample - TimeSpan.FromMilliseconds(plotWidth / Xzoom);
                    minY = nodes.Min(n => (n.displaySignal(3) && n.sampleList.Count > 0) ? n.sampleList.Min(s => (s.sampleTime > leftBoundary ? s.light : 90000.0f)) : 90000.0f); // perfs could be optimized
                    maxY = nodes.Max(n => (n.displaySignal(3) && n.sampleList.Count > 0) ? n.sampleList.Max(s => (s.sampleTime > leftBoundary ? s.light : 0.01f)) : 0.01f); // perfs could be optimized
                    if (minY > maxY)
                    {
                        minY = 0.01f;
                        maxY = 90000.0f;
                    }
                    botY = Math.Floor(Math.Log10(Math.Max(0.02f, minY))) - 0.25;
                    topY = Math.Max(botY + 1.25, Math.Ceiling(Math.Log10(maxY)));
                    Yspan = topY - botY;
                }
                Yratio = (double)plotHeight / Yspan;
                Xratio = Xzoom / 10000.0;
                showLogGrad = (Yratio > 50.0);

                axisStrFormat.LineAlignment = StringAlignment.Center;
                axisStrFormat.Alignment = StringAlignment.Far;
                grid.DashStyle = DashStyle.Dash;
                grid.DashCap = DashCap.Round;
                e.Graphics.TextRenderingHint = TextRenderingHint.AntiAlias;
                e.Graphics.SmoothingMode = SmoothingMode.AntiAlias;
                e.Graphics.FillRectangle(myBrush, -1, -1, splitContainer3.Panel1.ClientSize.Width, splitContainer3.Panel1.ClientSize.Height);
                myBrush.Color = Color.WhiteSmoke;
                if (radio == RadioType.TXonly)
                {
                    axisStrFormat.Alignment = StringAlignment.Center;
                    e.Graphics.DrawString(TXmessage, axisFont, myBrush, splitContainer3.Panel1.ClientSize.Width / 2.0f, splitContainer3.Panel1.ClientSize.Height / 2.0f, axisStrFormat);
                    return;
                }
                for (x = splitContainer3.Panel1.ClientSize.Width - (float)(pixelPerDiv * ((latestSample.ToBinary() / (long)10000) % (long)Xdiv) / Xdiv); x > 36.0f; x -= (float)pixelPerDiv)
                    e.Graphics.DrawLine(grid, x, 20.0f, x, splitContainer3.Panel1.ClientSize.Height);

                for (double yValue = topY; yValue > botY; yValue -= 1.0)
                {
                    y = (float)((topY - yValue) * Yratio + 20.0);
                    e.Graphics.DrawLine(grid, 36.0f, y, splitContainer3.Panel1.ClientSize.Width, y);
                    if (y - lastY >= 14.0f)
                    {
                        e.Graphics.DrawString("10" + expUnicode[(int)yValue + 6], axisFont, myBrush, 36.0f, y, axisStrFormat);
                        if (showLogGrad && (lastY > 0.0f))
                            for (int i = 2; i < 10; ++i)
                            {
                                y = (float)((topY - (yValue + log10of[i])) * Yratio + 20.0);
                                e.Graphics.DrawLine(grid, 36.0f, y, splitContainer3.Panel1.ClientSize.Width, y);
                            }
                        lastY = y; // sketchy logic but works reliably in this specific situation
                    }
                }

                signalPen.StartCap = LineCap.RoundAnchor;
                signalPen.EndCap = LineCap.RoundAnchor;
                for (int i = nodes.Count - 1; i >= 0; --i)
                    if (nodes[i].displaySignal(3) && (nodes[i].sampleList.Count > 0))
                    {
                        lastX = (float)(splitContainer3.Panel1.ClientSize.Width - 1 - Xratio * (latestSample.ToBinary() - nodes[i].sampleList[nodes[i].sampleList.Count - 1].sampleTime.ToBinary()));
                        if (lastX >= 36.0f)
                        {
                            signalPen.Color = nodes[i].BackColor;
                            lastY = (nodes[i].sampleList[nodes[i].sampleList.Count - 1].light > 0 ? (float)((topY - Math.Log10(nodes[i].sampleList[nodes[i].sampleList.Count - 1].light)) * Yratio + 20.0) : splitContainer3.Panel1.ClientSize.Height);
                            for (int j = nodes[i].sampleList.Count - 1; j >= 0; --j)
                            {
                                x = (float)(splitContainer3.Panel1.ClientSize.Width - Xratio * (latestSample.ToBinary() - nodes[i].sampleList[j].sampleTime.ToBinary()));
                                y = (nodes[i].sampleList[j].light > 0 ? (float)((topY - Math.Log10(nodes[i].sampleList[j].light)) * Yratio + 20.0) : splitContainer3.Panel1.ClientSize.Height);
                                if (x <= 36.0f)
                                {
                                    y = lastY + (y - lastY) * (lastX - 36.0f) / (lastX - x);
                                    signalPen.EndCap = LineCap.NoAnchor;
                                    e.Graphics.DrawLine(signalPen, lastX, lastY, 36.0f, y);
                                    signalPen.EndCap = LineCap.RoundAnchor;
                                    break;
                                }
                                e.Graphics.DrawLine(signalPen, lastX, lastY, x, y);
                                lastX = x;
                                lastY = y;
                            }
                        }
                    }

                axisLines.StartCap = LineCap.ArrowAnchor;
                myBrush.Color = Color.WhiteSmoke;
                e.Graphics.DrawLine(axisLines, 36.0f, 14.0f, 36.0f, splitContainer3.Panel1.ClientSize.Height);
                if (Xdiv > 100000.0)
                    str += (Xdiv / 60000.0) + " min./div)";
                else
                    str += (Xdiv / 1000.0) + " sec./div)";
                axisStrFormat.Alignment = StringAlignment.Center;
                e.Graphics.DrawString(str, axisFont, myBrush, splitContainer3.Panel1.ClientSize.Width / 2.0f, 10.0f, axisStrFormat);
            }
        }

        private void OnPaintAccelGraph(object sender, PaintEventArgs e)
        {
            if (!((USBthread is Thread) && USBthread.IsAlive))
                return;

            SplitterPanel currentPanel = (SplitterPanel)sender;
            int sensorIndex = (currentPanel.Equals(splitContainer4.Panel1) ? 4 : (currentPanel.Equals(splitContainer5.Panel1) ? 5 : 6));
            using (SolidBrush myBrush = new SolidBrush(Color.Black))
            using (StringFormat axisStrFormat = new StringFormat())
            using (Font axisFont = new Font("Arial", 156.0f / CurrentAutoScaleDimensions.Height))
            using (Pen axisLines = new Pen(Color.WhiteSmoke, 2.4f))
            using (Pen grid = new Pen(Color.DimGray, 0.95f))
            using (Pen signalPen = new Pen(Color.Black, 3.0f))
            {
                string str = "node acceleration along " + Convert.ToChar(84 + sensorIndex) + " axis (in G) over time (";
                int plotWidth = currentPanel.ClientSize.Width - 42;
                int plotHeight = currentPanel.ClientSize.Height - 20;
                DateTime leftBoundary, latestSample = DateTime.MaxValue;
                double Xzoom = zoom / (double)nodeSettings.Sen_PktRate;
                double Xdiv = 1000.0 * selectXdiv(Xzoom); // to be optimized
                double Xratio, pixelPerDiv = Xdiv * Xzoom; // to be optimized
                double topY = 10.0, botY = -10.0, Yspan = 20.0;
                float lastY = -99.9f, lastX, x, y, minY = -10.0f, maxY = 10.0f;
                double Ydiv, Yratio, Yrounding;

                float sampValue;

                if (nodes.Count > 0)
                {
                    latestSample = nodes.Max(n => (n.displaySignal(sensorIndex) && n.sampleList.Count > 0) ? n.sampleList[n.sampleList.Count - 1].sampleTime : DateTime.MinValue);
                    if (latestSample == DateTime.MinValue)
                        latestSample = DateTime.MaxValue;
                    leftBoundary = latestSample - TimeSpan.FromMilliseconds(plotWidth / Xzoom);
                    minY = nodes.Min(n => (n.displaySignal(sensorIndex) && n.sampleList.Count > 0) ? n.sampleList.Min(s => (s.sampleTime > leftBoundary ? (sensorIndex == 4 ? s.accelX : (sensorIndex == 5 ? s.accelY : s.accelZ)) : 10.0f)) : 10.0f); // perfs could be optimized
                    maxY = nodes.Max(n => (n.displaySignal(sensorIndex) && n.sampleList.Count > 0) ? n.sampleList.Max(s => (s.sampleTime > leftBoundary ? (sensorIndex == 4 ? s.accelX : (sensorIndex == 5 ? s.accelY : s.accelZ)) : -10.0f)) : -10.0f); // perfs could be optimized
                    if (minY > maxY)
                    {
                        minY = -10.0f;
                        maxY = 10.0f;
                    }
                    Yspan = maxY - minY;
                    if (Yspan < 0.1)
                        Yrounding = 0.02;
                    else if (Yspan < 0.15)
                        Yrounding = 0.025;
                    else if (Yspan < 0.3)
                        Yrounding = 0.05;
                    else if (Yspan < 0.5)
                        Yrounding = 0.1;
                    else if (Yspan < 1.0)
                        Yrounding = 0.2;
                    else if (Yspan < 1.5)
                        Yrounding = 0.25;
                    else if (Yspan < 3.0)
                        Yrounding = 0.5;
                    else if (Yspan < 5.0)
                        Yrounding = 1.0;
                    else
                        Yrounding = 2.0;
                    topY = Yrounding * Math.Ceiling(maxY / Yrounding);
                    botY = Yrounding * Math.Floor(minY / Yrounding);
                    if (topY == botY)
                    {
                        if (topY < -7.98)
                            topY = 7.98;
                        else
                            botY -= 0.02;
                    }
                    Yspan = topY - botY;
                }
                Ydiv = selectYdiv(Yspan, plotHeight);
                Yratio = (double)plotHeight / Yspan;
                Xratio = Xzoom / 10000.0;

                axisStrFormat.LineAlignment = StringAlignment.Center;
                axisStrFormat.Alignment = StringAlignment.Far;
                grid.DashStyle = DashStyle.Dash;
                grid.DashCap = DashCap.Round;
                e.Graphics.TextRenderingHint = TextRenderingHint.AntiAlias;
                e.Graphics.SmoothingMode = SmoothingMode.AntiAlias;
                e.Graphics.FillRectangle(myBrush, -1, -1, currentPanel.ClientSize.Width, currentPanel.ClientSize.Height);
                myBrush.Color = Color.WhiteSmoke;
                if (radio == RadioType.TXonly)
                {
                    axisStrFormat.Alignment = StringAlignment.Center;
                    e.Graphics.DrawString(TXmessage, axisFont, myBrush, currentPanel.ClientSize.Width / 2.0f, currentPanel.ClientSize.Height / 2.0f, axisStrFormat);
                    return;
                }
                for (x = currentPanel.ClientSize.Width - (float)(pixelPerDiv * ((latestSample.ToBinary() / (long)10000) % (long)Xdiv) / Xdiv); x > 42.0f; x -= (float)pixelPerDiv)
                    e.Graphics.DrawLine(grid, x, 20.0f, x, currentPanel.ClientSize.Height);

                for (double yValue = Ydiv * Math.Floor(topY / Ydiv); yValue > botY; yValue -= Ydiv)
                {
                    y = (float)((topY - yValue) * Yratio + 20.0);
                    e.Graphics.DrawLine(grid, 42.0f, y, currentPanel.ClientSize.Width, y);
                    if (y - lastY >= 14.0)
                    {
                        e.Graphics.DrawString(yValue.ToString("0.##"), axisFont, myBrush, 42.0f, y, axisStrFormat);
                        lastY = y;
                    }
                }

                signalPen.StartCap = LineCap.RoundAnchor;
                signalPen.EndCap = LineCap.RoundAnchor;
                for (int i = nodes.Count - 1; i >= 0; --i)
                    if (nodes[i].displaySignal(sensorIndex) && (nodes[i].sampleList.Count > 0))
                    {
                        lastX = (float)(currentPanel.ClientSize.Width - 1 - Xratio * (latestSample.ToBinary() - nodes[i].sampleList[nodes[i].sampleList.Count - 1].sampleTime.ToBinary()));
                        if (lastX >= 42.0f)
                        {
                            signalPen.Color = nodes[i].BackColor;
                            sampValue = (sensorIndex == 4 ? nodes[i].sampleList[nodes[i].sampleList.Count - 1].accelX : (sensorIndex == 5 ? nodes[i].sampleList[nodes[i].sampleList.Count - 1].accelY : nodes[i].sampleList[nodes[i].sampleList.Count - 1].accelZ)); // not optimal
                            lastY = (float)((topY - sampValue) * Yratio + 20.0);
                            for (int j = nodes[i].sampleList.Count - 1; j >= 0; --j)
                            {
                                x = (float)(currentPanel.ClientSize.Width - Xratio * (latestSample.ToBinary() - nodes[i].sampleList[j].sampleTime.ToBinary()));
                                sampValue = (sensorIndex == 4 ? nodes[i].sampleList[j].accelX : (sensorIndex == 5 ? nodes[i].sampleList[j].accelY : nodes[i].sampleList[j].accelZ)); // not optimal
                                y = (float)((topY - sampValue) * Yratio + 20.0);
                                if (x <= 42.0f)
                                {
                                    y = lastY + (y - lastY) * (lastX - 42.0f) / (lastX - x);
                                    signalPen.EndCap = LineCap.NoAnchor;
                                    e.Graphics.DrawLine(signalPen, lastX, lastY, 42.0f, y);
                                    signalPen.EndCap = LineCap.RoundAnchor;
                                    break;
                                }
                                e.Graphics.DrawLine(signalPen, lastX, lastY, x, y);
                                lastX = x;
                                lastY = y;
                            }
                        }
                    }

                axisLines.StartCap = LineCap.ArrowAnchor;
                myBrush.Color = Color.WhiteSmoke;
                e.Graphics.DrawLine(axisLines, 42.0f, 14.0f, 42.0f, currentPanel.ClientSize.Height);
                if (Xdiv > 100000.0)
                    str += (Xdiv / 60000.0) + " min./div)";
                else
                    str += (Xdiv / 1000.0) + " sec./div)";
                axisStrFormat.Alignment = StringAlignment.Center;
                e.Graphics.DrawString(str, axisFont, myBrush, currentPanel.ClientSize.Width / 2.0f, 10.0f, axisStrFormat);
            }
        }

        private void OnPaintAccelMagGraph(object sender, PaintEventArgs e)
        {
            if (!((USBthread is Thread) && USBthread.IsAlive))
                return;

            using (SolidBrush myBrush = new SolidBrush(Color.Black))
            using (StringFormat axisStrFormat = new StringFormat())
            using (Font axisFont = new Font("Arial", 156.0f / CurrentAutoScaleDimensions.Height))
            using (Pen axisLines = new Pen(Color.WhiteSmoke, 2.4f))
            using (Pen grid = new Pen(Color.DimGray, 0.95f))
            using (Pen signalPen = new Pen(Color.Black, 3.0f))
            {
                string str = "node absolute acceleration (in G) over time (";
                int plotWidth = splitContainer7.Panel1.ClientSize.Width - 42;
                int plotHeight = splitContainer7.Panel1.ClientSize.Height - 20;
                DateTime leftBoundary, latestSample = DateTime.MaxValue;
                double Xzoom = zoom / (double)nodeSettings.Sen_PktRate;
                double Xdiv = 1000.0 * selectXdiv(Xzoom); // to be optimized
                double Xratio, pixelPerDiv = Xdiv * Xzoom; // to be optimized
                double topY = 14.0, botY = 0.0, Yspan = 14.0;
                float lastY = -99.9f, lastX, x, y, minY = 0.0f, maxY = 14.0f;
                double Ydiv, Yratio, Yrounding;

                if (nodes.Count > 0)
                {
                    latestSample = nodes.Max(n => (n.displaySignal(7) && n.sampleList.Count > 0) ? n.sampleList[n.sampleList.Count - 1].sampleTime : DateTime.MinValue);
                    if (latestSample == DateTime.MinValue)
                        latestSample = DateTime.MaxValue;
                    leftBoundary = latestSample - TimeSpan.FromMilliseconds(plotWidth / Xzoom);
                    minY = nodes.Min(n => (n.displaySignal(7) && n.sampleList.Count > 0) ? n.sampleList.Min(s => (s.sampleTime > leftBoundary ? s.accelMag : 14.0f)) : 14.0f); // perfs could be optimized
                    maxY = nodes.Max(n => (n.displaySignal(7) && n.sampleList.Count > 0) ? n.sampleList.Max(s => (s.sampleTime > leftBoundary ? s.accelMag : 0.0f)) : 0.0f); // perfs could be optimized
                    if (minY > maxY)
                    {
                        minY = 0.0f;
                        maxY = 14.0f;
                    }
                    Yspan = maxY - minY;
                    if (Yspan < 0.1)
                        Yrounding = 0.02;
                    else if (Yspan < 0.15)
                        Yrounding = 0.025;
                    else if (Yspan < 0.3)
                        Yrounding = 0.05;
                    else if (Yspan < 0.5)
                        Yrounding = 0.1;
                    else if (Yspan < 1.0)
                        Yrounding = 0.2;
                    else if (Yspan < 1.5)
                        Yrounding = 0.25;
                    else if (Yspan < 3.0)
                        Yrounding = 0.5;
                    else if (Yspan < 5.0)
                        Yrounding = 1.0;
                    else
                        Yrounding = 2.0;
                    topY = Yrounding * Math.Ceiling(maxY / Yrounding);
                    botY = Yrounding * Math.Floor(minY / Yrounding);
                    if (topY == botY)
                    {
                        if (topY < 0.02)
                            topY = 0.02;
                        else
                            botY -= 0.02;
                    }
                    Yspan = topY - botY;
                }
                Ydiv = selectYdiv(Yspan, plotHeight);
                Yratio = (double)plotHeight / Yspan;
                Xratio = Xzoom / 10000.0;

                axisStrFormat.LineAlignment = StringAlignment.Center;
                axisStrFormat.Alignment = StringAlignment.Far;
                grid.DashStyle = DashStyle.Dash;
                grid.DashCap = DashCap.Round;
                e.Graphics.TextRenderingHint = TextRenderingHint.AntiAlias;
                e.Graphics.SmoothingMode = SmoothingMode.AntiAlias;
                e.Graphics.FillRectangle(myBrush, -1, -1, splitContainer7.Panel1.ClientSize.Width, splitContainer7.Panel1.ClientSize.Height);
                myBrush.Color = Color.WhiteSmoke;
                if (radio == RadioType.TXonly)
                {
                    axisStrFormat.Alignment = StringAlignment.Center;
                    e.Graphics.DrawString(TXmessage, axisFont, myBrush, splitContainer7.Panel1.ClientSize.Width / 2.0f, splitContainer7.Panel1.ClientSize.Height / 2.0f, axisStrFormat);
                    return;
                }
                for (x = splitContainer7.Panel1.ClientSize.Width - (float)(pixelPerDiv * ((latestSample.ToBinary() / (long)10000) % (long)Xdiv) / Xdiv); x > 42.0f; x -= (float)pixelPerDiv)
                    e.Graphics.DrawLine(grid, x, 20.0f, x, splitContainer7.Panel1.ClientSize.Height);

                for (double yValue = Ydiv * Math.Floor(topY / Ydiv); yValue > botY; yValue -= Ydiv)
                {
                    y = (float)((topY - yValue) * Yratio + 20.0);
                    e.Graphics.DrawLine(grid, 42.0f, y, splitContainer7.Panel1.ClientSize.Width, y);
                    if (y - lastY >= 14.0)
                    {
                        e.Graphics.DrawString(yValue.ToString("0.##"), axisFont, myBrush, 42.0f, y, axisStrFormat);
                        lastY = y;
                    }
                }

                signalPen.StartCap = LineCap.RoundAnchor;
                signalPen.EndCap = LineCap.RoundAnchor;
                for (int i = nodes.Count - 1; i >= 0; --i)
                    if (nodes[i].displaySignal(7) && (nodes[i].sampleList.Count > 0))
                    {
                        lastX = (float)(splitContainer7.Panel1.ClientSize.Width - 1 - Xratio * (latestSample.ToBinary() - nodes[i].sampleList[nodes[i].sampleList.Count - 1].sampleTime.ToBinary()));
                        if (lastX >= 42.0f)
                        {
                            signalPen.Color = nodes[i].BackColor;
                            lastY = (float)((topY - nodes[i].sampleList[nodes[i].sampleList.Count - 1].accelMag) * Yratio + 20.0);
                            for (int j = nodes[i].sampleList.Count - 1; j >= 0; --j)
                            {
                                x = (float)(splitContainer7.Panel1.ClientSize.Width - Xratio * (latestSample.ToBinary() - nodes[i].sampleList[j].sampleTime.ToBinary()));
                                y = (float)((topY - nodes[i].sampleList[j].accelMag) * Yratio + 20.0);
                                if (x <= 42.0f)
                                {
                                    y = lastY + (y - lastY) * (lastX - 42.0f) / (lastX - x);
                                    signalPen.EndCap = LineCap.NoAnchor;
                                    e.Graphics.DrawLine(signalPen, lastX, lastY, 42.0f, y);
                                    signalPen.EndCap = LineCap.RoundAnchor;
                                    break;
                                }
                                e.Graphics.DrawLine(signalPen, lastX, lastY, x, y);
                                lastX = x;
                                lastY = y;
                            }
                        }
                    }

                axisLines.StartCap = LineCap.ArrowAnchor;
                myBrush.Color = Color.WhiteSmoke;
                e.Graphics.DrawLine(axisLines, 42.0f, 14.0f, 42.0f, splitContainer7.Panel1.ClientSize.Height);
                if (Xdiv > 100000.0)
                    str += (Xdiv / 60000.0) + " min./div)";
                else
                    str += (Xdiv / 1000.0) + " sec./div)";
                axisStrFormat.Alignment = StringAlignment.Center;
                e.Graphics.DrawString(str, axisFont, myBrush, splitContainer7.Panel1.ClientSize.Width / 2.0f, 10.0f, axisStrFormat);
            }
        }

        private void OnPaintBERgraph(object sender, PaintEventArgs e)
        {
            if (!((USBthread is Thread) && USBthread.IsAlive))
                return;

            using (SolidBrush myBrush = new SolidBrush(Color.Black))
            using (StringFormat axisStrFormat = new StringFormat())
            using (Font axisFont = new Font("Arial", 195.0f / CurrentAutoScaleDimensions.Height))
            using (Pen axisLines = new Pen(Color.WhiteSmoke, 2.4f))
            using (Pen grid = new Pen(Color.CornflowerBlue, 0.95f))
            {
                string str;
                double maxY = -0.0, minY = -1.25;
                if (maxBER >= minBER)
                {
                    minY = Math.Max(-6.0, Math.Floor(Math.Log10(minBER))) - 0.25;
                    maxY = Math.Max(minY + 1.25, Math.Ceiling(Math.Log10(maxBER)));
                }
                double Yratio = (BERsplitContainer.Panel1.ClientSize.Height - 32) / (maxY - minY);
                double Xzoom = zoom / (double)nodeSettings.BER_GUIperiod;
                double Xdiv = selectXdiv(Xzoom);

                float y;
                double x, pixelPerDiv = 1000.0 * Xdiv * Xzoom;
                grid.DashStyle = DashStyle.Dash;
                grid.DashCap = DashCap.Round;
                e.Graphics.TextRenderingHint = TextRenderingHint.AntiAlias;
                e.Graphics.SmoothingMode = SmoothingMode.AntiAlias;
                e.Graphics.FillRectangle(myBrush, -1, -1, BERsplitContainer.Panel1.ClientSize.Width, BERsplitContainer.Panel1.ClientSize.Height);
                myBrush.Color = Color.WhiteSmoke;
                if (radio == RadioType.TXonly)
                {
                    axisStrFormat.LineAlignment = StringAlignment.Center;
                    axisStrFormat.Alignment = StringAlignment.Center;
                    e.Graphics.DrawString(TXmessage, axisFont, myBrush, BERsplitContainer.Panel1.ClientSize.Width / 2.0f, BERsplitContainer.Panel1.ClientSize.Height / 2.0f, axisStrFormat);
                    return;
                }
                for (x = BERsplitContainer.Panel1.ClientSize.Width - pixelPerDiv * (((double)lastSampleTime.Ticks / 10000000.0) % Xdiv) / Xdiv; x > 36.0; x -= pixelPerDiv)
                    e.Graphics.DrawLine(grid, (float)x, 32.0f, (float)x, (float)BERsplitContainer.Panel1.ClientSize.Height);
                grid.Color = Color.LightSteelBlue;
                axisStrFormat.LineAlignment = StringAlignment.Far;
                if (BERsplitContainer.Panel1.ClientSize.Height + Math.Round(minY) * Yratio > 56.0)
                    e.Graphics.DrawString("0", axisFont, myBrush, 20, BERsplitContainer.Panel1.ClientSize.Height, axisStrFormat);
                axisStrFormat.LineAlignment = StringAlignment.Center;
                for (double i = Math.Round(minY); i < maxY; ++i)
                {
                    y = (float)((maxY - i) * Yratio + 32.0);
                    e.Graphics.DrawLine(grid, 36.0f, y, (float)BERsplitContainer.Panel1.ClientSize.Width, y);
                    e.Graphics.DrawString("10" + expUnicode[(int)i + 6], axisFont, myBrush, -5.0f, y, axisStrFormat);
                    for (int j = 2; j < 10; ++j)
                    {
                        y = (float)((maxY - (i + log10of[j])) * Yratio + 32.0);
                        e.Graphics.DrawLine(grid, 36.0f, y, (float)BERsplitContainer.Panel1.ClientSize.Width, y);
                    }
                }
                e.Graphics.DrawLine(grid, 36.0f, 32.0f, (float)BERsplitContainer.Panel1.ClientSize.Width, 32.0f);
                e.Graphics.DrawString("10" + expUnicode[(int)maxY + 6], axisFont, myBrush, -5.0f, 32.0f, axisStrFormat);
                axisStrFormat.Alignment = StringAlignment.Center;

                if (sampleList.Count > 0)
                {
                    double lastX = BERsplitContainer.Panel1.ClientSize.Width - Xzoom * (lastSampleTime - sampleList[sampleList.Count - 1].sampleTime).TotalMilliseconds;
                    if (lastX > 36.0)
                    {
                        myBrush.Color = Color.PaleVioletRed;
                        for (int i = sampleList.Count - 1; i >= 0; --i)
                        {
                            x = BERsplitContainer.Panel1.ClientSize.Width - Xzoom * (lastSampleTime - sampleList[i].sampleTime).TotalMilliseconds;
                            if (x < 36.0)
                                break;
                            e.Graphics.DrawString("\u00D7", axisFont, myBrush, (float)(x + 0.75), (float)((maxY - Math.Max(Math.Log10(sampleList[i].BER), minY)) * Yratio + 33.9), axisStrFormat);
                        }

                        using (LinearGradientBrush gradBrush = new LinearGradientBrush(new Point(0, 0), new Point(0, BERsplitContainer.Panel1.ClientSize.Height), Color.Black, Color.Black))
                        {
                            float lastY;
                            Color pureGreen = Color.FromArgb(0x00, 0xFF, 0x00);
                            List<Color> usedColor = new List<Color>(6);
                            List<float> gradPos = new List<float>(6);

                            gradPos.Add(0.0f);
                            usedColor.Add(Color.Red);

                            if (maxY > -0.5)
                                gradPos.Add((float)((Yratio + 32.0) / BERsplitContainer.Panel1.ClientSize.Height));
                            else
                                gradPos.Add(32.0f / BERsplitContainer.Panel1.ClientSize.Height);
                            if (maxY > -1.5)
                                usedColor.Add(Color.Red);
                            else if (maxY > -2.5)
                                usedColor.Add(Color.FromArgb(0xFF, 0x80, 0x00));
                            else if (maxY > -3.5)
                                usedColor.Add(Color.Yellow);
                            else if (maxY > -4.5)
                                usedColor.Add(Color.FromArgb(0x80, 0xFF, 0x00));
                            else
                                usedColor.Add(pureGreen);

                            if ((maxY > -2.5) && (minY < -3.5))
                            {
                                gradPos.Add((float)(((maxY + 3.0) * Yratio + 32.0) / BERsplitContainer.Panel1.ClientSize.Height));
                                usedColor.Add(Color.Yellow);
                            }

                            if ((maxY > -4.5) && (minY < -5.5))
                            {
                                gradPos.Add((float)(((maxY + 5.0) * Yratio + 32.0) / BERsplitContainer.Panel1.ClientSize.Height));
                                usedColor.Add(pureGreen);
                            }

                            gradPos.Add((float)(((maxY - minY - 0.25) * Yratio + 32.0) / BERsplitContainer.Panel1.ClientSize.Height));
                            if (minY < -4.5)
                                usedColor.Add(pureGreen);
                            else if (minY < -3.5)
                                usedColor.Add(Color.FromArgb(0x80, 0xFF, 0x00));
                            else if (minY < -2.5)
                                usedColor.Add(Color.Yellow);
                            else if (minY < -1.5)
                                usedColor.Add(Color.FromArgb(0xFF, 0x80, 0x00));
                            else
                                usedColor.Add(Color.Red);

                            gradPos.Add(1.0f);
                            usedColor.Add(Color.DeepSkyBlue);

                            ColorBlend gradient = new ColorBlend(gradPos.Count);
                            gradient.Colors = usedColor.ToArray();
                            gradient.Positions = gradPos.ToArray();
                            gradBrush.InterpolationColors = gradient;
                            lastY = (float)((maxY - Math.Max(Math.Log10(sampleList[sampleList.Count - 1].avg), minY)) * Yratio + 32.0);
                            using (Pen gradPen = new Pen(gradBrush, 3.0f))
                                for (int i = sampleList.Count - 2; i >= 0; --i)
                                {
                                    x = BERsplitContainer.Panel1.ClientSize.Width - Xzoom * (lastSampleTime - sampleList[i].sampleTime).TotalMilliseconds;
                                    y = (float)((maxY - Math.Max(Math.Log10(sampleList[i].avg), minY)) * Yratio + 32.0);
                                    if (x < 36.0)
                                    {
                                        y = lastY + (y - lastY) * (float)((lastX - 36.0) / (lastX - x));
                                        e.Graphics.DrawLine(gradPen, (float)lastX, lastY, 36.0f, y);
                                        break;
                                    }
                                    e.Graphics.DrawLine(gradPen, (float)lastX, lastY, (float)x, y);
                                    lastX = x;
                                    lastY = y;
                                }
                        }
                    }
                }

                axisLines.StartCap = LineCap.ArrowAnchor;
                myBrush.Color = Color.WhiteSmoke;
                e.Graphics.DrawLine(axisLines, 36, 28, 36, BERsplitContainer.Panel1.ClientSize.Height);
                if (Xdiv > 60.0)
                    str = "Bit Error Rate over time (" + (Xdiv / 60.0) + " min./div)";
                else
                    str = "Bit Error Rate over time (" + Xdiv + " sec./div)";
                e.Graphics.DrawString(str, axisFont, myBrush, BERsplitContainer.Panel1.ClientSize.Width / 2.0f, 16.0f, axisStrFormat);
            }
        }

        private void OnPaintPktDist(object sender, PaintEventArgs e)
        {
            if (!((USBthread is Thread) && USBthread.IsAlive))
                return;

            using (SolidBrush myBrush = new SolidBrush(Color.Black))
            using (StringFormat strFormat = new StringFormat())
            using (Font axisFont = new Font("Arial", 143.0f / CurrentAutoScaleDimensions.Height))
            using (Pen axisLines = new Pen(Color.WhiteSmoke, 2.4f))
            {
                strFormat.LineAlignment = StringAlignment.Center;
                strFormat.Alignment = StringAlignment.Center;
                e.Graphics.TextRenderingHint = TextRenderingHint.AntiAlias;
                e.Graphics.SmoothingMode = SmoothingMode.AntiAlias;
                e.Graphics.FillRectangle(myBrush, -1, -1, BERsplitContainer.Panel2.ClientSize.Width, BERsplitContainer.Panel2.ClientSize.Height);
                myBrush.Color = Color.WhiteSmoke;
                if (radio == RadioType.TXonly)
                    return;
                if (BERsplitContainer.Panel2.ClientSize.Width < 100)
                {
                    strFormat.FormatFlags |= StringFormatFlags.DirectionVertical;
                    e.Graphics.DrawString("[Expand to display]", axisFont, myBrush, BERsplitContainer.Panel2.ClientSize.Width / 2.0f, BERsplitContainer.Panel2.ClientSize.Height / 2.0f, strFormat);
                    return;
                }
                int i = 0;
                float space, headerHeight, h, ratio = 0.0f, graphWidth = BERsplitContainer.Panel2.ClientSize.Width - 28.0f;
                if (BERsplitContainer.Panel2.ClientSize.Width > 125)
                {
                    e.Graphics.DrawString("Packet distribution", axisFont, myBrush, BERsplitContainer.Panel2.ClientSize.Width / 2.0f, 9.0f, strFormat);
                    headerHeight = 60.0f;
                }
                else
                {
                    e.Graphics.DrawString("Packet\r\ndistribution", axisFont, myBrush, BERsplitContainer.Panel2.ClientSize.Width / 2.0f, 18.0f, strFormat);
                    headerHeight = 78.0f;
                }
                e.Graphics.DrawString("% of packets", axisFont, myBrush, BERsplitContainer.Panel2.ClientSize.Width / 2.0f + 14.0f, BERsplitContainer.Panel2.ClientSize.Height - 8.0f, strFormat);
                using (SolidBrush barBrush = new SolidBrush(Color.DeepSkyBlue))
                {
                    for (h = BERsplitContainer.Panel2.ClientSize.Height - 23; i < 30; ++i)
                    {
                        if (h < headerHeight)
                            break;
                        e.Graphics.FillRectangle(barBrush, 27.5f, h - 10.0f, 0.5f + currentPktDist[i] * graphWidth, 16.0f);
                        if (i < 20)
                            barBrush.Color = Color.FromArgb(255 * (i + 1) / 20, 255, 0);
                        else
                            barBrush.Color = Color.FromArgb(255, 255 * (39 - i) / 20, 0);
                        if (currentPktDist[i] > 0.5f)
                        {
                            strFormat.Alignment = StringAlignment.Far;
                            myBrush.Color = Color.Black;
                            space = currentPktDist[i] * graphWidth;
                        }
                        else
                        {
                            strFormat.Alignment = StringAlignment.Near;
                            space = (1 - currentPktDist[i]) * graphWidth;
                        }
                        e.Graphics.DrawString((100.0f * currentPktDist[i]).ToString((space > 50.0f) ? "0.0##" : ((space > 42.0f) ? "0.0#" : "0.0")), axisFont, myBrush, 28.0f + currentPktDist[i] * graphWidth, h, strFormat);
                        myBrush.Color = Color.WhiteSmoke;
                        if (i % 5 == 0)
                        {
                            strFormat.Alignment = StringAlignment.Far;
                            e.Graphics.DrawString(i.ToString("F0"), axisFont, myBrush, 28.0f, h, strFormat);
                        }
                        h -= 18.0f;
                    }
                    for (int j = i; j < 31; ++j)
                        ratio += currentPktDist[j];
                    e.Graphics.FillRectangle(barBrush, 27.5f, h - 10.0f, 0.5f + ratio * graphWidth, 16.0f);
                    if (ratio > 0.5f)
                    {
                        strFormat.Alignment = StringAlignment.Far;
                        myBrush.Color = Color.Black;
                        space = ratio * graphWidth;
                    }
                    else
                    {
                        strFormat.Alignment = StringAlignment.Near;
                        space = (1 - ratio) * graphWidth;
                    }
                    e.Graphics.DrawString((100.0f * ratio).ToString((space > 50.0f) ? "0.0##" : ((space > 42.0f) ? "0.0#" : "0.0")), axisFont, myBrush, 28.0f + ratio * graphWidth, h, strFormat);
                    myBrush.Color = Color.WhiteSmoke;
                    strFormat.Alignment = StringAlignment.Far;
                    e.Graphics.DrawString(i.ToString("F0") + '+', axisFont, myBrush, 28.0f, h, strFormat);

                    h -= 18.0f;
                    barBrush.Color = Color.Red;
                    e.Graphics.FillRectangle(barBrush, 27.5f, h - 10.0f, 0.5f + currentPktDist[31] * graphWidth, 16.0f);
                }
                if (currentPktDist[31] > 0.5f)
                {
                    strFormat.Alignment = StringAlignment.Far;
                    myBrush.Color = Color.Black;
                    space = currentPktDist[31] * graphWidth;
                }
                else
                {
                    strFormat.Alignment = StringAlignment.Near;
                    space = (1 - currentPktDist[31]) * graphWidth;
                }
                e.Graphics.DrawString((100.0f * currentPktDist[31]).ToString((space > 50.0f) ? "0.0##" : ((space > 42.0f) ? "0.0#" : "0.0")), axisFont, myBrush, 28.0f + currentPktDist[31] * graphWidth, h, strFormat);
                myBrush.Color = Color.WhiteSmoke;
                strFormat.Alignment = StringAlignment.Far;
                e.Graphics.DrawString("Mis", axisFont, myBrush, 28.0f, h, strFormat);
                strFormat.FormatFlags |= StringFormatFlags.DirectionVertical;
                e.Graphics.DrawString("errors per packet", axisFont, myBrush, 9.0f, BERsplitContainer.Panel2.ClientSize.Height - 45, strFormat);
            }
        }
    }
}
