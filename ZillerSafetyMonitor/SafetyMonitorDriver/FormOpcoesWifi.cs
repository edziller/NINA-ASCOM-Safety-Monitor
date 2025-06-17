using System;
using System.IO.Ports;
using System.Windows.Forms;

namespace ASCOM.EZZillerSafetyMonitor
{
    public partial class FormOpcoesWifi : Form
    {
        private SafetyMonitorHardware hardware;
        private SerialPort serialPort;

        public FormOpcoesWifi(SafetyMonitorHardware hw)
        {
            InitializeComponent();
            hardware = hw;

            // 1. Reutiliza a porta já aberta
            serialPort = hardware.Port;

            // 2. Inscreve‐se no evento de dados
            serialPort.DataReceived += SerialPort_DataReceived;

            // 3. Desabilita conectar até terminar o scan
            btnConectar.Enabled = false;

            // 4. Inicia o scan de redes no ESP
            serialPort.WriteLine("SCAN_WIFI");
        }

        // 5. Desinscreve o evento ao fechar o form
        protected override void OnFormClosed(FormClosedEventArgs e)
        {
            serialPort.DataReceived -= SerialPort_DataReceived;
            base.OnFormClosed(e);
        }

        // 6. Recebe cada linha vinda do ESP
        private void SerialPort_DataReceived(object sender, SerialDataReceivedEventArgs e)
        {
            try
            {
                string linha = serialPort.ReadLine().Trim();

                if (linha.StartsWith("SSID:"))
                {
                    string ssid = linha.Substring(5);
                    // Atualiza comboBox na thread de UI
                    Invoke((MethodInvoker)(() =>
                    {
                        if (!comboBoxSSID.Items.Contains(ssid))
                            comboBoxSSID.Items.Add(ssid);
                    }));
                }
                else if (linha == "#")
                {
                    // fim do scan: habilita botão conectar
                    Invoke((MethodInvoker)(() => btnConectar.Enabled = true));
                }
            }
            catch
            {
                // podemos ignorar erros de parsing
            }
        }

        // 7. Botão “Conectar” envia SSID/senha e fecha o form
        private void btnConectar_Click(object sender, EventArgs e)
        {
            if (comboBoxSSID.SelectedItem == null)
            {
                MessageBox.Show("Por favor, selecione uma rede.", "Aviso", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                return;
            }

            string ssid = comboBoxSSID.SelectedItem.ToString();
            string senha = txtSenha.Text.Trim();

            // Envia ao firmware e mostra mensagem
            serialPort.WriteLine($"WIFI:{ssid},{senha}");
            MessageBox.Show("Credenciais enviadas. O ESP irá reiniciar e conectar.", "OK", MessageBoxButtons.OK, MessageBoxIcon.Information);

            this.Close();
        }

        private void FormOpcoesWifi_Load(object sender, EventArgs e)
        {

        }
    }
}
