using ASCOM.Utilities;
using System;
using System.IO.Ports;
using System.Windows.Forms;

namespace ASCOM.EZZillerSafetyMonitor
{
    public partial class SetupDialogForm : Form
    {
        private readonly SafetyMonitorHardware hardware;

        public SetupDialogForm(SafetyMonitorHardware hw)
        {
            InitializeComponent();
            hardware = hw;

            // 1) Carrega altitude salva (se houver) do ASCOM Profile
            var profile = new Profile { DeviceType = "SafetyMonitor" };
            string altSalva = profile.GetValue("ASCOM.EZZillerSafetyMonitor.SafetyMonitor", "Altitude", "1000");
            txtAltitude.Text = altSalva;
        }

        private void SetupDialogForm_Load(object sender, EventArgs e)
        {
            // 2) Popular lista de COMs
            comboBoxComPort.Items.Clear();
            comboBoxComPort.Items.AddRange(SerialPort.GetPortNames());
            if (!string.IsNullOrEmpty(hardware.ComPort) && comboBoxComPort.Items.Contains(hardware.ComPort))
                comboBoxComPort.SelectedItem = hardware.ComPort;
            else if (comboBoxComPort.Items.Count > 0)
                comboBoxComPort.SelectedIndex = 0;
        }

        private void CmdOK_Click(object sender, EventArgs e)
        {
            var profile = new Profile { DeviceType = "SafetyMonitor" };

            // 1) COM port
            if (comboBoxComPort.SelectedItem != null)
            {
                hardware.ComPort = comboBoxComPort.SelectedItem.ToString();
                profile.WriteValue("ASCOM.EZZillerSafetyMonitor.SafetyMonitor", "COMPort", hardware.ComPort);
            }

            // 2) Altitude
            if (int.TryParse(txtAltitude.Text.Trim(), out int alt))
            {
                hardware.Altitude = alt;
                profile.WriteValue("ASCOM.EZZillerSafetyMonitor.SafetyMonitor", "Altitude", alt.ToString());
            }

            DialogResult = DialogResult.OK;
            Close();
        }

        private void CmdCancel_Click(object sender, EventArgs e)
        {
            DialogResult = DialogResult.Cancel;
            Close();
        }

        // Esses métodos ficam vazios ou podem ser removidos se não fizerem nada útil:
        private void BrowseToAscom(object sender, EventArgs e) =>
            System.Diagnostics.Process.Start("https://ascom-standards.org/");
        private void comboBoxComPort_SelectedIndexChanged(object sender, EventArgs e) { }
        private void chkTrace_CheckedChanged(object sender, EventArgs e) { }
        private void txtAltitude_TextChanged(object sender, EventArgs e) { }
        private void label2_Click(object sender, EventArgs e) { }
        private void label3_Click(object sender, EventArgs e) { }
        private void Label1_Click(object sender, EventArgs e) { }
    }
}
