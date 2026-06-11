# Ziller Safety Monitor — NINA ASCOM Safety Monitor

Estação meteorológica remota baseada em **ESP8266** integrada ao software de astrofotografia **NINA** por meio de um **driver ASCOM Safety Monitor** totalmente funcional.

> Desenvolvido por Eduardo Ziller — Belo Horizonte, Brasil  
> Instagram: [@eduardoziller](https://instagram.com/eduardoziller) · YouTube: [Astronoziller](https://youtube.com/@astronoziller)

---

## 🔧 Visão geral

- **Firmware**: roda no ESP8266 (NodeMCU) e coleta dados de sensores ambientais em tempo real
- **Driver ASCOM**: desenvolvido em C# para comunicação via porta serial com o ESP, reportando `SAFE/UNSAFE` ao NINA
- **PCB dedicada**: duas placas projetadas no EasyEDA (PCB ESP 50×70mm + PCB Sensores 40×45mm)
- **Compatível com ASCOM Platform 6.6 ou superior**

---

## 📡 Sensores suportados

| Sensor | Função | Interface |
|--------|--------|-----------|
| MLX90614 | Temperatura IR do fundo do céu (detecção de nuvens) | I²C |
| BME280 *(v5 — recomendado)* | Temperatura, umidade e pressão | I²C |
| HTU21D *(v4)* | Temperatura e umidade | I²C |
| BMP280 *(v4)* | Pressão atmosférica | I²C |
| DHT11 *(v4-DHT — legado)* | Temperatura e umidade | 1-Wire |
| Sensor resistivo de chuva | Detecção de precipitação | Analógico |

---

## 🌦️ Lógica de segurança

O firmware avalia continuamente as leituras e determina `SAFE` ou `UNSAFE` com base em:

1. **Chuva detectada** → UNSAFE imediato
2. **Céu nublado + umidade alta + pressão baixa** → UNSAFE imediato  
3. **Céu nublado + umidade alta por tempo > Timeout** → UNSAFE
4. **Falha de sensor** → UNSAFE (comportamento fail-safe)

O sistema mantém `UNSAFE` até que **todas** as variáveis estejam seguras simultaneamente.

---

## 🗂️ Estrutura do projeto

```
.
├── Firmware/
│   ├── NINA-ASCOM-Safety-Monitor_v4-DHT.ino   # DHT11 + BMP280 + MLX90614
│   ├── NINA-ASCOM-Safety-Monitor_v4-HTU.ino   # HTU21D + BMP280 + MLX90614
│   └── NINA-ASCOM-Safety-Monitor_v5-BME.ino   # BME280 + MLX90614 (recomendado)
│
├── Driver/
│   ├── ZillerSafetyMonitor.sln
│   ├── ZillerSafetyMonitor/
│   │   ├── SafetyMonitorDriver.cs
│   │   ├── SafetyMonitorHardware.cs
│   │   ├── MonitorForm.cs / .Designer.cs
│   │   ├── SetupDialogForm.cs / .Designer.cs
│   │   └── FormOpcoesWifi.cs / .Designer.cs
│   └── Installer/
│       ├── Ziller_Safety_Monitor_Setup.iss
│       └── license.txt
│
├── PCB/
│   ├── Gerbers_PCB_ESP_50x70/
│   └── Gerbers_PCB_Sensores_40x45/
│
└── README.md
```

---

## 🔌 Hardware — PCBs

### PCB Sensores (40×45mm)
Contém os módulos de sensores I²C + componentes de proteção:

| Componente | Valor | Função |
|-----------|-------|--------|
| R1, R2 | 2.2kΩ 0402 | Pull-up SDA e SCL |
| C1 | 100nF X7R 0402 | Decoupling MLX90614 VDD |
| C2 | 10µF SMD | Bulk MLX90614 VDD |
| C3 | 100nF X7R 0402 | Filtro A0 sensor de chuva |
| U3 | PRTR5V0U2X SOT363 | Proteção ESD SDA/SCL |
| H1-H4 | NPTH 2.2mm | Fixação mecânica |

### PCB ESP (50×70mm)
Contém o NodeMCU ESP8266 e conector inter-board (6 pinos JST):

```
Pinout do conector inter-board:
  Pin 1: GND
  Pin 2: SDA  (D2 / GPIO4)
  Pin 3: GND
  Pin 4: SCL  (D1 / GPIO5)
  Pin 5: VCC 3.3V
  Pin 6: ANALOG (sensor de chuva → A0)
```

### Configuração I²C recomendada
```cpp
Wire.setClock(25000);             // 25kHz — estável com cabo de 15-20cm
Wire.setClockStretchLimit(25000); // 25ms — aguarda MLX90614 converter
```

---

## 💻 Instalação do Firmware

### Pré-requisitos (Arduino IDE)

**Placa:** `esp8266 by ESP8266 Community` via Board Manager

**Bibliotecas** (Sketch → Gerenciar Bibliotecas):

Para **v5-BME** (recomendado):
```
Adafruit BME280 Library
Adafruit MLX90614 Library
ESPAsyncTCP
ESPAsyncWebServer
```

Para **v4-HTU**:
```
Adafruit HTU21DF Library
Adafruit BMP280 Library
Adafruit MLX90614 Library
ESPAsyncTCP
ESPAsyncWebServer
```

Para **v4-DHT** (legado):
```
DHTesp
Adafruit BMP280 Library
Adafruit MLX90614 Library
ESPAsyncTCP
ESPAsyncWebServer
```

### Gravação
1. Selecione a placa `NodeMCU 1.0 (ESP-12E Module)`
2. Baud rate: `115200`
3. Carregue o firmware
4. Configure o WiFi pelo botão "Opções Wi-Fi" no Monitor

---

## 🌐 Página web embarcada

Quando conectado ao WiFi, o ESP serve uma interface completa:

| Endereço | Conteúdo |
|---------|----------|
| `http://<IP>/` | Painel de leituras em tempo real + extremos do dia |
| `http://<IP>/espstatus` | Diagnóstico de memória, uptime e saúde dos sensores |
| `http://<IP>/i2clog` | Log de eventos de recuperação do barramento I²C |
| `http://<IP>/logs` | Logs diários em CSV |

---

## 🛡️ Recuperação automática do barramento I²C

O firmware implementa recuperação automática quando 2+ sensores falham simultaneamente:

1. **9 pulsos SCL** — libera sensor que trava SDA em nível baixo
2. **Condição STOP** — reseta máquina de estado do sensor
3. **Wire reinit** — reinicializa o master ESP8266 nos pinos corretos
4. **Reinit sensores** — reinicializa cada sensor individualmente
5. **Após 3 tentativas** — reinicia o ESP automaticamente

Todos os eventos são registrados em `/i2c_eventos.csv` com timestamp, sensor responsável e resultado.

---

## 🖥️ Instalação do Driver ASCOM

### Pré-requisitos
- Windows 10 ou superior
- [ASCOM Platform 6.6+](https://ascom-standards.org/Downloads/Index.htm)
- .NET Framework 4.8
- Driver USB-Serial CP210x ou CH340 (instalado com Arduino IDE)

### Instalação
1. Execute `ZillerSafetyMonitor_Setup_v2.0.exe` como Administrador
2. Aceite os termos de licença
3. Siga as instruções do assistente

### Configuração no NINA
1. Equipamentos → Safety Monitor → selecione `Ziller Safety Monitor`
2. Clique em ⚙️ Setup e configure:
   - Porta COM do ESP8266
   - Altitude do observatório (metros)
   - Limites de IR, chuva, umidade, pressão e timeout
3. Clique em Conectar

---

## 📊 Parâmetros calibráveis

| Parâmetro | Padrão | Descrição |
|-----------|--------|-----------|
| Limite IR Céu | 8.0°C | T IR acima da qual o céu é nublado |
| Limite Chuva | 850 | Valor ADC abaixo do qual há chuva (0-1023) |
| Limite ΔP | 0.0 hPa | Queda de pressão considerada preocupante |
| Limite Umidade | 70% | Umidade acima da qual é considerada alta |
| Timeout Nublado | 10 min | Tempo nublado+úmido para UNSAFE |
| Altitude | — m | Altitude do observatório para pressão nominal |

---

## 📝 Histórico de versões

### v5-BME (atual — recomendado)
- BME280 substitui BMP280 + HTU21D — um único CI para T, U e P
- Apenas 2 dispositivos no barramento I²C (MLX + BME) — maior estabilidade
- Clock I²C 25kHz + stretch 25ms

### v4-HTU
- HTU21D substitui DHT11 — precisão ±0.3°C/±2% vs ±2°C/±5%
- Recuperação automática do barramento I²C
- Log de eventos I²C em arquivo
- Página `/i2clog` com histórico de falhas
- Extremos de temperatura e umidade do dia na página web
- Tendência suavizada (média de 6 amostras vs 15 min atrás)
- Thresholds de tendência por grandeza: P=1.5hPa, U=3%, IR=3°C

### v4-DHT
- Versão legado com DHT11 + BMP280 + MLX90614
- Atualizada com toda a infraestrutura de recuperação I²C da v4-HTU

### v2/v3
- Watchdog de hardware (wdtEnable)
- EEPROM com magic byte para defaults na primeira execução
- Validação NaN em todos os sensores
- Clock I²C 50kHz → 25kHz
- Scan WiFi assíncrono
- UI tema escuro com beacon SAFE/UNSAFE
- Instalador com tela de licença (Inno Setup 6)

---

## 📄 Licença

MIT License — veja [license.txt](Driver/Installer/license.txt) para detalhes.

© 2025 Eduardo Ziller — Belo Horizonte, Brasil
