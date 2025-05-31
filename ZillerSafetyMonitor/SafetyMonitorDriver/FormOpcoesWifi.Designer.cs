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
            this.comboBoxSSID = new ComboBox();
            this.txtSenha = new TextBox();
            this.btnConectar = new Button();

            this.SuspendLayout();

            this.comboBoxSSID.DropDownStyle = ComboBoxStyle.DropDownList;
            this.comboBoxSSID.Location = new System.Drawing.Point(20, 20);
            this.comboBoxSSID.Size = new System.Drawing.Size(200, 20);

            this.txtSenha.Location = new System.Drawing.Point(20, 60);
            this.txtSenha.Size = new System.Drawing.Size(200, 20);
            this.txtSenha.PasswordChar = '*';

            this.btnConectar.Text = "Conectar";
            this.btnConectar.Location = new System.Drawing.Point(20, 100);
            this.btnConectar.Click += new EventHandler(this.btnConectar_Click);

            this.Controls.Add(this.comboBoxSSID);
            this.Controls.Add(this.txtSenha);
            this.Controls.Add(this.btnConectar);

            this.Text = "Configurar Wi-Fi";
            this.ClientSize = new System.Drawing.Size(250, 150);
            this.ResumeLayout(false);
        }
              

    }
}
