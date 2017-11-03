using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
//添加这个类就可以使用SerialPort方法 获取可用串口
using System.IO.Ports;
using System.Threading;
using System.IO;

namespace InterFace
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
            //线程设置为其他线程可以调用该线程的资源
            Control.CheckForIllegalCrossThreadCalls = false;
        }

        private Thread serverThread;   // 启动服务线程  
        string Path="E:\\test.txt";
        byte[] byData = new byte[5];

        private void Form1_Load(object sender, EventArgs e)
        {
            button6.Enabled = false;

            #region  波特率初始化
            string[] BaudRate = { "9600", "115200", "315200" };
            comboBox2.DropDownStyle = ComboBoxStyle.DropDownList;
            for (int i = 0; i < BaudRate.Length; i++)
            {
                comboBox2.Items.Add(BaudRate[i]);
            }
            comboBox2.Text = "9600";
            serialPort1.BaudRate = 9600;
            #endregion

            #region  串口初始化
            //让下拉框只能选择已有选项，不能输入
            comboBox1.DropDownStyle = ComboBoxStyle.DropDownList;
            //自动搜索并且添加可用串口
            string[] PortName = SerialPort.GetPortNames();//读取
            for (int i = 0; i < PortName.Length; i++)//添加
            {
                comboBox1.Items.Add(PortName[i]);
            }

            try
            {
                comboBox1.SelectedIndex = 0;
            }
            catch
            {
                MessageBox.Show("没有可用端口");
            }
            //串口接受消息响应设置
            serialPort1.DataReceived += new SerialDataReceivedEventHandler(serialPort1_DataReceived);
            #endregion
        }

        void serialPort1_DataReceived(object sender, SerialDataReceivedEventArgs e)
        {
            byte ReceiveData;
            //读取数据
            ReceiveData = (byte)serialPort1.ReadByte();
            RecvHandle(ReceiveData);
        }

        private void Form1_FormClosing(object sender, FormClosingEventArgs e)
        {
            // 在未停止 服务 前，不允许关闭窗体  
            if (serialPort1.IsOpen == true)
            {
                MessageBox.Show("请先停止服务!", "Server提示");
                e.Cancel = true;
            }
        }

        private void RecvHandle(byte ReceiveData)
        {
            #region  缓存区数据处理
            float usart_temp = 0;
            if ((global.USART_RX_STA & 0x8000) == 0) //接收未完成
            {
                if ((global.USART_RX_STA & 0x4000) != 0) //接收到了0x0d
                {
                    if (ReceiveData != 0x0a)
                    {
                        global.USART_RX_STA = 0; //接收错误,重新开始
                    }
                    else
                    {
                        global.USART_RX_STA |= 0x8000;	//接收完成了
                    }
                }
                else //还没收到0X0D
                {
                    if (ReceiveData == 0x0d)
                    {
                        global.USART_RX_STA |= 0x4000;
                    }
                    else
                    {
                        global.USART_RX_BUF[global.USART_RX_STA & 0X3FFF] = (byte)ReceiveData;
                        global.USART_RX_STA++;
                        if (global.USART_RX_STA > (global.USART_REC_LEN - 1))
                        {
                            global.USART_RX_STA = 0; //接收数据错误,重新开始接收
                        }
                    }
                }
            }
            #endregion
            #region  接收数据显示
            if ((global.USART_RX_STA & 0x8000) != 0)
            {
                int len;
                len = global.USART_RX_STA & 0x3fff; //得到此次接收到的数据长度
                //检验第一位  确保数据正确
                if (global.USART_RX_BUF[0] == 0xff)
                {
                    if (len == 5)
                    {
                        usart_temp = (float)(global.USART_RX_BUF[3]) + ((float)global.USART_RX_BUF[4]) / 100;
                        if (global.USART_RX_BUF[2] == 0x01)
                        {
                            usart_temp = -usart_temp;
                        }
                    }
                    else if (len == 3)
                    {
                        usart_temp = (float)global.USART_RX_BUF[2];
                    }
                    switch (global.USART_RX_BUF[1])
                    {

                    }
                }

            #endregion
                global.USART_RX_STA = 0;
            }
        }

        int SerialOpenFlag = 0;
        private void button1_Click(object sender, EventArgs e)
        {
            //打开串口
            if (SerialOpenFlag==0)
            {
                try
                {
                    serialPort1.Open();
                    button1.Text = "关闭";
                    SerialOpenFlag = 1;
                }
                catch
                {
                    MessageBox.Show("打开失败");
                }
            }
            //关闭串口
            else
            {
                try
                {
                    serialPort1.Close();
                    button1.Text = "打开";
                    SerialOpenFlag = 0;
                }
                catch
                {
                    MessageBox.Show("关闭失败");
                }
            }
        }

        private void button2_Click(object sender, EventArgs e)
        {
            #region  波特率初始化
            string[] BaudRate = { "9600", "115200", "315200" };
            comboBox2.DropDownStyle = ComboBoxStyle.DropDownList;
            for (int i = 0; i < BaudRate.Length; i++)
            {
                comboBox2.Items.Add(BaudRate[i]);
            }
            comboBox2.Text = "115200";
            serialPort1.BaudRate = 115200;
            #endregion

            //清空之前的选项
            comboBox1.Items.Clear();

            //自动搜索并且添加可用串口
            string[] PortName = SerialPort.GetPortNames();//读取
            for (int i = 0; i < PortName.Length; i++)//添加
            {
                comboBox1.Items.Add(PortName[i]);
            }

            try
            {
                comboBox1.SelectedIndex = 0;
            }
            catch
            {
                MessageBox.Show("没有可用端口");
            }
        }

        private void comboBox1_SelectedIndexChanged(object sender, EventArgs e)
        {
            //设置端口名称
            serialPort1.PortName = comboBox1.Text;
        }

        private void comboBox2_SelectedIndexChanged(object sender, EventArgs e)
        {
            //设置波特率
            serialPort1.BaudRate = int.Parse(comboBox2.Text);
        }

        private void button5_Click(object sender, EventArgs e)
        {
            serverThread = new Thread(Listen);
            serverThread.Start();
            button6.Enabled = true;
            button5.Enabled = false;
        }

        private void Listen()
        {
            while (true)
            {
                if (IsFileInUse(Path) == false)
                {
                    Read();
                    if ((byData[0] > 0) && (byData[1] > 0) && (byData[0] < 255) && (byData[1] < 255))
                    {
                        label5.Text = byData[0].ToString();//X坐标
                        label3.Text = byData[1].ToString();//Y坐标
                        label11.Text = (byData[0] - 60).ToString();//X差值
                        label12.Text = (byData[1] - 80).ToString();//Y差值
                        SendData(byData[0], byData[1]);
                    }
                }
                Thread.Sleep(5);
            }
        }

        public static bool IsFileInUse(string fileName)
        {
            bool inUse = true;

            FileStream fs = null;
            try
            {

                fs = new FileStream(fileName, FileMode.Open, FileAccess.Read,

                FileShare.None);

                inUse = false;
            }
            catch
            {
            }
            finally
            {
                if (fs != null)

                    fs.Close();
            }
            return inUse;//true表示正在使用,false没有使用  
        }

        /**
         * 将文件中数字读入到byData数组
         * */
        public void Read()
        {
            try
            {
                StreamReader file = new StreamReader(Path, Encoding.Default);
                string line;

                line = file.ReadLine();
                int temp = int.Parse(line);
                byData[0] = (byte)temp;

                file.Read();
                file.Read();

                line = file.ReadLine();
                temp = int.Parse(line);
                byData[1] = (byte)temp;

                file.Close();
                ////读取数据之后删除这个文件
                //File.Delete(Path);
            }
            catch
            {
                //MessageBox.Show("文件打不开！");
            }
        }

        /*
         *将数组中数据通过串口发送出去
         *第一个和最后两个数据是校验位
         *第二个是横坐标，第三个是纵坐标
         */
        public void SendData(byte X_axis, byte Y_axis)
        {
            
            //发送数据
            byte[] Send_Data = new byte[5] { 0xff, X_axis, Y_axis, 0x0d, 0x0a };
            //byte[] Send_Data = new byte[5] { 0xff, 30, 214, 0x0d, 0x0a };
            //第零位开始发  发5个
            try
            {
                serialPort1.Write(Send_Data, 0, 5);
            }
            catch
            {
                //MessageBox.Show("数据发送失败");
            }
           // Thread.Sleep(10);
        }

        private void button6_Click(object sender, EventArgs e)
        {
            button5.Enabled = true;
            button6.Enabled = false;
            try
            {
                if (serverThread.IsAlive)
                {
                    serverThread.Abort();
                }
            }
            catch (Exception exc)
            {
                MessageBox.Show(exc.Message, "Server提示");
            }  
        }

    }
    #region 全局变量
    public class global
    {
        /**********接收数据缓存****************/
        public static int USART_REC_LEN = 200;
        public static byte[] USART_RX_BUF = new byte[USART_REC_LEN];//接收缓冲,最大200个字节.
        //接收状态
        //bit15，	接收完成标志
        //bit14，	接收到0x0d
        //bit13~0，	接收到的有效字节数目
        public static int USART_RX_STA = 0;         		//接收状态标记
        public static byte[] CCD1 = new byte[130];//CCD1数组.
        public static byte[] CCD2 = new byte[130];//CCD2数组.
        public static byte[] CMOS = new byte[76810];//CMOS数组.
    }  
    #endregion
}
