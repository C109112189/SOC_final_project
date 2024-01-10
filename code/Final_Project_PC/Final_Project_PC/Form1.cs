using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.IO.Ports;
using static System.Windows.Forms.VisualStyles.VisualStyleElement;
using static System.Windows.Forms.AxHost;

namespace Final_Project_PC
{
    public partial class Form1 : Form
    {
        const int ROW = 4, COL = 4;                     //ROW是向下生長(高)    //COL是向右生長(寬)
        const int GRID_WIDTH = 65, GRID_HEIGHT = 75;
        const int GAME_POS_X = 150, GAME_POS_Y = 50;

        public Task ReceiveSerial;

        StringBuilder port = new StringBuilder();
        SerialPort SerialPort = new SerialPort();
        //string text = "Is Working!";
        int score = 0;

        //Color WIN = Color.Yellow;
        //Color CONTINUE = Color.Yellow;
        //Color LOSE = Color.Green;
        Color BG_COLOR = Color.Black;

        //Game Item Color
        Color EMPTY = Color.Black;
        Color WALL = Color.Gray;
        Color Box_Desti = Color.Red;
        Color Box_Pos = Color.FromArgb(151, 62, 4);
        Color Person_1 = Color.Beige;
        Color Person_2 = Color.Beige;
        Color Box_On_Target = Color.Green;



        Color[,] GridsColor = new Color[ROW, COL];
        Label[,] grids = new Label[ROW, COL];

        public Form1()
        {
            InitializeComponent();
            SerialPortSelect.Items.AddRange(SerialPort.GetPortNames());

            for (int i = 0; i < ROW; i++)
            {
                for (int j = 0; j < COL; j++)
                {
                    grids[i, j] = new Label();
                    grids[i, j].Width = GRID_WIDTH;
                    grids[i, j].Height = GRID_HEIGHT;

                    grids[i, j].BackColor = BG_COLOR;
                    grids[i, j].Left = j * GRID_WIDTH + GAME_POS_X;
                    grids[i, j].Top = i * GRID_HEIGHT + GAME_POS_Y;
                    grids[i, j].Visible = true;

                    this.Controls.Add(grids[i, j]);
                }
            }
            SerialPortDisconnect.Enabled = false;
        }

        private void Form1_Load(object sender, EventArgs e)
        {

        }

        private void SerialPortConnect_Click(object sender, EventArgs e)
        {
            try
            {
                SerialPort.PortName = port.ToString();
                SerialPort.BaudRate = 115200;
                SerialPort.Parity = Parity.None;
                SerialPort.DataBits = 8;
                SerialPort.StopBits = StopBits.One;
                SerialPort.Open();
                SerialPortConnect.Enabled = false;
                SerialPortDisconnect.Enabled = true;

                SerialPort.DiscardInBuffer();       //可加可不加

                if (SerialPort.IsOpen)
                {
                    //Console.WriteLine(text + "Port is : " + SerialPort.PortName);
                    Receive_Data();

                }
            }
            catch (Exception exex)
            {
                MessageBox.Show(exex.Message);
                SerialPort.Close();

            }
        }

        private void SerialPortDisconnect_Click(object sender, EventArgs e)
        {
            SerialPortSelect.Enabled = true;
            SerialPortConnect.Enabled = false;
            SerialPort.Close();
        }

        private void SerialPortSelect_SelectedIndexChanged(object sender, EventArgs e)
        {
            port.Append(SerialPortSelect.Text);
        }

        private void Score_Text_TextChanged(object sender, EventArgs e)
        {

        }

        private void Receive_Data()
        {
            try
            {
                ReceiveSerial = new Task(Receive_Data_available);
                ReceiveSerial.Start();

            }
            catch (Exception exec)
            {
                MessageBox.Show(exec.Message);
            }
        }

        private void Receive_Data_available()
        {
            int row = 0, col = 0, i;
            int number = 5;                                   // 設定接收所有資料的位元組數量
            int Map_number = 4;                               // 設定接收地圖資料的位元組數量
            int Game_State_number = 4;                        // 遊戲狀態的位元組位置
            int Game_State = 0;

            byte[] ReceiveData = new byte[number];             //接收UART資料 : {地圖資料[72 bits]} + {遊戲遊玩狀態[1bit]}

            while (SerialPort.IsOpen)
            {
                if (SerialPort.BytesToRead == number)
                {
                    SerialPort.Read(ReceiveData, 0, number);                        // 讀取接收的資料到位元組陣列中 || 0是陣列索引值
                    Game_State = ReceiveData[Game_State_number] % 10;            // 將UART的第[72]資料儲存到Sokoban_Game_State中

                    string receivedText = Encoding.ASCII.GetString(ReceiveData);    // 將位元組陣列轉換為字串-之後會刪除
                    //UpdateRichTextBox(receivedText);                                // 更新 RichTextBox中的文字-之後會刪除                   
                    
                    UpdateScore(Game_State);

                    //for (i=0; i<number; i++)
                    //{
                    //    Console.Write($"{ReceiveData[i]}");
                    //}
                    //MessageBox.Show("Game_State_number is"+ Sokoban_Game_State);
                    if (Game_State == 2)
                    {
                        MessageBox.Show("遊戲失敗！\n" + "1.重新遊玩遊戲請按下確認鍵後按「中間」的重置按鈕\n" + "2.想離開遊戲請按「上」的按鈕並按下確認鍵");
                        Task.Delay(20);
                    }
                    for (i = 0; i < Map_number; i++)
                    {
                        //switch (ReceiveData[0] % 10)
                        switch (ReceiveData[0] % 10)
                        {
                            case 0:
                                {
                                    GridsColor[0, 0] = Color.Black;
                                    GridsColor[0, 1] = Color.White;
                                    GridsColor[0, 2] = Color.White; ;
                                    GridsColor[0, 3] = Color.White; ;
                                }
                                break;
                            case 1:
                                {
                                    GridsColor[0, 0] = Color.White;
                                    GridsColor[0, 1] = Color.Black;
                                    GridsColor[0, 2] = Color.White; ;
                                    GridsColor[0, 3] = Color.White; ;
                                }
                                break;
                            case 2:
                                {
                                    GridsColor[0, 0] = Color.White;
                                    GridsColor[0, 1] = Color.White;
                                    GridsColor[0, 2] = Color.Black; ;
                                    GridsColor[0, 3] = Color.White; ;
                                }
                                break;
                            case 3:
                                {
                                    GridsColor[0, 0] = Color.White;
                                    GridsColor[0, 1] = Color.White;
                                    GridsColor[0, 2] = Color.White; ;
                                    GridsColor[0, 3] = Color.Black; ;
                                }
                                break;
                        }
                        //switch (ReceiveData[1] % 10)
                        switch (ReceiveData[1] % 10)
                        {
                            case 0:
                                {
                                    GridsColor[1, 0] = Color.Black;
                                    GridsColor[1, 1] = Color.White;
                                    GridsColor[1, 2] = Color.White; ;
                                    GridsColor[1, 3] = Color.White; ;
                                }
                                break;
                            case 1:
                                {
                                    GridsColor[1, 0] = Color.White;
                                    GridsColor[1, 1] = Color.Black;
                                    GridsColor[1, 2] = Color.White; ;
                                    GridsColor[1, 3] = Color.White; ;
                                }
                                break;
                            case 2:
                                {
                                    GridsColor[1, 0] = Color.White;
                                    GridsColor[1, 1] = Color.White;
                                    GridsColor[1, 2] = Color.Black; ;
                                    GridsColor[1, 3] = Color.White; ;
                                }
                                break;
                            case 3:
                                {
                                    GridsColor[1, 0] = Color.White;
                                    GridsColor[1, 1] = Color.White;
                                    GridsColor[1, 2] = Color.White; ;
                                    GridsColor[1, 3] = Color.Black; ;
                                }
                                break;
                        }
                        switch (ReceiveData[2] % 10)
                        //switch (ReceiveData[2] )
                        {
                            case 0:
                                {
                                    GridsColor[2, 0] = Color.Black;
                                    GridsColor[2, 1] = Color.White;
                                    GridsColor[2, 2] = Color.White; ;
                                    GridsColor[2, 3] = Color.White; ;
                                }
                                break;
                            case 1:
                                {
                                    GridsColor[2, 0] = Color.White;
                                    GridsColor[2, 1] = Color.Black;
                                    GridsColor[2, 2] = Color.White; ;
                                    GridsColor[2, 3] = Color.White; ;
                                }
                                break;
                            case 2:
                                {
                                    GridsColor[2, 0] = Color.White;
                                    GridsColor[2, 1] = Color.White;
                                    GridsColor[2, 2] = Color.Black; ;
                                    GridsColor[2, 3] = Color.White; ;
                                }
                                break;
                            case 3:
                                {
                                    GridsColor[2, 0] = Color.White;
                                    GridsColor[2, 1] = Color.White;
                                    GridsColor[2, 2] = Color.White; ;
                                    GridsColor[2, 3] = Color.Black; ;
                                }
                                break;
                        }
                        //switch (ReceiveData[3] % 10)
                        switch (ReceiveData[3] % 10 )
                        {
                            case 0:
                                {
                                    GridsColor[3, 0] = Color.Black;
                                    GridsColor[3, 1] = Color.White;
                                    GridsColor[3, 2] = Color.White; ;
                                    GridsColor[3, 3] = Color.White; ;
                                }
                                break;
                            case 1:
                                {
                                    GridsColor[3, 0] = Color.White;
                                    GridsColor[3, 1] = Color.Black;
                                    GridsColor[3, 2] = Color.White; ;
                                    GridsColor[3, 3] = Color.White; ;
                                }
                                break;
                            case 2:
                                {
                                    GridsColor[3, 0] = Color.White;
                                    GridsColor[3, 1] = Color.White;
                                    GridsColor[3, 2] = Color.Black; ;
                                    GridsColor[3, 3] = Color.White; ;
                                }
                                break;
                            case 3:
                                {
                                    GridsColor[3, 0] = Color.White;
                                    GridsColor[3, 1] = Color.White;
                                    GridsColor[3, 2] = Color.White; ;
                                    GridsColor[3, 3] = Color.Black; ;
                                }
                                break;
                        }

                        Console.Write($"{ReceiveData[i]}");                        
                        col = col + 1;
                        if (col == COL)
                        {
                            row = row + 1;
                            col = 0;
                            Console.WriteLine();
                        }
                        if (row == ROW)
                        {
                            row = 0;
                            col = 0;

                        }
                    }
                    UpadteScreen();
                }
                else
                {
                    // 若資料長度不足，等待一段時間再嘗試讀取
                    Task.Delay(20).Wait();
                }
            }
        }

        private void UpadteScreen()
        {
            //if(grids.InvokeRequired)
            this.Invoke(new EventHandler(delegate
            {
                for (int i = 0; i < ROW; i++)
                {
                    for (int j = 0; j < COL; j++)
                    {
                        grids[i, j].BackColor = GridsColor[i, j];
                    }
                }
            }
            ));
        }

        //之後會刪除
        /*private void UpdateRichTextBox(string text)
        {
            
            if (uart_richTextBox_test.InvokeRequired)
            {
                uart_richTextBox_test.Invoke(new Action<string>(UpdateRichTextBox), text); // 在 UI 線程中更新 RichTextBox
            }
            else
            {
                uart_richTextBox_test.AppendText(text + Environment.NewLine); // 將接收到的資料顯示在 RichTextBox 中並換行
            }
        }*/
        private void UpdateScore(int state)
        {
            // 檢查是否已啟動計分
            switch (state)
            {
                case 0:
                    score = 0;
                    break;
                case 1:
                    score++;
                    break;
                case 2:
                    score = 0;
                    break;
                case 3:
                    score = score;
                    break;
                case 4:
                    score = score;
                    break;
                default:
                    score = 0;
                    break;
            }

            // 在 UI 執行緒中更新 TextBox 的值
            BeginInvoke(new Action(() => Score_Text.Text = $"{score}")); 

        }
    }
}
