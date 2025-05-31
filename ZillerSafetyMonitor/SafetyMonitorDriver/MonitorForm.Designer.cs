using System;
using System.Drawing;
using System.Windows.Forms;

namespace ASCOM.EZZillerSafetyMonitor
{
    public partial class MonitorForm : Form
    {
        private PictureBox pictureBoxLogo;
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
        private Button btnAtualizar;
        private Button btnOpcoes;
        private Button btnRedefinir;

        private void InitializeComponent()
        {
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(MonitorForm));
            this.pictureBoxLogo = new System.Windows.Forms.PictureBox();
            this.groupBoxCalibraveis = new System.Windows.Forms.GroupBox();
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
            this.txtStatus = new System.Windows.Forms.TextBox();
            this.txtEstadoChuva = new System.Windows.Forms.TextBox();
            this.txtEstadoCeu = new System.Windows.Forms.TextBox();
            this.txtTIR = new System.Windows.Forms.TextBox();
            this.txtStatusPressao = new System.Windows.Forms.TextBox();
            this.txtPressao = new System.Windows.Forms.TextBox();
            this.txtPressaoNom = new System.Windows.Forms.TextBox();
            this.txtOrvalho = new System.Windows.Forms.TextBox();
            this.txtUmidade = new System.Windows.Forms.TextBox();
            this.txtTemperatura = new System.Windows.Forms.TextBox();
            this.labelTemperatura = new System.Windows.Forms.Label();
            this.labelSeguranca = new System.Windows.Forms.Label();
            this.labelCondicaoNuvens = new System.Windows.Forms.Label();
            this.labelCondicaoCeu = new System.Windows.Forms.Label();
            this.labelTIR = new System.Windows.Forms.Label();
            this.labelStatusPressao = new System.Windows.Forms.Label();
            this.labelPressao = new System.Windows.Forms.Label();
            this.labelPressaoNominal = new System.Windows.Forms.Label();
            this.labelPontoOrvalho = new System.Windows.Forms.Label();
            this.labelUmidadeRelativa = new System.Windows.Forms.Label();
            this.btnAtualizar = new System.Windows.Forms.Button();
            this.btnOpcoes = new System.Windows.Forms.Button();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBoxLogo)).BeginInit();
            this.groupBoxCalibraveis.SuspendLayout();
            this.groupBoxDados.SuspendLayout();
            this.SuspendLayout();
            // 
            // pictureBoxLogo
            // 
            this.pictureBoxLogo.Image = ((System.Drawing.Image)(resources.GetObject("pictureBoxLogo.Image")));
            this.pictureBoxLogo.Location = new System.Drawing.Point(280, 12);
            this.pictureBoxLogo.Name = "pictureBoxLogo";
            this.pictureBoxLogo.Size = new System.Drawing.Size(240, 109);
            this.pictureBoxLogo.SizeMode = System.Windows.Forms.PictureBoxSizeMode.Zoom;
            this.pictureBoxLogo.TabIndex = 0;
            this.pictureBoxLogo.TabStop = false;
            this.pictureBoxLogo.Click += new System.EventHandler(this.pictureBoxLogo_Click);
            // 
            // groupBoxCalibraveis
            // 
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
            this.groupBoxCalibraveis.Location = new System.Drawing.Point(280, 130);
            this.groupBoxCalibraveis.Name = "groupBoxCalibraveis";
            this.groupBoxCalibraveis.Size = new System.Drawing.Size(240, 277);
            this.groupBoxCalibraveis.TabIndex = 1;
            this.groupBoxCalibraveis.TabStop = false;
            this.groupBoxCalibraveis.Text = "Parâmetros Calibráveis";
            this.groupBoxCalibraveis.Enter += new System.EventHandler(this.groupBoxCalibraveis_Enter);
            // 
            // txtLimiteChuva
            // 
            this.txtLimiteChuva.Location = new System.Drawing.Point(134, 169);
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
            this.txtLimiteUmidade.Location = new System.Drawing.Point(134, 133);
            this.txtLimiteUmidade.Name = "txtLimiteUmidade";
            this.txtLimiteUmidade.Size = new System.Drawing.Size(100, 22);
            this.txtLimiteUmidade.TabIndex = 18;
            // 
            // labelLimiteUmidade
            // 
            this.labelLimiteUmidade.AutoSize = true;
            this.labelLimiteUmidade.Location = new System.Drawing.Point(6, 140);
            this.labelLimiteUmidade.Name = "labelLimiteUmidade";
            this.labelLimiteUmidade.Size = new System.Drawing.Size(101, 16);
            this.labelLimiteUmidade.TabIndex = 17;
            this.labelLimiteUmidade.Text = "Limite Umidade";
            this.labelLimiteUmidade.Click += new System.EventHandler(this.label16_Click);
            // 
            // txtLimiteDeltaP
            // 
            this.txtLimiteDeltaP.Location = new System.Drawing.Point(134, 96);
            this.txtLimiteDeltaP.Name = "txtLimiteDeltaP";
            this.txtLimiteDeltaP.Size = new System.Drawing.Size(100, 22);
            this.txtLimiteDeltaP.TabIndex = 16;
            // 
            // labelDeltaP
            // 
            this.labelDeltaP.AutoSize = true;
            this.labelDeltaP.Location = new System.Drawing.Point(6, 102);
            this.labelDeltaP.Name = "labelDeltaP";
            this.labelDeltaP.Size = new System.Drawing.Size(89, 16);
            this.labelDeltaP.TabIndex = 15;
            this.labelDeltaP.Text = "Limite Delta P";
            this.labelDeltaP.Click += new System.EventHandler(this.label15_Click);
            // 
            // txtLimiteIR
            // 
            this.txtLimiteIR.Location = new System.Drawing.Point(134, 58);
            this.txtLimiteIR.Name = "txtLimiteIR";
            this.txtLimiteIR.Size = new System.Drawing.Size(100, 22);
            this.txtLimiteIR.TabIndex = 14;
            // 
            // labelLimiteIR
            // 
            this.labelLimiteIR.AutoSize = true;
            this.labelLimiteIR.Location = new System.Drawing.Point(6, 64);
            this.labelLimiteIR.Name = "labelLimiteIR";
            this.labelLimiteIR.Size = new System.Drawing.Size(58, 16);
            this.labelLimiteIR.TabIndex = 13;
            this.labelLimiteIR.Text = "Limite IR";
            this.labelLimiteIR.Click += new System.EventHandler(this.label14_Click);
            // 
            // txtAltitude
            // 
            this.txtAltitude.Location = new System.Drawing.Point(134, 25);
            this.txtAltitude.Name = "txtAltitude";
            this.txtAltitude.Size = new System.Drawing.Size(100, 22);
            this.txtAltitude.TabIndex = 14;
            this.txtAltitude.TextChanged += new System.EventHandler(this.txtAltitude_TextChanged);
            // 
            // labelAltitude
            // 
            this.labelAltitude.AutoSize = true;
            this.labelAltitude.Location = new System.Drawing.Point(6, 28);
            this.labelAltitude.Name = "labelAltitude";
            this.labelAltitude.Size = new System.Drawing.Size(73, 16);
            this.labelAltitude.TabIndex = 13;
            this.labelAltitude.Text = "Altitude (m)";
            // 
            // btnSalvar
            // 
            this.btnSalvar.Location = new System.Drawing.Point(6, 236);
            this.btnSalvar.Name = "btnSalvar";
            this.btnSalvar.Size = new System.Drawing.Size(90, 30);
            this.btnSalvar.TabIndex = 2;
            this.btnSalvar.Text = "Salvar";
            // 
            // btnRedefinir
            // 
            this.btnRedefinir.Location = new System.Drawing.Point(102, 236);
            this.btnRedefinir.Name = "btnRedefinir";
            this.btnRedefinir.Size = new System.Drawing.Size(90, 30);
            this.btnRedefinir.TabIndex = 3;
            this.btnRedefinir.Text = "Redefinir";
            this.btnRedefinir.Click += new System.EventHandler(this.btnRedefinir_Click);
            // 
            // groupBoxDados
            // 
            this.groupBoxDados.Controls.Add(this.txtStatus);
            this.groupBoxDados.Controls.Add(this.txtEstadoChuva);
            this.groupBoxDados.Controls.Add(this.txtEstadoCeu);
            this.groupBoxDados.Controls.Add(this.txtTIR);
            this.groupBoxDados.Controls.Add(this.txtStatusPressao);
            this.groupBoxDados.Controls.Add(this.txtPressao);
            this.groupBoxDados.Controls.Add(this.txtPressaoNom);
            this.groupBoxDados.Controls.Add(this.txtOrvalho);
            this.groupBoxDados.Controls.Add(this.txtUmidade);
            this.groupBoxDados.Controls.Add(this.txtTemperatura);
            this.groupBoxDados.Controls.Add(this.labelTemperatura);
            this.groupBoxDados.Controls.Add(this.labelSeguranca);
            this.groupBoxDados.Controls.Add(this.labelCondicaoNuvens);
            this.groupBoxDados.Controls.Add(this.labelCondicaoCeu);
            this.groupBoxDados.Controls.Add(this.labelTIR);
            this.groupBoxDados.Controls.Add(this.labelStatusPressao);
            this.groupBoxDados.Controls.Add(this.labelPressao);
            this.groupBoxDados.Controls.Add(this.labelPressaoNominal);
            this.groupBoxDados.Controls.Add(this.labelPontoOrvalho);
            this.groupBoxDados.Controls.Add(this.labelUmidadeRelativa);
            this.groupBoxDados.Location = new System.Drawing.Point(10, 10);
            this.groupBoxDados.Name = "groupBoxDados";
            this.groupBoxDados.Size = new System.Drawing.Size(260, 397);
            this.groupBoxDados.TabIndex = 0;
            this.groupBoxDados.TabStop = false;
            this.groupBoxDados.Text = "Dados do Sensor";
            // 
            // txtStatus
            // 
            this.txtStatus.Location = new System.Drawing.Point(148, 287);
            this.txtStatus.Name = "txtStatus";
            this.txtStatus.Size = new System.Drawing.Size(100, 22);
            this.txtStatus.TabIndex = 12;
            // 
            // txtEstadoChuva
            // 
            this.txtEstadoChuva.Location = new System.Drawing.Point(148, 261);
            this.txtEstadoChuva.Name = "txtEstadoChuva";
            this.txtEstadoChuva.Size = new System.Drawing.Size(100, 22);
            this.txtEstadoChuva.TabIndex = 11;
            // 
            // txtEstadoCeu
            // 
            this.txtEstadoCeu.Location = new System.Drawing.Point(148, 232);
            this.txtEstadoCeu.Name = "txtEstadoCeu";
            this.txtEstadoCeu.Size = new System.Drawing.Size(100, 22);
            this.txtEstadoCeu.TabIndex = 10;
            // 
            // txtTIR
            // 
            this.txtTIR.Location = new System.Drawing.Point(148, 205);
            this.txtTIR.Name = "txtTIR";
            this.txtTIR.Size = new System.Drawing.Size(100, 22);
            this.txtTIR.TabIndex = 9;
            // 
            // txtStatusPressao
            // 
            this.txtStatusPressao.Location = new System.Drawing.Point(148, 178);
            this.txtStatusPressao.Name = "txtStatusPressao";
            this.txtStatusPressao.Size = new System.Drawing.Size(100, 22);
            this.txtStatusPressao.TabIndex = 8;
            // 
            // txtPressao
            // 
            this.txtPressao.Location = new System.Drawing.Point(148, 146);
            this.txtPressao.Name = "txtPressao";
            this.txtPressao.Size = new System.Drawing.Size(100, 22);
            this.txtPressao.TabIndex = 6;
            // 
            // txtPressaoNom
            // 
            this.txtPressaoNom.Location = new System.Drawing.Point(148, 120);
            this.txtPressaoNom.Name = "txtPressaoNom";
            this.txtPressaoNom.Size = new System.Drawing.Size(100, 22);
            this.txtPressaoNom.TabIndex = 5;
            // 
            // txtOrvalho
            // 
            this.txtOrvalho.Location = new System.Drawing.Point(148, 93);
            this.txtOrvalho.Name = "txtOrvalho";
            this.txtOrvalho.Size = new System.Drawing.Size(100, 22);
            this.txtOrvalho.TabIndex = 4;
            // 
            // txtUmidade
            // 
            this.txtUmidade.Location = new System.Drawing.Point(148, 69);
            this.txtUmidade.Name = "txtUmidade";
            this.txtUmidade.Size = new System.Drawing.Size(100, 22);
            this.txtUmidade.TabIndex = 3;
            // 
            // txtTemperatura
            // 
            this.txtTemperatura.Location = new System.Drawing.Point(148, 43);
            this.txtTemperatura.Name = "txtTemperatura";
            this.txtTemperatura.Size = new System.Drawing.Size(100, 22);
            this.txtTemperatura.TabIndex = 2;
            // 
            // labelTemperatura
            // 
            this.labelTemperatura.AutoSize = true;
            this.labelTemperatura.Location = new System.Drawing.Point(7, 46);
            this.labelTemperatura.Name = "labelTemperatura";
            this.labelTemperatura.Size = new System.Drawing.Size(109, 16);
            this.labelTemperatura.TabIndex = 0;
            this.labelTemperatura.Text = "Temperatura (°C)";
            this.labelTemperatura.Click += new System.EventHandler(this.label1_Click);
            // 
            // labelSeguranca
            // 
            this.labelSeguranca.AutoSize = true;
            this.labelSeguranca.Location = new System.Drawing.Point(7, 289);
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
            this.labelCondicaoNuvens.Location = new System.Drawing.Point(7, 263);
            this.labelCondicaoNuvens.Name = "labelCondicaoNuvens";
            this.labelCondicaoNuvens.Size = new System.Drawing.Size(106, 16);
            this.labelCondicaoNuvens.TabIndex = 0;
            this.labelCondicaoNuvens.Text = "Condição Chuva";
            this.labelCondicaoNuvens.Click += new System.EventHandler(this.label1_Click);
            // 
            // labelCondicaoCeu
            // 
            this.labelCondicaoCeu.AutoSize = true;
            this.labelCondicaoCeu.Location = new System.Drawing.Point(7, 234);
            this.labelCondicaoCeu.Name = "labelCondicaoCeu";
            this.labelCondicaoCeu.Size = new System.Drawing.Size(111, 16);
            this.labelCondicaoCeu.TabIndex = 0;
            this.labelCondicaoCeu.Text = "Condição do Céu";
            this.labelCondicaoCeu.Click += new System.EventHandler(this.label1_Click);
            // 
            // labelTIR
            // 
            this.labelTIR.AutoSize = true;
            this.labelTIR.Location = new System.Drawing.Point(7, 207);
            this.labelTIR.Name = "labelTIR";
            this.labelTIR.Size = new System.Drawing.Size(120, 16);
            this.labelTIR.TabIndex = 0;
            this.labelTIR.Text = "T fundo do céu (°C)";
            this.labelTIR.Click += new System.EventHandler(this.label1_Click);
            // 
            // labelStatusPressao
            // 
            this.labelStatusPressao.AutoSize = true;
            this.labelStatusPressao.Location = new System.Drawing.Point(7, 180);
            this.labelStatusPressao.Name = "labelStatusPressao";
            this.labelStatusPressao.Size = new System.Drawing.Size(98, 16);
            this.labelStatusPressao.TabIndex = 0;
            this.labelStatusPressao.Text = "Status Pressão";
            this.labelStatusPressao.Click += new System.EventHandler(this.label1_Click);
            // 
            // labelPressao
            // 
            this.labelPressao.AutoSize = true;
            this.labelPressao.Location = new System.Drawing.Point(7, 152);
            this.labelPressao.Name = "labelPressao";
            this.labelPressao.Size = new System.Drawing.Size(125, 16);
            this.labelPressao.TabIndex = 0;
            this.labelPressao.Text = "Pressão atual (hPa)";
            this.labelPressao.Click += new System.EventHandler(this.label1_Click);
            // 
            // labelPressaoNominal
            // 
            this.labelPressaoNominal.AutoSize = true;
            this.labelPressaoNominal.Location = new System.Drawing.Point(7, 126);
            this.labelPressaoNominal.Name = "labelPressaoNominal";
            this.labelPressaoNominal.Size = new System.Drawing.Size(156, 20);
            this.labelPressaoNominal.TabIndex = 0;
            this.labelPressaoNominal.Text = "Pressão Nom.(hPa)";
            this.labelPressaoNominal.Click += new System.EventHandler(this.label1_Click);
            // 
            // labelPontoOrvalho
            // 
            this.labelPontoOrvalho.AutoSize = true;
            this.labelPontoOrvalho.Location = new System.Drawing.Point(7, 100);
            this.labelPontoOrvalho.Name = "labelPontoOrvalho";
            this.labelPontoOrvalho.Size = new System.Drawing.Size(116, 16);
            this.labelPontoOrvalho.TabIndex = 0;
            this.labelPontoOrvalho.Text = "Ponto Orvalho (°C)";
            this.labelPontoOrvalho.Click += new System.EventHandler(this.label1_Click);
            // 
            // labelUmidadeRelativa
            // 
            this.labelUmidadeRelativa.AutoSize = true;
            this.labelUmidadeRelativa.Location = new System.Drawing.Point(7, 74);
            this.labelUmidadeRelativa.Name = "labelUmidadeRelativa";
            this.labelUmidadeRelativa.Size = new System.Drawing.Size(116, 16);
            this.labelUmidadeRelativa.TabIndex = 0;
            this.labelUmidadeRelativa.Text = "Umidade Relativa";
            this.labelUmidadeRelativa.Click += new System.EventHandler(this.label1_Click);
            // 
            // btnAtualizar
            // 
            this.btnAtualizar.Location = new System.Drawing.Point(16, 366);
            this.btnAtualizar.Name = "btnAtualizar";
            this.btnAtualizar.Size = new System.Drawing.Size(90, 30);
            this.btnAtualizar.TabIndex = 4;
            this.btnAtualizar.Text = "Atualizar";
            // 
            // btnOpcoes
            // 
            this.btnOpcoes.Location = new System.Drawing.Point(112, 366);
            this.btnOpcoes.Name = "btnOpcoes";
            this.btnOpcoes.Size = new System.Drawing.Size(90, 30);
            this.btnOpcoes.TabIndex = 5;
            this.btnOpcoes.Text = "Opções Wi-Fi";
            this.btnOpcoes.Click += new System.EventHandler(this.btnOpcoes_Click);
            // 
            // MonitorForm
            // 
            this.ClientSize = new System.Drawing.Size(427, 339);
            this.Controls.Add(this.pictureBoxLogo);
            this.Controls.Add(this.btnAtualizar);
            this.Controls.Add(this.btnOpcoes);
            this.Controls.Add(this.groupBoxCalibraveis);
            this.Controls.Add(this.groupBoxDados);
            this.Name = "MonitorForm";
            this.Text = "Monitor de Segurança";
            ((System.ComponentModel.ISupportInitialize)(this.pictureBoxLogo)).EndInit();
            this.groupBoxCalibraveis.ResumeLayout(false);
            this.groupBoxCalibraveis.PerformLayout();
            this.groupBoxDados.ResumeLayout(false);
            this.groupBoxDados.PerformLayout();
            this.ResumeLayout(false);

        }
    }
}



