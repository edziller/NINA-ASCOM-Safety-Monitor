Versão: 1.0
Desenvolvedor: Eduardo Ziller
Tipo de dispositivo: Safety Monitor
Compatível com: ASCOM Platform 6.6 ou superior

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

Instalação:
-----------
1. Execute o instalador gerado (.exe)
2. Após a instalação, o driver estará disponível nos menus de seleção de dispositivos ASCOM
3. No NINA ou ASCOM Diagnostics, selecione o driver "Ziller Safety Monitor" na categoria Safety Monitor
4. Configure a porta COM na janela de diálogo que será apresentada
5. Conecte seu ESP8266 à porta correta e inicie o envio de mensagens SAFE / UNSAFE

Requisitos:
-----------
- ASCOM Platform 6.6 ou superior
- .NET Framework 4.8
- Conexão serial ativa com o ESP8266
- Inno Setup (caso deseje recompilar o instalador)

Contato:
--------
Desenvolvido por Eduardo Ziller  
Email: eduardo.ziller@gmail.com

Mais informações:  
https://ascom-standards.org