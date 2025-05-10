// SafetyMonitorDriver.cs corrigido para usar SafetyMonitorHardware
using ASCOM.DeviceInterface;
using ASCOM.LocalServer;
using ASCOM.Utilities;
using System;
using System.Collections;
using System.Runtime.InteropServices;

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
                if (value != connected)
                {
                    hardware.SetConnected(value);
                    connected = value;
                }
            }
        }

        public bool IsSafe => hardware.IsSafe;

        public void SetupDialog()
        {
            using (var f = new SetupDialogForm(hardware))
            {
                f.ShowDialog();
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

