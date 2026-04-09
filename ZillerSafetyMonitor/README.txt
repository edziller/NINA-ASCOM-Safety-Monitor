Ziller Safety Monitor
Driver ASCOM para monitoramento meteorológico de observatório remoto
=====================================================================

Versão:      2.0
Desenvolvedor: Eduardo Ziller — Belo Horizonte, Brasil
GitHub:      https://github.com/edziller/NINA-ASCOM-Safety-Monitor
Instagram:   @eduardoziller
YouTube:     Astronoziller
Email:       eduardo.ziller@gmail.com
Site:        fotografiaeastronomia.blogspot.com.br

Compatível com: ASCOM Platform 6.6 ou superior / NINA 3.x ou superior

─────────────────────────────────────────────────────────────────────
DESCRIÇÃO
─────────────────────────────────────────────────────────────────────
O Ziller Safety Monitor é um driver ASCOM do tipo Safety Monitor
desenvolvido para proteger automaticamente observatórios astronômicos
remotos contra condições meteorológicas adversas.

O hardware é baseado em um microcontrolador ESP8266 conectado aos
seguintes sensores:
  • MLX90614   — temperatura IR do fundo do céu (detecção de nuvens)
  • DHT11       — temperatura e umidade relativa do ar
  • BMP280      — pressão atmosférica
  • Sensor analógico de chuva resistivo

O firmware processa continuamente as leituras e determina se a
condição é SAFE ou UNSAFE com base em três lógicas combinadas:
  1. Chuva detectada → UNSAFE imediato
  2. Céu nublado + umidade alta + pressão baixa → UNSAFE imediato
  3. Céu nublado + umidade alta por tempo > Timeout → UNSAFE

Quando o driver reporta UNSAFE ao NINA, o software interrompe a
sequência de captura, fecha o teto do observatório e desliga os
equipamentos — tudo automaticamente, sem intervenção humana.

─────────────────────────────────────────────────────────────────────
NOVIDADES DA VERSÃO 2.0
─────────────────────────────────────────────────────────────────────
  • Todos os parâmetros calibráveis configuráveis no Setup Dialog
    (Altitude, Limite IR, Limite Chuva, Limite ΔP, Limite Umidade,
    Timeout Nublado) — sem precisar abrir o Monitor para ajustar
  • Parâmetros persistidos no registro Windows via ASCOM Profile —
    sobrevivem a desconexões e reinicializações
  • Monitor de falhas de sensor em tempo real (DHT11, MLX90614, BMP280)
  • Comportamento fail-safe: driver reporta UNSAFE durante inicialização
    até receber confirmação SAFE do hardware
  • Reconexão WiFi automática no firmware (sem bloquear o loop)
  • Scan de redes WiFi assíncrono — não interrompe comunicação serial
  • Watchdog de hardware no ESP8266 (wdtEnable) — garante reinício
    automático em caso de travamento do firmware
  • Clock I²C reduzido para 50 kHz — melhora estabilidade do MLX90614
    em instalações com cabo longo entre ESP e sensores
  • Logs diários em CSV com contadores de falha de sensor
  • Página web embarcada no ESP com diagnóstico completo do sistema
  • Correção de race condition thread-safe no driver ASCOM
  • Correção de memory leak no LocalServer ao desconectar

─────────────────────────────────────────────────────────────────────
INSTALAÇÃO DO DRIVER
─────────────────────────────────────────────────────────────────────
Pré-requisitos:
  • Windows 10 ou superior (64-bit recomendado)
  • ASCOM Platform 6.6 ou superior — https://ascom-standards.org
  • .NET Framework 4.8
  • Driver USB-Serial CP210x (normalmente já instalado com Arduino IDE)
    https://www.silabs.com/developer-tools/usb-to-uart-bridge-vcp-drivers

Instalação:
  1. Execute "Ziller Safety Monitor Setup.exe" como Administrador
  2. Siga as instruções do instalador
  3. O driver estará disponível automaticamente em todos os
     softwares ASCOM após a instalação

─────────────────────────────────────────────────────────────────────
CONFIGURAÇÃO INICIAL
─────────────────────────────────────────────────────────────────────
  1. No NINA (ou ASCOM Diagnostics), selecione "Ziller Safety Monitor"
     na categoria Safety Monitor e clique em Configurar (Setup)
  2. No diálogo de Setup, defina:
       • Porta COM do ESP8266
       • Altitude do observatório em metros (para cálculo de pressão nominal)
       • Limite IR Céu: temperatura IR acima da qual o céu é considerado nublado
         (padrão: 8°C — ajuste conforme seu local e condições típicas)
       • Limite Chuva: valor bruto do sensor ADC abaixo do qual há chuva
         (padrão: 850 — valores menores = sensor mais molhado)
       • Limite ΔP: queda de pressão mínima em hPa considerada preocupante
         (padrão: 0.0 — ajuste negativo para maior sensibilidade)
       • Limite Umidade: porcentagem acima da qual a umidade é considerada alta
         (padrão: 70%)
       • Timeout Nublado: minutos de céu nublado + umidade alta para UNSAFE
         (padrão: 10 min)
  3. Clique OK e conecte o driver

─────────────────────────────────────────────────────────────────────
FIRMWARE DO ESP8266
─────────────────────────────────────────────────────────────────────
  1. Instale a Arduino IDE
  2. No Board Manager, adicione "esp8266 by ESP8266 Community"
  3. Instale as bibliotecas:
       • Adafruit BMP280
       • Adafruit MLX90614
       • DHTesp
       • ESPAsyncTCP
       • ESPAsyncWebServer
  4. Abra o arquivo NINA-ASCOM-Safety-Monitor.ino
  5. Selecione a placa "NodeMCU 1.0 (ESP-12E Module)" e a COM correta
  6. Carregue o firmware (baud rate: 115200)
  7. Use o botão "Opções Wi-Fi" no Monitor para configurar a rede —
     o ESP ficará acessível pelo IP exibido na página web

Página web embarcada (quando conectado ao WiFi):
  http://<IP-do-ESP>/            → painel de leituras em tempo real
  http://<IP-do-ESP>/espstatus   → diagnóstico de memória e saúde
  http://<IP-do-ESP>/logs        → logs diários em CSV

─────────────────────────────────────────────────────────────────────
USO COM O NINA
─────────────────────────────────────────────────────────────────────
  1. Em Equipamentos → Safety Monitor, selecione "Ziller Safety Monitor"
  2. Clique em Conectar
  3. A janela do Monitor abrirá automaticamente com todas as leituras
  4. Configure os limites no painel "Parâmetros Calibráveis" do Monitor
     e clique em Salvar para enviar ao dispositivo em tempo real
  5. O NINA respeitará automaticamente os estados SAFE/UNSAFE

─────────────────────────────────────────────────────────────────────
SUPORTE
─────────────────────────────────────────────────────────────────────
  • Issues e sugestões: https://github.com/edziller/NINA-ASCOM-Safety-Monitor/issues
  • Fórum ASCOM: https://ascomtalk.groups.io/g/Help
  • ASCOM Standards: https://ascom-standards.org
