namespace Final_Project_PC
{
    partial class Form1
    {
        /// <summary>
        /// 設計工具所需的變數。
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// 清除任何使用中的資源。
        /// </summary>
        /// <param name="disposing">如果應該處置受控資源則為 true，否則為 false。</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form 設計工具產生的程式碼

        /// <summary>
        /// 此為設計工具支援所需的方法 - 請勿使用程式碼編輯器修改
        /// 這個方法的內容。
        /// </summary>
        private void InitializeComponent()
        {
            this.SerialPortConnect = new System.Windows.Forms.Button();
            this.SerialPortDisconnect = new System.Windows.Forms.Button();
            this.comport_label = new System.Windows.Forms.Label();
            this.label1 = new System.Windows.Forms.Label();
            this.Score = new System.Windows.Forms.Label();
            this.SerialPortSelect = new System.Windows.Forms.ComboBox();
            this.Score_Text = new System.Windows.Forms.TextBox();
            this.SuspendLayout();
            // 
            // SerialPortConnect
            // 
            this.SerialPortConnect.Font = new System.Drawing.Font("新細明體", 13.8F);
            this.SerialPortConnect.Location = new System.Drawing.Point(572, 166);
            this.SerialPortConnect.Name = "SerialPortConnect";
            this.SerialPortConnect.Size = new System.Drawing.Size(125, 49);
            this.SerialPortConnect.TabIndex = 0;
            this.SerialPortConnect.Text = "Connect";
            this.SerialPortConnect.UseVisualStyleBackColor = true;
            this.SerialPortConnect.Click += new System.EventHandler(this.SerialPortConnect_Click);
            // 
            // SerialPortDisconnect
            // 
            this.SerialPortDisconnect.Font = new System.Drawing.Font("新細明體", 13.8F);
            this.SerialPortDisconnect.Location = new System.Drawing.Point(572, 249);
            this.SerialPortDisconnect.Name = "SerialPortDisconnect";
            this.SerialPortDisconnect.Size = new System.Drawing.Size(125, 49);
            this.SerialPortDisconnect.TabIndex = 1;
            this.SerialPortDisconnect.Text = "Disconnect";
            this.SerialPortDisconnect.UseVisualStyleBackColor = true;
            this.SerialPortDisconnect.Click += new System.EventHandler(this.SerialPortDisconnect_Click);
            // 
            // comport_label
            // 
            this.comport_label.AutoSize = true;
            this.comport_label.Font = new System.Drawing.Font("新細明體", 13.8F);
            this.comport_label.Location = new System.Drawing.Point(573, 47);
            this.comport_label.Name = "comport_label";
            this.comport_label.Size = new System.Drawing.Size(120, 23);
            this.comport_label.TabIndex = 2;
            this.comport_label.Text = "COM PORT";
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Font = new System.Drawing.Font("新細明體", 20F);
            this.label1.ForeColor = System.Drawing.SystemColors.MenuHighlight;
            this.label1.Location = new System.Drawing.Point(244, 9);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(185, 34);
            this.label1.TabIndex = 3;
            this.label1.Text = "別踩白塊兒";
            // 
            // Score
            // 
            this.Score.AutoSize = true;
            this.Score.Font = new System.Drawing.Font("新細明體", 14F);
            this.Score.Location = new System.Drawing.Point(52, 46);
            this.Score.Name = "Score";
            this.Score.Size = new System.Drawing.Size(62, 24);
            this.Score.TabIndex = 4;
            this.Score.Text = "Score";
            // 
            // SerialPortSelect
            // 
            this.SerialPortSelect.FormattingEnabled = true;
            this.SerialPortSelect.Location = new System.Drawing.Point(572, 83);
            this.SerialPortSelect.Name = "SerialPortSelect";
            this.SerialPortSelect.Size = new System.Drawing.Size(121, 23);
            this.SerialPortSelect.TabIndex = 5;
            this.SerialPortSelect.SelectedIndexChanged += new System.EventHandler(this.SerialPortSelect_SelectedIndexChanged);
            // 
            // Score_Text
            // 
            this.Score_Text.Font = new System.Drawing.Font("新細明體", 16F);
            this.Score_Text.Location = new System.Drawing.Point(56, 80);
            this.Score_Text.Name = "Score_Text";
            this.Score_Text.Size = new System.Drawing.Size(58, 39);
            this.Score_Text.TabIndex = 6;
            this.Score_Text.TextChanged += new System.EventHandler(this.Score_Text_TextChanged);
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(8F, 15F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(800, 450);
            this.Controls.Add(this.Score_Text);
            this.Controls.Add(this.SerialPortSelect);
            this.Controls.Add(this.Score);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.comport_label);
            this.Controls.Add(this.SerialPortDisconnect);
            this.Controls.Add(this.SerialPortConnect);
            this.Name = "Form1";
            this.Text = "Form1";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button SerialPortConnect;
        private System.Windows.Forms.Button SerialPortDisconnect;
        private System.Windows.Forms.Label comport_label;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label Score;
        private System.Windows.Forms.ComboBox SerialPortSelect;
        private System.Windows.Forms.TextBox Score_Text;
    }
}

