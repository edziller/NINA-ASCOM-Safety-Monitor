// SetupDialogForm.cs - leitura e salvamento da COMPort
using ASCOM.Utilities;
using System;
using System.IO.Ports;
using System.Windows.Forms;

namespace ASCOM.EZZillerSafetyMonitor
{
    public partial class SetupDialogForm : Form
    {
        private SafetyMonitorHardware hardware;

        public SetupDialogForm(SafetyMonitorHardware hw)
        {
            InitializeComponent();
            hardware = hw;
        }

        private void SetupDialogForm_Load(object sender, EventArgs e)
        {
            comboBoxComPort.Items.AddRange(SerialPort.GetPortNames());
            if (comboBoxComPort.Items.Contains(hardware.ComPort))
            {
                comboBoxComPort.SelectedItem = hardware.ComPort;
            }
            else if (comboBoxComPort.Items.Count > 0)
            {
                comboBoxComPort.SelectedIndex = 0;
            }
        }
        private void CmdOK_Click(object sender, EventArgs e)
        {
            if (comboBoxComPort.SelectedItem != null)
            {
                Profile p = new Profile();
                p.DeviceType = "SafetyMonitor";
                p.WriteValue("ASCOM.EZZillerSafetyMonitor.SafetyMonitor", "COMPort", comboBoxComPort.SelectedItem.ToString());
            }

            this.DialogResult = DialogResult.OK;
            this.Close();
        }

        private void CmdCancel_Click(object sender, EventArgs e)
        {
            this.DialogResult = DialogResult.Cancel;
            this.Close();
        }

        private void BrowseToAscom(object sender, EventArgs e)
        {
            System.Diagnostics.Process.Start("https://ascom-standards.org/");
        }

        
        private void ButtonOK_Click(object sender, EventArgs e)
        {
            if (comboBoxComPort.SelectedItem != null)
            {
                hardware.ComPort = comboBoxComPort.SelectedItem.ToString();
                Profile p = new Profile();
                p.DeviceType = "SafetyMonitor";
                p.WriteValue("ASCOM.EZZillerSafetyMonitor.SafetyMonitor", "COMPort", hardware.ComPort);
            }
            DialogResult = DialogResult.OK;
            Close();
        }

        private void ButtonCancel_Click(object sender, EventArgs e)
        {
            DialogResult = DialogResult.Cancel;
            Close();
        }

        private void Label1_Click(object sender, EventArgs e)
        {

        }

        private void comboBoxComPort_SelectedIndexChanged(object sender, EventArgs e)
        {

        }

        private void chkTrace_CheckedChanged(object sender, EventArgs e)
        {

        }

        private void label2_Click(object sender, EventArgs e)
        {

        }
    }
}
