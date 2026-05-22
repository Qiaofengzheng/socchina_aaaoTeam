using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Management;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace Modbus_RTU控制器
{
    public partial class MainWindow : Form
    {
        public MainWindow()
        {
            InitializeComponent();
            Control.CheckForIllegalCrossThreadCalls = false;
        }

        private void MainWindow_Load(object sender, EventArgs e)
        {
            GetCOMList();
        }
        private void GetCOMList()
        {
            comboBox1.Items.Clear();//防止重复添加串口
            try
            {
                using (ManagementObjectSearcher searcher = new ManagementObjectSearcher("select * from Win32_PnPEntity"))
                {
                    ManagementObjectCollection hardInfos = searcher.Get();
                    foreach (var hardInfo in hardInfos)
                    {
                        if (hardInfo.Properties["Name"].Value != null && (hardInfo.Properties["Name"].Value.ToString().Contains("CH340")))//检查包含CH340的端口
                        {
                            comboBox1.Items.Add(hardInfo.Properties["Name"].Value.ToString());
                        }
                    }
                }
                if (comboBox1.Items != null) comboBox1.Text = comboBox1.Items[0].ToString();
            }
            catch
            {
                MessageBox.Show("无CH340设备！");
            }
        }

        private void buttonScan_Click(object sender, EventArgs e)
        {
            GetCOMList();
        }

        private void CloseSerialPort()
        {
            serialPort1.Close();//已经打开就关闭
            buttonOpen.Text = "打开串口";
            buttonOpen.BackColor = Color.Red;
            timer1.Stop();
            textBoxBaudrate.Enabled = true;
            button1.Enabled = false;
            button2.Enabled = false;
            button3.Enabled = false;
        }

        private void buttonOpen_Click(object sender, EventArgs e)
        {
            if(serialPort1.IsOpen)//判断串口是否已经打开
            {
                CloseSerialPort();
            }
            else
            {
                try
                {
                    string[] str = comboBox1.Text.Split(new char[] { '(', ')' });//把端口号按照（，）拆分成三段
                    serialPort1.PortName = str[1];//只需要端口号
                    serialPort1.BaudRate = Convert.ToInt32(textBoxBaudrate.Text);
                    serialPort1.Open();
                    buttonOpen.Text = "关闭串口";
                    buttonOpen.BackColor = Color.Green;
                    textBoxBaudrate.Enabled = false;
                    button1.Enabled = true;
                    button2.Enabled = true;
                    button3.Enabled = true;
                    timer1.Start();
                }
                catch(Exception err)
                {
                    MessageBox.Show(err.Message);//显示错误信息
                    CloseSerialPort();
                }
            }
        }

        private void button_Click(object sender, EventArgs e)
        {
            timer1.Stop();
            byte[] array = new byte[8] { 0x01, 0x06, 0x9c, 0x00, 0x00, 0x00, 0x00, 0x00 };
            Button btn = (Button)sender;
            try
            {
                if (serialPort1.IsOpen && button1.BackColor == Color.Green && btn.Text == "LED1")
                {
                    array[3] = 0x41; array[5] = 0x00; array[6] = 0x8e; array[7] = 0xf7;
                }
                else if (serialPort1.IsOpen && button1.BackColor == Color.Red && btn.Text == "LED1")
                {
                    array[3] = 0x41; array[5] = 0x01; array[6] = 0x4e; array[7] = 0x36;
                }
                if (serialPort1.IsOpen && button2.BackColor == Color.Green && btn.Text == "LED2")
                {
                    array[3] = 0x42; array[5] = 0x00; array[6] = 0x8e; array[7] = 0x07;
                }
                else if (serialPort1.IsOpen && button2.BackColor == Color.Red && btn.Text == "LED2")
                {
                    array[3] = 0x42; array[5] = 0x01; array[6] = 0x4e; array[7] = 0xc6;
                }
                if (serialPort1.IsOpen && button3.BackColor == Color.Green && btn.Text == "LED3")
                {
                    array[3] = 0x43; array[5] = 0x00; array[6] = 0x4e; array[7] = 0x56;
                }
                else if (serialPort1.IsOpen && button3.BackColor == Color.Red && btn.Text == "LED3")
                {
                    array[3] = 0x43; array[5] = 0x01; array[6] = 0x8e; array[7] = 0x97;
                }

                serialPort1.Write(array, 0, 8);
                textBoxRec.AppendText(DateTime.Now.ToString("[HH:mm:ss:ff]") + "发 ▶ "
                + array[0].ToString("x2") + " "
                + array[1].ToString("x2") + " "
                + array[2].ToString("x2") + " "
                + array[3].ToString("x2") + " "
                + array[4].ToString("x2") + " "
                + array[5].ToString("x2") + " "
                + array[6].ToString("x2") + " "
                + array[7].ToString("x2") + "\r\n");
            }
            catch (Exception err)
            {
                MessageBox.Show(err.Message);
                CloseSerialPort();
            }
            timer1.Start();
        }

        private void ReadREG()
        {
            byte[] array = new byte[8]{ 0x01, 0x03, 0x9c, 0x41, 0x00, 0x00, 0x8e, 0x3b}; //读寄存器报文
            try
            {
                if (serialPort1.IsOpen)
                {
                    serialPort1.Write(array, 0, 8);
                    textBoxRec.AppendText(DateTime.Now.ToString("[HH:mm:ss:ff]") + "发 ▶ "
                    + array[0].ToString("x2") + " "//地址
                    + array[1].ToString("x2") + " "//功能码
                    + array[2].ToString("x2") + " "//REG1_H
                    + array[3].ToString("x2") + " "//REG1_L
                    + array[4].ToString("x2") + " "//DATA_H
                    + array[5].ToString("x2") + " "//DATA_L
                    + array[6].ToString("x2") + " "//CRC16_H
                    + array[7].ToString("x2") + "\r\n");//CRC16_L
                }
            }
            catch
            {
                MessageBox.Show("串口不存在！");
                CloseSerialPort();
            }
        }

        byte[] rec_array = new byte[20];
        private void Modbus_RTU_Protocol_Analysis()
        {
            textBoxRec.AppendText(DateTime.Now.ToString("[HH:mm:ss:ff]") + "收 ◀ "
                + rec_array[0].ToString("x2") + " "//地址
                + rec_array[1].ToString("x2") + " "//功能码
                + rec_array[2].ToString("x2") + " "//数据长度
                + rec_array[3].ToString("x2") + " "//REG1_H
                + rec_array[4].ToString("x2") + " "//REG1_L
                + rec_array[5].ToString("x2") + " "//REG2_H
                + rec_array[6].ToString("x2") + " "//REG2_L
                + rec_array[7].ToString("x2") + " "//REG3_H
                + rec_array[8].ToString("x2") + " "//REG3_L
                + rec_array[9].ToString("x2") + " "//REG4_H
                + rec_array[10].ToString("x2") + " "//REG4_L
                + rec_array[11].ToString("x2") + " "//CRC16_H
                + rec_array[12].ToString("x2") + "\r\n");//CRC16_L

            int tmp = 0x0000;
            int CRC_value = 0xffff;
            for (int i = 0; i < 11; ++i)//输入反转
            {
                tmp = 0x00;
                for (int j = 0; j < 8; ++j)
                {
                    tmp <<= 1;     //向高位移动
                    tmp |= rec_array[i] & 0x01;//提取输入数据最低位
                    rec_array[i] >>= 1;            //将最低位移出
                }
                rec_array[i] = Convert.ToByte(tmp);
            }
            for (int i = 0; i < 11; i++)
            {
                tmp = rec_array[i] << 8;
                for (int j = 0; j < 8; j++)
                {
                    if (Convert.ToBoolean((CRC_value ^ tmp) & 0x8000)) CRC_value = (CRC_value << 1) ^ 0x8005;
                    else CRC_value <<= 1;
                    tmp <<= 1;
                }
            }
            for (int i = 0; i < 16; i++)//输出反转
            {
                tmp <<= 1;     //向高位移动
                tmp |= CRC_value & 0x0001;//提取输入数据最低位
                CRC_value >>= 1;            //将最低位移出
            }
            CRC_value = tmp;
            tmp = 0x00;
            for (int i = 0; i < 8; i++)//输出反转
            {
                tmp <<= 1;     //向高位移动
                tmp |= rec_array[9] & 0x0001;//提取输入数据最低位
                rec_array[9] >>= 1;            //将最低位移出
            }
            rec_array[9] = Convert.ToByte(tmp);
            tmp = 0x0000;
            for (int i = 0; i < 8; i++)//输出反转
            {
                tmp <<= 1;     //向高位移动
                tmp |= rec_array[10] & 0x0001;//提取输入数据最低位
                rec_array[10] >>= 1;            //将最低位移出
            }
            rec_array[10] = Convert.ToByte(tmp);
            if (CRC_value == (rec_array[11] << 8 | rec_array[12]) || CRC_value == (rec_array[12] << 8 | rec_array[11]))
            {
                textBoxTemp.Text = ((Convert.ToDouble(rec_array[9] << 8 | rec_array[10]) - 5500) / 100).ToString("f2") + "℃";
                if (Convert.ToBoolean(rec_array[4])) button1.BackColor = Color.Red;//LED1已关闭
                else button1.BackColor = Color.Green;//LED1已打开
                if (Convert.ToBoolean(rec_array[6])) button2.BackColor = Color.Red;//LED2已关闭
                else button2.BackColor = Color.Green;//LED1已打开
                if (Convert.ToBoolean(rec_array[8])) button3.BackColor = Color.Red;//LED3已关闭
                else button3.BackColor = Color.Green;//LED1已打开
            }
        }

        private void serialPort1_DataReceived(object sender, System.IO.Ports.SerialDataReceivedEventArgs e)
        {
            byte i, index = 0;
            try
            {
                serialPort1.Read(rec_array, 0, 20);
                for (i = 0; i < 20 - 8; i++)
                {
                    if (index == 0)
                        if (rec_array[i] != 0x01) continue;
                    rec_array[index] = rec_array[i];
                    if (index == 7) break;
                    index++;
                }
                if (rec_array[1] == 0x03) Modbus_RTU_Protocol_Analysis();
                else
                {
                    textBoxRec.AppendText(DateTime.Now.ToString("[HH:mm:ss:ff]") + "收 ◀ "
                        + rec_array[0].ToString("x2") + " "//地址
                        + rec_array[1].ToString("x2") + " "//功能码 
                        + rec_array[2].ToString("x2") + " "//REG1_H
                        + rec_array[3].ToString("x2") + " "//REG1_L
                        + rec_array[4].ToString("x2") + " "//DATA_H
                        + rec_array[5].ToString("x2") + " "//DATA_L
                        + rec_array[6].ToString("x2") + " "//CRC_H
                        + rec_array[7].ToString("x2") + "\r\n");//CRC_L
                }
            }
            catch(Exception err)
            {
                MessageBox.Show(err.Message);
                CloseSerialPort();
            }
        }

        private void timer1_Tick(object sender, EventArgs e)
        {
            ReadREG();
        }
    }
}
