// MonitorForm.cs — v6
// Mudancas:
//  - Badge chuva exibe texto COMPLETO (sem abreviacao)
//  - Badge ceu exibe texto completo
//  - Rodape com labels e textboxes no mesmo Y (alinhamento correto)
//  - lblBeaconChuva com font menor para caber texto longo

using System;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Globalization;
using System.Windows.Forms;

namespace ASCOM.EZZillerSafetyMonitor
{
    public partial class MonitorForm : Form
    {
        private SafetyMonitorHardware hardware;

        // ── Paleta escura ──────────────────────────────────────────────────────
        static readonly Color DkForm      = Color.FromArgb(22,  22,  35);
        static readonly Color DkLeft      = Color.FromArgb(26,  26,  40);
        static readonly Color DkRight     = Color.FromArgb(30,  30,  46);
        static readonly Color DkFooter    = Color.FromArgb(18,  18,  28);
        static readonly Color DkText      = Color.FromArgb(218, 218, 232);
        static readonly Color DkMuted     = Color.FromArgb(130, 130, 158);
        static readonly Color DkSection   = Color.FromArgb(90,  90,  120);
        static readonly Color DkInput     = Color.FromArgb(36,  36,  54);
        static readonly Color DkInputText = Color.FromArgb(208, 208, 228);
        static readonly Color DkSafe      = Color.FromArgb(18,  58,  10);
        static readonly Color DkUnsafe    = Color.FromArgb(76,  14,  14);

        static readonly Color[] IkBg = {
            Color.FromArgb(56, 40, 10),
            Color.FromArgb(14, 36, 62),
            Color.FromArgb(34, 22, 58),
            Color.FromArgb(18, 34, 54),
            Color.FromArgb(34, 34, 34),
            Color.FromArgb(34, 34, 34),
            Color.FromArgb(58, 18, 14),
        };
        static readonly Color[] IkFg = {
            Color.FromArgb(210, 150, 55),
            Color.FromArgb(75,  150, 215),
            Color.FromArgb(145, 115, 215),
            Color.FromArgb(85,  145, 205),
            Color.FromArgb(145, 145, 145),
            Color.FromArgb(145, 145, 145),
            Color.FromArgb(205, 95,  75),
        };

        public MonitorForm(SafetyMonitorHardware hw)
        {
            hardware = hw;
            InitializeComponent();
            AplicarTema();
            AtualizarCalibraveis();
            hardware.LineReceived += Hardware_LineReceived;
            btnSalvar.Click    += btnSalvar_Click;
            btnOpcoes.Click    += btnOpcoes_Click;
            btnRedefinir.Click += btnRedefinir_Click;
            AtualizarSensores();
        }

        protected override void OnLoad(EventArgs e)
        {
            base.OnLoad(e);
            AtualizarCalibraveis();
        }

        private void AplicarTema()
        {
            Font fBase    = new Font("Segoe UI", 9f);
            Font fBold    = new Font("Segoe UI", 9f,  FontStyle.Bold);
            Font fSection = new Font("Segoe UI", 8f,  FontStyle.Bold);
            Font fBeacon  = new Font("Segoe UI", 20f, FontStyle.Bold);
            Font fVal     = new Font("Segoe UI", 10f);
            Font fMuted   = new Font("Segoe UI", 8f);
            Font fBadge   = new Font("Segoe UI", 9f,  FontStyle.Bold);
            // Badge de chuva usa fonte menor para caber o texto longo na mesma linha
            Font fBadgeChuva = new Font("Segoe UI", 8.5f);

            this.Font      = fBase;
            this.BackColor = DkForm;

            // Beacon
            pnlBeacon.BackColor    = DkSafe;
            pnlBeaconDot.BackColor = Color.FromArgb(60, 255, 255, 255);
            Circulo(pnlBeaconDot);

            txtStatus.Font      = fBeacon;
            txtStatus.ForeColor = Color.White;
            txtStatus.BackColor = DkSafe;

            // "Condicao do observatorio" — ACIMA do status
            lblBeaconSub.Font      = fMuted;
            lblBeaconSub.ForeColor = Color.FromArgb(180, 255, 255, 255);
            lblBeaconSub.BackColor = Color.Transparent;

            // Titulos dos badges
            foreach (var l in new Label[] { lblBeaconCeuTitle, lblBeaconChuvaTitle })
            {
                l.Font      = fMuted;
                l.ForeColor = Color.FromArgb(160, 255, 255, 255);
                l.BackColor = Color.Transparent;
            }

            // Badge céu
            lblBeaconCeu.Font      = fBadge;
            lblBeaconCeu.ForeColor = Color.White;
            lblBeaconCeu.BackColor = Color.FromArgb(40, 255, 255, 255);

            // Badge chuva — fonte levemente menor para suportar texto longo
            lblBeaconChuva.Font      = fBadgeChuva;
            lblBeaconChuva.ForeColor = Color.White;
            lblBeaconChuva.BackColor = Color.FromArgb(40, 255, 255, 255);

            labelAltitudeSensor.Font      = fMuted;
            labelAltitudeSensor.ForeColor = Color.FromArgb(160, 255, 255, 255);
            labelAltitudeSensor.BackColor = Color.Transparent;
            txtAltitudeSensor.Font      = fBold;
            txtAltitudeSensor.ForeColor = Color.White;
            txtAltitudeSensor.BackColor = DkSafe;

            // Linha divisoria beacon (horizontal entre status e badges)
            // e separador vertical antes da altitude
            pnlBeacon.Paint += (s, e) =>
            {
                using (var p = new Pen(Color.FromArgb(50, 255, 255, 255), 1f))
                {
                    e.Graphics.DrawLine(p, 6, 68, 630, 68);   // linha horizontal
                    e.Graphics.DrawLine(p, 632, 4, 632, 108); // separador altitude
                }
            };

            pnlLeft.BackColor  = DkLeft;
            pnlRight.BackColor = DkRight;
            pnlBody.BackColor  = DkForm;
            pnlFooter.BackColor= DkFooter;

            pnlLeft.Paint += (s, e) =>
            {
                using (var p = new Pen(Color.FromArgb(55, 55, 80), 1f))
                    e.Graphics.DrawLine(p, pnlLeft.Width - 1, 0, pnlLeft.Width - 1, pnlLeft.Height);
            };
            pnlFooter.Paint += (s, e) =>
            {
                using (var p = new Pen(Color.FromArgb(45, 45, 65), 1f))
                    e.Graphics.DrawLine(p, 0, 0, pnlFooter.Width, 0);
            };

            lblSectionSensores.Font      = fSection;
            lblSectionSensores.ForeColor = DkSection;
            lblSectionSensores.BackColor = Color.Transparent;
            lblSectionParams.Font        = fSection;
            lblSectionParams.ForeColor   = DkSection;
            lblSectionParams.BackColor   = Color.Transparent;

            Label[] icones = { lblIconTemp, lblIconHum, lblIconDew, lblIconPN,
                               lblIconPress, lblIconSP, lblIconIR };
            Font fIcon = new Font("Segoe UI", 7.5f, FontStyle.Bold);
            for (int i = 0; i < icones.Length; i++)
            {
                icones[i].Font      = fIcon;
                icones[i].BackColor = IkBg[i];
                icones[i].ForeColor = IkFg[i];
            }

            Label[] labelsLeit = {
                labelTemperatura, labelUmidadeRelativa, labelPontoOrvalho,
                labelPressaoNominal, labelPressao, labelStatusPressao, labelTIR
            };
            foreach (var l in labelsLeit) { l.Font = fBase; l.ForeColor = DkMuted; l.BackColor = Color.Transparent; }

            TextBox[] tbLeit = { txtTemperatura, txtUmidade, txtOrvalho,
                                 txtPressaoNom, txtPressao, txtStatusPressao, txtTIR };
            foreach (var tb in tbLeit) { tb.Font = fVal; tb.ForeColor = DkText; tb.BackColor = DkLeft; }

            // Rodape — tudo o mesmo Y via designer; apenas a cor aqui
            foreach (var l in new Label[] { labelTrendPressao, labelTrendUmidade, labelTrendCeu })
            {
                l.Font      = fMuted;
                l.ForeColor = DkMuted;
                l.BackColor = Color.Transparent;
            }
            foreach (var tb in new TextBox[] { txtTrendPressao, txtTrendUmidade, txtTrendCeu })
            {
                tb.Font      = fBold;
                tb.ForeColor = DkText;
                tb.BackColor = DkFooter;
            }

            lblConnDot.BackColor = Color.FromArgb(34, 197, 94);
            Circulo(lblConnDot);
            lblConnText.Font      = fMuted;
            lblConnText.ForeColor = DkMuted;
            lblConnText.BackColor = Color.Transparent;

            Label[] labelsParam = { labelLimiteIR, labelTimeoutNublado, labelDeltaP,
                                    labelLimiteUmidade, labelLimiteChuva, labelAltitude };
            foreach (var l in labelsParam) { l.Font = fBase; l.ForeColor = DkMuted; l.BackColor = Color.Transparent; }

            TextBox[] tbParam = { txtLimiteIR, txtTimeoutNublado, txtLimiteDeltaP,
                                  txtLimiteUmidade, txtLimiteChuva, txtAltitude };
            foreach (var tb in tbParam) { tb.Font = fVal; tb.BackColor = DkInput; tb.ForeColor = DkInputText; }

            BtnPrimario(btnSalvar, fBold);
            BtnSecundario(btnRedefinir, fBase);
            BtnSecundario(btnOpcoes, fBase);
        }

        private void BtnPrimario(Button b, Font f)
        {
            b.Font = f; b.BackColor = Color.FromArgb(30, 90, 12); b.ForeColor = Color.White;
            b.FlatStyle = FlatStyle.Flat; b.FlatAppearance.BorderSize = 0;
            b.FlatAppearance.MouseOverBackColor = Color.FromArgb(42, 118, 18);
            b.Cursor = Cursors.Hand;
        }
        private void BtnSecundario(Button b, Font f)
        {
            b.Font = f; b.BackColor = Color.FromArgb(42, 42, 62); b.ForeColor = Color.FromArgb(190, 190, 210);
            b.FlatStyle = FlatStyle.Flat; b.FlatAppearance.BorderColor = Color.FromArgb(65, 65, 90);
            b.FlatAppearance.BorderSize = 1; b.FlatAppearance.MouseOverBackColor = Color.FromArgb(55, 55, 78);
            b.Cursor = Cursors.Hand;
        }
        private static void Circulo(Control c)
        {
            var gp = new GraphicsPath();
            gp.AddEllipse(0, 0, c.Width, c.Height);
            c.Region = new Region(gp);
        }

        private void Hardware_LineReceived(object sender, string line)
        {
            if (!IsDisposed && IsHandleCreated)
                BeginInvoke((Action)AtualizarSensores);
        }

        private void AtualizarSensores()
        {
            bool seguro = hardware.IsSafe;
            Color corBeacon = seguro ? DkSafe : DkUnsafe;
            pnlBeacon.BackColor         = corBeacon;
            txtStatus.BackColor         = corBeacon;
            txtAltitudeSensor.BackColor = corBeacon;
            pnlBeaconDot.BackColor      = seguro
                ? Color.FromArgb(70, 255, 255, 255)
                : Color.FromArgb(70, 255, 80, 80);
            txtStatus.Text = seguro ? "SAFE" : "UNSAFE";

            // ── Badges beacon: texto COMPLETO ──────────────────────────────────
            bool nublado  = hardware.EstadoCeu.IndexOf("Nublado",   StringComparison.OrdinalIgnoreCase) >= 0;
            bool chovendo = hardware.EstadoChuva.IndexOf("Chovendo", StringComparison.OrdinalIgnoreCase) >= 0
                         || hardware.EstadoChuva.IndexOf("Risco",    StringComparison.OrdinalIgnoreCase) >= 0;

            // Badge céu — texto completo (curto: "Ceu limpo" ou "Nublado")
            lblBeaconCeu.Text = string.IsNullOrEmpty(hardware.EstadoCeu) || hardware.EstadoCeu == "---"
                ? "---"
                : hardware.EstadoCeu;
            lblBeaconCeu.BackColor = nublado
                ? Color.FromArgb(70, 40, 14)
                : Color.FromArgb(12, 50, 22);
            lblBeaconCeu.ForeColor = nublado
                ? Color.FromArgb(245, 190, 80)
                : Color.FromArgb(90, 210, 130);

            // Badge chuva — texto COMPLETO (pode ser longo)
            lblBeaconChuva.Text = string.IsNullOrEmpty(hardware.EstadoChuva) || hardware.EstadoChuva == "---"
                ? "---"
                : hardware.EstadoChuva;
            lblBeaconChuva.BackColor = chovendo
                ? Color.FromArgb(70, 14, 14)
                : Color.FromArgb(14, 36, 62);
            lblBeaconChuva.ForeColor = chovendo
                ? Color.FromArgb(235, 100, 90)
                : Color.FromArgb(80, 155, 215);

            // Altitude
            txtAltitudeSensor.Text = hardware.Altitude.ToString("F0", CultureInfo.InvariantCulture) + " m";

            // Leituras
            txtTemperatura.Text   = hardware.Temperatura.ToString("F1") + " \u00B0C";
            txtUmidade.Text       = hardware.Umidade.ToString("F1") + " %";
            txtOrvalho.Text       = hardware.PontoOrvalho.ToString("F1") + " \u00B0C";
            txtPressaoNom.Text    = hardware.PressaoNominal.ToString("F1") + " hPa";
            txtPressao.Text       = hardware.Pressao.ToString("F1") + " hPa";
            txtStatusPressao.Text = hardware.StatusPressao;
            txtTIR.Text           = hardware.TemperaturaIR.ToString("F1") + " \u00B0C";
            txtTrendPressao.Text  = hardware.TrendPressao;
            txtTrendUmidade.Text  = hardware.TrendUmidade;
            txtTrendCeu.Text      = hardware.TrendCeu;
            txtEstadoCeu.Text     = hardware.EstadoCeu;
            txtEstadoChuva.Text   = hardware.EstadoChuva;

            // Icone de pressao: âmbar se baixa
            bool pressaBaixa = hardware.StatusPressao.IndexOf("Baixa", StringComparison.OrdinalIgnoreCase) >= 0;
            lblIconSP.BackColor = pressaBaixa ? Color.FromArgb(56, 38, 10) : IkBg[5];
            lblIconSP.ForeColor = pressaBaixa ? Color.FromArgb(210, 150, 55) : IkFg[5];
        }

        private void AtualizarCalibraveis()
        {
            if (!txtAltitude.Focused)
                txtAltitude.Text = hardware.Altitude.ToString("F0", CultureInfo.InvariantCulture);
            txtLimiteIR.Text       = hardware.LimiteIR.ToString("F1", CultureInfo.InvariantCulture);
            txtTimeoutNublado.Text = hardware.TimeoutNublado.ToString(CultureInfo.InvariantCulture);
            txtLimiteChuva.Text    = hardware.LimiteChuva.ToString(CultureInfo.InvariantCulture);
            txtLimiteDeltaP.Text   = hardware.LimiteDeltaP.ToString("F1", CultureInfo.InvariantCulture);
            txtLimiteUmidade.Text  = hardware.LimiteUmidade.ToString("F1", CultureInfo.InvariantCulture);
        }

        private void btnSalvar_Click(object sender, EventArgs e)
        {
            if (!float.TryParse(txtLimiteIR.Text.Trim(),      NumberStyles.Float,   CultureInfo.InvariantCulture, out float ir))  { MessageBox.Show("Limite IR invalido");       return; }
            if (!int.TryParse(txtTimeoutNublado.Text.Trim(),  NumberStyles.Integer, CultureInfo.InvariantCulture, out int tNub)) { MessageBox.Show("Timeout invalido");         return; }
            if (!int.TryParse(txtLimiteChuva.Text.Trim(),     NumberStyles.Integer, CultureInfo.InvariantCulture, out int chuva)){ MessageBox.Show("Limite Chuva invalido");    return; }
            if (!float.TryParse(txtLimiteDeltaP.Text.Trim(),  NumberStyles.Float,   CultureInfo.InvariantCulture, out float dp))  { MessageBox.Show("Limite dP invalido");      return; }
            if (!float.TryParse(txtLimiteUmidade.Text.Trim(), NumberStyles.Float,   CultureInfo.InvariantCulture, out float um))  { MessageBox.Show("Limite Umidade invalido"); return; }
            if (!int.TryParse(txtAltitude.Text.Trim(),        NumberStyles.Integer, CultureInfo.InvariantCulture, out int alt))  { MessageBox.Show("Altitude invalida");        return; }

            if (chuva < 0 || chuva > 1023) { MessageBox.Show("Limite Chuva: 0-1023");     return; }
            if (um < 0 || um > 100)         { MessageBox.Show("Limite Umidade: 0-100%");   return; }
            if (tNub < 1 || tNub > 120)     { MessageBox.Show("Timeout: 1-120 min");       return; }
            if (alt < 0 || alt > 9999)      { MessageBox.Show("Altitude: 0-9999 m");       return; }

            hardware.LimiteIR = ir; hardware.TimeoutNublado = tNub; hardware.LimiteChuva = chuva;
            hardware.LimiteDeltaP = dp; hardware.LimiteUmidade = um; hardware.Altitude = alt;
            hardware.EnviarLimiteIR(); hardware.EnviarTimeoutNublado(); hardware.EnviarLimiteChuva();
            hardware.EnviarLimiteDeltaP(); hardware.EnviarLimiteUmidade(); hardware.EnviarAltitude();
            AtualizarCalibraveis();
            MessageBox.Show("Parametros enviados.", "OK", MessageBoxButtons.OK, MessageBoxIcon.Information);
        }

        private void btnRedefinir_Click(object sender, EventArgs e)
        {
            txtLimiteIR.Text = "8.0"; txtTimeoutNublado.Text = "10"; txtLimiteChuva.Text = "850";
            txtLimiteDeltaP.Text = "0.0"; txtLimiteUmidade.Text = "70.0";
            MessageBox.Show("Valores padrao preenchidos. Clique em Salvar para enviar.",
                "Atencao", MessageBoxButtons.OK, MessageBoxIcon.Warning);
        }

        private void btnOpcoes_Click(object sender, EventArgs e)
        {
            hardware.PausePolling();
            try { using (var f = new FormOpcoesWifi(hardware)) f.ShowDialog(); }
            finally { hardware.ResumePolling(); }
        }

        protected override void OnFormClosed(FormClosedEventArgs e)
        {
            hardware.LineReceived -= Hardware_LineReceived;
            base.OnFormClosed(e);
        }

        // Stubs do Designer
        private void MonitorForm_Load(object sender, EventArgs e) { }
        private void groupBoxCalibraveis_Enter(object sender, EventArgs e) { }
        private void label1_Click(object sender, EventArgs e) { }
        private void label2_Click(object sender, EventArgs e) { }
        private void label14_Click(object sender, EventArgs e) { }
        private void label15_Click(object sender, EventArgs e) { }
        private void label16_Click(object sender, EventArgs e) { }
        private void label17_Click(object sender, EventArgs e) { }
        private void txtAltitude_TextChanged(object sender, EventArgs e) { }
        private void txtTrendPressao_TextChanged(object sender, EventArgs e) { }
    }
}
