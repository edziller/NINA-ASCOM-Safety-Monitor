using System;
using System.Globalization;
using System.IO.Ports;
using System.Windows.Forms;
using static System.Windows.Forms.VisualStyles.VisualStyleElement.ProgressBar;


namespace ASCOM.EZZillerSafetyMonitor
{
    public partial class MonitorForm : Form
    {
        protected override void OnLoad(EventArgs e)
        {
            base.OnLoad(e);

            // Garante que sempre mostre a altitude atual do hardware quando o form abrir
            txtAltitude.Text = hardware.Altitude.ToString("F0", CultureInfo.InvariantCulture);
        }
        private SafetyMonitorHardware hardware;

        public MonitorForm(SafetyMonitorHardware hw)
        {
            InitializeComponent();

            txtLimiteIR.Text = hw.LimiteIR.ToString("F1", CultureInfo.InvariantCulture);
            txtTimeoutNublado.Text = hw.TimeoutNublado.ToString(CultureInfo.InvariantCulture);
            txtLimiteChuva.Text = hw.LimiteChuva.ToString(CultureInfo.InvariantCulture);
            txtLimiteDeltaP.Text = hw.LimiteDeltaP.ToString("F1", CultureInfo.InvariantCulture);
            txtLimiteUmidade.Text = hw.LimiteUmidade.ToString("F1", CultureInfo.InvariantCulture);

            btnSalvar.Click += btnSalvar_Click;
            btnOpcoes.Click += btnOpcoes_Click;
            btnRedefinir.Click += btnRedefinir_Click;
            
            hardware = hw;
            hardware.LineReceived += Hardware_LineReceived;
            hardware.EnviarAltitude();
            AtualizarSensores();
            AtualizarCalibraveis();

        }

        private void Hardware_LineReceived(object sender, string line)
        {
            foreach (var parte in line.Split(';'))
            {
                try
                {
                    if (parte.StartsWith("ALT:"))
                        hardware.Altitude = int.Parse(parte.Substring(4));
                    else if (parte.StartsWith("T:"))
                        hardware.Temperatura = float.Parse(parte.Substring(2), CultureInfo.InvariantCulture);
                    else if (parte.StartsWith("U:"))
                        hardware.Umidade = float.Parse(parte.Substring(2), CultureInfo.InvariantCulture);
                    else if (parte.StartsWith("TO:"))
                        hardware.PontoOrvalho = float.Parse(parte.Substring(3), CultureInfo.InvariantCulture);
                    else if (parte.StartsWith("PN:"))
                        hardware.PressaoNominal = float.Parse(parte.Substring(3), CultureInfo.InvariantCulture);
                    else if (parte.StartsWith("P:"))
                        hardware.Pressao = float.Parse(parte.Substring(2), CultureInfo.InvariantCulture);
                    else if (parte.StartsWith("DP:"))
                        hardware.DeltaP = float.Parse(parte.Substring(3), CultureInfo.InvariantCulture);
                    else if (parte.StartsWith("IR:"))
                        hardware.TemperaturaIR = float.Parse(parte.Substring(3), CultureInfo.InvariantCulture);
                    else if (parte.StartsWith("CEU:"))
                        hardware.EstadoCeu = parte.Substring(4);
                    else if (parte.StartsWith("CHUVA:"))
                        hardware.EstadoChuva = parte.Substring("CHUVA:".Length);
                    else if (parte.StartsWith("SP:"))
                        hardware.StatusPressao = parte.Substring(3);
                    else if (parte.StartsWith("TP:"))
                        hardware.TrendPressao = parte.Substring(3);
                    else if (parte.StartsWith("TU:"))
                        hardware.TrendUmidade = parte.Substring(3);
                    else if (parte.StartsWith("TC:"))
                        hardware.TrendCeu = parte.Substring(3);
                    else if (parte.StartsWith("SEG:"))
                        hardware.IsSafe = parte.Substring(4) == "SAFE";
                }
                catch
                {
                    // ignore just this part, keep going
                }
            }
            // only once per full line, back on the UI thread:
            this.BeginInvoke((Action)AtualizarSensores);
            
        }

        private void AtualizarCalibraveis()
        {
            // usa CultureInfo.InvariantCulture para garantir . como separador decimal
            if (!txtAltitude.Focused)
            {
                txtAltitude.Text = hardware.Altitude.ToString("F0", CultureInfo.InvariantCulture);
            }

            txtLimiteIR.Text = hardware.LimiteIR.ToString("F1", CultureInfo.InvariantCulture);
            txtTimeoutNublado.Text = hardware.TimeoutNublado.ToString(CultureInfo.InvariantCulture);
            txtLimiteChuva.Text = hardware.LimiteChuva.ToString(CultureInfo.InvariantCulture);
            txtLimiteDeltaP.Text = hardware.LimiteDeltaP.ToString("F1", CultureInfo.InvariantCulture);
            txtLimiteUmidade.Text = hardware.LimiteUmidade.ToString("F1", CultureInfo.InvariantCulture); ;
        }

        private void AtualizarSensores()
        {
            txtAltitudeSensor.Text = hardware.Altitude.ToString("F0", CultureInfo.InvariantCulture);
            txtTemperatura.Text = hardware.Temperatura.ToString("F1");
            txtUmidade.Text = hardware.Umidade.ToString("F1");
            txtOrvalho.Text = hardware.PontoOrvalho.ToString("F1");
            txtPressao.Text = hardware.Pressao.ToString("F1");
            txtPressaoNom.Text = hardware.PressaoNominal.ToString("F1");
            txtTIR.Text = hardware.TemperaturaIR.ToString("F1");
            txtStatusPressao.Text = hardware.StatusPressao;
            txtEstadoCeu.Text = hardware.EstadoCeu;
            txtEstadoChuva.Text = hardware.EstadoChuva;
            txtTrendPressao.Text = hardware.TrendPressao;
            txtTrendUmidade.Text = hardware.TrendUmidade;
            txtTrendCeu.Text = hardware.TrendCeu;
            txtStatus.Text = hardware.IsSafe ? "SAFE" : "UNSAFE";

            if (hardware.IsSafe)
            {
                txtStatus.BackColor = System.Drawing.Color.LightGreen; // ou Color.Green
            }
            else
            {
                txtStatus.BackColor = System.Drawing.Color.LightCoral; // ou Color.Red
            }

        }

        private void btnSalvar_Click(object sender, EventArgs e)
        {
            // 1) Parse e valida cada campo usando TryParse para não explodir a UI
            if (!float.TryParse(txtLimiteIR.Text.Trim(), NumberStyles.Float, CultureInfo.InvariantCulture, out float ir))
            {
                MessageBox.Show("Limite IR inferior inválido");
                return;
            }
            if (!int.TryParse(txtTimeoutNublado.Text.Trim(), NumberStyles.Integer, CultureInfo.InvariantCulture, out int tNub))
            {
                MessageBox.Show("Timeout Nublado inválido");
                return;
            }
            if (!int.TryParse(txtLimiteChuva.Text.Trim(), NumberStyles.Integer, CultureInfo.InvariantCulture, out int chuva))
            {
                MessageBox.Show("Limite Chuva inválido");
                return;
            }
            if (!float.TryParse(txtLimiteDeltaP.Text.Trim(),NumberStyles.Float, CultureInfo.InvariantCulture, out float dp))
            {
                MessageBox.Show("Limite ΔP inválido");
                return;
            }
            if (!float.TryParse(txtLimiteUmidade.Text.Trim(), NumberStyles.Float, CultureInfo.InvariantCulture, out float u))
            {
                MessageBox.Show("Limite Umidade inválido");
                return;
            }
            if (!int.TryParse(txtAltitude.Text.Trim(), NumberStyles.Integer, CultureInfo.InvariantCulture, out int alt))
            {
                MessageBox.Show("Altitude inválida");
                return;
            }

            // 2) Atribui tudo à camada de hardware
            hardware.LimiteIR = ir;
            hardware.TimeoutNublado = tNub;
            hardware.LimiteChuva = chuva;
            hardware.LimiteDeltaP = dp;
            hardware.LimiteUmidade = u;
            hardware.Altitude = alt;

            // 3) Envia ao ESP via métodos dedicados
            hardware.EnviarLimiteIR();
            hardware.EnviarTimeoutNublado();
            hardware.EnviarLimiteChuva();
            hardware.EnviarLimiteDeltaP();
            hardware.EnviarLimiteUmidade();
            hardware.EnviarAltitude();

            AtualizarCalibraveis();

            MessageBox.Show("Parâmetros enviados.", "OK", MessageBoxButtons.OK, MessageBoxIcon.Information);
        }

        private void btnRedefinir_Click(object sender, EventArgs e)
        {
            // valores-padrão
            
            txtLimiteIR.Text = "2.0";
            txtTimeoutNublado.Text = "10";
            txtLimiteChuva.Text = "890";
            txtLimiteDeltaP.Text = "-3.0";
            txtLimiteUmidade.Text = "75.0";
        }

        private void btnAtualizar_Click(object sender, EventArgs e)
        {
            AtualizarSensores();
            AtualizarCalibraveis();
        }

        private void btnOpcoes_Click(object sender, EventArgs e)
        {
            using (var f = new FormOpcoesWifi(hardware))
                f.ShowDialog();
        }

        private void pictureBoxLogo_Click(object sender, EventArgs e)
        {

        }

        private void groupBoxCalibraveis_Enter(object sender, EventArgs e)
        {

        }

        private void label1_Click(object sender, EventArgs e)
        {

        }

        private void textBox1_TextChanged(object sender, EventArgs e)
        {

        }

        private void label14_Click(object sender, EventArgs e)
        {

        }

        private void label15_Click(object sender, EventArgs e)
        {

        }

        private void label16_Click(object sender, EventArgs e)
        {

        }

        private void label17_Click(object sender, EventArgs e)
        {

        }

        private void txtAltitude_TextChanged(object sender, EventArgs e)
        {

        }

        private void label1_Click_1(object sender, EventArgs e)
        {

        }

        private void texLimiteIRSuperior_TextChanged(object sender, EventArgs e)
        {

        }

        private void label2_Click(object sender, EventArgs e)
        {

        }

        private void txtTrendPressao_TextChanged(object sender, EventArgs e)
        {

        }

        private void MonitorForm_Load(object sender, EventArgs e)
        {

        }
    }

}
    