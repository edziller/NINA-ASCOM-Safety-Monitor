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
        private volatile bool _closingMonitor;

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

                // 1) abre/fecha a camada de hardware
                hardware.SetConnected(value);
                connected = value;

                if (value)
                {
                    // 2) incrementa o lock para manter o server vivo
                    Server.IncrementServerLockCount();

                    // 3) dispara UMA thread STA que irá criar o MonitorForm
                    _closingMonitor = false;
                    _monitorThread = new Thread(() =>
                    {
                        // Esta lambda roda num STA separado e mantém o form vivo
                        monitorForm = new MonitorForm(hardware);
                        monitorForm.FormClosed += (s, e) =>
                        {
                            // Quando o usuário fechar o form, libera o lock
                            Server.DecrementServerLockLock();
                            Application.ExitThread();
                        };
                        Application.Run(monitorForm);
                    });
                    _monitorThread.SetApartmentState(ApartmentState.STA);
                    _monitorThread.IsBackground = true;
                    _monitorThread.Start();
                }
                else
                {
                    // 4) ao desconectar, pede pra fechar o form
                    _closingMonitor = true;
                    if (monitorForm != null && monitorForm.IsHandleCreated)
                    {
                        // Fecha o form no seu próprio thread de UI
                        monitorForm.BeginInvoke((Action)(() =>
                        {
                            if (!monitorForm.IsDisposed)
                                monitorForm.Close();
                        }));
                    }
                    // Limpa a referência da thread
                    _monitorThread = null;
                }
            }
        }

        public bool IsSafe => hardware.IsSafe;

        public void SetupDialog()
        {
            using (var f = new SetupDialogForm(hardware))
            {
                if (f.ShowDialog() == DialogResult.OK)
                {
                    // Desliga e religa para forçar re-leitura de COMPort e Altitude
                    hardware.SetConnected(false);
                    hardware.SetConnected(true);
                }
            }
        }

        public string Description => "Ziller Safety Monitor via ESP8266";
        public string DriverInfo => "Driver ASCOM EZ para ESP8266 - v1.0";
        public string DriverVersion => "1.0";
        public short InterfaceVersion => 1;
        public string Name => "Ziller Safety Monitor";

        public void Dispose() { }

        public void CommandBlind(string command, bool raw) => throw new MethodNotImplementedException();
        public bool CommandBool(string command, bool raw) => throw new MethodNotImplementedException();
        public string CommandString(string command, bool raw) => throw new MethodNotImplementedException();
        public string Action(string actionName, string actionParameters) => throw new ActionNotImplementedException(actionName);
        public ArrayList SupportedActions => new ArrayList();
    }
}
