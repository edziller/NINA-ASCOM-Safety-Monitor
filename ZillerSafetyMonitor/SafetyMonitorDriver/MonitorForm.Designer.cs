using System;
using System.Drawing;
using System.Windows.Forms;

namespace ASCOM.EZZillerSafetyMonitor
{
    public partial class MonitorForm : Form
    {
        private System.Windows.Forms.Label labelAltitudeSensor;
        private System.Windows.Forms.TextBox txtAltitudeSensor;
        private GroupBox groupBoxCalibraveis;
        private GroupBox groupBoxDados;
        private Label labelTemperatura;
        private Label labelPontoOrvalho;
        private Label labelUmidadeRelativa;
        private Label labelCondicaoCeu;
        private Label labelTIR;
        private Label labelStatusPressao;
        private Label labelPressao;
        private Label labelPressaoNominal;
        private Label labelSeguranca;
        private Label labelCondicaoNuvens;
        private TextBox txtStatus;
        private TextBox txtEstadoChuva;
        private TextBox txtEstadoCeu;
        private TextBox txtTIR;
        private TextBox txtStatusPressao;
        private TextBox txtPressao;
        private TextBox txtPressaoNom;
        private TextBox txtOrvalho;
        private TextBox txtUmidade;
        private TextBox txtTemperatura;
        private TextBox txtLimiteIR;
        private Label labelLimiteIR;
        private TextBox txtAltitude;
        private Label labelAltitude;
        private TextBox txtLimiteDeltaP;
        private Label labelDeltaP;
        private TextBox txtLimiteUmidade;
        private Label labelLimiteUmidade;
        private TextBox txtLimiteChuva;
        private Label labelLimiteChuva;
        private Button btnSalvar;
        private Button btnOpcoes;
        private Button btnRedefinir;

        private void InitializeComponent()
        {
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(MonitorForm));
            this.groupBoxCalibraveis = new System.Windows.Forms.GroupBox();
            this.txtTimeoutNublado = new System.Windows.Forms.TextBox();
            this.labelTimeoutNublado = new System.Windows.Forms.Label();
            this.txtLimiteChuva = new System.Windows.Forms.TextBox();
            this.labelLimiteChuva = new System.Windows.Forms.Label();
            this.txtLimiteUmidade = new System.Windows.Forms.TextBox();
            this.labelLimiteUmidade = new System.Windows.Forms.Label();
            this.txtLimiteDeltaP = new System.Windows.Forms.TextBox();
            this.labelDeltaP = new System.Windows.Forms.Label();
            this.txtLimiteIR = new System.Windows.Forms.TextBox();
            this.labelLimiteIR = new System.Windows.Forms.Label();
            this.txtAltitude = new System.Windows.Forms.TextBox();
            this.labelAltitude = new System.Windows.Forms.Label();
            this.btnSalvar = new System.Windows.Forms.Button();
            this.btnRedefinir = new System.Windows.Forms.Button();
            this.groupBoxDados = new System.Windows.Forms.GroupBox();
            this.labelAltitudeSensor = new System.Windows.Forms.Label();
            this.txtAltitudeSensor = new System.Windows.Forms.TextBox();
            this.txtTrendCeu = new System.Windows.Forms.TextBox();
            this.txtTrendPressao = new System.Windows.Forms.TextBox();
            this.txtTrendUmidade = new System.Windows.Forms.TextBox();
            this.labelTrendCeu = new System.Windows.Forms.Label();
            this.labelTrendPressao = new System.Windows.Forms.Label();
            this.labelTrendUmidade = new System.Windows.Forms.Label();
            this.txtTIR = new System.Windows.Forms.TextBox();
            this.txtStatusPressao = new System.Windows.Forms.TextBox();
            this.txtPressao = new System.Windows.Forms.TextBox();
            this.txtPressaoNom = new System.Windows.Forms.TextBox();
            this.txtOrvalho = new System.Windows.Forms.TextBox();
            this.txtUmidade = new System.Windows.Forms.TextBox();
            this.txtTemperatura = new System.Windows.Forms.TextBox();
            this.labelTemperatura = new System.Windows.Forms.Label();
            this.labelTIR = new System.Windows.Forms.Label();
            this.labelStatusPressao = new System.Windows.Forms.Label();
            this.labelPressao = new System.Windows.Forms.Label();
            this.labelPressaoNominal = new System.Windows.Forms.Label();
            this.labelPontoOrvalho = new System.Windows.Forms.Label();
            this.labelUmidadeRelativa = new System.Windows.Forms.Label();
            this.txtStatus = new System.Windows.Forms.TextBox();
            this.btnOpcoes = new System.Windows.Forms.Button();
            this.txtEstadoChuva = new System.Windows.Forms.TextBox();
            this.txtEstadoCeu = new System.Windows.Forms.TextBox();
            this.labelSeguranca = new System.Windows.Forms.Label();
            this.labelCondicaoNuvens = new System.Windows.Forms.Label();
            this.labelCondicaoCeu = new System.Windows.Forms.Label();
            this.groupBoxCalibraveis.SuspendLayout();
            this.groupBoxDados.SuspendLayout();
            this.SuspendLayout();
            // 
            // groupBoxCalibraveis
            // 
            this.groupBoxCalibraveis.Controls.Add(this.txtTimeoutNublado);
            this.groupBoxCalibraveis.Controls.Add(this.labelTimeoutNublado);
            this.groupBoxCalibraveis.Controls.Add(this.txtLimiteChuva);
            this.groupBoxCalibraveis.Controls.Add(this.labelLimiteChuva);
            this.groupBoxCalibraveis.Controls.Add(this.txtLimiteUmidade);
            this.groupBoxCalibraveis.Controls.Add(this.labelLimiteUmidade);
            this.groupBoxCalibraveis.Controls.Add(this.txtLimiteDeltaP);
            this.groupBoxCalibraveis.Controls.Add(this.labelDeltaP);
            this.groupBoxCalibraveis.Controls.Add(this.txtLimiteIR);
            this.groupBoxCalibraveis.Controls.Add(this.labelLimiteIR);
            this.groupBoxCalibraveis.Controls.Add(this.txtAltitude);
            this.groupBoxCalibraveis.Controls.Add(this.labelAltitude);
            this.groupBoxCalibraveis.Controls.Add(this.btnSalvar);
            this.groupBoxCalibraveis.Controls.Add(this.btnRedefinir);
            this.groupBoxCalibraveis.Location = new System.Drawing.Point(280, 10);
            this.groupBoxCalibraveis.Name = "groupBoxCalibraveis";
            this.groupBoxCalibraveis.Size = new System.Drawing.Size(265, 346);
            this.groupBoxCalibraveis.TabIndex = 1;
            this.groupBoxCalibraveis.TabStop = false;
            this.groupBoxCalibraveis.Text = "Parâmetros Calibráveis";
            this.groupBoxCalibraveis.Enter += new System.EventHandler(this.groupBoxCalibraveis_Enter);
            // 
            // txtTimeoutNublado
            // 
            this.txtTimeoutNublado.Location = new System.Drawing.Point(151, 88);
            this.txtTimeoutNublado.Name = "txtTimeoutNublado";
            this.txtTimeoutNublado.Size = new System.Drawing.Size(100, 22);
            this.txtTimeoutNublado.TabIndex = 24;
            // 
            // labelTimeoutNublado
            // 
            this.labelTimeoutNublado.AutoSize = true;
            this.labelTimeoutNublado.Location = new System.Drawing.Point(3, 88);
            this.labelTimeoutNublado.Name = "labelTimeoutNublado";
            this.labelTimeoutNublado.Size = new System.Drawing.Size(143, 16);
            this.labelTimeoutNublado.TabIndex = 23;
            this.labelTimeoutNublado.Text = "Timeout Nublado (min)";
            this.labelTimeoutNublado.Click += new System.EventHandler(this.label2_Click);
            // 
            // txtLimiteChuva
            // 
            this.txtLimiteChuva.Location = new System.Drawing.Point(151, 173);
            this.txtLimiteChuva.Name = "txtLimiteChuva";
            this.txtLimiteChuva.Size = new System.Drawing.Size(100, 22);
            this.txtLimiteChuva.TabIndex = 20;
            // 
            // labelLimiteChuva
            // 
            this.labelLimiteChuva.AutoSize = true;
            this.labelLimiteChuva.Location = new System.Drawing.Point(6, 176);
            this.labelLimiteChuva.Name = "labelLimiteChuva";
            this.labelLimiteChuva.Size = new System.Drawing.Size(83, 16);
            this.labelLimiteChuva.TabIndex = 19;
            this.labelLimiteChuva.Text = "Limite Chuva";
            this.labelLimiteChuva.Click += new System.EventHandler(this.label17_Click);
            // 
            // txtLimiteUmidade
            // 
            this.txtLimiteUmidade.Location = new System.Drawing.Point(151, 144);
            this.txtLimiteUmidade.Name = "txtLimiteUmidade";
            this.txtLimiteUmidade.Size = new System.Drawing.Size(100, 22);
            this.txtLimiteUmidade.TabIndex = 18;
            // 
            // labelLimiteUmidade
            // 
            this.labelLimiteUmidade.AutoSize = true;
            this.labelLimiteUmidade.Location = new System.Drawing.Point(6, 149);
            this.labelLimiteUmidade.Name = "labelLimiteUmidade";
            this.labelLimiteUmidade.Size = new System.Drawing.Size(101, 16);
            this.labelLimiteUmidade.TabIndex = 17;
            this.labelLimiteUmidade.Text = "Limite Umidade";
            this.labelLimiteUmidade.Click += new System.EventHandler(this.label16_Click);
            // 
            // txtLimiteDeltaP
            // 
            this.txtLimiteDeltaP.Location = new System.Drawing.Point(151, 116);
            this.txtLimiteDeltaP.Name = "txtLimiteDeltaP";
            this.txtLimiteDeltaP.Size = new System.Drawing.Size(100, 22);
            this.txtLimiteDeltaP.TabIndex = 16;
            // 
            // labelDeltaP
            // 
            this.labelDeltaP.AutoSize = true;
            this.labelDeltaP.Location = new System.Drawing.Point(6, 117);
            this.labelDeltaP.Name = "labelDeltaP";
            this.labelDeltaP.Size = new System.Drawing.Size(89, 16);
            this.labelDeltaP.TabIndex = 15;
            this.labelDeltaP.Text = "Limite Delta P";
            this.labelDeltaP.Click += new System.EventHandler(this.label15_Click);
            // 
            // txtLimiteIR
            // 
            this.txtLimiteIR.Location = new System.Drawing.Point(151, 60);
            this.txtLimiteIR.Name = "txtLimiteIR";
            this.txtLimiteIR.Size = new System.Drawing.Size(100, 22);
            this.txtLimiteIR.TabIndex = 14;
            // 
            // labelLimiteIR
            // 
            this.labelLimiteIR.AutoSize = true;
            this.labelLimiteIR.Location = new System.Drawing.Point(6, 62);
            this.labelLimiteIR.Name = "labelLimiteIR";
            this.labelLimiteIR.Size = new System.Drawing.Size(107, 16);
            this.labelLimiteIR.TabIndex = 13;
            this.labelLimiteIR.Text = "Limite IR Ceu(ºC)";
            this.labelLimiteIR.Click += new System.EventHandler(this.label14_Click);
            // 
            // txtAltitude
            // 
            this.txtAltitude.Location = new System.Drawing.Point(151, 32);
            this.txtAltitude.Name = "txtAltitude";
            this.txtAltitude.Size = new System.Drawing.Size(100, 22);
            this.txtAltitude.TabIndex = 14;
            this.txtAltitude.TextChanged += new System.EventHandler(this.txtAltitude_TextChanged);
            // 
            // labelAltitude
            // 
            this.labelAltitude.AutoSize = true;
            this.labelAltitude.Location = new System.Drawing.Point(6, 35);
            this.labelAltitude.Name = "labelAltitude";
            this.labelAltitude.Size = new System.Drawing.Size(73, 16);
            this.labelAltitude.TabIndex = 13;
            this.labelAltitude.Text = "Altitude (m)";
            // 
            // btnSalvar
            // 
            this.btnSalvar.Location = new System.Drawing.Point(23, 298);
            this.btnSalvar.Name = "btnSalvar";
            this.btnSalvar.Size = new System.Drawing.Size(90, 30);
            this.btnSalvar.TabIndex = 2;
            this.btnSalvar.Text = "Salvar";
            // 
            // btnRedefinir
            // 
            this.btnRedefinir.Location = new System.Drawing.Point(137, 298);
            this.btnRedefinir.Name = "btnRedefinir";
            this.btnRedefinir.Size = new System.Drawing.Size(90, 30);
            this.btnRedefinir.TabIndex = 3;
            this.btnRedefinir.Text = "Redefinir";
            this.btnRedefinir.Click += new System.EventHandler(this.btnRedefinir_Click);
            // 
            // groupBoxDados
            // 
            this.groupBoxDados.Controls.Add(this.labelAltitudeSensor);
            this.groupBoxDados.Controls.Add(this.txtAltitudeSensor);
            this.groupBoxDados.Controls.Add(this.txtTrendCeu);
            this.groupBoxDados.Controls.Add(this.txtTrendPressao);
            this.groupBoxDados.Controls.Add(this.txtTrendUmidade);
            this.groupBoxDados.Controls.Add(this.labelTrendCeu);
            this.groupBoxDados.Controls.Add(this.labelTrendPressao);
            this.groupBoxDados.Controls.Add(this.labelTrendUmidade);
            this.groupBoxDados.Controls.Add(this.txtTIR);
            this.groupBoxDados.Controls.Add(this.txtStatusPressao);
            this.groupBoxDados.Controls.Add(this.txtPressao);
            this.groupBoxDados.Controls.Add(this.txtPressaoNom);
            this.groupBoxDados.Controls.Add(this.txtOrvalho);
            this.groupBoxDados.Controls.Add(this.txtUmidade);
            this.groupBoxDados.Controls.Add(this.txtTemperatura);
            this.groupBoxDados.Controls.Add(this.labelTemperatura);
            this.groupBoxDados.Controls.Add(this.labelTIR);
            this.groupBoxDados.Controls.Add(this.labelStatusPressao);
            this.groupBoxDados.Controls.Add(this.labelPressao);
            this.groupBoxDados.Controls.Add(this.labelPressaoNominal);
            this.groupBoxDados.Controls.Add(this.labelPontoOrvalho);
            this.groupBoxDados.Controls.Add(this.labelUmidadeRelativa);
            this.groupBoxDados.Location = new System.Drawing.Point(10, 10);
            this.groupBoxDados.Name = "groupBoxDados";
            this.groupBoxDados.Size = new System.Drawing.Size(260, 346);
            this.groupBoxDados.TabIndex = 0;
            this.groupBoxDados.TabStop = false;
            this.groupBoxDados.Text = "Dados do Sensor";
            // 
            // labelAltitudeSensor
            // 
            this.labelAltitudeSensor.AutoSize = true;
            this.labelAltitudeSensor.Location = new System.Drawing.Point(9, 30);
            this.labelAltitudeSensor.Name = "labelAltitudeSensor";
            this.labelAltitudeSensor.Size = new System.Drawing.Size(73, 16);
            this.labelAltitudeSensor.TabIndex = 0;
            this.labelAltitudeSensor.Text = "Altitude (m)";
            // 
            // txtAltitudeSensor
            // 
            this.txtAltitudeSensor.Location = new System.Drawing.Point(148, 25);
            this.txtAltitudeSensor.Name = "txtAltitudeSensor";
            this.txtAltitudeSensor.ReadOnly = true;
            this.txtAltitudeSensor.Size = new System.Drawing.Size(100, 22);
            this.txtAltitudeSensor.TabIndex = 1;
            // 
            // txtTrendCeu
            // 
            this.txtTrendCeu.Location = new System.Drawing.Point(148, 299);
            this.txtTrendCeu.Name = "txtTrendCeu";
            this.txtTrendCeu.Size = new System.Drawing.Size(100, 22);
            this.txtTrendCeu.TabIndex = 30;
            // 
            // txtTrendPressao
            // 
            this.txtTrendPressao.Location = new System.Drawing.Point(148, 243);
            this.txtTrendPressao.Name = "txtTrendPressao";
            this.txtTrendPressao.Size = new System.Drawing.Size(100, 22);
            this.txtTrendPressao.TabIndex = 29;
            this.txtTrendPressao.TextChanged += new System.EventHandler(this.txtTrendPressao_TextChanged);
            // 
            // txtTrendUmidade
            // 
            this.txtTrendUmidade.Location = new System.Drawing.Point(148, 105);
            this.txtTrendUmidade.Name = "txtTrendUmidade";
            this.txtTrendUmidade.Size = new System.Drawing.Size(100, 22);
            this.txtTrendUmidade.TabIndex = 28;
            // 
            // labelTrendCeu
            // 
            this.labelTrendCeu.AutoSize = true;
            this.labelTrendCeu.Location = new System.Drawing.Point(6, 301);
            this.labelTrendCeu.Name = "labelTrendCeu";
            this.labelTrendCeu.Size = new System.Drawing.Size(121, 16);
            this.labelTrendCeu.TabIndex = 27;
            this.labelTrendCeu.Text = "Tendencia Nuvens";
            // 
            // labelTrendPressao
            // 
            this.labelTrendPressao.AutoSize = true;
            this.labelTrendPressao.Location = new System.Drawing.Point(7, 246);
            this.labelTrendPressao.Name = "labelTrendPressao";
            this.labelTrendPressao.Size = new System.Drawing.Size(126, 16);
            this.labelTrendPressao.TabIndex = 26;
            this.labelTrendPressao.Text = "Tendencia Pressão";
            // 
            // labelTrendUmidade
            // 
            this.labelTrendUmidade.AutoSize = true;
            this.labelTrendUmidade.Location = new System.Drawing.Point(7, 108);
            this.labelTrendUmidade.Name = "labelTrendUmidade";
            this.labelTrendUmidade.Size = new System.Drawing.Size(131, 16);
            this.labelTrendUmidade.TabIndex = 25;
            this.labelTrendUmidade.Text = "Tendencia Umidade";
            // 
            // txtTIR
            // 
            this.txtTIR.Location = new System.Drawing.Point(148, 270);
            this.txtTIR.Name = "txtTIR";
            this.txtTIR.Size = new System.Drawing.Size(100, 22);
            this.txtTIR.TabIndex = 9;
            // 
            // txtStatusPressao
            // 
            this.txtStatusPressao.Location = new System.Drawing.Point(148, 215);
            this.txtStatusPressao.Name = "txtStatusPressao";
            this.txtStatusPressao.Size = new System.Drawing.Size(100, 22);
            this.txtStatusPressao.TabIndex = 8;
            // 
            // txtPressao
            // 
            this.txtPressao.Location = new System.Drawing.Point(148, 186);
            this.txtPressao.Name = "txtPressao";
            this.txtPressao.Size = new System.Drawing.Size(100, 22);
            this.txtPressao.TabIndex = 6;
            // 
            // txtPressaoNom
            // 
            this.txtPressaoNom.Location = new System.Drawing.Point(148, 160);
            this.txtPressaoNom.Name = "txtPressaoNom";
            this.txtPressaoNom.Size = new System.Drawing.Size(100, 22);
            this.txtPressaoNom.TabIndex = 5;
            // 
            // txtOrvalho
            // 
            this.txtOrvalho.Location = new System.Drawing.Point(148, 133);
            this.txtOrvalho.Name = "txtOrvalho";
            this.txtOrvalho.Size = new System.Drawing.Size(100, 22);
            this.txtOrvalho.TabIndex = 4;
            // 
            // txtUmidade
            // 
            this.txtUmidade.Location = new System.Drawing.Point(148, 78);
            this.txtUmidade.Name = "txtUmidade";
            this.txtUmidade.Size = new System.Drawing.Size(100, 22);
            this.txtUmidade.TabIndex = 3;
            // 
            // txtTemperatura
            // 
            this.txtTemperatura.Location = new System.Drawing.Point(148, 53);
            this.txtTemperatura.Name = "txtTemperatura";
            this.txtTemperatura.Size = new System.Drawing.Size(100, 22);
            this.txtTemperatura.TabIndex = 2;
            // 
            // labelTemperatura
            // 
            this.labelTemperatura.AutoSize = true;
            this.labelTemperatura.Location = new System.Drawing.Point(8, 56);
            this.labelTemperatura.Name = "labelTemperatura";
            this.labelTemperatura.Size = new System.Drawing.Size(109, 16);
            this.labelTemperatura.TabIndex = 0;
            this.labelTemperatura.Text = "Temperatura (°C)";
            this.labelTemperatura.Click += new System.EventHandler(this.label1_Click);
            // 
            // labelTIR
            // 
            this.labelTIR.AutoSize = true;
            this.labelTIR.Location = new System.Drawing.Point(7, 272);
            this.labelTIR.Name = "labelTIR";
            this.labelTIR.Size = new System.Drawing.Size(120, 16);
            this.labelTIR.TabIndex = 0;
            this.labelTIR.Text = "T fundo do céu (°C)";
            this.labelTIR.Click += new System.EventHandler(this.label1_Click);
            // 
            // labelStatusPressao
            // 
            this.labelStatusPressao.AutoSize = true;
            this.labelStatusPressao.Location = new System.Drawing.Point(7, 218);
            this.labelStatusPressao.Name = "labelStatusPressao";
            this.labelStatusPressao.Size = new System.Drawing.Size(98, 16);
            this.labelStatusPressao.TabIndex = 0;
            this.labelStatusPressao.Text = "Status Pressão";
            this.labelStatusPressao.Click += new System.EventHandler(this.label1_Click);
            // 
            // labelPressao
            // 
            this.labelPressao.AutoSize = true;
            this.labelPressao.Location = new System.Drawing.Point(7, 190);
            this.labelPressao.Name = "labelPressao";
            this.labelPressao.Size = new System.Drawing.Size(125, 16);
            this.labelPressao.TabIndex = 0;
            this.labelPressao.Text = "Pressão atual (hPa)";
            this.labelPressao.Click += new System.EventHandler(this.label1_Click);
            // 
            // labelPressaoNominal
            // 
            this.labelPressaoNominal.AutoSize = true;
            this.labelPressaoNominal.Location = new System.Drawing.Point(7, 163);
            this.labelPressaoNominal.Name = "labelPressaoNominal";
            this.labelPressaoNominal.Size = new System.Drawing.Size(125, 16);
            this.labelPressaoNominal.TabIndex = 0;
            this.labelPressaoNominal.Text = "Pressão Nom.(hPa)";
            this.labelPressaoNominal.Click += new System.EventHandler(this.label1_Click);
            // 
            // labelPontoOrvalho
            // 
            this.labelPontoOrvalho.AutoSize = true;
            this.labelPontoOrvalho.Location = new System.Drawing.Point(7, 136);
            this.labelPontoOrvalho.Name = "labelPontoOrvalho";
            this.labelPontoOrvalho.Size = new System.Drawing.Size(116, 16);
            this.labelPontoOrvalho.TabIndex = 0;
            this.labelPontoOrvalho.Text = "Ponto Orvalho (°C)";
            this.labelPontoOrvalho.Click += new System.EventHandler(this.label1_Click);
            // 
            // labelUmidadeRelativa
            // 
            this.labelUmidadeRelativa.AutoSize = true;
            this.labelUmidadeRelativa.Location = new System.Drawing.Point(7, 81);
            this.labelUmidadeRelativa.Name = "labelUmidadeRelativa";
            this.labelUmidadeRelativa.Size = new System.Drawing.Size(116, 16);
            this.labelUmidadeRelativa.TabIndex = 0;
            this.labelUmidadeRelativa.Text = "Umidade Relativa";
            this.labelUmidadeRelativa.Click += new System.EventHandler(this.label1_Click);
            // 
            // txtStatus
            // 
            this.txtStatus.Location = new System.Drawing.Point(140, 428);
            this.txtStatus.Name = "txtStatus";
            this.txtStatus.Size = new System.Drawing.Size(396, 22);
            this.txtStatus.TabIndex = 12;
            this.txtStatus.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            // 
            // btnOpcoes
            // 
            this.btnOpcoes.Location = new System.Drawing.Point(445, 477);
            this.btnOpcoes.Name = "btnOpcoes";
            this.btnOpcoes.Size = new System.Drawing.Size(90, 30);
            this.btnOpcoes.TabIndex = 5;
            this.btnOpcoes.Text = "Opções Wi-Fi";
            this.btnOpcoes.Click += new System.EventHandler(this.btnOpcoes_Click);
            // 
            // txtEstadoChuva
            // 
            this.txtEstadoChuva.Location = new System.Drawing.Point(140, 400);
            this.txtEstadoChuva.Name = "txtEstadoChuva";
            this.txtEstadoChuva.Size = new System.Drawing.Size(396, 22);
            this.txtEstadoChuva.TabIndex = 11;
            this.txtEstadoChuva.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            // 
            // txtEstadoCeu
            // 
            this.txtEstadoCeu.Location = new System.Drawing.Point(140, 373);
            this.txtEstadoCeu.Name = "txtEstadoCeu";
            this.txtEstadoCeu.Size = new System.Drawing.Size(396, 22);
            this.txtEstadoCeu.TabIndex = 10;
            this.txtEstadoCeu.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            // 
            // labelSeguranca
            // 
            this.labelSeguranca.AutoSize = true;
            this.labelSeguranca.Location = new System.Drawing.Point(54, 430);
            this.labelSeguranca.Name = "labelSeguranca";
            this.labelSeguranca.Size = new System.Drawing.Size(73, 16);
            this.labelSeguranca.TabIndex = 0;
            this.labelSeguranca.Text = "Segurança";
            this.labelSeguranca.Click += new System.EventHandler(this.label1_Click);
            // 
            // labelCondicaoNuvens
            // 
            this.labelCondicaoNuvens.AccessibleRole = System.Windows.Forms.AccessibleRole.OutlineButton;
            this.labelCondicaoNuvens.AutoSize = true;
            this.labelCondicaoNuvens.Location = new System.Drawing.Point(23, 403);
            this.labelCondicaoNuvens.Name = "labelCondicaoNuvens";
            this.labelCondicaoNuvens.Size = new System.Drawing.Size(106, 16);
            this.labelCondicaoNuvens.TabIndex = 0;
            this.labelCondicaoNuvens.Text = "Condição Chuva";
            this.labelCondicaoNuvens.Click += new System.EventHandler(this.label1_Click);
            // 
            // labelCondicaoCeu
            // 
            this.labelCondicaoCeu.AutoSize = true;
            this.labelCondicaoCeu.Location = new System.Drawing.Point(23, 373);
            this.labelCondicaoCeu.Name = "labelCondicaoCeu";
            this.labelCondicaoCeu.Size = new System.Drawing.Size(111, 16);
            this.labelCondicaoCeu.TabIndex = 0;
            this.labelCondicaoCeu.Text = "Condição do Céu";
            this.labelCondicaoCeu.Click += new System.EventHandler(this.label1_Click);
            // 
            // MonitorForm
            // 
            this.ClientSize = new System.Drawing.Size(552, 519);
            this.Controls.Add(this.groupBoxCalibraveis);
            this.Controls.Add(this.groupBoxDados);
            this.Controls.Add(this.btnOpcoes);
            this.Controls.Add(this.labelCondicaoNuvens);
            this.Controls.Add(this.labelSeguranca);
            this.Controls.Add(this.txtEstadoChuva);
            this.Controls.Add(this.txtEstadoCeu);
            this.Controls.Add(this.txtStatus);
            this.Controls.Add(this.labelCondicaoCeu);
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.Name = "MonitorForm";
            this.Text = "Monitor de Segurança";
            this.Load += new System.EventHandler(this.MonitorForm_Load);
            this.groupBoxCalibraveis.ResumeLayout(false);
            this.groupBoxCalibraveis.PerformLayout();
            this.groupBoxDados.ResumeLayout(false);
            this.groupBoxDados.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }
        private TextBox txtTimeoutNublado;
        private Label labelTimeoutNublado;
        private TextBox txtTrendCeu;
        private TextBox txtTrendPressao;
        private TextBox txtTrendUmidade;
        private Label labelTrendCeu;
        private Label labelTrendPressao;
        private Label labelTrendUmidade;
    }
}



