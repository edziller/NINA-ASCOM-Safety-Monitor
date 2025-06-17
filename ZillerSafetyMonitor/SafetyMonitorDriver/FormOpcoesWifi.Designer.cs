using System;
using System.IO.Ports;
using System.Windows.Forms;

namespace ASCOM.EZZillerSafetyMonitor
{
    public partial class FormOpcoesWifi : Form
    {
        //private SerialPort serialPort;
        private ComboBox comboBoxSSID;
        private TextBox txtSenha;
        private Button btnConectar;

       
        private void InitializeComponent()
        {
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(FormOpcoesWifi));
            this.comboBoxSSID = new System.Windows.Forms.ComboBox();
            this.txtSenha = new System.Windows.Forms.TextBox();
            this.btnConectar = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // comboBoxSSID
            // 
            this.comboBoxSSID.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.comboBoxSSID.Location = new System.Drawing.Point(20, 20);
            this.comboBoxSSID.Name = "comboBoxSSID";
            this.comboBoxSSID.Size = new System.Drawing.Size(200, 24);
            this.comboBoxSSID.TabIndex = 0;
            // 
            // txtSenha
            // 
            this.txtSenha.Location = new System.Drawing.Point(20, 60);
            this.txtSenha.Name = "txtSenha";
            this.txtSenha.PasswordChar = '*';
            this.txtSenha.Size = new System.Drawing.Size(200, 22);
            this.txtSenha.TabIndex = 1;
            // 
            // btnConectar
            // 
            this.btnConectar.Location = new System.Drawing.Point(20, 100);
            this.btnConectar.Name = "btnConectar";
            this.btnConectar.Size = new System.Drawing.Size(75, 23);
            this.btnConectar.TabIndex = 2;
            this.btnConectar.Text = "Conectar";
            this.btnConectar.Click += new System.EventHandler(this.btnConectar_Click);
            // 
            // FormOpcoesWifi
            // 
            this.ClientSize = new System.Drawing.Size(250, 150);
            this.Controls.Add(this.comboBoxSSID);
            this.Controls.Add(this.txtSenha);
            this.Controls.Add(this.btnConectar);
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.Name = "FormOpcoesWifi";
            this.Text = "Configurar Wi-Fi";
            this.Load += new System.EventHandler(this.FormOpcoesWifi_Load);
            this.ResumeLayout(false);
            this.PerformLayout();

        }
              

    }
}
