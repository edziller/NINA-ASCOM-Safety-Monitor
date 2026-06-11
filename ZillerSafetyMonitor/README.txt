Ziller Safety Monitor
Driver ASCOM para monitoramento meteorológico de observatório remoto
=====================================================================

Versão:      2.0
Desenvolvedor: Eduardo Ziller — Belo Horizonte, Brasil
GitHub:      https://github.com/edziller/NINA-ASCOM-Safety-Monitor
Instagram:   @eduardoziller
YouTube:     Astronoziller
Email:       eduardo.ziller@gmail.com

Compatível com: ASCOM Platform 6.6 ou superior / NINA 3.x ou superior

─────────────────────────────────────────────────────────────────────
DESCRIÇÃO
─────────────────────────────────────────────────────────────────────
O Ziller Safety Monitor é um driver ASCOM do tipo Safety Monitor
desenvolvido para proteger automaticamente observatórios astronômicos
remotos contra condições meteorológicas adversas.

O hardware é baseado em um microcontrolador ESP8266 (NodeMCU) com
duas PCBs dedicadas, conectado aos seguintes sensores:

  Versão v5-BME (recomendada):
    • MLX90614  — temperatura IR do fundo do céu (detecção de nuvens)
    • BME280    — temperatura, umidade e pressão atmosférica

  Versão v4-HTU:
    • MLX90614  — temperatura IR do fundo do céu
    • HTU21D    — temperatura e umidade relativa do ar
    • BMP280    — pressão atmosférica

  Versão v4-DHT (legado):
    • MLX90614  — temperatura IR do fundo do céu
    • DHT11     — temperatura e umidade (1-wire)
    • BMP280    — pressão atmosférica

  Todos os sensores:
    • Sensor analógico de chuva resistivo

O firmware processa continuamente as leituras e determina se a
condição é SAFE ou UNSAFE com base em três lógicas combinadas:

  1. Chuva detectada → UNSAFE imediato
  2. Céu nublado + umidade alta + pressão baixa → UNSAFE imediato
  3. Céu nublado + umidade alta por tempo > Timeout → UNSAFE

Comportamento fail-safe: qualquer falha de sensor força UNSAFE.
O sistema mantém UNSAFE até que todas as variáveis sejam seguras.

Quando UNSAFE, o NINA interrompe a sequência de captura, fecha o
teto do observatório e desliga os equipamentos automaticamente.

─────────────────────────────────────────────────────────────────────
NOVIDADES DA VERSÃO 2.0
─────────────────────────────────────────────────────────────────────
Hardware e firmware:
  • PCB dedicada em duas placas (ESP 50x70mm + Sensores 40x45mm)
    com pull-ups I²C, capacitores de desacoplamento e proteção ESD
  • Suporte ao BME280 (v5) — um único sensor para T, U e P,
    eliminando o DHT11/HTU21D e aumentando a estabilidade I²C
  • Recuperação automática do barramento I²C:
      - 9 pulsos SCL para liberar sensor travado
      - Reinicialização completa do Wire e dos sensores
      - Reinício automático do ESP após 3 tentativas sem sucesso
  • Clock I²C: 25 kHz + clock stretch limit de 25 ms
  • Log de eventos I²C em arquivo CSV com timestamp
  • Tendência suavizada: média de 6 amostras vs 15 min atrás
    (elimina falsas tendências por ruído de sensor)
  • Extremos de temperatura e umidade do dia na página web
  • Watchdog de hardware no ESP8266 (reinício automático em travamento)

Interface e driver:
  • UI tema escuro com beacon SAFE/UNSAFE em destaque
  • Estado do céu e chuva exibidos no card de status
  • Pressão nominal visível ao lado da pressão atual
  • Parâmetros calibráveis diretamente no Setup Dialog do ASCOM
  • Parâmetros persistidos via ASCOM Profile (sobrevivem reinicializações)

Página web embarcada (acessível pelo IP do ESP):
  /             → painel completo com extremos do dia
  /espstatus    → diagnóstico de memória, uptime e saúde dos sensores
  /i2clog       → log de eventos de recuperação I²C em tempo real
  /logs         → logs diários em CSV para download

Instalador:
  • Tela de aceite de licença (MIT)
  • Verificação automática do ASCOM Platform antes de instalar
  • Suporte a Português Brasileiro e Inglês
  • Registro COM automático pós-instalação

─────────────────────────────────────────────────────────────────────
INSTALAÇÃO DO DRIVER
─────────────────────────────────────────────────────────────────────
Pré-requisitos:
  • Windows 10 ou superior
  • ASCOM Platform 6.6 ou superior
    https://ascom-standards.org/Downloads/Index.htm
  • .NET Framework 4.8
  • Driver USB-Serial CP210x ou CH340
    (normalmente instalado junto com a Arduino IDE)

Instalação:
  1. Execute "ZillerSafetyMonitor_Setup_v2.0.exe" como Administrador
  2. Aceite os termos de licença (MIT)
  3. Siga as instruções do assistente de instalação
  4. O driver será registrado automaticamente no ASCOM

─────────────────────────────────────────────────────────────────────
INSTALAÇÃO DO FIRMWARE
─────────────────────────────────────────────────────────────────────
  1. Instale a Arduino IDE (https://arduino.cc)
  2. No Board Manager, adicione:
       URL: http://arduino.esp8266.com/stable/package_esp8266com_index.json
       Placa: "esp8266 by ESP8266 Community"

  3. Instale as bibliotecas (Sketch → Gerenciar Bibliotecas):

     Para v5-BME (recomendado):
       • Adafruit BME280 Library
       • Adafruit MLX90614 Library
       • ESPAsyncTCP
       • ESPAsyncWebServer

     Para v4-HTU:
       • Adafruit HTU21DF Library
       • Adafruit BMP280 Library
       • Adafruit MLX90614 Library
       • ESPAsyncTCP
       • ESPAsyncWebServer

  4. Abra o arquivo .ino da versão desejada
  5. Selecione: Tools → Board → NodeMCU 1.0 (ESP-12E Module)
  6. Selecione a porta COM correta
  7. Carregue o firmware (Upload)
  8. Use o botão "Opções Wi-Fi" no Monitor para configurar a rede WiFi

─────────────────────────────────────────────────────────────────────
CONFIGURAÇÃO INICIAL
─────────────────────────────────────────────────────────────────────
  1. No NINA → Equipamentos → Safety Monitor
     Selecione "Ziller Safety Monitor" e clique em ⚙️ Setup

  2. Configure os parâmetros:
       • Porta COM do ESP8266
       • Altitude do observatório em metros
       • Limite IR Céu: 8.0°C (padrão — ajuste conforme seu local)
       • Limite Chuva: 850 (padrão — valor ADC do sensor, 0-1023)
       • Limite ΔP: 0.0 hPa (padrão)
       • Limite Umidade: 70% (padrão)
       • Timeout Nublado: 10 minutos (padrão)

  3. Clique OK e depois em Conectar

  4. O Monitor abrirá com todas as leituras em tempo real
     Os parâmetros também podem ser ajustados no Monitor e
     enviados ao ESP em tempo real clicando em "Salvar"

─────────────────────────────────────────────────────────────────────
DIAGNÓSTICO
─────────────────────────────────────────────────────────────────────
Se o sistema apresentar instabilidade no barramento I²C:

  1. Acesse http://<IP-do-ESP>/i2clog para ver o histórico de falhas
  2. Acesse http://<IP-do-ESP>/espstatus para ver saúde geral
  3. Verifique a resistência de pull-up no barramento I²C:
       Medido de SDA/SCL para VCC com sensores desconectados
       Valor esperado: 1.0kΩ a 2.5kΩ para operação estável a 25kHz
  4. Verifique a tensão VCC nos sensores: deve estar entre 3.1V e 3.3V
  5. Inspecione visualmente as soldas dos componentes SMD da PCB

─────────────────────────────────────────────────────────────────────
SUPORTE
─────────────────────────────────────────────────────────────────────
  • Issues e sugestões:
    https://github.com/edziller/NINA-ASCOM-Safety-Monitor/issues
  • Fórum ASCOM:
    https://ascomtalk.groups.io/g/Help
  • ASCOM Standards:
    https://ascom-standards.org
