; ============================================================================
; Ziller Safety Monitor — ASCOM Driver Installer
; Eduardo Ziller, Belo Horizonte, Brasil
; https://github.com/edziller/NINA-ASCOM-Safety-Monitor
;
; Para compilar:
;   1. Coloque este arquivo na pasta raiz do projeto (ZillerSafetyMonitor\)
;   2. Coloque license.txt na mesma pasta deste .iss
;   3. Compile com Inno Setup 6 ou superior
;
; Estrutura esperada de pastas relativas a este arquivo:
;   .\license.txt
;   .\README.txt
;   .\bin\Release\ASCOM.EZZillerSafetyMonitor.exe
; ============================================================================

#define MyAppName      "Ziller Safety Monitor"
#define MyAppVersion   "2.0"
#define MyAppPublisher "Eduardo Ziller"
#define MyAppURL       "https://github.com/edziller/NINA-ASCOM-Safety-Monitor"
#define MyAppExeName   "ASCOM.EZZillerSafetyMonitor.exe"
#define MyAppID        "bb813ac2-2d0c-492e-94a8-c30e16aae2b3"

; ============================================================================
[Setup]
; ── Identificação ─────────────────────────────────────────────────────────────
AppID                = {{{#MyAppID}}
AppName              = {#MyAppName}
AppVerName           = {#MyAppName} {#MyAppVersion}
AppVersion           = {#MyAppVersion}
AppPublisher         = {#MyAppPublisher}
AppPublisherURL      = {#MyAppURL}
AppSupportURL        = {#MyAppURL}/issues
AppUpdatesURL        = {#MyAppURL}/releases
AppCopyright         = Copyright (C) 2025 {#MyAppPublisher}

; ── Informações de versão no executável do instalador ─────────────────────────
VersionInfoVersion         = 2.0.0.0
VersionInfoCompany         = {#MyAppPublisher}
VersionInfoDescription     = {#MyAppName} ASCOM Driver Installer
VersionInfoCopyright       = Copyright (C) 2025 {#MyAppPublisher}
VersionInfoProductName     = {#MyAppName}
VersionInfoProductVersion  = {#MyAppVersion}

; ── Requisitos ────────────────────────────────────────────────────────────────
PrivilegesRequired = admin

; ── Tela de licença: gera a página "Eu aceito" automaticamente ────────────────
; license.txt deve estar na mesma pasta deste .iss
LicenseFile = license.txt

; ── Imagem lateral do assistente (opcional) ───────────────────────────────────
; Se você tiver os ASCOM Developer Components instalados, descomente a linha
; abaixo para usar a imagem padrão da ASCOM:
;WizardImageFile = "C:\Program Files (x86)\ASCOM\Platform 6 Developer Components\Installer Generator\Resources\WizardImage.bmp"
;
; Alternativa: coloque um BMP seu (164x314 pixels, 256 cores) e referencie:
;WizardImageFile = wizard.bmp
;WizardSmallImageFile = wizard_small.bmp

; ── Destino de instalação ─────────────────────────────────────────────────────
; {cf} = C:\ProgramData   (compatível com todos os usuários)
DefaultDirName       = {cf}\ASCOM\SafetyMonitor\ZillerSafetyMonitor
DisableDirPage       = yes
DisableProgramGroupPage = yes

; Pasta de desinstalação padrão ASCOM
UninstallFilesDir    = {cf}\ASCOM\Uninstall\SafetyMonitor\{#MyAppName}

; ── Saída ─────────────────────────────────────────────────────────────────────
; Caminhos relativos: o .exe do instalador será gerado na mesma pasta do .iss
OutputDir            = .
OutputBaseFilename   = ZillerSafetyMonitor_Setup_v{#MyAppVersion}
Compression          = lzma2/ultra64
SolidCompression     = yes
InternalCompressLevel= ultra

; ── Ícone do instalador ───────────────────────────────────────────────────────
; Descomente se tiver um .ico disponível na pasta:
;SetupIconFile = icon.ico

; ============================================================================
[Languages]
; Idioma padrão: inglês (necessário para ASCOM)
Name: "english";            MessagesFile: "compiler:Default.isl"
; Português brasileiro (se o Inno Setup estiver instalado com o pacote de idiomas)
Name: "brazilianportuguese"; MessagesFile: "compiler:Languages\BrazilianPortuguese.isl"

; ============================================================================
[Files]
; Executável principal do driver (LocalServer ASCOM)
; Caminho relativo ao .iss — ajuste se a estrutura de pastas for diferente
Source: "bin\Release\{#MyAppExeName}"; \
        DestDir: "{app}"; \
        Flags: ignoreversion

; README — exibido ao final da instalação
Source: "README.txt"; \
        DestDir: "{app}"; \
        Flags: isreadme ignoreversion

; Licença — também instalada junto com o programa para referência futura
Source: "license.txt"; \
        DestDir: "{app}"; \
        Flags: ignoreversion

; ============================================================================
[Icons]
; Atalho no Menu Iniciar (opcional — ASCOM drivers geralmente não criam atalhos)
; Name: "{group}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"
; Name: "{group}\Desinstalar {#MyAppName}"; Filename: "{uninstallexe}"

; ============================================================================
[Run]
; Registra o driver COM após a instalação
Filename: "{app}\{#MyAppExeName}"; \
          Parameters: "/register"; \
          Description: "Registering ASCOM driver..."; \
          Flags: runhidden waituntilterminated

; Abre o README ao final (opcional — já é aberto automaticamente pelo flag isreadme)
; Filename: "notepad.exe"; Parameters: "{app}\README.txt"; \
;           Description: "View README"; Flags: postinstall skipifsilent

; ============================================================================
[UninstallRun]
; Remove o registro COM ao desinstalar
Filename: "{app}\{#MyAppExeName}"; \
          Parameters: "/unregister"; \
          Flags: runhidden waituntilterminated

; ============================================================================
[Registry]
; Registra o driver nas chaves ASCOM para que clientes como NINA o encontrem

; Para aplicações 32-bit (WOW6432Node)
Root: HKLM; \
      Subkey: "SOFTWARE\WOW6432Node\ASCOM\SafetyMonitor Drivers\ASCOM.EZZillerSafetyMonitor.SafetyMonitor"; \
      ValueName: ""; ValueType: string; ValueData: "{#MyAppName}"; \
      Flags: uninsdeletekey createvalueifdoesntexist

; Para aplicações 64-bit
Root: HKLM64; \
      Subkey: "SOFTWARE\ASCOM\SafetyMonitor Drivers\ASCOM.EZZillerSafetyMonitor.SafetyMonitor"; \
      ValueName: ""; ValueType: string; ValueData: "{#MyAppName}"; \
      Flags: uninsdeletekey createvalueifdoesntexist

; ============================================================================
[Code]

{ ── Constante: versão mínima do ASCOM Platform exigida ───────────────────── }
const
  REQUIRED_PLATFORM_VERSION = 6.6;   { Exige Platform 6.6 (conforme README) }

{ ── Lê a versão instalada do ASCOM Platform ──────────────────────────────── }
function PlatformVersion(): Double;
var
  S: String;
begin
  Result := 0.0;
  { Tenta chave 32-bit primeiro, depois 64-bit }
  if not RegQueryStringValue(HKEY_LOCAL_MACHINE_32, 'Software\ASCOM', 'PlatformVersion', S) then
    RegQueryStringValue(HKEY_LOCAL_MACHINE, 'Software\ASCOM', 'PlatformVersion', S);
  if S <> '' then
  begin
    try
      Result := StrToFloat(S);
    except
      Result := 0.0;
    end;
  end;
end;

{ ── Verifica o ASCOM Platform antes de iniciar o assistente ──────────────── }
function InitializeSetup(): Boolean;
var
  PV: Double;
begin
  PV := PlatformVersion();

  if PV = 0.0 then
  begin
    MsgBox(
      'ASCOM Platform not detected on this computer.' + #13#10 + #13#10 +
      'The Ziller Safety Monitor requires ASCOM Platform ' +
      Format('%.1f', [REQUIRED_PLATFORM_VERSION]) + ' or later.' + #13#10 + #13#10 +
      'Please download and install it from:' + #13#10 +
      'https://ascom-standards.org/Downloads/Index.htm' + #13#10 + #13#10 +
      'Setup will now exit.',
      mbCriticalError, MB_OK
    );
    Result := False;
    Exit;
  end;

  if PV < REQUIRED_PLATFORM_VERSION then
  begin
    MsgBox(
      'ASCOM Platform ' + Format('%.1f', [PV]) + ' is installed,' + #13#10 +
      'but this driver requires Platform ' +
      Format('%.1f', [REQUIRED_PLATFORM_VERSION]) + ' or later.' + #13#10 + #13#10 +
      'Please update your ASCOM Platform from:' + #13#10 +
      'https://ascom-standards.org/Downloads/Index.htm' + #13#10 + #13#10 +
      'Setup will now exit.',
      mbCriticalError, MB_OK
    );
    Result := False;
    Exit;
  end;

  { Platform OK — prossegue para a tela de licença e instalação }
  Result := True;
end;

{ ── Mensagem de conclusão personalizada ──────────────────────────────────── }
procedure CurStepChanged(CurStep: TSetupStep);
begin
  if CurStep = ssDone then
  begin
    MsgBox(
      '{#MyAppName} {#MyAppVersion} installed successfully!' + #13#10 + #13#10 +
      'To use the driver:' + #13#10 +
      '  1. Connect the ESP8266 to a USB port' + #13#10 +
      '  2. Open NINA (or any ASCOM client)' + #13#10 +
      '  3. Select "Ziller Safety Monitor" as the Safety Monitor device' + #13#10 +
      '  4. Click Setup to choose the COM port and configure parameters' + #13#10 + #13#10 +
      'For help and source code:' + #13#10 +
      '{#MyAppURL}',
      mbInformation, MB_OK
    );
  end;
end;
