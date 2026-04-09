// FormOpcoesWifi.cs — v2
// Correções aplicadas:
//  - Polling do hardware já foi pausado pelo MonitorForm antes de abrir este form.
//    Aqui apenas inscrevemos/desinscrevemos DataReceived sem conflito com o timer.
//  - DataReceived: usa SerialPort.ReadExisting() em vez de ReadLine() para não bloquear.
//    Acumula dados em buffer e processa linhas completas.
//  - Timeout de scan: se o ESP não responder em 15s, habilita o botão e mostra erro.
//  - Senha pode conter vírgula: usa índice da PRIMEIRA vírgula corretamente.

using System;
using System.IO.Ports;
using System.Windows.Forms;

namespace ASCOM.EZZillerSafetyMonitor
{
    public partial class FormOpcoesWifi : Form
    {
        private SafetyMonitorHardware hardware;
        private SerialPort serialPort;
        private System.Windows.Forms.Timer timeoutTimer;
        private string receiveBuffer = "";

        public FormOpcoesWifi(SafetyMonitorHardware hw)
        {
            InitializeComponent();
            hardware   = hw;
            serialPort = hardware.Port;

            // FIX: DataReceived usa acumulação de buffer — não chama ReadLine()
            serialPort.DataReceived += SerialPort_DataReceived;

            btnConectar.Enabled = false;
            lblStatus.Text      = "Procurando redes...";

            // Timeout: se o scan não terminar em 15s, algo deu errado
            timeoutTimer = new System.Windows.Forms.Timer { Interval = 15000 };
            timeoutTimer.Tick += (s, e) =>
            {
                timeoutTimer.Stop();
                btnConectar.Enabled = true;
                lblStatus.Text      = "Scan expirou. Verifique a conexão com o dispositivo.";
            };
            timeoutTimer.Start();

            // Inicia o scan (o firmware usará scanNetworksAsync — não vai bloquear o ESP)
            serialPort.WriteLine("SCAN_WIFI");
        }

        protected override void OnFormClosed(FormClosedEventArgs e)
        {
            timeoutTimer?.Stop();
            timeoutTimer?.Dispose();
            // FIX: desinscreve o evento — o MonitorForm vai reinscrever o polling ao retornar
            serialPort.DataReceived -= SerialPort_DataReceived;
            base.OnFormClosed(e);
        }

        // ── Recepção de dados da serial ───────────────────────────────────────
        // FIX: ReadExisting() consome tudo que está disponível sem bloquear.
        //      O buffer acumula até ter uma linha completa com '\n'.
        private void SerialPort_DataReceived(object sender, SerialDataReceivedEventArgs e)
        {
            try
            {
                string novos = serialPort.ReadExisting();
                receiveBuffer += novos;

                // Processa todas as linhas completas no buffer
                int idx;
                while ((idx = receiveBuffer.IndexOf('\n')) >= 0)
                {
                    string linha = receiveBuffer.Substring(0, idx).Trim();
                    receiveBuffer = receiveBuffer.Substring(idx + 1);
                    ProcessarLinha(linha);
                }
            }
            catch
            {
                // Erros de leitura são ignorados silenciosamente
            }
        }

        private void ProcessarLinha(string linha)
        {
            if (linha.StartsWith("SSID:"))
            {
                string ssid = linha.Substring(5);
                if (ssid.Length > 0)
                {
                    Invoke((MethodInvoker)(() =>
                    {
                        if (!comboBoxSSID.Items.Contains(ssid))
                            comboBoxSSID.Items.Add(ssid);
                        lblStatus.Text = $"{comboBoxSSID.Items.Count} rede(s) encontrada(s)...";
                    }));
                }
            }
            else if (linha == "#")
            {
                // Fim do scan
                Invoke((MethodInvoker)(() =>
                {
                    timeoutTimer.Stop();
                    btnConectar.Enabled = true;
                    lblStatus.Text = comboBoxSSID.Items.Count > 0
                        ? $"{comboBoxSSID.Items.Count} rede(s) encontrada(s). Selecione e conecte."
                        : "Nenhuma rede encontrada.";
                }));
            }
            else if (linha == "SCANNING:1")
            {
                // O ESP sinalizou que iniciou o scan assíncrono — ok, aguardamos "#"
                Invoke((MethodInvoker)(() => lblStatus.Text = "Scan em progresso..."));
            }
        }

        // ── Botão Conectar ────────────────────────────────────────────────────
        private void btnConectar_Click(object sender, EventArgs e)
        {
            if (comboBoxSSID.SelectedItem == null)
            {
                MessageBox.Show("Por favor, selecione uma rede.", "Aviso",
                    MessageBoxButtons.OK, MessageBoxIcon.Warning);
                return;
            }

            string ssid  = comboBoxSSID.SelectedItem.ToString();
            string senha = txtSenha.Text;  // NÃO chama Trim() — senha pode ter espaços válidos

            // Envia: "WIFI:ssid,senha" — o firmware pega a primeira vírgula como separador
            serialPort.WriteLine($"WIFI:{ssid},{senha}");

            MessageBox.Show(
                "Credenciais enviadas.\nO dispositivo irá reiniciar e conectar à rede selecionada.",
                "Credenciais enviadas",
                MessageBoxButtons.OK,
                MessageBoxIcon.Information);

            this.Close();
        }

        private void FormOpcoesWifi_Load(object sender, EventArgs e) { }
    }
}
