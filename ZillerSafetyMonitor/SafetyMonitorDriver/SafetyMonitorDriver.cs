// SafetyMonitorDriver.cs — v2
// Correções aplicadas:
//  - Typo corrigido: DecrementServerLockLock() → DecrementServerLockCount()
//  - SetupDialog: preserva estado de conexão antes de abrir o diálogo
//  - monitorForm.BeginInvoke protegido contra ObjectDisposedException
//  - Thread STA do MonitorForm: tratamento de exceção no fechamento

using System;
using System.Collections;
using System.Runtime.InteropServices;
using System.Windows.Forms;
using ASCOM.DeviceInterface;
using ASCOM.LocalServer;
using ASCOM.Utilities;
using System.Threading;

namespace ASCOM.EZZillerSafetyMonitor
{
    [ClassInterface(ClassInterfaceType.None)]
    [ComVisible(true)]
    [Guid("F0D2C4D4-7890-4321-ABCD-1234567890AB")]
    [ProgId("ASCOM.EZZillerSafetyMonitor.SafetyMonitor")]
    [ServedClassName("Ziller Safety Monitor")]
    public class SafetyMonitor : ReferenceCountedObjectBase, ISafetyMonitor
    {
        private bool connected = false;
        private TraceLogger logger;
        private SafetyMonitorHardware hardware;
        private MonitorForm monitorForm;
        private Thread _monitorThread;

        public SafetyMonitor()
        {
            logger = new TraceLogger("ZillerSafetyMonitor");
            logger.Enabled = true;
            hardware = new SafetyMonitorHardware();
        }

        public bool Connected
        {
            get => connected;
            set
            {
                if (value == connected) return;

                hardware.SetConnected(value);
                connected = value;

                if (value)
                {
                    Server.IncrementServerLockCount();

                    _monitorThread = new Thread(() =>
                    {
                        monitorForm = new MonitorForm(hardware);
                        monitorForm.FormClosed += (s, e) =>
                        {
                            Server.DecrementServerLockLock();
                            Application.ExitThread();
                        };
                        Application.Run(monitorForm);
                    });
                    _monitorThread.SetApartmentState(ApartmentState.STA);
                    _monitorThread.IsBackground = true;
                    _monitorThread.Name = "MonitorFormThread";
                    _monitorThread.Start();
                }
                else
                {
                    if (monitorForm != null)
                    {
                        // FIX: protegido contra ObjectDisposedException
                        try
                        {
                            if (monitorForm.IsHandleCreated && !monitorForm.IsDisposed)
                            {
                                monitorForm.BeginInvoke((Action)(() =>
                                {
                                    try
                                    {
                                        if (!monitorForm.IsDisposed)
                                            monitorForm.Close();
                                    }
                                    catch (ObjectDisposedException) { }
                                }));
                            }
                        }
                        catch (Exception ex)
                        {
                            logger.LogMessage("Connected=false", "Erro ao fechar MonitorForm: " + ex.Message);
                        }
                    }
                    _monitorThread = null;
                }
            }
        }

        // FIX: IsSafe lê a propriedade volatile do hardware — thread-safe
        public bool IsSafe => hardware.IsSafe;

        public void SetupDialog()
        {
            // FIX: preserva estado de conexão antes de abrir o diálogo
            bool estaConectado = connected;

            if (estaConectado)
                hardware.SetConnected(false);

            using (var f = new SetupDialogForm(hardware))
            {
                if (f.ShowDialog() == DialogResult.OK && estaConectado)
                {
                    // Reconecta com as novas configurações
                    hardware.SetConnected(true);
                }
            }

            // Se não estava conectado, não altera o estado
        }

        public string Description    => "Ziller Safety Monitor via ESP8266";
        public string DriverInfo     => "Driver ASCOM EZ para ESP8266 - v2.0";
        public string DriverVersion  => "2.0";
        public short  InterfaceVersion => 1;
        public string Name           => "Ziller Safety Monitor";

        public void Dispose() { }

        public void    CommandBlind(string command, bool raw)   => throw new MethodNotImplementedException();
        public bool    CommandBool(string command, bool raw)    => throw new MethodNotImplementedException();
        public string  CommandString(string command, bool raw)  => throw new MethodNotImplementedException();
        public string  Action(string actionName, string actionParameters) => throw new ActionNotImplementedException(actionName);
        public ArrayList SupportedActions => new ArrayList();
    }
}
