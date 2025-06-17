(pt-br)
Versão: 1.0
Desenvolvedor: Eduardo Ziller
Tipo de dispositivo: Safety Monitor
Compatível com: ASCOM Platform 6.6 ou superior
https://github.com/edziller/NINA-ASCOM-Safety-Monitor

Descrição:
-----------
Este é um driver ASCOM para o monitoramento de segurança automatizado do céu, utilizando um ESP8266 conectado via porta serial. Ele interpreta os comandos SAFE e UNSAFE recebidos da estação meteorológica para informar softwares como o NINA sobre as condições de segurança para astrofotografia.

Funcionalidades:
----------------
- Leitura da porta serial (COM) configurável
- Interpretação de mensagens SAFE / UNSAFE
- Compatível com softwares de automação como NINA e ASCOM Diagnostics
- Interface de configuração para seleção da COMPort
- Armazena configurações via ASCOM Profile

Instalação do software:
-----------
1. Execute o instalador ZillerSafetyMonitor.exe com permissões de administrador. Siga as orientações do instalador.
2. Após a instalação, o driver estará disponível nos menus de seleção de dispositivos ASCOM
3. Conecte seu ESP8266 à porta correta e inicie o envio de mensagens SAFE / UNSAFE
4. No NINA ou ASCOM Diagnostics, selecione o driver "Ziller Safety Monitor" na categoria Safety Monitor
5. Selecione a altitude do seu observatório em metros
6. Configure a porta COM na janela de diálogo que será apresentada

Nota: Caso a porta COM não apareça disponível, é necessário instalar o CP210x USB to UART Bridge VCP Drivers disponóvel em https://www.silabs.com/developer-tools/usb-to-uart-bridge-vcp-drivers?tab=downloads. Normalmente esse driver já é pré-instalado quando se usa a Arduino IDE com a Board Manager do ESP8266 para compilação e carregamento do Firmware.

Requisitos:
-----------
- ASCOM Platform 6.6 ou superior
- .NET Framework 4.8
- Conexão serial ativa com o ESP8266

Hardware e Firmware:
--------------------

1. Baixe a Arduino IDE e instale no Board Manager a esp8266 by ESP Community
2. Abra o arquivo NINA-ASCOM-Safety-Monitor.ino
3. Conecte a placa ESP8266 no computador, selecione a COM correspondente e taxa de bits de 115200kbps
4. Inclua a SSID e senha do seu WIFI, caso queira monitorar as variáveis pelo navegador (linhas 65 e 66)
5. Carregue o código no ESP8266

Nota: podem ser usados outros hardwares para a aplicação, contudo, devem ser ajustadas as bibliotecas e código para uso adequado.

Contato:
--------
Desenvolvido por Eduardo Ziller  
Email: eduardo.ziller@gmail.com
Site: fotografiaeastronomia.blogspot.com.br
YouTube: Astronoziller
Instagram: eduardoziller

Mais informações:  
https://ascom-standards.org