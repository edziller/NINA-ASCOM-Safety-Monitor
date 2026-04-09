// SetupDialogForm.Designer.cs — v3
// Expandido: COM + Altitude + 5 parâmetros calibráveis
// FIX: TopMost REMOVIDO (causava abertura minimizada/atrás de outras janelas)

namespace ASCOM.EZZillerSafetyMonitor
{
    partial class SetupDialogForm
    {
        private System.ComponentModel.IContainer components = null;

        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
                components.Dispose();
            base.Dispose(disposing);
        }

        private void InitializeComponent()
        {
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(SetupDialogForm));
            this.cmdOK = new System.Windows.Forms.Button();
            this.cmdCancel = new System.Windows.Forms.Button();
            this.picASCOM = new System.Windows.Forms.PictureBox();
            this.picZILLER = new System.Windows.Forms.PictureBox();
            this.chkTrace = new System.Windows.Forms.CheckBox();
            this.label1 = new System.Windows.Forms.Label();
            this.comboBoxComPort = new System.Windows.Forms.ComboBox();
            this.txtAltitude = new System.Windows.Forms.TextBox();
            this.txtLimiteIR = new System.Windows.Forms.TextBox();
            this.txtTimeoutNublado = new System.Windows.Forms.TextBox();
            this.txtLimiteDeltaP = new System.Windows.Forms.TextBox();
            this.txtLimiteUmidade = new System.Windows.Forms.TextBox();
            this.txtLimiteChuva = new System.Windows.Forms.TextBox();
            this.lblComPort = new System.Windows.Forms.Label();
            this.lblAltitude = new System.Windows.Forms.Label();
            this.lblLimiteIR = new System.Windows.Forms.Label();
            this.lblTimeoutNublado = new System.Windows.Forms.Label();
            this.lblLimiteDeltaP = new System.Windows.Forms.Label();
            this.lblLimiteUmidade = new System.Windows.Forms.Label();
            this.lblLimiteChuva = new System.Windows.Forms.Label();
            this.grpConexao = new System.Windows.Forms.GroupBox();
            this.grpParametros = new System.Windows.Forms.GroupBox();
            this.lblDica = new System.Windows.Forms.Label();
            ((System.ComponentModel.ISupportInitialize)(this.picASCOM)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.picZILLER)).BeginInit();
            this.grpConexao.SuspendLayout();
            this.grpParametros.SuspendLayout();
            this.SuspendLayout();
            // 
            // cmdOK
            // 
            this.cmdOK.Location = new System.Drawing.Point(142, 306);
            this.cmdOK.Margin = new System.Windows.Forms.Padding(2, 2, 2, 2);
            this.cmdOK.Name = "cmdOK";
            this.cmdOK.Size = new System.Drawing.Size(56, 23);
            this.cmdOK.TabIndex = 7;
            this.cmdOK.Text = "OK";
            this.cmdOK.Click += new System.EventHandler(this.CmdOK_Click);
            // 
            // cmdCancel
            // 
            this.cmdCancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.cmdCancel.Location = new System.Drawing.Point(206, 306);
            this.cmdCancel.Margin = new System.Windows.Forms.Padding(2, 2, 2, 2);
            this.cmdCancel.Name = "cmdCancel";
            this.cmdCancel.Size = new System.Drawing.Size(56, 23);
            this.cmdCancel.TabIndex = 8;
            this.cmdCancel.Text = "Cancelar";
            this.cmdCancel.Click += new System.EventHandler(this.CmdCancel_Click);
            // 
            // picASCOM
            // 
            this.picASCOM.Cursor = System.Windows.Forms.Cursors.Hand;
            this.picASCOM.Image = ((System.Drawing.Image)(resources.GetObject("picASCOM.Image")));
            this.picASCOM.Location = new System.Drawing.Point(216, 6);
            this.picASCOM.Margin = new System.Windows.Forms.Padding(2, 2, 2, 2);
            this.picASCOM.Name = "picASCOM";
            this.picASCOM.Size = new System.Drawing.Size(48, 56);
            this.picASCOM.SizeMode = System.Windows.Forms.PictureBoxSizeMode.AutoSize;
            this.picASCOM.TabIndex = 1;
            this.picASCOM.TabStop = false;
            this.picASCOM.Click += new System.EventHandler(this.BrowseToAscom);
            this.picASCOM.DoubleClick += new System.EventHandler(this.BrowseToAscom);
            // 
            // picZILLER
            // 
            this.picZILLER.Cursor = System.Windows.Forms.Cursors.Hand;
            this.picZILLER.Image = ((System.Drawing.Image)(resources.GetObject("picZILLER.Image")));
            this.picZILLER.Location = new System.Drawing.Point(23, -16);
            this.picZILLER.Margin = new System.Windows.Forms.Padding(2, 2, 2, 2);
            this.picZILLER.Name = "picZILLER";
            this.picZILLER.Size = new System.Drawing.Size(205, 92);
            this.picZILLER.SizeMode = System.Windows.Forms.PictureBoxSizeMode.Zoom;
            this.picZILLER.TabIndex = 0;
            this.picZILLER.TabStop = false;
            this.picZILLER.Click += new System.EventHandler(this.BrowseToAscom);
            this.picZILLER.DoubleClick += new System.EventHandler(this.BrowseToAscom);
            // 
            // chkTrace
            // 
            this.chkTrace.AutoSize = true;
            this.chkTrace.Location = new System.Drawing.Point(9, 309);
            this.chkTrace.Margin = new System.Windows.Forms.Padding(2, 2, 2, 2);
            this.chkTrace.Name = "chkTrace";
            this.chkTrace.Size = new System.Drawing.Size(69, 17);
            this.chkTrace.TabIndex = 9;
            this.chkTrace.Text = "Trace on";
            this.chkTrace.CheckedChanged += new System.EventHandler(this.chkTrace_CheckedChanged);
            // 
            // label1
            // 
            this.label1.Location = new System.Drawing.Point(150, 6);
            this.label1.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(75, 16);
            this.label1.TabIndex = 2;
            this.label1.Click += new System.EventHandler(this.Label1_Click);
            // 
            // comboBoxComPort
            // 
            this.comboBoxComPort.Location = new System.Drawing.Point(98, 17);
            this.comboBoxComPort.Margin = new System.Windows.Forms.Padding(2, 2, 2, 2);
            this.comboBoxComPort.Name = "comboBoxComPort";
            this.comboBoxComPort.Size = new System.Drawing.Size(144, 21);
            this.comboBoxComPort.TabIndex = 0;
            this.comboBoxComPort.SelectedIndexChanged += new System.EventHandler(this.comboBoxComPort_SelectedIndexChanged);
            // 
            // txtAltitude
            // 
            this.txtAltitude.Location = new System.Drawing.Point(98, 40);
            this.txtAltitude.Margin = new System.Windows.Forms.Padding(2, 2, 2, 2);
            this.txtAltitude.Name = "txtAltitude";
            this.txtAltitude.Size = new System.Drawing.Size(68, 20);
            this.txtAltitude.TabIndex = 1;
            this.txtAltitude.TextChanged += new System.EventHandler(this.txtAltitude_TextChanged);
            // 
            // txtLimiteIR
            // 
            this.txtLimiteIR.Location = new System.Drawing.Point(158, 16);
            this.txtLimiteIR.Margin = new System.Windows.Forms.Padding(2, 2, 2, 2);
            this.txtLimiteIR.Name = "txtLimiteIR";
            this.txtLimiteIR.Size = new System.Drawing.Size(88, 20);
            this.txtLimiteIR.TabIndex = 2;
            // 
            // txtTimeoutNublado
            // 
            this.txtTimeoutNublado.Location = new System.Drawing.Point(158, 44);
            this.txtTimeoutNublado.Margin = new System.Windows.Forms.Padding(2, 2, 2, 2);
            this.txtTimeoutNublado.Name = "txtTimeoutNublado";
            this.txtTimeoutNublado.Size = new System.Drawing.Size(88, 20);
            this.txtTimeoutNublado.TabIndex = 3;
            // 
            // txtLimiteDeltaP
            // 
            this.txtLimiteDeltaP.Location = new System.Drawing.Point(158, 72);
            this.txtLimiteDeltaP.Margin = new System.Windows.Forms.Padding(2, 2, 2, 2);
            this.txtLimiteDeltaP.Name = "txtLimiteDeltaP";
            this.txtLimiteDeltaP.Size = new System.Drawing.Size(88, 20);
            this.txtLimiteDeltaP.TabIndex = 4;
            // 
            // txtLimiteUmidade
            // 
            this.txtLimiteUmidade.Location = new System.Drawing.Point(158, 99);
            this.txtLimiteUmidade.Margin = new System.Windows.Forms.Padding(2, 2, 2, 2);
            this.txtLimiteUmidade.Name = "txtLimiteUmidade";
            this.txtLimiteUmidade.Size = new System.Drawing.Size(88, 20);
            this.txtLimiteUmidade.TabIndex = 5;
            // 
            // txtLimiteChuva
            // 
            this.txtLimiteChuva.Location = new System.Drawing.Point(158, 127);
            this.txtLimiteChuva.Margin = new System.Windows.Forms.Padding(2, 2, 2, 2);
            this.txtLimiteChuva.Name = "txtLimiteChuva";
            this.txtLimiteChuva.Size = new System.Drawing.Size(88, 20);
            this.txtLimiteChuva.TabIndex = 6;
            // 
            // lblComPort
            // 
            this.lblComPort.AutoSize = true;
            this.lblComPort.Location = new System.Drawing.Point(6, 20);
            this.lblComPort.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.lblComPort.Name = "lblComPort";
            this.lblComPort.Size = new System.Drawing.Size(62, 13);
            this.lblComPort.TabIndex = 0;
            this.lblComPort.Text = "Porta COM:";
            // 
            // lblAltitude
            // 
            this.lblAltitude.AutoSize = true;
            this.lblAltitude.Location = new System.Drawing.Point(6, 42);
            this.lblAltitude.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.lblAltitude.Name = "lblAltitude";
            this.lblAltitude.Size = new System.Drawing.Size(62, 13);
            this.lblAltitude.TabIndex = 1;
            this.lblAltitude.Text = "Altitude (m):";
            // 
            // lblLimiteIR
            // 
            this.lblLimiteIR.AutoSize = true;
            this.lblLimiteIR.Location = new System.Drawing.Point(6, 18);
            this.lblLimiteIR.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.lblLimiteIR.Name = "lblLimiteIR";
            this.lblLimiteIR.Size = new System.Drawing.Size(95, 13);
            this.lblLimiteIR.TabIndex = 0;
            this.lblLimiteIR.Text = "Limite IR Ceu (oC):";
            // 
            // lblTimeoutNublado
            // 
            this.lblTimeoutNublado.AutoSize = true;
            this.lblTimeoutNublado.Location = new System.Drawing.Point(6, 46);
            this.lblTimeoutNublado.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.lblTimeoutNublado.Name = "lblTimeoutNublado";
            this.lblTimeoutNublado.Size = new System.Drawing.Size(116, 13);
            this.lblTimeoutNublado.TabIndex = 3;
            this.lblTimeoutNublado.Text = "Timeout Nublado (min):";
            // 
            // lblLimiteDeltaP
            // 
            this.lblLimiteDeltaP.AutoSize = true;
            this.lblLimiteDeltaP.Location = new System.Drawing.Point(6, 73);
            this.lblLimiteDeltaP.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.lblLimiteDeltaP.Name = "lblLimiteDeltaP";
            this.lblLimiteDeltaP.Size = new System.Drawing.Size(103, 13);
            this.lblLimiteDeltaP.TabIndex = 4;
            this.lblLimiteDeltaP.Text = "Limite Delta P (hPa):";
            // 
            // lblLimiteUmidade
            // 
            this.lblLimiteUmidade.AutoSize = true;
            this.lblLimiteUmidade.Location = new System.Drawing.Point(6, 101);
            this.lblLimiteUmidade.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.lblLimiteUmidade.Name = "lblLimiteUmidade";
            this.lblLimiteUmidade.Size = new System.Drawing.Size(99, 13);
            this.lblLimiteUmidade.TabIndex = 5;
            this.lblLimiteUmidade.Text = "Limite Umidade (%):";
            // 
            // lblLimiteChuva
            // 
            this.lblLimiteChuva.AutoSize = true;
            this.lblLimiteChuva.Location = new System.Drawing.Point(6, 128);
            this.lblLimiteChuva.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.lblLimiteChuva.Name = "lblLimiteChuva";
            this.lblLimiteChuva.Size = new System.Drawing.Size(113, 13);
            this.lblLimiteChuva.TabIndex = 6;
            this.lblLimiteChuva.Text = "Limite Chuva (0-1023):";
            // 
            // grpConexao
            // 
            this.grpConexao.Controls.Add(this.lblComPort);
            this.grpConexao.Controls.Add(this.comboBoxComPort);
            this.grpConexao.Controls.Add(this.lblAltitude);
            this.grpConexao.Controls.Add(this.txtAltitude);
            this.grpConexao.Location = new System.Drawing.Point(9, 58);
            this.grpConexao.Margin = new System.Windows.Forms.Padding(2, 2, 2, 2);
            this.grpConexao.Name = "grpConexao";
            this.grpConexao.Padding = new System.Windows.Forms.Padding(2, 2, 2, 2);
            this.grpConexao.Size = new System.Drawing.Size(255, 65);
            this.grpConexao.TabIndex = 3;
            this.grpConexao.TabStop = false;
            this.grpConexao.Text = "Conexão";
            // 
            // grpParametros
            // 
            this.grpParametros.Controls.Add(this.lblLimiteIR);
            this.grpParametros.Controls.Add(this.txtLimiteIR);
            this.grpParametros.Controls.Add(this.lblTimeoutNublado);
            this.grpParametros.Controls.Add(this.txtTimeoutNublado);
            this.grpParametros.Controls.Add(this.lblLimiteDeltaP);
            this.grpParametros.Controls.Add(this.txtLimiteDeltaP);
            this.grpParametros.Controls.Add(this.lblLimiteUmidade);
            this.grpParametros.Controls.Add(this.txtLimiteUmidade);
            this.grpParametros.Controls.Add(this.lblLimiteChuva);
            this.grpParametros.Controls.Add(this.txtLimiteChuva);
            this.grpParametros.Controls.Add(this.lblDica);
            this.grpParametros.Location = new System.Drawing.Point(9, 130);
            this.grpParametros.Margin = new System.Windows.Forms.Padding(2, 2, 2, 2);
            this.grpParametros.Name = "grpParametros";
            this.grpParametros.Padding = new System.Windows.Forms.Padding(2, 2, 2, 2);
            this.grpParametros.Size = new System.Drawing.Size(255, 167);
            this.grpParametros.TabIndex = 4;
            this.grpParametros.TabStop = false;
            this.grpParametros.Text = "Parâmetros de Segurança  (? = passe o mouse)";
            // 
            // lblDica
            // 
            this.lblDica.Font = new System.Drawing.Font("Segoe UI", 7.5F, System.Drawing.FontStyle.Italic);
            this.lblDica.ForeColor = System.Drawing.Color.Gray;
            this.lblDica.Location = new System.Drawing.Point(7, 149);
            this.lblDica.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.lblDica.Name = "lblDica";
            this.lblDica.Size = new System.Drawing.Size(240, 13);
            this.lblDica.TabIndex = 7;
            this.lblDica.Text = "Passe o mouse sobre cada campo para ver a descricao.";
            // 
            // SetupDialogForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(273, 335);
            this.Controls.Add(this.picASCOM);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.grpConexao);
            this.Controls.Add(this.grpParametros);
            this.Controls.Add(this.chkTrace);
            this.Controls.Add(this.cmdOK);
            this.Controls.Add(this.cmdCancel);
            this.Controls.Add(this.picZILLER);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.Margin = new System.Windows.Forms.Padding(2, 2, 2, 2);
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "SetupDialogForm";
            this.SizeGripStyle = System.Windows.Forms.SizeGripStyle.Hide;
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "Ziller Safety Monitor — Setup";
            this.Load += new System.EventHandler(this.SetupDialogForm_Load);
            ((System.ComponentModel.ISupportInitialize)(this.picASCOM)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.picZILLER)).EndInit();
            this.grpConexao.ResumeLayout(false);
            this.grpConexao.PerformLayout();
            this.grpParametros.ResumeLayout(false);
            this.grpParametros.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        private System.Windows.Forms.Button     cmdOK;
        private System.Windows.Forms.Button     cmdCancel;
        private System.Windows.Forms.Label      label1;
        private System.Windows.Forms.PictureBox picASCOM;
        private System.Windows.Forms.PictureBox picZILLER;
        private System.Windows.Forms.CheckBox   chkTrace;
        private System.Windows.Forms.ComboBox   comboBoxComPort;
        private System.Windows.Forms.TextBox    txtAltitude;
        private System.Windows.Forms.TextBox    txtLimiteIR;
        private System.Windows.Forms.TextBox    txtTimeoutNublado;
        private System.Windows.Forms.TextBox    txtLimiteDeltaP;
        private System.Windows.Forms.TextBox    txtLimiteUmidade;
        private System.Windows.Forms.TextBox    txtLimiteChuva;
        private System.Windows.Forms.Label      lblComPort;
        private System.Windows.Forms.Label      lblAltitude;
        private System.Windows.Forms.Label      lblLimiteIR;
        private System.Windows.Forms.Label      lblTimeoutNublado;
        private System.Windows.Forms.Label      lblLimiteDeltaP;
        private System.Windows.Forms.Label      lblLimiteUmidade;
        private System.Windows.Forms.Label      lblLimiteChuva;
        private System.Windows.Forms.GroupBox   grpConexao;
        private System.Windows.Forms.GroupBox   grpParametros;
        private System.Windows.Forms.Label      lblDica;
    }
}
