// SafetyMonitorHardware.cs atualizado com propriedade pública ComPort
using ASCOM.Utilities;
using System;
using System.IO.Ports;
using Timers = System.Timers;

namespace ASCOM.EZZillerSafetyMonitor
{
    public class SafetyMonitorHardware
    {
        private SerialPort port;
        private Timers.Timer pollTimer;
        private bool safe = true;
        private TraceLogger logger;
        private string lastLine = "";

        public string ComPort { get; set; } = "";

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
            Profile profile = new Profile();
            profile.DeviceType = "SafetyMonitor";
            string portaLida = profile.GetValue("ASCOM.EZZillerSafetyMonitor.SafetyMonitor", "COMPort", "").Trim();

            if (string.IsNullOrWhiteSpace(portaLida))
            {
                logger.LogMessage("Inicialização", "Erro: Porta COM não foi definida. Verifique o SetupDialog.");
                throw new InvalidOperationException("A porta COM não foi definida. Execute o SetupDialog e selecione uma porta válida.");
            }

            ComPort = portaLida;
            logger.LogMessage("Inicialização", $"Tentando abrir porta: {ComPort}");

            try
            {
                port = new SerialPort(ComPort, 115200)
                {
                    DtrEnable = true,
                    RtsEnable = true,
                    NewLine = "\n",
                    ReadTimeout = 1000
                };

                port.Open();

                pollTimer = new Timers.Timer(2000);
                pollTimer.Elapsed += PollTimer_Elapsed;
                pollTimer.Start();

                logger.LogMessage("Inicialização", $"COM {ComPort} aberta com sucesso");
            }
            catch (Exception ex)
            {
                logger.LogMessage("Erro na inicialização da COM", ex.Message);
                throw;
            }
        }

        private void PollTimer_Elapsed(object sender, Timers.ElapsedEventArgs e)
        {
            try
            {
                if (port != null && port.IsOpen)
                {
                    string line = port.ReadLine().Trim().ToUpper();

                    if (line != lastLine)
                    {
                        lastLine = line;
                        logger.LogMessage("Serial", $"Recebido: {line}");
                    }

                    if (line == "SAFE")
                    {
                        safe = true;
                    }
                    else if (line == "UNSAFE")
                    {
                        safe = false;
                    }
                    else
                    {
                        logger.LogMessage("Serial", $"Ignorado: {line}");
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
            get
            {
                logger.LogMessage("IsSafe", safe ? "SAFE" : "UNSAFE");
                return safe;
            }
        }

    }
}
