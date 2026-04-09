// FormOpcoesWifi.Designer.cs — v2
// Adicionado: lblStatus para feedback durante o scan

using System;
using System.Windows.Forms;

namespace ASCOM.EZZillerSafetyMonitor
{
    public partial class FormOpcoesWifi : Form
    {
        private ComboBox comboBoxSSID;
        private TextBox  txtSenha;
        private Button   btnConectar;
        private Label    lblSSID;
        private Label    lblSenha;
        private Label    lblStatus;   // FIX: novo label de status do scan

        private void InitializeComponent()
        {
            System.ComponentModel.ComponentResourceManager resources =
                new System.ComponentModel.ComponentResourceManager(typeof(FormOpcoesWifi));

            this.comboBoxSSID = new System.Windows.Forms.ComboBox();
            this.txtSenha     = new System.Windows.Forms.TextBox();
            this.btnConectar  = new System.Windows.Forms.Button();
            this.lblSSID      = new System.Windows.Forms.Label();
            this.lblSenha     = new System.Windows.Forms.Label();
            this.lblStatus    = new System.Windows.Forms.Label();
            this.SuspendLayout();

            // lblSSID
            this.lblSSID.AutoSize = true;
            this.lblSSID.Location = new System.Drawing.Point(12, 12);
            this.lblSSID.Text     = "Rede Wi-Fi:";

            // comboBoxSSID
            this.comboBoxSSID.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.comboBoxSSID.Location      = new System.Drawing.Point(15, 30);
            this.comboBoxSSID.Name          = "comboBoxSSID";
            this.comboBoxSSID.Size          = new System.Drawing.Size(250, 24);
            this.comboBoxSSID.TabIndex      = 0;

            // lblSenha
            this.lblSenha.AutoSize = true;
            this.lblSenha.Location = new System.Drawing.Point(12, 64);
            this.lblSenha.Text     = "Senha:";

            // txtSenha
            this.txtSenha.Location     = new System.Drawing.Point(15, 82);
            this.txtSenha.Name         = "txtSenha";
            this.txtSenha.PasswordChar = '*';
            this.txtSenha.Size         = new System.Drawing.Size(250, 22);
            this.txtSenha.TabIndex     = 1;

            // lblStatus — FIX: feedback em tempo real do scan
            this.lblStatus.AutoSize  = false;
            this.lblStatus.Location  = new System.Drawing.Point(15, 116);
            this.lblStatus.Name      = "lblStatus";
            this.lblStatus.Size      = new System.Drawing.Size(250, 34);
            this.lblStatus.ForeColor = System.Drawing.Color.Gray;
            this.lblStatus.Text      = "Aguardando...";

            // btnConectar
            this.btnConectar.Location = new System.Drawing.Point(15, 156);
            this.btnConectar.Name     = "btnConectar";
            this.btnConectar.Size     = new System.Drawing.Size(100, 28);
            this.btnConectar.TabIndex = 2;
            this.btnConectar.Text     = "Conectar";
            this.btnConectar.Click   += new System.EventHandler(this.btnConectar_Click);

            // FormOpcoesWifi
            this.AutoScaleDimensions = new System.Drawing.SizeF(8F, 16F);
            this.AutoScaleMode       = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize          = new System.Drawing.Size(282, 200);
            this.Controls.Add(this.lblSSID);
            this.Controls.Add(this.comboBoxSSID);
            this.Controls.Add(this.lblSenha);
            this.Controls.Add(this.txtSenha);
            this.Controls.Add(this.lblStatus);
            this.Controls.Add(this.btnConectar);
            try {
                this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            } catch { }
            this.Name            = "FormOpcoesWifi";
            this.Text            = "Configurar Wi-Fi";
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
            this.MaximizeBox     = false;
            this.MinimizeBox     = false;
            this.StartPosition   = System.Windows.Forms.FormStartPosition.CenterParent;
            this.Load           += new System.EventHandler(this.FormOpcoesWifi_Load);
            this.ResumeLayout(false);
            this.PerformLayout();
        }
    }
}
