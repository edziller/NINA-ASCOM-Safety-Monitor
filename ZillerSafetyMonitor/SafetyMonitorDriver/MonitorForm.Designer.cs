// MonitorForm.Designer.cs — v6
// Mudancas:
//  - Janela 700x560
//  - Beacon 112px: subtitulo NO TOPO, SAFE/UNSAFE no meio, badges ceu+chuva na base
//  - Badge de chuva largo (400px) para texto completo sem quebra
//  - Rodape 34px com todos os controles no mesmo Y (alinhamento correto)
//  - Label tendencia renomeado para "Tend. Nuvens:"

namespace ASCOM.EZZillerSafetyMonitor
{
    partial class MonitorForm
    {
        private System.ComponentModel.IContainer components = null;
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null)) components.Dispose();
            base.Dispose(disposing);
        }

        private void InitializeComponent()
        {
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(MonitorForm));
            this.pnlBeacon = new System.Windows.Forms.Panel();
            this.lblBeaconSub = new System.Windows.Forms.Label();
            this.pnlBeaconDot = new System.Windows.Forms.Panel();
            this.txtStatus = new System.Windows.Forms.TextBox();
            this.lblBeaconCeuTitle = new System.Windows.Forms.Label();
            this.lblBeaconCeu = new System.Windows.Forms.Label();
            this.lblBeaconChuvaTitle = new System.Windows.Forms.Label();
            this.lblBeaconChuva = new System.Windows.Forms.Label();
            this.labelAltitudeSensor = new System.Windows.Forms.Label();
            this.txtAltitudeSensor = new System.Windows.Forms.TextBox();
            this.pnlBody = new System.Windows.Forms.Panel();
            this.pnlRight = new System.Windows.Forms.Panel();
            this.lblSectionParams = new System.Windows.Forms.Label();
            this.labelLimiteIR = new System.Windows.Forms.Label();
            this.txtLimiteIR = new System.Windows.Forms.TextBox();
            this.labelTimeoutNublado = new System.Windows.Forms.Label();
            this.txtTimeoutNublado = new System.Windows.Forms.TextBox();
            this.labelDeltaP = new System.Windows.Forms.Label();
            this.txtLimiteDeltaP = new System.Windows.Forms.TextBox();
            this.labelLimiteUmidade = new System.Windows.Forms.Label();
            this.txtLimiteUmidade = new System.Windows.Forms.TextBox();
            this.labelLimiteChuva = new System.Windows.Forms.Label();
            this.txtLimiteChuva = new System.Windows.Forms.TextBox();
            this.labelAltitude = new System.Windows.Forms.Label();
            this.txtAltitude = new System.Windows.Forms.TextBox();
            this.btnSalvar = new System.Windows.Forms.Button();
            this.btnRedefinir = new System.Windows.Forms.Button();
            this.btnOpcoes = new System.Windows.Forms.Button();
            this.pnlLeft = new System.Windows.Forms.Panel();
            this.lblSectionSensores = new System.Windows.Forms.Label();
            this.lblIconTemp = new System.Windows.Forms.Label();
            this.labelTemperatura = new System.Windows.Forms.Label();
            this.txtTemperatura = new System.Windows.Forms.TextBox();
            this.lblIconHum = new System.Windows.Forms.Label();
            this.labelUmidadeRelativa = new System.Windows.Forms.Label();
            this.txtUmidade = new System.Windows.Forms.TextBox();
            this.lblIconDew = new System.Windows.Forms.Label();
            this.labelPontoOrvalho = new System.Windows.Forms.Label();
            this.txtOrvalho = new System.Windows.Forms.TextBox();
            this.lblIconPN = new System.Windows.Forms.Label();
            this.labelPressaoNominal = new System.Windows.Forms.Label();
            this.txtPressaoNom = new System.Windows.Forms.TextBox();
            this.lblIconPress = new System.Windows.Forms.Label();
            this.labelPressao = new System.Windows.Forms.Label();
            this.txtPressao = new System.Windows.Forms.TextBox();
            this.lblIconSP = new System.Windows.Forms.Label();
            this.labelStatusPressao = new System.Windows.Forms.Label();
            this.txtStatusPressao = new System.Windows.Forms.TextBox();
            this.lblIconIR = new System.Windows.Forms.Label();
            this.labelTIR = new System.Windows.Forms.Label();
            this.txtTIR = new System.Windows.Forms.TextBox();
            this.lblIconCeu = new System.Windows.Forms.Label();
            this.labelCondicaoCeu = new System.Windows.Forms.Label();
            this.txtEstadoCeu = new System.Windows.Forms.TextBox();
            this.lblIconChuva = new System.Windows.Forms.Label();
            this.labelCondicaoNuvens = new System.Windows.Forms.Label();
            this.txtEstadoChuva = new System.Windows.Forms.TextBox();
            this.pnlFooter = new System.Windows.Forms.Panel();
            this.labelTrendPressao = new System.Windows.Forms.Label();
            this.txtTrendPressao = new System.Windows.Forms.TextBox();
            this.labelTrendUmidade = new System.Windows.Forms.Label();
            this.txtTrendUmidade = new System.Windows.Forms.TextBox();
            this.labelTrendCeu = new System.Windows.Forms.Label();
            this.txtTrendCeu = new System.Windows.Forms.TextBox();
            this.lblConnDot = new System.Windows.Forms.Label();
            this.lblConnText = new System.Windows.Forms.Label();
            this.labelSeguranca = new System.Windows.Forms.Label();
            this.groupBoxCalibraveis = new System.Windows.Forms.GroupBox();
            this.groupBoxDados = new System.Windows.Forms.GroupBox();
            this.pnlBeacon.SuspendLayout();
            this.pnlBody.SuspendLayout();
            this.pnlRight.SuspendLayout();
            this.pnlLeft.SuspendLayout();
            this.pnlFooter.SuspendLayout();
            this.SuspendLayout();
            // 
            // pnlBeacon
            // 
            this.pnlBeacon.Controls.Add(this.lblBeaconSub);
            this.pnlBeacon.Controls.Add(this.pnlBeaconDot);
            this.pnlBeacon.Controls.Add(this.txtStatus);
            this.pnlBeacon.Controls.Add(this.lblBeaconCeuTitle);
            this.pnlBeacon.Controls.Add(this.lblBeaconCeu);
            this.pnlBeacon.Controls.Add(this.lblBeaconChuvaTitle);
            this.pnlBeacon.Controls.Add(this.lblBeaconChuva);
            this.pnlBeacon.Controls.Add(this.labelAltitudeSensor);
            this.pnlBeacon.Controls.Add(this.txtAltitudeSensor);
            this.pnlBeacon.Dock = System.Windows.Forms.DockStyle.Top;
            this.pnlBeacon.Location = new System.Drawing.Point(0, 0);
            this.pnlBeacon.Margin = new System.Windows.Forms.Padding(2, 2, 2, 2);
            this.pnlBeacon.Name = "pnlBeacon";
            this.pnlBeacon.Size = new System.Drawing.Size(525, 91);
            this.pnlBeacon.TabIndex = 2;
            // 
            // lblBeaconSub
            // 
            this.lblBeaconSub.AutoSize = true;
            this.lblBeaconSub.Location = new System.Drawing.Point(44, 5);
            this.lblBeaconSub.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.lblBeaconSub.Name = "lblBeaconSub";
            this.lblBeaconSub.Size = new System.Drawing.Size(128, 13);
            this.lblBeaconSub.TabIndex = 0;
            this.lblBeaconSub.Text = "Condicao do observatorio";
            // 
            // pnlBeaconDot
            // 
            this.pnlBeaconDot.Location = new System.Drawing.Point(6, 20);
            this.pnlBeaconDot.Margin = new System.Windows.Forms.Padding(2, 2, 2, 2);
            this.pnlBeaconDot.Name = "pnlBeaconDot";
            this.pnlBeaconDot.Size = new System.Drawing.Size(32, 34);
            this.pnlBeaconDot.TabIndex = 1;
            // 
            // txtStatus
            // 
            this.txtStatus.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.txtStatus.Location = new System.Drawing.Point(44, 20);
            this.txtStatus.Margin = new System.Windows.Forms.Padding(2, 2, 2, 2);
            this.txtStatus.Name = "txtStatus";
            this.txtStatus.ReadOnly = true;
            this.txtStatus.Size = new System.Drawing.Size(430, 13);
            this.txtStatus.TabIndex = 2;
            this.txtStatus.TabStop = false;
            // 
            // lblBeaconCeuTitle
            // 
            this.lblBeaconCeuTitle.AutoSize = true;
            this.lblBeaconCeuTitle.Location = new System.Drawing.Point(6, 64);
            this.lblBeaconCeuTitle.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.lblBeaconCeuTitle.Name = "lblBeaconCeuTitle";
            this.lblBeaconCeuTitle.Size = new System.Drawing.Size(29, 13);
            this.lblBeaconCeuTitle.TabIndex = 3;
            this.lblBeaconCeuTitle.Text = "Ceu:";
            // 
            // lblBeaconCeu
            // 
            this.lblBeaconCeu.Location = new System.Drawing.Point(38, 59);
            this.lblBeaconCeu.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.lblBeaconCeu.Name = "lblBeaconCeu";
            this.lblBeaconCeu.Size = new System.Drawing.Size(142, 28);
            this.lblBeaconCeu.TabIndex = 4;
            this.lblBeaconCeu.Text = "---";
            this.lblBeaconCeu.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // lblBeaconChuvaTitle
            // 
            this.lblBeaconChuvaTitle.AutoSize = true;
            this.lblBeaconChuvaTitle.Location = new System.Drawing.Point(186, 64);
            this.lblBeaconChuvaTitle.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.lblBeaconChuvaTitle.Name = "lblBeaconChuvaTitle";
            this.lblBeaconChuvaTitle.Size = new System.Drawing.Size(41, 13);
            this.lblBeaconChuvaTitle.TabIndex = 5;
            this.lblBeaconChuvaTitle.Text = "Chuva:";
            // 
            // lblBeaconChuva
            // 
            this.lblBeaconChuva.Location = new System.Drawing.Point(225, 59);
            this.lblBeaconChuva.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.lblBeaconChuva.Name = "lblBeaconChuva";
            this.lblBeaconChuva.Size = new System.Drawing.Size(249, 28);
            this.lblBeaconChuva.TabIndex = 6;
            this.lblBeaconChuva.Text = "---";
            this.lblBeaconChuva.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
            // 
            // labelAltitudeSensor
            // 
            this.labelAltitudeSensor.Location = new System.Drawing.Point(452, 6);
            this.labelAltitudeSensor.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.labelAltitudeSensor.Name = "labelAltitudeSensor";
            this.labelAltitudeSensor.Size = new System.Drawing.Size(58, 12);
            this.labelAltitudeSensor.TabIndex = 7;
            this.labelAltitudeSensor.Text = "Altitude";
            this.labelAltitudeSensor.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // txtAltitudeSensor
            // 
            this.txtAltitudeSensor.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.txtAltitudeSensor.Location = new System.Drawing.Point(465, 29);
            this.txtAltitudeSensor.Margin = new System.Windows.Forms.Padding(2, 2, 2, 2);
            this.txtAltitudeSensor.Name = "txtAltitudeSensor";
            this.txtAltitudeSensor.ReadOnly = true;
            this.txtAltitudeSensor.Size = new System.Drawing.Size(45, 13);
            this.txtAltitudeSensor.TabIndex = 8;
            this.txtAltitudeSensor.TabStop = false;
            this.txtAltitudeSensor.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            // 
            // pnlBody
            // 
            this.pnlBody.Controls.Add(this.pnlRight);
            this.pnlBody.Controls.Add(this.pnlLeft);
            this.pnlBody.Dock = System.Windows.Forms.DockStyle.Fill;
            this.pnlBody.Location = new System.Drawing.Point(0, 91);
            this.pnlBody.Margin = new System.Windows.Forms.Padding(2, 2, 2, 2);
            this.pnlBody.Name = "pnlBody";
            this.pnlBody.Size = new System.Drawing.Size(525, 336);
            this.pnlBody.TabIndex = 0;
            // 
            // pnlRight
            // 
            this.pnlRight.Controls.Add(this.lblSectionParams);
            this.pnlRight.Controls.Add(this.labelLimiteIR);
            this.pnlRight.Controls.Add(this.txtLimiteIR);
            this.pnlRight.Controls.Add(this.labelTimeoutNublado);
            this.pnlRight.Controls.Add(this.txtTimeoutNublado);
            this.pnlRight.Controls.Add(this.labelDeltaP);
            this.pnlRight.Controls.Add(this.txtLimiteDeltaP);
            this.pnlRight.Controls.Add(this.labelLimiteUmidade);
            this.pnlRight.Controls.Add(this.txtLimiteUmidade);
            this.pnlRight.Controls.Add(this.labelLimiteChuva);
            this.pnlRight.Controls.Add(this.txtLimiteChuva);
            this.pnlRight.Controls.Add(this.labelAltitude);
            this.pnlRight.Controls.Add(this.txtAltitude);
            this.pnlRight.Controls.Add(this.btnSalvar);
            this.pnlRight.Controls.Add(this.btnRedefinir);
            this.pnlRight.Controls.Add(this.btnOpcoes);
            this.pnlRight.Dock = System.Windows.Forms.DockStyle.Fill;
            this.pnlRight.Location = new System.Drawing.Point(231, 0);
            this.pnlRight.Margin = new System.Windows.Forms.Padding(2, 2, 2, 2);
            this.pnlRight.Name = "pnlRight";
            this.pnlRight.Size = new System.Drawing.Size(294, 336);
            this.pnlRight.TabIndex = 0;
            // 
            // lblSectionParams
            // 
            this.lblSectionParams.AutoSize = true;
            this.lblSectionParams.Location = new System.Drawing.Point(8, 8);
            this.lblSectionParams.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.lblSectionParams.Name = "lblSectionParams";
            this.lblSectionParams.Size = new System.Drawing.Size(154, 13);
            this.lblSectionParams.TabIndex = 0;
            this.lblSectionParams.Text = "PARAMETROS CALIBRAVEIS";
            // 
            // labelLimiteIR
            // 
            this.labelLimiteIR.AutoSize = true;
            this.labelLimiteIR.Location = new System.Drawing.Point(8, 35);
            this.labelLimiteIR.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.labelLimiteIR.Name = "labelLimiteIR";
            this.labelLimiteIR.Size = new System.Drawing.Size(92, 13);
            this.labelLimiteIR.TabIndex = 1;
            this.labelLimiteIR.Text = "Limite IR Ceu (oC)";
            this.labelLimiteIR.Click += new System.EventHandler(this.label14_Click);
            // 
            // txtLimiteIR
            // 
            this.txtLimiteIR.Location = new System.Drawing.Point(165, 32);
            this.txtLimiteIR.Margin = new System.Windows.Forms.Padding(2, 2, 2, 2);
            this.txtLimiteIR.Name = "txtLimiteIR";
            this.txtLimiteIR.Size = new System.Drawing.Size(106, 20);
            this.txtLimiteIR.TabIndex = 0;
            // 
            // labelTimeoutNublado
            // 
            this.labelTimeoutNublado.AutoSize = true;
            this.labelTimeoutNublado.Location = new System.Drawing.Point(8, 67);
            this.labelTimeoutNublado.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.labelTimeoutNublado.Name = "labelTimeoutNublado";
            this.labelTimeoutNublado.Size = new System.Drawing.Size(113, 13);
            this.labelTimeoutNublado.TabIndex = 2;
            this.labelTimeoutNublado.Text = "Timeout Nublado (min)";
            this.labelTimeoutNublado.Click += new System.EventHandler(this.label2_Click);
            // 
            // txtTimeoutNublado
            // 
            this.txtTimeoutNublado.Location = new System.Drawing.Point(165, 64);
            this.txtTimeoutNublado.Margin = new System.Windows.Forms.Padding(2, 2, 2, 2);
            this.txtTimeoutNublado.Name = "txtTimeoutNublado";
            this.txtTimeoutNublado.Size = new System.Drawing.Size(106, 20);
            this.txtTimeoutNublado.TabIndex = 1;
            // 
            // labelDeltaP
            // 
            this.labelDeltaP.AutoSize = true;
            this.labelDeltaP.Location = new System.Drawing.Point(8, 100);
            this.labelDeltaP.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.labelDeltaP.Name = "labelDeltaP";
            this.labelDeltaP.Size = new System.Drawing.Size(100, 13);
            this.labelDeltaP.TabIndex = 3;
            this.labelDeltaP.Text = "Limite Delta P (hPa)";
            this.labelDeltaP.Click += new System.EventHandler(this.label15_Click);
            // 
            // txtLimiteDeltaP
            // 
            this.txtLimiteDeltaP.Location = new System.Drawing.Point(165, 97);
            this.txtLimiteDeltaP.Margin = new System.Windows.Forms.Padding(2, 2, 2, 2);
            this.txtLimiteDeltaP.Name = "txtLimiteDeltaP";
            this.txtLimiteDeltaP.Size = new System.Drawing.Size(106, 20);
            this.txtLimiteDeltaP.TabIndex = 2;
            // 
            // labelLimiteUmidade
            // 
            this.labelLimiteUmidade.AutoSize = true;
            this.labelLimiteUmidade.Location = new System.Drawing.Point(8, 132);
            this.labelLimiteUmidade.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.labelLimiteUmidade.Name = "labelLimiteUmidade";
            this.labelLimiteUmidade.Size = new System.Drawing.Size(96, 13);
            this.labelLimiteUmidade.TabIndex = 4;
            this.labelLimiteUmidade.Text = "Limite Umidade (%)";
            this.labelLimiteUmidade.Click += new System.EventHandler(this.label16_Click);
            // 
            // txtLimiteUmidade
            // 
            this.txtLimiteUmidade.Location = new System.Drawing.Point(165, 129);
            this.txtLimiteUmidade.Margin = new System.Windows.Forms.Padding(2, 2, 2, 2);
            this.txtLimiteUmidade.Name = "txtLimiteUmidade";
            this.txtLimiteUmidade.Size = new System.Drawing.Size(106, 20);
            this.txtLimiteUmidade.TabIndex = 3;
            // 
            // labelLimiteChuva
            // 
            this.labelLimiteChuva.AutoSize = true;
            this.labelLimiteChuva.Location = new System.Drawing.Point(8, 165);
            this.labelLimiteChuva.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.labelLimiteChuva.Name = "labelLimiteChuva";
            this.labelLimiteChuva.Size = new System.Drawing.Size(110, 13);
            this.labelLimiteChuva.TabIndex = 5;
            this.labelLimiteChuva.Text = "Limite Chuva (0-1023)";
            this.labelLimiteChuva.Click += new System.EventHandler(this.label17_Click);
            // 
            // txtLimiteChuva
            // 
            this.txtLimiteChuva.Location = new System.Drawing.Point(165, 162);
            this.txtLimiteChuva.Margin = new System.Windows.Forms.Padding(2, 2, 2, 2);
            this.txtLimiteChuva.Name = "txtLimiteChuva";
            this.txtLimiteChuva.Size = new System.Drawing.Size(106, 20);
            this.txtLimiteChuva.TabIndex = 4;
            // 
            // labelAltitude
            // 
            this.labelAltitude.AutoSize = true;
            this.labelAltitude.Location = new System.Drawing.Point(8, 197);
            this.labelAltitude.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.labelAltitude.Name = "labelAltitude";
            this.labelAltitude.Size = new System.Drawing.Size(59, 13);
            this.labelAltitude.TabIndex = 6;
            this.labelAltitude.Text = "Altitude (m)";
            // 
            // txtAltitude
            // 
            this.txtAltitude.Location = new System.Drawing.Point(165, 194);
            this.txtAltitude.Margin = new System.Windows.Forms.Padding(2, 2, 2, 2);
            this.txtAltitude.Name = "txtAltitude";
            this.txtAltitude.Size = new System.Drawing.Size(106, 20);
            this.txtAltitude.TabIndex = 5;
            this.txtAltitude.TextChanged += new System.EventHandler(this.txtAltitude_TextChanged);
            // 
            // btnSalvar
            // 
            this.btnSalvar.Location = new System.Drawing.Point(8, 229);
            this.btnSalvar.Margin = new System.Windows.Forms.Padding(2, 2, 2, 2);
            this.btnSalvar.Name = "btnSalvar";
            this.btnSalvar.Size = new System.Drawing.Size(64, 23);
            this.btnSalvar.TabIndex = 6;
            this.btnSalvar.Text = "Salvar";
            // 
            // btnRedefinir
            // 
            this.btnRedefinir.Location = new System.Drawing.Point(76, 229);
            this.btnRedefinir.Margin = new System.Windows.Forms.Padding(2, 2, 2, 2);
            this.btnRedefinir.Name = "btnRedefinir";
            this.btnRedefinir.Size = new System.Drawing.Size(68, 23);
            this.btnRedefinir.TabIndex = 7;
            this.btnRedefinir.Text = "Redefinir";
            this.btnRedefinir.Click += new System.EventHandler(this.btnRedefinir_Click);
            // 
            // btnOpcoes
            // 
            this.btnOpcoes.Location = new System.Drawing.Point(148, 229);
            this.btnOpcoes.Margin = new System.Windows.Forms.Padding(2, 2, 2, 2);
            this.btnOpcoes.Name = "btnOpcoes";
            this.btnOpcoes.Size = new System.Drawing.Size(98, 23);
            this.btnOpcoes.TabIndex = 8;
            this.btnOpcoes.Text = "Opcoes Wi-Fi";
            // 
            // pnlLeft
            // 
            this.pnlLeft.Controls.Add(this.lblSectionSensores);
            this.pnlLeft.Controls.Add(this.lblIconTemp);
            this.pnlLeft.Controls.Add(this.labelTemperatura);
            this.pnlLeft.Controls.Add(this.txtTemperatura);
            this.pnlLeft.Controls.Add(this.lblIconHum);
            this.pnlLeft.Controls.Add(this.labelUmidadeRelativa);
            this.pnlLeft.Controls.Add(this.txtUmidade);
            this.pnlLeft.Controls.Add(this.lblIconDew);
            this.pnlLeft.Controls.Add(this.labelPontoOrvalho);
            this.pnlLeft.Controls.Add(this.txtOrvalho);
            this.pnlLeft.Controls.Add(this.lblIconPN);
            this.pnlLeft.Controls.Add(this.labelPressaoNominal);
            this.pnlLeft.Controls.Add(this.txtPressaoNom);
            this.pnlLeft.Controls.Add(this.lblIconPress);
            this.pnlLeft.Controls.Add(this.labelPressao);
            this.pnlLeft.Controls.Add(this.txtPressao);
            this.pnlLeft.Controls.Add(this.lblIconSP);
            this.pnlLeft.Controls.Add(this.labelStatusPressao);
            this.pnlLeft.Controls.Add(this.txtStatusPressao);
            this.pnlLeft.Controls.Add(this.lblIconIR);
            this.pnlLeft.Controls.Add(this.labelTIR);
            this.pnlLeft.Controls.Add(this.txtTIR);
            this.pnlLeft.Controls.Add(this.lblIconCeu);
            this.pnlLeft.Controls.Add(this.labelCondicaoCeu);
            this.pnlLeft.Controls.Add(this.txtEstadoCeu);
            this.pnlLeft.Controls.Add(this.lblIconChuva);
            this.pnlLeft.Controls.Add(this.labelCondicaoNuvens);
            this.pnlLeft.Controls.Add(this.txtEstadoChuva);
            this.pnlLeft.Dock = System.Windows.Forms.DockStyle.Left;
            this.pnlLeft.Location = new System.Drawing.Point(0, 0);
            this.pnlLeft.Margin = new System.Windows.Forms.Padding(2, 2, 2, 2);
            this.pnlLeft.Name = "pnlLeft";
            this.pnlLeft.Size = new System.Drawing.Size(231, 336);
            this.pnlLeft.TabIndex = 1;
            // 
            // lblSectionSensores
            // 
            this.lblSectionSensores.AutoSize = true;
            this.lblSectionSensores.Location = new System.Drawing.Point(8, 8);
            this.lblSectionSensores.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.lblSectionSensores.Name = "lblSectionSensores";
            this.lblSectionSensores.Size = new System.Drawing.Size(148, 13);
            this.lblSectionSensores.TabIndex = 0;
            this.lblSectionSensores.Text = "LEITURAS DOS SENSORES";
            // 
            // lblIconTemp
            // 
            this.lblIconTemp.Location = new System.Drawing.Point(8, 36);
            this.lblIconTemp.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.lblIconTemp.Name = "lblIconTemp";
            this.lblIconTemp.Size = new System.Drawing.Size(18, 20);
            this.lblIconTemp.TabIndex = 1;
            this.lblIconTemp.Text = "oC";
            this.lblIconTemp.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // labelTemperatura
            // 
            this.labelTemperatura.AutoSize = true;
            this.labelTemperatura.Location = new System.Drawing.Point(32, 40);
            this.labelTemperatura.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.labelTemperatura.Name = "labelTemperatura";
            this.labelTemperatura.Size = new System.Drawing.Size(67, 13);
            this.labelTemperatura.TabIndex = 2;
            this.labelTemperatura.Text = "Temperatura";
            this.labelTemperatura.Click += new System.EventHandler(this.label1_Click);
            // 
            // txtTemperatura
            // 
            this.txtTemperatura.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.txtTemperatura.Location = new System.Drawing.Point(134, 37);
            this.txtTemperatura.Margin = new System.Windows.Forms.Padding(2, 2, 2, 2);
            this.txtTemperatura.Name = "txtTemperatura";
            this.txtTemperatura.ReadOnly = true;
            this.txtTemperatura.Size = new System.Drawing.Size(90, 13);
            this.txtTemperatura.TabIndex = 3;
            this.txtTemperatura.TabStop = false;
            this.txtTemperatura.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            // 
            // lblIconHum
            // 
            this.lblIconHum.Location = new System.Drawing.Point(8, 68);
            this.lblIconHum.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.lblIconHum.Name = "lblIconHum";
            this.lblIconHum.Size = new System.Drawing.Size(18, 20);
            this.lblIconHum.TabIndex = 4;
            this.lblIconHum.Text = "%";
            this.lblIconHum.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // labelUmidadeRelativa
            // 
            this.labelUmidadeRelativa.AutoSize = true;
            this.labelUmidadeRelativa.Location = new System.Drawing.Point(32, 72);
            this.labelUmidadeRelativa.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.labelUmidadeRelativa.Name = "labelUmidadeRelativa";
            this.labelUmidadeRelativa.Size = new System.Drawing.Size(86, 13);
            this.labelUmidadeRelativa.TabIndex = 5;
            this.labelUmidadeRelativa.Text = "Umidade relativa";
            this.labelUmidadeRelativa.Click += new System.EventHandler(this.label1_Click);
            // 
            // txtUmidade
            // 
            this.txtUmidade.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.txtUmidade.Location = new System.Drawing.Point(134, 69);
            this.txtUmidade.Margin = new System.Windows.Forms.Padding(2, 2, 2, 2);
            this.txtUmidade.Name = "txtUmidade";
            this.txtUmidade.ReadOnly = true;
            this.txtUmidade.Size = new System.Drawing.Size(90, 13);
            this.txtUmidade.TabIndex = 6;
            this.txtUmidade.TabStop = false;
            this.txtUmidade.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            // 
            // lblIconDew
            // 
            this.lblIconDew.Location = new System.Drawing.Point(8, 101);
            this.lblIconDew.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.lblIconDew.Name = "lblIconDew";
            this.lblIconDew.Size = new System.Drawing.Size(18, 20);
            this.lblIconDew.TabIndex = 7;
            this.lblIconDew.Text = "DP";
            this.lblIconDew.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // labelPontoOrvalho
            // 
            this.labelPontoOrvalho.AutoSize = true;
            this.labelPontoOrvalho.Location = new System.Drawing.Point(32, 105);
            this.labelPontoOrvalho.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.labelPontoOrvalho.Name = "labelPontoOrvalho";
            this.labelPontoOrvalho.Size = new System.Drawing.Size(88, 13);
            this.labelPontoOrvalho.TabIndex = 8;
            this.labelPontoOrvalho.Text = "Ponto de orvalho";
            this.labelPontoOrvalho.Click += new System.EventHandler(this.label1_Click);
            // 
            // txtOrvalho
            // 
            this.txtOrvalho.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.txtOrvalho.Location = new System.Drawing.Point(134, 102);
            this.txtOrvalho.Margin = new System.Windows.Forms.Padding(2, 2, 2, 2);
            this.txtOrvalho.Name = "txtOrvalho";
            this.txtOrvalho.ReadOnly = true;
            this.txtOrvalho.Size = new System.Drawing.Size(90, 13);
            this.txtOrvalho.TabIndex = 9;
            this.txtOrvalho.TabStop = false;
            this.txtOrvalho.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            // 
            // lblIconPN
            // 
            this.lblIconPN.Location = new System.Drawing.Point(8, 133);
            this.lblIconPN.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.lblIconPN.Name = "lblIconPN";
            this.lblIconPN.Size = new System.Drawing.Size(18, 20);
            this.lblIconPN.TabIndex = 10;
            this.lblIconPN.Text = "PN";
            this.lblIconPN.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // labelPressaoNominal
            // 
            this.labelPressaoNominal.AutoSize = true;
            this.labelPressaoNominal.Location = new System.Drawing.Point(32, 137);
            this.labelPressaoNominal.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.labelPressaoNominal.Name = "labelPressaoNominal";
            this.labelPressaoNominal.Size = new System.Drawing.Size(84, 13);
            this.labelPressaoNominal.TabIndex = 11;
            this.labelPressaoNominal.Text = "Pressao nominal";
            this.labelPressaoNominal.Click += new System.EventHandler(this.label1_Click);
            // 
            // txtPressaoNom
            // 
            this.txtPressaoNom.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.txtPressaoNom.Location = new System.Drawing.Point(134, 134);
            this.txtPressaoNom.Margin = new System.Windows.Forms.Padding(2, 2, 2, 2);
            this.txtPressaoNom.Name = "txtPressaoNom";
            this.txtPressaoNom.ReadOnly = true;
            this.txtPressaoNom.Size = new System.Drawing.Size(90, 13);
            this.txtPressaoNom.TabIndex = 12;
            this.txtPressaoNom.TabStop = false;
            this.txtPressaoNom.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            // 
            // lblIconPress
            // 
            this.lblIconPress.Location = new System.Drawing.Point(8, 166);
            this.lblIconPress.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.lblIconPress.Name = "lblIconPress";
            this.lblIconPress.Size = new System.Drawing.Size(18, 20);
            this.lblIconPress.TabIndex = 13;
            this.lblIconPress.Text = "hPa";
            this.lblIconPress.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // labelPressao
            // 
            this.labelPressao.AutoSize = true;
            this.labelPressao.Location = new System.Drawing.Point(32, 170);
            this.labelPressao.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.labelPressao.Name = "labelPressao";
            this.labelPressao.Size = new System.Drawing.Size(71, 13);
            this.labelPressao.TabIndex = 14;
            this.labelPressao.Text = "Pressao atual";
            this.labelPressao.Click += new System.EventHandler(this.label1_Click);
            // 
            // txtPressao
            // 
            this.txtPressao.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.txtPressao.Location = new System.Drawing.Point(134, 167);
            this.txtPressao.Margin = new System.Windows.Forms.Padding(2, 2, 2, 2);
            this.txtPressao.Name = "txtPressao";
            this.txtPressao.ReadOnly = true;
            this.txtPressao.Size = new System.Drawing.Size(90, 13);
            this.txtPressao.TabIndex = 15;
            this.txtPressao.TabStop = false;
            this.txtPressao.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            // 
            // lblIconSP
            // 
            this.lblIconSP.Location = new System.Drawing.Point(8, 198);
            this.lblIconSP.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.lblIconSP.Name = "lblIconSP";
            this.lblIconSP.Size = new System.Drawing.Size(18, 20);
            this.lblIconSP.TabIndex = 16;
            this.lblIconSP.Text = "dP";
            this.lblIconSP.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // labelStatusPressao
            // 
            this.labelStatusPressao.AutoSize = true;
            this.labelStatusPressao.Location = new System.Drawing.Point(32, 202);
            this.labelStatusPressao.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.labelStatusPressao.Name = "labelStatusPressao";
            this.labelStatusPressao.Size = new System.Drawing.Size(92, 13);
            this.labelStatusPressao.TabIndex = 17;
            this.labelStatusPressao.Text = "Status da pressao";
            this.labelStatusPressao.Click += new System.EventHandler(this.label1_Click);
            // 
            // txtStatusPressao
            // 
            this.txtStatusPressao.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.txtStatusPressao.Location = new System.Drawing.Point(134, 199);
            this.txtStatusPressao.Margin = new System.Windows.Forms.Padding(2, 2, 2, 2);
            this.txtStatusPressao.Name = "txtStatusPressao";
            this.txtStatusPressao.ReadOnly = true;
            this.txtStatusPressao.Size = new System.Drawing.Size(90, 13);
            this.txtStatusPressao.TabIndex = 18;
            this.txtStatusPressao.TabStop = false;
            this.txtStatusPressao.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            // 
            // lblIconIR
            // 
            this.lblIconIR.Location = new System.Drawing.Point(8, 231);
            this.lblIconIR.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.lblIconIR.Name = "lblIconIR";
            this.lblIconIR.Size = new System.Drawing.Size(18, 20);
            this.lblIconIR.TabIndex = 19;
            this.lblIconIR.Text = "IR";
            this.lblIconIR.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // labelTIR
            // 
            this.labelTIR.AutoSize = true;
            this.labelTIR.Location = new System.Drawing.Point(32, 235);
            this.labelTIR.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.labelTIR.Name = "labelTIR";
            this.labelTIR.Size = new System.Drawing.Size(83, 13);
            this.labelTIR.TabIndex = 20;
            this.labelTIR.Text = "T. fundo do ceu";
            this.labelTIR.Click += new System.EventHandler(this.label1_Click);
            // 
            // txtTIR
            // 
            this.txtTIR.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.txtTIR.Location = new System.Drawing.Point(134, 232);
            this.txtTIR.Margin = new System.Windows.Forms.Padding(2, 2, 2, 2);
            this.txtTIR.Name = "txtTIR";
            this.txtTIR.ReadOnly = true;
            this.txtTIR.Size = new System.Drawing.Size(90, 13);
            this.txtTIR.TabIndex = 21;
            this.txtTIR.TabStop = false;
            this.txtTIR.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            // 
            // lblIconCeu
            // 
            this.lblIconCeu.Location = new System.Drawing.Point(0, 0);
            this.lblIconCeu.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.lblIconCeu.Name = "lblIconCeu";
            this.lblIconCeu.Size = new System.Drawing.Size(1, 1);
            this.lblIconCeu.TabIndex = 22;
            this.lblIconCeu.Visible = false;
            // 
            // labelCondicaoCeu
            // 
            this.labelCondicaoCeu.Location = new System.Drawing.Point(0, 0);
            this.labelCondicaoCeu.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.labelCondicaoCeu.Name = "labelCondicaoCeu";
            this.labelCondicaoCeu.Size = new System.Drawing.Size(75, 19);
            this.labelCondicaoCeu.TabIndex = 23;
            this.labelCondicaoCeu.Visible = false;
            // 
            // txtEstadoCeu
            // 
            this.txtEstadoCeu.Location = new System.Drawing.Point(0, 0);
            this.txtEstadoCeu.Margin = new System.Windows.Forms.Padding(2, 2, 2, 2);
            this.txtEstadoCeu.Name = "txtEstadoCeu";
            this.txtEstadoCeu.Size = new System.Drawing.Size(2, 20);
            this.txtEstadoCeu.TabIndex = 24;
            this.txtEstadoCeu.Visible = false;
            // 
            // lblIconChuva
            // 
            this.lblIconChuva.Location = new System.Drawing.Point(0, 0);
            this.lblIconChuva.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.lblIconChuva.Name = "lblIconChuva";
            this.lblIconChuva.Size = new System.Drawing.Size(1, 1);
            this.lblIconChuva.TabIndex = 25;
            this.lblIconChuva.Visible = false;
            // 
            // labelCondicaoNuvens
            // 
            this.labelCondicaoNuvens.Location = new System.Drawing.Point(0, 0);
            this.labelCondicaoNuvens.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.labelCondicaoNuvens.Name = "labelCondicaoNuvens";
            this.labelCondicaoNuvens.Size = new System.Drawing.Size(75, 19);
            this.labelCondicaoNuvens.TabIndex = 26;
            this.labelCondicaoNuvens.Visible = false;
            // 
            // txtEstadoChuva
            // 
            this.txtEstadoChuva.Location = new System.Drawing.Point(0, 0);
            this.txtEstadoChuva.Margin = new System.Windows.Forms.Padding(2, 2, 2, 2);
            this.txtEstadoChuva.Name = "txtEstadoChuva";
            this.txtEstadoChuva.Size = new System.Drawing.Size(2, 20);
            this.txtEstadoChuva.TabIndex = 27;
            this.txtEstadoChuva.Visible = false;
            // 
            // pnlFooter
            // 
            this.pnlFooter.Controls.Add(this.labelTrendPressao);
            this.pnlFooter.Controls.Add(this.txtTrendPressao);
            this.pnlFooter.Controls.Add(this.labelTrendUmidade);
            this.pnlFooter.Controls.Add(this.txtTrendUmidade);
            this.pnlFooter.Controls.Add(this.labelTrendCeu);
            this.pnlFooter.Controls.Add(this.txtTrendCeu);
            this.pnlFooter.Controls.Add(this.lblConnDot);
            this.pnlFooter.Controls.Add(this.lblConnText);
            this.pnlFooter.Dock = System.Windows.Forms.DockStyle.Bottom;
            this.pnlFooter.Location = new System.Drawing.Point(0, 427);
            this.pnlFooter.Margin = new System.Windows.Forms.Padding(2, 2, 2, 2);
            this.pnlFooter.Name = "pnlFooter";
            this.pnlFooter.Size = new System.Drawing.Size(525, 28);
            this.pnlFooter.TabIndex = 1;
            // 
            // labelTrendPressao
            // 
            this.labelTrendPressao.AutoSize = true;
            this.labelTrendPressao.Location = new System.Drawing.Point(8, 8);
            this.labelTrendPressao.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.labelTrendPressao.Name = "labelTrendPressao";
            this.labelTrendPressao.Size = new System.Drawing.Size(48, 13);
            this.labelTrendPressao.TabIndex = 0;
            this.labelTrendPressao.Text = "Pressao:";
            // 
            // txtTrendPressao
            // 
            this.txtTrendPressao.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.txtTrendPressao.Location = new System.Drawing.Point(56, 8);
            this.txtTrendPressao.Margin = new System.Windows.Forms.Padding(2, 2, 2, 2);
            this.txtTrendPressao.Name = "txtTrendPressao";
            this.txtTrendPressao.ReadOnly = true;
            this.txtTrendPressao.Size = new System.Drawing.Size(57, 13);
            this.txtTrendPressao.TabIndex = 1;
            this.txtTrendPressao.TabStop = false;
            this.txtTrendPressao.TextChanged += new System.EventHandler(this.txtTrendPressao_TextChanged);
            // 
            // labelTrendUmidade
            // 
            this.labelTrendUmidade.AutoSize = true;
            this.labelTrendUmidade.Location = new System.Drawing.Point(120, 8);
            this.labelTrendUmidade.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.labelTrendUmidade.Name = "labelTrendUmidade";
            this.labelTrendUmidade.Size = new System.Drawing.Size(52, 13);
            this.labelTrendUmidade.TabIndex = 2;
            this.labelTrendUmidade.Text = "Umidade:";
            // 
            // txtTrendUmidade
            // 
            this.txtTrendUmidade.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.txtTrendUmidade.Location = new System.Drawing.Point(171, 8);
            this.txtTrendUmidade.Margin = new System.Windows.Forms.Padding(2, 2, 2, 2);
            this.txtTrendUmidade.Name = "txtTrendUmidade";
            this.txtTrendUmidade.ReadOnly = true;
            this.txtTrendUmidade.Size = new System.Drawing.Size(57, 13);
            this.txtTrendUmidade.TabIndex = 3;
            this.txtTrendUmidade.TabStop = false;
            // 
            // labelTrendCeu
            // 
            this.labelTrendCeu.AutoSize = true;
            this.labelTrendCeu.Location = new System.Drawing.Point(236, 8);
            this.labelTrendCeu.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.labelTrendCeu.Name = "labelTrendCeu";
            this.labelTrendCeu.Size = new System.Drawing.Size(78, 13);
            this.labelTrendCeu.TabIndex = 4;
            this.labelTrendCeu.Text = "Tend. Nuvens:";
            // 
            // txtTrendCeu
            // 
            this.txtTrendCeu.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.txtTrendCeu.Location = new System.Drawing.Point(310, 8);
            this.txtTrendCeu.Margin = new System.Windows.Forms.Padding(2, 2, 2, 2);
            this.txtTrendCeu.Name = "txtTrendCeu";
            this.txtTrendCeu.ReadOnly = true;
            this.txtTrendCeu.Size = new System.Drawing.Size(57, 13);
            this.txtTrendCeu.TabIndex = 5;
            this.txtTrendCeu.TabStop = false;
            // 
            // lblConnDot
            // 
            this.lblConnDot.Location = new System.Drawing.Point(381, 10);
            this.lblConnDot.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.lblConnDot.Name = "lblConnDot";
            this.lblConnDot.Size = new System.Drawing.Size(8, 8);
            this.lblConnDot.TabIndex = 6;
            // 
            // lblConnText
            // 
            this.lblConnText.AutoSize = true;
            this.lblConnText.Location = new System.Drawing.Point(392, 8);
            this.lblConnText.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.lblConnText.Name = "lblConnText";
            this.lblConnText.Size = new System.Drawing.Size(59, 13);
            this.lblConnText.TabIndex = 7;
            this.lblConnText.Text = "Conectado";
            // 
            // labelSeguranca
            // 
            this.labelSeguranca.Location = new System.Drawing.Point(0, 0);
            this.labelSeguranca.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.labelSeguranca.Name = "labelSeguranca";
            this.labelSeguranca.Size = new System.Drawing.Size(75, 19);
            this.labelSeguranca.TabIndex = 5;
            this.labelSeguranca.Visible = false;
            // 
            // groupBoxCalibraveis
            // 
            this.groupBoxCalibraveis.Location = new System.Drawing.Point(0, 0);
            this.groupBoxCalibraveis.Margin = new System.Windows.Forms.Padding(2, 2, 2, 2);
            this.groupBoxCalibraveis.Name = "groupBoxCalibraveis";
            this.groupBoxCalibraveis.Padding = new System.Windows.Forms.Padding(2, 2, 2, 2);
            this.groupBoxCalibraveis.Size = new System.Drawing.Size(1, 1);
            this.groupBoxCalibraveis.TabIndex = 3;
            this.groupBoxCalibraveis.TabStop = false;
            this.groupBoxCalibraveis.Visible = false;
            this.groupBoxCalibraveis.Enter += new System.EventHandler(this.groupBoxCalibraveis_Enter);
            // 
            // groupBoxDados
            // 
            this.groupBoxDados.Location = new System.Drawing.Point(0, 0);
            this.groupBoxDados.Margin = new System.Windows.Forms.Padding(2, 2, 2, 2);
            this.groupBoxDados.Name = "groupBoxDados";
            this.groupBoxDados.Padding = new System.Windows.Forms.Padding(2, 2, 2, 2);
            this.groupBoxDados.Size = new System.Drawing.Size(1, 1);
            this.groupBoxDados.TabIndex = 4;
            this.groupBoxDados.TabStop = false;
            this.groupBoxDados.Visible = false;
            // 
            // MonitorForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(525, 455);
            this.Controls.Add(this.pnlBody);
            this.Controls.Add(this.pnlFooter);
            this.Controls.Add(this.pnlBeacon);
            this.Controls.Add(this.groupBoxCalibraveis);
            this.Controls.Add(this.groupBoxDados);
            this.Controls.Add(this.labelSeguranca);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.Margin = new System.Windows.Forms.Padding(2, 2, 2, 2);
            this.MaximizeBox = false;
            this.Name = "MonitorForm";
            this.Text = "Ziller Safety Monitor";
            this.Load += new System.EventHandler(this.MonitorForm_Load);
            this.pnlBeacon.ResumeLayout(false);
            this.pnlBeacon.PerformLayout();
            this.pnlBody.ResumeLayout(false);
            this.pnlRight.ResumeLayout(false);
            this.pnlRight.PerformLayout();
            this.pnlLeft.ResumeLayout(false);
            this.pnlLeft.PerformLayout();
            this.pnlFooter.ResumeLayout(false);
            this.pnlFooter.PerformLayout();
            this.ResumeLayout(false);

        }

        private System.Windows.Forms.Panel    pnlBeacon, pnlBeaconDot, pnlBody, pnlLeft, pnlRight, pnlFooter;
        private System.Windows.Forms.TextBox  txtStatus, txtAltitudeSensor;
        private System.Windows.Forms.Label    lblBeaconSub, lblBeaconCeuTitle, lblBeaconCeu;
        private System.Windows.Forms.Label    lblBeaconChuvaTitle, lblBeaconChuva, labelAltitudeSensor;
        private System.Windows.Forms.Label    lblSectionSensores, lblSectionParams, lblConnDot, lblConnText;
        private System.Windows.Forms.Label    lblIconTemp, lblIconHum, lblIconDew, lblIconPN;
        private System.Windows.Forms.Label    lblIconPress, lblIconSP, lblIconIR, lblIconCeu, lblIconChuva;
        private System.Windows.Forms.Label    labelTemperatura, labelUmidadeRelativa, labelPontoOrvalho;
        private System.Windows.Forms.Label    labelPressaoNominal, labelPressao, labelStatusPressao, labelTIR;
        private System.Windows.Forms.Label    labelCondicaoCeu, labelCondicaoNuvens;
        private System.Windows.Forms.TextBox  txtTemperatura, txtUmidade, txtOrvalho;
        private System.Windows.Forms.TextBox  txtPressaoNom, txtPressao, txtStatusPressao, txtTIR;
        private System.Windows.Forms.TextBox  txtEstadoCeu, txtEstadoChuva;
        private System.Windows.Forms.Label    labelTrendPressao, labelTrendUmidade, labelTrendCeu;
        private System.Windows.Forms.TextBox  txtTrendPressao, txtTrendUmidade, txtTrendCeu;
        private System.Windows.Forms.Label    labelLimiteIR, labelTimeoutNublado, labelDeltaP;
        private System.Windows.Forms.Label    labelLimiteUmidade, labelLimiteChuva, labelAltitude;
        private System.Windows.Forms.TextBox  txtLimiteIR, txtTimeoutNublado, txtLimiteDeltaP;
        private System.Windows.Forms.TextBox  txtLimiteUmidade, txtLimiteChuva, txtAltitude;
        private System.Windows.Forms.Button   btnSalvar, btnRedefinir, btnOpcoes;
        private System.Windows.Forms.Label    labelSeguranca;
        private System.Windows.Forms.GroupBox groupBoxCalibraveis, groupBoxDados;
    }
}
