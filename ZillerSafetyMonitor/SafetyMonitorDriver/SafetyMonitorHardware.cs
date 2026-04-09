// SafetyMonitorHardware.cs — v3
// Correções v3:
//  - SUBSTITUÍDO Timer+ReadLine() por SerialPort.DataReceived (elimina travamento)
//  - DataReceived acumula em buffer, processa apenas linhas completas
//  - PausePolling/ResumePolling des/reinscrevem DataReceived
//  - Profile.GetValue com 4 parâmetros (SubKey="", Default=valor)
//  - Todos os parâmetros lidos/salvos no ASCOM Profile

using ASCOM.Utilities;
using System;
using System.Globalization;
using System.IO.Ports;

namespace ASCOM.EZZillerSafetyMonitor
{
    public class SafetyMonitorHardware
    {
        private SerialPort port;
        public  SerialPort Port => port;
        private string _receiveBuffer = "";
        private volatile bool safe = false;
        private TraceLogger logger;
        private string lastLine = "";
        private const string DRIVER_ID = "ASCOM.EZZillerSafetyMonitor.SafetyMonitor";

        public string ComPort  { get; set; } = "";
        public int    Altitude { get; set; } = 1000;
        public float  Temperatura    { get; set; }
        public float  Umidade        { get; set; }
        public float  PontoOrvalho   { get; set; }
        public float  Pressao        { get; set; }
        public float  PressaoNominal { get; set; }
        public float  DeltaP         { get; set; }
        public float  TemperaturaIR  { get; set; }
        public string EstadoCeu      { get; set; } = "---";
        public string EstadoChuva    { get; set; } = "---";
        public string StatusPressao  { get; set; } = "---";
        public string TrendPressao   { get; set; } = "---";
        public string TrendUmidade   { get; set; } = "---";
        public string TrendCeu       { get; set; } = "---";
        public float LimiteIR       { get; set; } = 8.0f;
        public int   LimiteChuva    { get; set; } = 850;
        public float LimiteDeltaP   { get; set; } = 0.0f;
        public float LimiteUmidade  { get; set; } = 70.0f;
        public int   TimeoutNublado { get; set; } = 10;
        public event EventHandler<string> LineReceived;

        public SafetyMonitorHardware()
        {
            logger = new TraceLogger("ZillerSafetyMonitor");
            logger.Enabled = true;
        }

        public bool IsSafe => safe;

        public void SetConnected(bool connected)
        {
            if (connected) { InitialiseHardware(); return; }
            try
            {
                if (port != null) port.DataReceived -= Port_DataReceived;
                if (port != null && port.IsOpen) port.Close();
                port?.Dispose();
            }
            catch (Exception ex) { logger.LogMessage("SetConnected(false)", ex.Message); }
            finally { port = null; _receiveBuffer = ""; safe = false; }
        }

        public void InitialiseHardware()
        {
            var profile = new Profile { DeviceType = "SafetyMonitor" };

            string porta = profile.GetValue(DRIVER_ID, "COMPort", "", "");
            if (string.IsNullOrWhiteSpace(porta))
                throw new InvalidOperationException("Selecione uma porta COM em Setup.");
            ComPort = porta;

            string altStr = profile.GetValue(DRIVER_ID, "Altitude",       "", "1000");
            string irStr  = profile.GetValue(DRIVER_ID, "LimiteIR",       "", "8.0");
            string chStr  = profile.GetValue(DRIVER_ID, "LimiteChuva",    "", "850");
            string dpStr  = profile.GetValue(DRIVER_ID, "LimiteDeltaP",   "", "0.0");
            string umStr  = profile.GetValue(DRIVER_ID, "LimiteUmidade",  "", "70.0");
            string toStr  = profile.GetValue(DRIVER_ID, "TimeoutNublado", "", "10");

            Altitude       = int.TryParse(altStr, out int alt)   ? alt : 1000;
            LimiteIR       = float.TryParse(irStr,  System.Globalization.NumberStyles.Float, CultureInfo.InvariantCulture, out float ir) ? ir : 8.0f;
            LimiteChuva    = int.TryParse(chStr, out int ch)     ? ch  : 850;
            LimiteDeltaP   = float.TryParse(dpStr,  System.Globalization.NumberStyles.Float, CultureInfo.InvariantCulture, out float dp) ? dp : 0.0f;
            LimiteUmidade  = float.TryParse(umStr,  System.Globalization.NumberStyles.Float, CultureInfo.InvariantCulture, out float um) ? um : 70.0f;
            TimeoutNublado = int.TryParse(toStr, out int to)     ? to  : 10;

            port = new SerialPort(ComPort, 115200)
            {
                NewLine = "\n", DtrEnable = false, RtsEnable = false,
                Encoding = System.Text.Encoding.UTF8,
            };
            port.DataReceived += Port_DataReceived;
            port.Open();

            SolicitarParametros();
            EnviarLimiteIR(); EnviarTimeoutNublado(); EnviarLimiteChuva();
            EnviarLimiteDeltaP(); EnviarLimiteUmidade(); EnviarAltitude();
        }

        private void Port_DataReceived(object sender, SerialDataReceivedEventArgs e)
        {
            try
            {
                if (port == null || !port.IsOpen) return;
                _receiveBuffer += port.ReadExisting();
                int idx;
                while ((idx = _receiveBuffer.IndexOf('\n')) >= 0)
                {
                    string line = _receiveBuffer.Substring(0, idx).Trim();
                    _receiveBuffer = _receiveBuffer.Substring(idx + 1);
                    if (string.IsNullOrEmpty(line) || line == lastLine) continue;
                    lastLine = line;
                    logger.LogMessage("Serial", "Recebido: " + line);
                    ParseLine(line);
                    LineReceived?.Invoke(this, line);
                }
            }
            catch (Exception ex) { logger.LogMessage("DataReceived", ex.Message); }
        }

        private void ParseLine(string line)
        {
            foreach (var p in line.Split(new[] { ';' }, StringSplitOptions.RemoveEmptyEntries))
            {
                try
                {
                    if      (p.StartsWith("T:"))      Temperatura    = float.Parse(p.Substring(2), CultureInfo.InvariantCulture);
                    else if (p.StartsWith("U:"))      Umidade        = float.Parse(p.Substring(2), CultureInfo.InvariantCulture);
                    else if (p.StartsWith("TO:"))     PontoOrvalho   = float.Parse(p.Substring(3), CultureInfo.InvariantCulture);
                    else if (p.StartsWith("PN:"))     PressaoNominal = float.Parse(p.Substring(3), CultureInfo.InvariantCulture);
                    else if (p.StartsWith("P:"))      Pressao        = float.Parse(p.Substring(2), CultureInfo.InvariantCulture);
                    else if (p.StartsWith("DP:"))     DeltaP         = float.Parse(p.Substring(3), CultureInfo.InvariantCulture);
                    else if (p.StartsWith("IR:"))     TemperaturaIR  = float.Parse(p.Substring(3), CultureInfo.InvariantCulture);
                    else if (p.StartsWith("CEU:"))    EstadoCeu      = p.Substring(4);
                    else if (p.StartsWith("CHUVA:"))  EstadoChuva    = p.Substring(6);
                    else if (p.StartsWith("SP:"))     StatusPressao  = p.Substring(3);
                    else if (p.StartsWith("TP:"))     TrendPressao   = p.Substring(3);
                    else if (p.StartsWith("TU:"))     TrendUmidade   = p.Substring(3);
                    else if (p.StartsWith("TC:"))     TrendCeu       = p.Substring(3);
                    else if (p.StartsWith("SEG:"))    safe           = p.Substring(4).Equals("SAFE", StringComparison.OrdinalIgnoreCase);
                    else if (p.StartsWith("PARAMS:")) ParseParams(p.Substring(7));
                }
                catch { }
            }
        }

        private void ParseParams(string payload)
        {
            foreach (var kv in payload.Split(','))
            {
                var sep = kv.IndexOf(':'); if (sep < 0) continue;
                var key = kv.Substring(0, sep); var val = kv.Substring(sep + 1);
                try
                {
                    switch (key)
                    {
                        case "IR":    LimiteIR       = float.Parse(val, CultureInfo.InvariantCulture); break;
                        case "CHUVA": LimiteChuva    = int.Parse(val,   CultureInfo.InvariantCulture); break;
                        case "P":     LimiteDeltaP   = float.Parse(val, CultureInfo.InvariantCulture); break;
                        case "U":     LimiteUmidade  = float.Parse(val, CultureInfo.InvariantCulture); break;
                        case "TO":    TimeoutNublado = int.Parse(val,   CultureInfo.InvariantCulture); break;
                    }
                }
                catch { }
            }
        }

        public void PausePolling()
        {
            if (port != null) port.DataReceived -= Port_DataReceived;
        }

        public void ResumePolling()
        {
            if (port != null && port.IsOpen)
            {
                port.DataReceived -= Port_DataReceived;
                port.DataReceived += Port_DataReceived;
            }
        }

        private void Send(string cmd)
        {
            if (port == null || !port.IsOpen) return;
            try { port.WriteLine(cmd); }
            catch (Exception ex) { logger.LogMessage("Send", ex.Message); }
        }

        public void SolicitarParametros()  => Send("GET_PARAMS");

        // FIX: concatenação em vez de interpolação com aspas aninhadas
        // ($"...{x.ToString(\"F1\",...)}") é sintaxe inválida em C# — causa os erros de compilação
        public void EnviarAltitude()       => Send("ALT:"             + Altitude.ToString("F0", CultureInfo.InvariantCulture));
        public void EnviarLimiteIR()       => Send("LIMITE_IR:"       + LimiteIR.ToString("F1", CultureInfo.InvariantCulture));
        public void EnviarLimiteChuva()    => Send("LIMITE_CHUVA:"    + LimiteChuva.ToString(CultureInfo.InvariantCulture));
        public void EnviarLimiteDeltaP()   => Send("LIMITE_P:"        + LimiteDeltaP.ToString("F1", CultureInfo.InvariantCulture));
        public void EnviarLimiteUmidade()  => Send("LIMITE_U:"        + LimiteUmidade.ToString("F1", CultureInfo.InvariantCulture));
        public void EnviarTimeoutNublado() => Send("TIMEOUT_NUBLADO:" + TimeoutNublado.ToString(CultureInfo.InvariantCulture));

        // Persiste parâmetros calibráveis no ASCOM Profile
        public void SalvarParametrosProfile()
        {
            var profile = new Profile { DeviceType = "SafetyMonitor" };
            profile.WriteValue(DRIVER_ID, "LimiteIR",       LimiteIR.ToString("F1",  CultureInfo.InvariantCulture));
            profile.WriteValue(DRIVER_ID, "LimiteChuva",    LimiteChuva.ToString(    CultureInfo.InvariantCulture));
            profile.WriteValue(DRIVER_ID, "LimiteDeltaP",   LimiteDeltaP.ToString("F1",  CultureInfo.InvariantCulture));
            profile.WriteValue(DRIVER_ID, "LimiteUmidade",  LimiteUmidade.ToString("F1", CultureInfo.InvariantCulture));
            profile.WriteValue(DRIVER_ID, "TimeoutNublado", TimeoutNublado.ToString( CultureInfo.InvariantCulture));
        }
    }
}
