; — Ziller Safety Monitor ASCOM Driver Installer —
; Generated and edited by Eduardo Ziller on 05/05/2025

[Setup]
; require admin for COM registration/unregistration
PrivilegesRequired=admin

AppID={{bb813ac2-2d0c-492e-94a8-c30e16aae2b3}}
AppName=ASCOM Ziller Safety Monitor SafetyMonitor Driver
AppVerName=ASCOM Ziller Safety Monitor SafetyMonitor Driver 7.0.2
AppVersion=7.0.2
AppPublisher=Eduardo Ziller <eduardo.ziller@gmail.com>
AppPublisherURL=mailto:eduardo.ziller@gmail.com
AppSupportURL=https://ascomtalk.groups.io/g/Help
AppUpdatesURL=https://ascom-standards.org/
VersionInfoVersion=1.0.0
MinVersion=6.1.7601

; Put there by Platform if Driver Installer Support selected
WizardImageFile="C:\Program Files (x86)\ASCOM\Platform 6 Developer Components\Installer Generator\Resources\WizardImage.bmp"
LicenseFile="C:\Program Files (x86)\ASCOM\Platform 6 Developer Components\Installer Generator\Resources\CreativeCommons.txt"
; {cf}\ASCOM\Uninstall\SafetyMonitor folder created by Platform, always
UninstallFilesDir="{cf}\ASCOM\Uninstall\SafetyMonitor\Ziller Safety Monitor"

; install into Program Files (x86)\ASCOM\SafetyMonitor
DefaultDirName={cf}\ASCOM\SafetyMonitor\ZillerSafetyMonitor
DisableDirPage=yes
DisableProgramGroupPage=yes

OutputDir="."
OutputBaseFilename=Ziller Safety Monitor Setup
Compression=lzma
SolidCompression=yes

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Files]
; your  AnyCPU Local-Server EXE
Source: "C:\Users\duzil\OneDrive\Documents\Astronomia\NINA-ASCOM-Safety-Monitor\ZillerSafetyMonitor\bin\Release\ASCOM.EZZillerSafetyMonitor.exe"; DestDir: "{app}"; Flags: ignoreversion
; your README
Source: "C:\Users\duzil\OneDrive\Documents\Astronomia\NINA-ASCOM-Safety-Monitor\ZillerSafetyMonitor\README.txt"; DestDir: "{app}"; Flags: isreadme

[Run]
Filename: "{app}\ASCOM.EZZillerSafetyMonitor.exe"; Parameters: "/register"; Description: "Registering ASCOM driver…"; Flags: runhidden waituntilterminated

[UninstallRun]
Filename: "{app}\ASCOM.EZZillerSafetyMonitor.exe"; Parameters: "/unregister"; Flags: runhidden waituntilterminated

[Registry]
; 1) Para clientes 32-bit (WOW6432Node)
Root: HKLM; Subkey: "SOFTWARE\WOW6432Node\ASCOM\SafetyMonitor Drivers\ASCOM.EZZillerSafetyMonitor.SafetyMonitor"; \
     ValueName: ""; ValueType: string; ValueData: "Ziller Safety Monitor"; Flags: uninsdeletekey

; 2) Para clientes 64-bit
Root: HKLM64; Subkey: "SOFTWARE\ASCOM\SafetyMonitor Drivers\ASCOM.EZZillerSafetyMonitor.SafetyMonitor"; \
        ValueName: ""; ValueType: string; ValueData: "Ziller Safety Monitor"; Flags: uninsdeletekey

[Code]
const
  REQUIRED_PLATFORM_VERSION = 6.2;

function PlatformVersion(): Double;
var
  S: String;
begin
  Result := 0.0;
  if RegQueryStringValue(HKEY_LOCAL_MACHINE_32, 'Software\ASCOM', 'PlatformVersion', S) then
  begin
    try
      Result := StrToFloat(S);
    except
      Result := 0.0;
    end;
  end;
end;

function InitializeSetup(): Boolean;
var
  PV: Double;
begin
  PV := PlatformVersion();
  if PV < REQUIRED_PLATFORM_VERSION then
  begin
    if PV = 0.0 then
      MsgBox(
        'No ASCOM Platform detected. Please install Platform ' +
        Format('%.1f', [REQUIRED_PLATFORM_VERSION]) +
        ' or later from https://ascom-standards.org',
        mbCriticalError, MB_OK
      )
    else
      MsgBox(
        'ASCOM Platform ' + Format('%.1f', [PV]) +
        ' is installed, but this driver requires ' +
        Format('%.1f', [REQUIRED_PLATFORM_VERSION]) +
        ' or later. Please update your ASCOM Platform.',
        mbCriticalError, MB_OK
      );
    Result := False;
  end
  else
    Result := True;
end;