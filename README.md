# NINA-ASCOM-Safety-Monitor

Este projeto integra uma **estação meteorológica remota baseada em ESP8266** com o software de astrofotografia **NINA**, por meio de um **driver ASCOM Safety Monitor** totalmente funcional.

## 🔧 Visão geral

- **Firmware**: roda no ESP8266 (NodeMCU) e coleta dados de sensores ambientais (chuva, céu, temperatura, etc).
- **Driver ASCOM**: desenvolvido em C# para se comunicar via porta serial com o ESP e relatar o status `SAFE/UNSAFE` ao NINA.
- **Compatível com o ASCOM Platform 6.2 ou superior**

---

## 📡 Funcionalidades

- Monitoramento de:
  - **Chuvas** (sensor analógico)
  - **Temperatura IR do céu** (MLX90614)
  - **Umidade e temperatura** (DHT11)
  - **Pressão atmosférica** (BMP280)
- Lógica de decisão confiável integrada no firmware:
  - Prioriza chuva
  - Considera céu nublado por ≥ 5 minutos
  - Mantém estado `UNSAFE` até que todas as variáveis estejam seguras
- Comunicação via **porta serial (COMx)** com taxa de 115200 bps
- Interface ASCOM segura e compatível com o NINA

---

## 🧰 Estrutura do projeto

```plaintext
.
├── Firmware/                        # Código do ESP8266
│   └── NINA-ASCOM-Safety-Monitor.ino
│
├── Driver/                          # Projeto C# do Driver ASCOM
│   ├── ZillerSafetyMonitor.sln
│   ├── ZillerSafetyMonitor/
│   │   ├── *.cs, *.csproj, etc
│   └── README.txt (interno do instalador)
│
├── Docs/
│   └── esquema_fisico.fritzing      # Esquemático elétrico (a ser melhorado)
│
└── README.md                       # Este arquivo
