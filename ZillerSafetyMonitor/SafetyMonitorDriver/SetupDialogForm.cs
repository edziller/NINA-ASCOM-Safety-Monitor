// SetupDialogForm.cs — v3
// Novidades v3:
//  - Todos os parâmetros calibráveis adicionados ao Setup Dialog
//  - Tooltips explicativos ao passar o mouse sobre cada campo
//  - FIX: form nao abre mais minimizado (WindowState forcado em Load)
//  - FIX: Profile.GetValue com 4 parametros

using ASCOM.Utilities;
using System;
using System.Globalization;
using System.IO.Ports;
using System.Windows.Forms;

namespace ASCOM.EZZillerSafetyMonitor
{
    public partial class SetupDialogForm : Form
    {
        private readonly SafetyMonitorHardware hardware;
        private const string DRIVER_ID = "ASCOM.EZZillerSafetyMonitor.SafetyMonitor";

        public SetupDialogForm(SafetyMonitorHardware hw)
        {
            InitializeComponent();
            hardware = hw;
            ConfigurarTooltips();
            CarregarValores();
        }

        private void ConfigurarTooltips()
        {
            var tt = new ToolTip { AutoPopDelay = 8000, InitialDelay = 500, ReshowDelay = 300, ShowAlways = true };

            tt.SetToolTip(lblAltitude,
                "Altitude do observatorio em metros acima do nivel do mar.\n" +
                "Usada para calcular a pressao atmosferica nominal local\n" +
                "e detectar variacoes anormais de pressao.");
            tt.SetToolTip(txtAltitude,
                "Altitude do observatorio em metros acima do nivel do mar.\n" +
                "Usada para calcular a pressao atmosferica nominal local\n" +
                "e detectar variacoes anormais de pressao.");

            tt.SetToolTip(lblLimiteIR,
                "Temperatura maxima do fundo do ceu (em graus C) a partir da\n" +
                "qual o sistema considera o ceu como nublado.\n" +
                "Valores de IR abaixo deste limite indicam ceu limpo.\n" +
                "Valores tipicos: entre 0 e 10 graus C conforme o clima local.");
            tt.SetToolTip(txtLimiteIR,
                "Temperatura maxima do fundo do ceu (em graus C) a partir da\n" +
                "qual o sistema considera o ceu como nublado.\n" +
                "Valores de IR abaixo deste limite indicam ceu limpo.\n" +
                "Valores tipicos: entre 0 e 10 graus C conforme o clima local.");

            tt.SetToolTip(lblTimeoutNublado,
                "Tempo em minutos que o ceu precisa permanecer nublado\n" +
                "com umidade alta antes de gerar uma condicao UNSAFE.\n" +
                "Evita alarmes falsos por nuvens passageiras.\n" +
                "Valor recomendado: 10 minutos.");
            tt.SetToolTip(txtTimeoutNublado,
                "Tempo em minutos que o ceu precisa permanecer nublado\n" +
                "com umidade alta antes de gerar uma condicao UNSAFE.\n" +
                "Evita alarmes falsos por nuvens passageiras.\n" +
                "Valor recomendado: 10 minutos.");

            tt.SetToolTip(lblLimiteDeltaP,
                "Variacao minima de pressao (em hPa) em relacao a pressao\n" +
                "nominal calculada para a altitude configurada.\n" +
                "Valores negativos indicam queda de pressao -- sinal de\n" +
                "frente de mau tempo combinado com umidade alta.\n" +
                "Ex: -3.0 significa 3 hPa abaixo do esperado para a altitude.");
            tt.SetToolTip(txtLimiteDeltaP,
                "Variacao minima de pressao (em hPa) em relacao a pressao\n" +
                "nominal calculada para a altitude configurada.\n" +
                "Valores negativos indicam queda de pressao -- sinal de\n" +
                "frente de mau tempo combinado com umidade alta.\n" +
                "Ex: -3.0 significa 3 hPa abaixo do esperado para a altitude.");

            tt.SetToolTip(lblLimiteUmidade,
                "Percentual de umidade relativa do ar a partir do qual\n" +
                "o sistema considera a umidade como alta.\n" +
                "Combinada com ceu nublado ou pressao baixa,\n" +
                "pode gerar condicao UNSAFE. Valor recomendado: 70-85%.");
            tt.SetToolTip(txtLimiteUmidade,
                "Percentual de umidade relativa do ar a partir do qual\n" +
                "o sistema considera a umidade como alta.\n" +
                "Combinada com ceu nublado ou pressao baixa,\n" +
                "pode gerar condicao UNSAFE. Valor recomendado: 70-85%.");

            tt.SetToolTip(lblLimiteChuva,
                "Valor bruto (0-1023) do sensor de chuva analogico.\n" +
                "Valores ABAIXO do limite indicam presenca de agua no sensor\n" +
                "(chuva detectada = UNSAFE imediato, independente de outras condicoes).\n" +
                "0 = completamente molhado, 1023 = completamente seco.\n" +
                "Valor recomendado: 850.");
            tt.SetToolTip(txtLimiteChuva,
                "Valor bruto (0-1023) do sensor de chuva analogico.\n" +
                "Valores ABAIXO do limite indicam presenca de agua no sensor\n" +
                "(chuva detectada = UNSAFE imediato, independente de outras condicoes).\n" +
                "0 = completamente molhado, 1023 = completamente seco.\n" +
                "Valor recomendado: 850.");
        }

        private void CarregarValores()
        {
            var profile = new Profile { DeviceType = "SafetyMonitor" };
            txtAltitude.Text       = profile.GetValue(DRIVER_ID, "Altitude",       "", "1000");
            txtLimiteIR.Text       = profile.GetValue(DRIVER_ID, "LimiteIR",       "", "8.0");
            txtTimeoutNublado.Text = profile.GetValue(DRIVER_ID, "TimeoutNublado", "", "10");
            txtLimiteDeltaP.Text   = profile.GetValue(DRIVER_ID, "LimiteDeltaP",   "", "0.0");
            txtLimiteUmidade.Text  = profile.GetValue(DRIVER_ID, "LimiteUmidade",  "", "70.0");
            txtLimiteChuva.Text    = profile.GetValue(DRIVER_ID, "LimiteChuva",    "", "850");
        }

        private void SetupDialogForm_Load(object sender, EventArgs e)
        {
            // FIX: garante abertura normal, nunca minimizado
            WindowState = FormWindowState.Normal;
            BringToFront();
            Activate();

            comboBoxComPort.Items.Clear();
            comboBoxComPort.Items.AddRange(SerialPort.GetPortNames());
            if (!string.IsNullOrEmpty(hardware.ComPort) && comboBoxComPort.Items.Contains(hardware.ComPort))
                comboBoxComPort.SelectedItem = hardware.ComPort;
            else if (comboBoxComPort.Items.Count > 0)
                comboBoxComPort.SelectedIndex = 0;
        }

        private void CmdOK_Click(object sender, EventArgs e)
        {
            if (comboBoxComPort.SelectedItem == null)
            { MessageBox.Show("Selecione uma porta COM.", "Atencao", MessageBoxButtons.OK, MessageBoxIcon.Warning); return; }

            if (!int.TryParse(txtAltitude.Text.Trim(), out int alt) || alt < 0 || alt > 9999)
            { MessageBox.Show("Altitude invalida (0-9999 m).", "Atencao", MessageBoxButtons.OK, MessageBoxIcon.Warning); return; }

            if (!float.TryParse(txtLimiteIR.Text.Trim(), NumberStyles.Float, CultureInfo.InvariantCulture, out float ir))
            { MessageBox.Show("Limite IR invalido.", "Atencao", MessageBoxButtons.OK, MessageBoxIcon.Warning); return; }

            if (!int.TryParse(txtTimeoutNublado.Text.Trim(), out int to) || to < 1 || to > 120)
            { MessageBox.Show("Timeout Nublado invalido (1-120 min).", "Atencao", MessageBoxButtons.OK, MessageBoxIcon.Warning); return; }

            if (!float.TryParse(txtLimiteDeltaP.Text.Trim(), NumberStyles.Float, CultureInfo.InvariantCulture, out float dp))
            { MessageBox.Show("Limite Delta P invalido.", "Atencao", MessageBoxButtons.OK, MessageBoxIcon.Warning); return; }

            if (!float.TryParse(txtLimiteUmidade.Text.Trim(), NumberStyles.Float, CultureInfo.InvariantCulture, out float um) || um < 0 || um > 100)
            { MessageBox.Show("Limite Umidade invalido (0-100%).", "Atencao", MessageBoxButtons.OK, MessageBoxIcon.Warning); return; }

            if (!int.TryParse(txtLimiteChuva.Text.Trim(), out int ch) || ch < 0 || ch > 1023)
            { MessageBox.Show("Limite Chuva invalido (0-1023).", "Atencao", MessageBoxButtons.OK, MessageBoxIcon.Warning); return; }

            var profile = new Profile { DeviceType = "SafetyMonitor" };

            hardware.ComPort       = comboBoxComPort.SelectedItem.ToString();
            hardware.Altitude      = alt;
            hardware.LimiteIR      = ir;
            hardware.TimeoutNublado= to;
            hardware.LimiteDeltaP  = dp;
            hardware.LimiteUmidade = um;
            hardware.LimiteChuva   = ch;

            profile.WriteValue(DRIVER_ID, "COMPort",        hardware.ComPort, "");
            profile.WriteValue(DRIVER_ID, "Altitude",       alt.ToString(), "");
            profile.WriteValue(DRIVER_ID, "LimiteIR",       ir.ToString("F1", CultureInfo.InvariantCulture), "");
            profile.WriteValue(DRIVER_ID, "TimeoutNublado", to.ToString(), "");
            profile.WriteValue(DRIVER_ID, "LimiteDeltaP",   dp.ToString("F1", CultureInfo.InvariantCulture), "");
            profile.WriteValue(DRIVER_ID, "LimiteUmidade",  um.ToString("F1", CultureInfo.InvariantCulture), "");
            profile.WriteValue(DRIVER_ID, "LimiteChuva",    ch.ToString(), "");

            DialogResult = DialogResult.OK;
            Close();
        }

        private void CmdCancel_Click(object sender, EventArgs e) { DialogResult = DialogResult.Cancel; Close(); }
        private void BrowseToAscom(object sender, EventArgs e) => System.Diagnostics.Process.Start("https://ascom-standards.org/");
        private void comboBoxComPort_SelectedIndexChanged(object sender, EventArgs e) { }
        private void chkTrace_CheckedChanged(object sender, EventArgs e) { }
        private void txtAltitude_TextChanged(object sender, EventArgs e) { }
        private void label2_Click(object sender, EventArgs e) { }
        private void label3_Click(object sender, EventArgs e) { }
        private void Label1_Click(object sender, EventArgs e) { }
    }
}
