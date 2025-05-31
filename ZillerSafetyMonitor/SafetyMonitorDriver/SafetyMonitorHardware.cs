// SafetyMonitorHardware.cs atualizado com propriedade pública ComPort
using ASCOM.Utilities;
using System;
using System.Globalization;
using System.IO.Ports;
using Timers = System.Timers;

namespace ASCOM.EZZillerSafetyMonitor
{
    public class SafetyMonitorHardware
    {
        private SerialPort port;
        public SerialPort Port => port;
        private Timers.Timer pollTimer;
        private bool safe = true;
        private TraceLogger logger;
        private string lastLine = "";

        public string ComPort { get; set; } = "";
        public int Altitude { get; set; }

        // Propriedades expostas para MonitorForm
        public float Temperatura { get; set; }
        public float Umidade { get; set; }
        public float PontoOrvalho { get; set; }
        public float Pressao { get; set; }
        public float PressaoNominal { get; set; }
        public float DeltaP { get; set; }
        public float TemperaturaIR { get; set; }
        public string EstadoCeu { get; set; } = "---";
        public string EstadoChuva { get; set; } = "---";
        public string StatusPressao { get; set; } = "---";

        public float LimiteIR { get; set; } = 2.0f;
        public int LimiteChuva { get; set; } = 890;
        public float LimiteDeltaP { get; set; } = -5.0f;
        public float LimiteUmidade { get; set; } = 85.0f;

        public SafetyMonitorHardware()
        {
            logger = new TraceLogger("ZillerSafetyMonitor");
            logger.Enabled = true;
        }

        public void SetConnected(bool connected)
        {
            if (connected)
            {
                InitialiseHardware();
            }
            else
            {
                pollTimer?.Stop();
                pollTimer?.Dispose();
                pollTimer = null;

                if (port != null && port.IsOpen)
                {
                    port.Close();
                    port.Dispose();
                }
            }
        }

        public void InitialiseHardware()
        {
            // Carrega todos os parâmetros do ASCOM Profile
            var profile = new Profile { DeviceType = "SafetyMonitor" };
            // Porta
            string porta = profile.GetValue("ASCOM.EZZillerSafetyMonitor.SafetyMonitor", "COMPort", "");
            if (string.IsNullOrWhiteSpace(porta))
                throw new InvalidOperationException("Selecione uma porta COM.");
            ComPort = porta;

            // Altitude
            string altStr = profile.GetValue("ASCOM.EZZillerSafetyMonitor.SafetyMonitor", "Altitude", Altitude.ToString());
            if (int.TryParse(altStr, out int alt))
                Altitude = alt;

            // Abra o SerialPort
            port = new SerialPort(ComPort, 115200)
            {
                NewLine = "\n",
                ReadTimeout = 1000,
                DtrEnable = false,
                RtsEnable = false
            };
            port.Encoding = System.Text.Encoding.UTF8;
            port.Open();
            EnviarAltitude();

            // Inicie o timer de leitura
            pollTimer = new Timers.Timer(2000);
            pollTimer.Elapsed += PollTimer_Elapsed;
            pollTimer.Start();

        }
        public event EventHandler<string> LineReceived;
        private void PollTimer_Elapsed(object sender, Timers.ElapsedEventArgs e)
        {
            try
            {
                if (port != null && port.IsOpen)
                {
                    string line = port.ReadLine().Trim();
                    LineReceived?.Invoke(this, line);

                    // só log novos
                    if (line != lastLine)
                    {
                        lastLine = line;
                        logger.LogMessage("Serial", $"Recebido: {line}");
                    }

                    // aqui parser completo, incluindo SEG:
                    var parts = line.Split(new[] { ';' }, StringSplitOptions.RemoveEmptyEntries);
                    foreach (var p in parts)
                    {
                        if (p.StartsWith("SAFE") || p.StartsWith("UNSAFE"))
                        {
                            // em teoria não precisa mais
                        }
                        else if (p.StartsWith("SEG:"))
                        {
                            safe = p.Substring(4).Equals("SAFE", StringComparison.OrdinalIgnoreCase);
                        }
                    }
                }
            }
            catch (TimeoutException) { }
            catch (Exception ex)
            {
                logger.LogMessage("Erro na leitura serial", ex.Message);
            }
        }


        public bool IsSafe
        {
            get => safe;
            set => safe = value;
        }

        public void EnviarAltitude()
        {
            port.WriteLine($"ALT:{Altitude.ToString("F0", CultureInfo.InvariantCulture)}");
        }
        public void EnviarLimiteIR()
        {
            port.WriteLine($"LIMITE_IR:{LimiteIR.ToString("F1", CultureInfo.InvariantCulture)}");
        }

        public void EnviarLimiteChuva()
        {
            port.WriteLine($"LIMITE_CHUVA:{LimiteChuva.ToString(CultureInfo.InvariantCulture)}");
        }

        public void EnviarLimiteDeltaP()
        {
            port.WriteLine($"LIMITE_P:{LimiteDeltaP.ToString("F1", CultureInfo.InvariantCulture)}");
        }

        public void EnviarLimiteUmidade()
        {
            port.WriteLine($"LIMITE_U:{LimiteUmidade.ToString("F1", CultureInfo.InvariantCulture)}");
        }
    }
}