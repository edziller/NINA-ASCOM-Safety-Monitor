// Ziller Safety Monitor
// Eduardo Ziller, Belo Horizonte, Brasil
// Entusiasta e astrônomo amador
// IG: @eduardoziller
//
// v5-BME — substituição de BMP280 + HTU21D pelo BME280 (sensor unico T/U/P):
//  - BME280 substitui BMP280 (pressao) e HTU21D (temp/umidade) em um unico CI
//  - Barramento I2C com apenas 2 sensores: MLX90614 (0x5A) + BME280 (0x76)
//  - Clock I2C: 50kHz — menos sensores = menos capacitancia = clock mais rapido
//  - Clock stretch: 25ms (adequado para MLX90614)
//  - BME280 em MODE_NORMAL — muito mais estavel que o BMP280
//  - Threshold de umidade: +-3% (BME280 preciso como o HTU21D)
//  - Recuperacao I2C automatica, log de eventos, watchdog HW mantidos


#include <Wire.h>

#include <Adafruit_MLX90614.h>
#include <Adafruit_BME280.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <time.h>
#include <EEPROM.h>
#include <LittleFS.h>

Adafruit_MLX90614 mlx;

Adafruit_BME280   bme;
AsyncWebServer server(80);

// ── Pinos ─────────────────────────────────────────────────────────────────────
// HTU21D usa I²C (SDA/SCL) — sem pino dedicado
#define PIN_SENSOR_CHUVA     A0
#define PIN_SDA              4   // D2 — GPIO4
#define PIN_SCL              5   // D1 — GPIO5

// ── Recuperação I²C ───────────────────────────────────────────────────────────
// Se 2+ sensores falharem simultaneamente → barramento provavelmente travado
// Após MAX_TENTATIVAS_RESTART sem recuperar → reinicia o ESP
#define FALHAS_PARA_RECUPERAR    3   // falhas consecutivas por sensor
#define MAX_TENTATIVAS_RESTART   3   // reinicia rapido — melhor que ficar em loop   // reinicia após N tentativas sem sucesso

// ── Configurações gerais ──────────────────────────────────────────────────────
#define LOG_INTERVAL_MINUTES   5
#define WIFI_RECONNECT_INTERVAL 10000UL
#define NUMSAMPLES             20
#define TZ_OFFSET              (-3L * 3600L)
#define MIN_FREE_SPIFFS        51200
#define EEPROM_MAGIC_ADDR      12        // byte de controle de primeira execução
#define EEPROM_MAGIC_VALUE     0xAB      // valor arbitrário: "configurado"

// ── Janela de tendência (30 min / amostra a cada 5 s = 360 pontos) ────────────
#define TREND_WINDOW           360

// ── Thresholds de tendência por grandeza ──────────────────────────────────────
#define TREND_THRESH_P   1.5f   // hPa — limiar para tendência de pressão
#define TREND_THRESH_U   3.0f   // %   — HTU21D é estável, 3% já é tendência real
#define TREND_THRESH_IR  3.0f   // °C  — limiar para tendência de cobertura de nuvens

// ── Parâmetros do cálculo suavizado de tendência ──────────────────────────────
#define TREND_SMOOTH  6
#define TREND_LAG   180

// ── Histórico para tendências ─────────────────────────────────────────────────
float histPressure[TREND_WINDOW];
float histHumidity[TREND_WINDOW];
float histIR[TREND_WINDOW];
int   histIndex = 0;
bool  histInit  = false;

// ── Endereços EEPROM ──────────────────────────────────────────────────────────
const int ADDR_ALTITUDE       = 0;   // 2 bytes (uint16)
const int ADDR_LIM_IR         = 2;   // 2 bytes (uint16 = IR×10)
const int ADDR_LIM_CHUVA      = 4;   // 2 bytes (uint16)
const int ADDR_LIM_DELTA_P    = 6;   // 2 bytes (uint16 = ΔP×10 + offset 1000)
const int ADDR_LIM_UMIDADE    = 8;   // 2 bytes (uint16 = U×10)
const int ADDR_TIMEOUT_NUBLADO= 10;  // 2 bytes (uint16 = minutos)
// ADDR 12 = EEPROM_MAGIC_ADDR (byte único)

// ── Parâmetros calibráveis ────────────────────────────────────────────────────
struct Parametros {
  float    h;                  // altitude (m)
  int      limite_chuva;       // ex: 800  (valor bruto ADC; menor = mais molhado)
  float    limite_p;           // ex: -3.0 (ΔP mínimo em hPa)
  float    limite_u;           // ex: 75.0 (% umidade)
  float    limite_IR_ceu;      // ex: 2.0  (°C — IR acima disto = nublado)
  uint16_t timeout_nublado;    // minutos até UNSAFE por nublado persistente
} params;

// ── Leituras dos sensores (último valor válido) ───────────────────────────────
float u = 50.0f, t = 20.0f, to = 10.0f;
float p = 1013.25f, p_nominal = 1013.25f, delta_p = 0.0f;
float tIRceu = -10.0f;
int   media_sens_chuva = 1023;
int   condicaoSeguranca = 0;   // 0 = SAFE, != 0 = UNSAFE

// ── Contadores de falha por sensor ───────────────────────────────────────────
uint8_t falhasBME = 0, falhasMLX = 0;

// ── Contador de tentativas de recuperação I²C ─────────────────────────────────
uint8_t  tentativasRecuperacao  = 0;
uint16_t totalRecuperacoes      = 0;   // acumulado desde o boot
String   ultimaRecuperacaoHora  = "---";
String   sensoresNaUltimaFalha  = "---";

// ── Extremos do dia (resetados a meia-noite) ─────────────────────────────────
float tMax = -99.0f, tMin = 99.0f;
float uMax =   0.0f, uMin = 100.0f;
int   diaAtual = -1;

// ── Strings de estado ────────────────────────────────────────────────────────
String estado_chuva   = "Sem chuva";
String estado_nuvens  = "Céu limpo";
String pressaoStatus  = "Normal";
String trendPressao   = "---";
String trendUmidade   = "---";
String trendCeu       = "---";

// ── Temporização global ───────────────────────────────────────────────────────
unsigned long agora = 0, lastRead = 0, wifiLastCheck = 0;

// ── WiFi ──────────────────────────────────────────────────────────────────────
String ssid     = "";
String password = "";
bool   wifiCredenciaisCarregadas = false; 

// ── Scan WiFi assíncrono ──────────────────────────────────────────────────────
bool scanEmProgresso = false;

const char *stationName = "Estacao Meteorologica Observatorio Eureka";

// ─────────────────────────────────────────────────────────────────────────────
//  PARÂMETROS PADRÃO
// ─────────────────────────────────────────────────────────────────────────────
void setDefaultParameters() {
  params.h               = 0;
  params.limite_chuva    = 850;
  params.limite_p        = 0.0f;
  params.limite_u        = 70.0f;
  params.limite_IR_ceu   = 8.0f;
  params.timeout_nublado = 10;
}

// ─────────────────────────────────────────────────────────────────────────────
//  EEPROM
// ─────────────────────────────────────────────────────────────────────────────
void carregarParametrosEEPROM() {
  uint16_t a    = EEPROM.read(ADDR_ALTITUDE)        | (EEPROM.read(ADDR_ALTITUDE + 1)        << 8);
  params.h      = constrain((float)a, 0.0f, 10000.0f);

  uint16_t ir10 = EEPROM.read(ADDR_LIM_IR)          | (EEPROM.read(ADDR_LIM_IR + 1)          << 8);
  params.limite_IR_ceu = ir10 / 10.0f;

  uint16_t c    = EEPROM.read(ADDR_LIM_CHUVA)       | (EEPROM.read(ADDR_LIM_CHUVA + 1)       << 8);
  params.limite_chuva = (int)c;

  uint16_t dp10 = EEPROM.read(ADDR_LIM_DELTA_P)     | (EEPROM.read(ADDR_LIM_DELTA_P + 1)     << 8);
  params.limite_p = ((float)dp10 - 1000.0f) / 10.0f;

  uint16_t u10  = EEPROM.read(ADDR_LIM_UMIDADE)     | (EEPROM.read(ADDR_LIM_UMIDADE + 1)     << 8);
  params.limite_u = u10 / 10.0f;

  uint16_t tout = EEPROM.read(ADDR_TIMEOUT_NUBLADO) | (EEPROM.read(ADDR_TIMEOUT_NUBLADO + 1) << 8);
  params.timeout_nublado = tout;
}

void salvarParametrosEEPROM() {
  uint16_t a = (uint16_t)params.h;
  EEPROM.write(ADDR_ALTITUDE,     a & 0xFF);
  EEPROM.write(ADDR_ALTITUDE + 1, (a >> 8) & 0xFF);

  uint16_t ir10 = (uint16_t)(params.limite_IR_ceu * 10.0f);
  EEPROM.write(ADDR_LIM_IR,     ir10 & 0xFF);
  EEPROM.write(ADDR_LIM_IR + 1, (ir10 >> 8) & 0xFF);

  uint16_t c = (uint16_t)params.limite_chuva;
  EEPROM.write(ADDR_LIM_CHUVA,     c & 0xFF);
  EEPROM.write(ADDR_LIM_CHUVA + 1, (c >> 8) & 0xFF);

  uint16_t dp10 = (uint16_t)(params.limite_p * 10.0f + 1000.0f);
  EEPROM.write(ADDR_LIM_DELTA_P,     dp10 & 0xFF);
  EEPROM.write(ADDR_LIM_DELTA_P + 1, (dp10 >> 8) & 0xFF);

  uint16_t u10 = (uint16_t)(params.limite_u * 10.0f);
  EEPROM.write(ADDR_LIM_UMIDADE,     u10 & 0xFF);
  EEPROM.write(ADDR_LIM_UMIDADE + 1, (u10 >> 8) & 0xFF);

  uint16_t tout = params.timeout_nublado;
  EEPROM.write(ADDR_TIMEOUT_NUBLADO,     tout & 0xFF);
  EEPROM.write(ADDR_TIMEOUT_NUBLADO + 1, (tout >> 8) & 0xFF);

  EEPROM.commit();
}

// ─────────────────────────────────────────────────────────────────────────────
//  CREDENCIAIS WiFi (LittleFS)
// ─────────────────────────────────────────────────────────────────────────────
void salvarCredenciaisSPIFFS(const String &s, const String &senha) {
  File f = LittleFS.open("/wifi.txt", "w");
  if (!f) {
    Serial.println("Erro ao abrir arquivo de credenciais");
    return;
  }
  f.println(s);
  f.println(senha);
  f.close();
  Serial.println("Credenciais Wi-Fi salvas.");
}

void carregarCredenciaisWiFi() {
  File f = LittleFS.open("/wifi.txt", "r");
  if (!f) {
    Serial.println("Nenhuma credencial WiFi salva. Operando offline.");
    return;
  }
  ssid     = f.readStringUntil('\n'); ssid.trim();
  password = f.readStringUntil('\n'); password.trim();
  f.close();

  if (ssid.length() == 0) {
    Serial.println("SSID vazio no arquivo. Operando offline.");
    return;
  }

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid.c_str(), password.c_str());
  Serial.print("Conectando ao Wi-Fi");
  int result = WiFi.waitForConnectResult();
  Serial.println();

  if (result == WL_CONNECTED) {
    wifiCredenciaisCarregadas = true; 
    Serial.println("Wi-Fi conectado.");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
  } else {
    // Credenciais existem mas não conectou agora → tenta depois no loop
    wifiCredenciaisCarregadas = true;   // credenciais são válidas para tentar de novo
    Serial.println("Nao foi possivel conectar. Tentara novamente em background.");
  }
}

// ─────────────────────────────────────────────────────────────────────────────
//  SETUP
// ─────────────────────────────────────────────────────────────────────────────
void setup() {
  delay(200);
  Serial.begin(115200);
  delay(100);

  // Watchdog de hardware do ESP8266 (8 segundos)
  ESP.wdtEnable(8000);

  EEPROM.begin(512);

  // Detecta primeira execução com magic byte
  if (EEPROM.read(EEPROM_MAGIC_ADDR) != EEPROM_MAGIC_VALUE) {
    Serial.println("Primeira execucao: gravando parametros padrao na EEPROM.");
    setDefaultParameters();
    salvarParametrosEEPROM();
    EEPROM.write(EEPROM_MAGIC_ADDR, EEPROM_MAGIC_VALUE);
    EEPROM.commit();
  } else {
    carregarParametrosEEPROM();
  }

  if (!LittleFS.begin()) {
    Serial.println("ERRO: falha ao montar LittleFS");
  } else {
    Serial.println("LittleFS montado.");
  }

  carregarCredenciaisWiFi();

  configTime(0, 0, "pool.ntp.org", "time.nist.gov");
  unsigned long tStart = millis();
  while (time(nullptr) < 24 * 3600 && millis() - tStart < 5000) {
    ESP.wdtFeed();
    delay(100);
  }

  // I2C a 25kHz + timeout de 25ms no clock stretch
  // setClockStretchLimit(25000): ESP desiste após 50ms se escravo não responder
  Wire.begin();
  Wire.setClock(50000);
  Wire.setClockStretchLimit(25000);
  Serial.println("Scan I2C:");
  for (byte addr = 1; addr < 127; addr++) {
    Wire.beginTransmission(addr);
    if (Wire.endTransmission() == 0) {
      Serial.print("  Encontrado: 0x");
      Serial.println(addr, HEX);
    }
  }

  // Inicializa sensores
  if (!mlx.begin()) {
    Serial.println("AVISO: MLX90614 nao encontrado. Verifique o I2C.");
  }
  // BME280 — substitui BMP280 + HTU21D (temperatura, umidade e pressao)
  // Endereco I2C: 0x76 (SDO=GND) ou 0x77 (SDO=VCC)
  if (!bme.begin(0x76)) {
    Serial.println("AVISO: BME280 nao encontrado. Verifique o I2C e o pino SDO.");
  } else {
    bme.setSampling(
      Adafruit_BME280::MODE_NORMAL,
      Adafruit_BME280::SAMPLING_X2,
      Adafruit_BME280::SAMPLING_X16,
      Adafruit_BME280::FILTER_X16,
      Adafruit_BME280::STANDBY_MS_500
    );
  }

  iniciarServidor();
  Serial.println("Ziller Safety Monitor iniciado.");
}

// ─────────────────────────────────────────────────────────────────────────────
//  LOOP PRINCIPAL
// ─────────────────────────────────────────────────────────────────────────────
void loop() {
  ESP.wdtFeed(); 

  handleLogging();
  agora = millis();
  tratarSerial();
  tratarScanWiFiAssincrono();

  if (agora - lastRead >= 5000) {
    lastRead = agora;
    lerSensores();
    calcularSeguranca();
  }

  if (wifiCredenciaisCarregadas
      && WiFi.status() != WL_CONNECTED
      && agora - wifiLastCheck > WIFI_RECONNECT_INTERVAL) {
    wifiLastCheck = agora;
    WiFi.disconnect();
    WiFi.begin(ssid.c_str(), password.c_str());
  }
}

// ─────────────────────────────────────────────────────────────────────────────
//  LOG DE EVENTOS I²C
// ─────────────────────────────────────────────────────────────────────────────
void logI2CEvento(uint8_t tentativa, bool reiniciando) {
  time_t tnow = time(nullptr) + TZ_OFFSET;
  struct tm ti;
  localtime_r(&tnow, &ti);
  char dateBuf[16], timeBuf[16];
  snprintf(dateBuf, sizeof(dateBuf), "%04d.%02d.%02d",
           ti.tm_year+1900, ti.tm_mon+1, ti.tm_mday);
  snprintf(timeBuf, sizeof(timeBuf), "%02d:%02d:%02d",
           ti.tm_hour, ti.tm_min, ti.tm_sec);

  File f = LittleFS.open("/i2c_eventos.csv", "a");
  if (!f) return;
  if (f.size() == 0)
    f.println("DATA,HORA,TENTATIVA,BME_FALHAS,MLX_FALHAS,SENSORES,RESULTADO");
  f.printf("%s,%s,%d,%d,%d,%s,%s\n",
    dateBuf, timeBuf, tentativa,
    falhasBME, falhasMLX,
    sensoresNaUltimaFalha.c_str(),
    reiniciando ? "RESTART_ESP" : "RECUPERADO");
  f.close();
}

// ─────────────────────────────────────────────────────────────────────────────
//  RECUPERAÇÃO DO BARRAMENTO I²C
//  Procedimento padrão SMBus:
//    1. 9 pulsos SCL  → libera escravo que trava SDA baixo
//    2. Condição STOP → reseta máquina de estado do escravo
//    3. Wire.end/begin → reinicializa master ESP8266
//    4. Reinit sensores
//    5. Após MAX_TENTATIVAS_RESTART falhas → ESP.restart()
// ─────────────────────────────────────────────────────────────────────────────
void recuperarI2C() {
  tentativasRecuperacao++;
  totalRecuperacoes++;

  // Registra quais sensores falharam e o horário
  String quem = "";
  if (falhasMLX >= FALHAS_PARA_RECUPERAR) quem += "MLX ";
  if (falhasBME >= FALHAS_PARA_RECUPERAR) quem += "BME";
  quem.trim();
  sensoresNaUltimaFalha = quem;

  time_t tnow = time(nullptr) + TZ_OFFSET;
  struct tm ti;
  localtime_r(&tnow, &ti);
  char timeBuf[12];
  snprintf(timeBuf, sizeof(timeBuf), "%02d:%02d:%02d",
           ti.tm_hour, ti.tm_min, ti.tm_sec);
  ultimaRecuperacaoHora = String(timeBuf);

  // ── Serial: mensagens detalhadas ─────────────────────────────────────────
  Serial.println();
  Serial.println("╔══════════════════════════════════════════════╗");
  Serial.printf( "║  [I2C] BARRAMENTO TRAVADO — Recuperacao #%d\n", tentativasRecuperacao);
  Serial.printf( "║  Hora: %s | Total desde boot: %d\n", timeBuf, totalRecuperacoes);
  Serial.printf( "║  Sensores falhando: %s\n", quem.c_str());
  Serial.printf( "║  Falhas: HTU=%d  MLX=%d  BMP=%d\n",
                 falhasBME, falhasMLX);
  Serial.println("╠══════════════════════════════════════════════╣");

  // Passo 1: 9 pulsos SCL
  yield(); // permite que o servidor web processe requests pendentes
  Serial.println("║  Passo 1: 9 pulsos SCL...");
  pinMode(PIN_SDA, INPUT_PULLUP);
  pinMode(PIN_SCL, OUTPUT);
  for (int i = 0; i < 9; i++) {
    digitalWrite(PIN_SCL, HIGH); delayMicroseconds(10);
    digitalWrite(PIN_SCL, LOW);  delayMicroseconds(10);
  }

  // Passo 2: Condição de STOP
  Serial.println("║  Passo 2: Condicao de STOP...");
  pinMode(PIN_SDA, OUTPUT);
  digitalWrite(PIN_SDA, LOW);  delayMicroseconds(10);
  digitalWrite(PIN_SCL, HIGH); delayMicroseconds(10);
  digitalWrite(PIN_SDA, HIGH); delayMicroseconds(10);

  // Passo 3: Reinicializa Wire
  Serial.println("║  Passo 3: Reinicializando Wire (50 kHz)...");
  // ESP8266: Wire.begin(sda,scl) forca reinicializacao correta dos pinos
  Wire.begin(PIN_SDA, PIN_SCL);
  Wire.setClock(50000);
  Wire.setClockStretchLimit(25000);
  delay(200);

  // Passo 4: Reinicializa sensores
  Serial.println("║  Passo 4: Reinicializando sensores...");
  bool mlxOk = mlx.begin();
  bool bmeOk = bme.begin(0x76);
  if (bmeOk) {
    bme.setSampling(Adafruit_BME280::MODE_NORMAL,
                    Adafruit_BME280::SAMPLING_X2,
                    Adafruit_BME280::SAMPLING_X16,
                    Adafruit_BME280::FILTER_X16,
                    Adafruit_BME280::STANDBY_MS_500);
  }
  Serial.printf("║  MLX=%s  BME=%s\n",
    mlxOk ? "OK" : "FALHOU",
    bmeOk ? "OK" : "FALHOU");

  // Zera contadores
  falhasMLX = 0; falhasBME = 0;

  // Verifica limite de tentativas
  yield(); // permite web server processar antes do possivel restart
  bool reiniciando = (tentativasRecuperacao >= MAX_TENTATIVAS_RESTART);

  // Grava no log antes de qualquer restart
  logI2CEvento(tentativasRecuperacao, reiniciando);

  if (reiniciando) {
    Serial.println("║  CRITICO: limite atingido — reiniciando ESP...");
    Serial.println("╚══════════════════════════════════════════════╝");
    delay(500);
    ESP.restart();
  }

  Serial.println("║  Recuperacao concluida. Aguardando proxima leitura...");
  Serial.println("╚══════════════════════════════════════════════╝");
  Serial.println();
}

// ─────────────────────────────────────────────────────────────────────────────
//  LEITURA DOS SENSORES  (com validação de NaN)
// ─────────────────────────────────────────────────────────────────────────────
void lerSensores() {
  // BME280: le temperatura, umidade e pressao de um unico sensor
  float novo_t = bme.readTemperature();
  float novo_u = bme.readHumidity();
  float novo_p = bme.readPressure() / 100.0f;

  bool bme_ok = !isnan(novo_t) && !isnan(novo_u) && !isnan(novo_p)
             && novo_u >= 0.0f && novo_u <= 100.0f
             && novo_p > 800.0f && novo_p < 1100.0f;

  if (bme_ok) {
    t = novo_t;
    u = novo_u;
    p = novo_p;
    falhasBME = 0;
  } else {
    falhasBME = min((int)falhasBME + 1, 255);
  }

  // MLX90614 — FIX: valida leitura
  float novo_ir = mlx.readObjectTempC();
  if (!isnan(novo_ir) && novo_ir > -40.0f && novo_ir < 85.0f) {
    tIRceu = novo_ir;
    falhasMLX = 0;
  } else {
    falhasMLX = min((int)falhasMLX + 1, 255);
    // Se o MLX falhar muitas vezes consecutivas, forçamos UNSAFE por precaução
    if (falhasMLX >= 3) {
      // Incapaz de avaliar o céu → conservativo: assume nublado
      tIRceu = params.limite_IR_ceu + 1.0f;
    }
  }

  // Inicializa buffers de histórico com o primeiro valor real
  if (!histInit) {
    for (int i = 0; i < TREND_WINDOW; i++) {
      histPressure[i] = p;
      histHumidity[i] = u;
      histIR[i]       = tIRceu;
    }
    histInit = true;
  }
  histIndex = (histIndex + 1) % TREND_WINDOW;
  histPressure[histIndex] = p;
  histHumidity[histIndex] = u;
  histIR[histIndex]       = tIRceu;

  float u_safe = constrain(u, 1.0f, 99.0f);
  float a_dew  = 17.27f, b_dew = 237.7f;
  float gamma  = (a_dew * t) / (b_dew + t) + log(u_safe / 100.0f);
  to = (b_dew * gamma) / (a_dew - gamma);

  // Pressão nominal pela fórmula barométrica 
  p_nominal = 1013.25f * exp(-0.0289644f * 9.81f * params.h / (8.31432f * (t + 273.15f)));
  delta_p   = p - p_nominal;

  // Atualiza maximos e minimos do dia (reset automatico a meia-noite)
  {
    time_t _tn = time(nullptr) + TZ_OFFSET;
    struct tm _td; localtime_r(&_tn, &_td);
    if (_td.tm_mday != diaAtual) {
      tMax = t; tMin = t; uMax = u; uMin = u;
      diaAtual = _td.tm_mday;
    }
    if (t > tMax) tMax = t;
    if (t < tMin) tMin = t;
    if (u > uMax) uMax = u;
    if (u < uMin) uMin = u;
  }

  // Sensor de chuva analógico — lê DEPOIS das transações I2C para evitar ruído
  int soma = 0;
  for (int i = 0; i < NUMSAMPLES; i++) {
    soma += analogRead(PIN_SENSOR_CHUVA);
    yield();
  }
  media_sens_chuva = soma / NUMSAMPLES;

  // ── Detecção e recuperação de travamento I²C ──────────────────────────────
  // Com 2 sensores: se ambos falharem simultaneamente = barramento travado
  int sensoresFalhando = (falhasMLX >= FALHAS_PARA_RECUPERAR ? 1 : 0)
                       + (falhasBME >= FALHAS_PARA_RECUPERAR ? 1 : 0);
  if (sensoresFalhando >= 2) {
    recuperarI2C();
  } else if (sensoresFalhando == 0) {
    tentativasRecuperacao = 0;
  }
}

// ─────────────────────────────────────────────────────────────────────────────
//  CÁLCULO DE SEGURANÇA
// ─────────────────────────────────────────────────────────────────────────────
void calcularSeguranca() {
 
  int oldIdx = (histIndex + 1) % TREND_WINDOW;
  float deltaP  = p      - histPressure[oldIdx];
  float deltaU  = u      - histHumidity[oldIdx];
  float deltaIR = tIRceu - histIR[oldIdx];

  auto trendOf = [](float d, float thresh) -> int {
    if (d >  thresh) return  1;
    if (d < -thresh) return -1;
    return 0;
  };

  int tP = trendOf(deltaP,  TREND_THRESH_P);
  int tU = trendOf(deltaU,  TREND_THRESH_U);
  int tI = trendOf(deltaIR, TREND_THRESH_IR);

  const char *txtT[3] = { "Queda", "Estavel", "Aumento" };
  trendPressao = String(txtT[tP + 1]);
  trendUmidade = String(txtT[tU + 1]);
  trendCeu     = String(txtT[tI + 1]);

  estado_nuvens = "Ceu limpo";
  estado_chuva  = "Sem chuva";

  bool chovendo   = (media_sens_chuva <= params.limite_chuva);
  bool ir_alta    = (tIRceu > params.limite_IR_ceu);
  bool umid_alta  = (u >= params.limite_u);
  bool press_baixa= (delta_p <= params.limite_p);

  static unsigned long inicio = 0;

  if (chovendo) {
    condicaoSeguranca = 100;
    estado_chuva  = "Chovendo";
    estado_nuvens = "Nublado";
    inicio = 0;

  } else if (ir_alta && umid_alta && press_baixa) {
    condicaoSeguranca = 100;
    estado_chuva  = "Risco alto: umid. alta, pressao baixa e nublado";
    estado_nuvens = "Nublado";
    inicio = 0;

  } else if (ir_alta && umid_alta) {
    if (inicio == 0) {
      inicio = agora;
      condicaoSeguranca = 0;
      estado_nuvens = "Nublado";
    } else if (agora - inicio >= (unsigned long)params.timeout_nublado * 60UL * 1000UL) {
      condicaoSeguranca = 100;
      estado_chuva  = "Risco: nublado e umid. alta por mais de " + String(params.timeout_nublado) + " min";
      estado_nuvens = "Nublado";
    } else {
      // Ainda dentro do timeout
      condicaoSeguranca = 0;
      estado_nuvens = "Nublado";
    }

  } else if (ir_alta) {
    condicaoSeguranca = 0;
    estado_nuvens = "Nublado";
    inicio = 0;

  } else {
    condicaoSeguranca = 0;
    estado_nuvens = "Ceu limpo";
    inicio = 0;
  }

  if (delta_p <= params.limite_p)                             pressaoStatus = "Baixa";
  else if (delta_p > params.limite_p && delta_p < 2.0f)      pressaoStatus = "Normal";
  else                                                        pressaoStatus = "Alta";
}

// ─────────────────────────────────────────────────────────────────────────────
//  SCAN WiFi ASSÍNCRONO
// ─────────────────────────────────────────────────────────────────────────────
void iniciarScanWiFi() {
  Serial.println("SCANNING:1");  // avisa o driver C# para ignorar timeouts
  WiFi.scanNetworksAsync([](int n) {
    // callback chamado quando o scan termina (não bloqueia)
    if (n == 0) {
      Serial.println("Nenhuma rede encontrada.");
    } else {
      for (int i = 0; i < n; ++i) {
        Serial.print("SSID:");
        Serial.println(WiFi.SSID(i));
      }
    }
    Serial.println("#");  // sinaliza fim do scan para o C#
    WiFi.scanDelete();
    scanEmProgresso = false;
  });
  scanEmProgresso = true;
}

void tratarScanWiFiAssincrono() {
  // Placeholder: o callback do scanNetworksAsync já faz tudo.
  // Esta função pode ser usada para timeout de scan no futuro.
  (void)scanEmProgresso;
}

// ─────────────────────────────────────────────────────────────────────────────
//  TRATAMENTO SERIAL
// ─────────────────────────────────────────────────────────────────────────────
void tratarSerial() {
  if (Serial.available()) {
    String linha = Serial.readStringUntil('\n');
    linha.trim();

    if (linha.startsWith("ALT:")) {
      float v = linha.substring(4).toFloat();
      if (v >= 0.0f && v <= 10000.0f) {
        params.h = v;
        salvarParametrosEEPROM();
        Serial.printf("Altitude atualizada: %.1f m\n", params.h);
      }
    } else if (linha.startsWith("LIMITE_IR:")) {
      float v = linha.substring(10).toFloat();
      params.limite_IR_ceu = v;
      salvarParametrosEEPROM();
      Serial.println("Limite IR atualizado");
    } else if (linha.startsWith("LIMITE_CHUVA:")) {
      int v = linha.substring(13).toInt();
      if (v >= 0 && v <= 1023) {
        params.limite_chuva = v;
        salvarParametrosEEPROM();
        Serial.println("Limite Chuva atualizado");
      }
    } else if (linha.startsWith("LIMITE_P:")) {
      float v = linha.substring(9).toFloat();
      params.limite_p = v;
      salvarParametrosEEPROM();
      Serial.println("Limite dP atualizado");
    } else if (linha.startsWith("LIMITE_U:")) {
      float v = linha.substring(9).toFloat();
      if (v >= 0.0f && v <= 100.0f) {
        params.limite_u = v;
        salvarParametrosEEPROM();
        Serial.println("Limite Umidade atualizado");
      }
    } else if (linha.startsWith("TIMEOUT_NUBLADO:")) {
      uint16_t v = (uint16_t)linha.substring(16).toInt();
      if (v > 0 && v <= 120) {
        params.timeout_nublado = v;
        salvarParametrosEEPROM();
        Serial.println("Timeout nublado atualizado");
      }
    } else if (linha.startsWith("WIFI:")) {
      String payload = linha.substring(5);
      int sep = payload.indexOf(',');
      if (sep > 0) {
        String ns   = payload.substring(0, sep);
        String pass = payload.substring(sep + 1);
        salvarCredenciaisSPIFFS(ns, pass);
        Serial.println("OK");
        delay(500);
        ESP.restart();
      } else {
        Serial.println("ERRO: formato WIFI invalido. Use WIFI:ssid,senha");
      }
    } else if (linha == "SCAN_WIFI") {
      // FIX: scan assíncrono — não bloqueia o loop
      if (!scanEmProgresso) {
        iniciarScanWiFi();
      } else {
        Serial.println("Scan ja em progresso.");
      }
    } else if (linha == "GET_PARAMS") {
      // Novo comando: envia parâmetros atuais para o C# sincronizar
      Serial.printf("PARAMS:ALT:%.0f,IR:%.1f,CHUVA:%d,P:%.1f,U:%.1f,TO:%d\n",
        params.h, params.limite_IR_ceu, params.limite_chuva,
        params.limite_p, params.limite_u, (int)params.timeout_nublado);
    }
  }

  // Publicação periódica na serial (a cada 5s)
  static unsigned long ultimaPublicacao = 0;
  if (agora - ultimaPublicacao >= 5000) {
    ultimaPublicacao = agora;
    Serial.printf(
      "ALT:%.1f;T:%.1f;U:%.1f;TO:%.1f;PN:%.1f;P:%.1f;DP:%.1f;IR:%.1f;"
      "SENSCH:%d;LIMCH:%d;CEU:%s;CHUVA:%s;SP:%s;TP:%s;TU:%s;TC:%s;SEG:%s;"
      "FB:%d;FM:%d\r\n",
      params.h, t, u, to, p_nominal, p, delta_p, tIRceu,
      media_sens_chuva, params.limite_chuva,
      estado_nuvens.c_str(), estado_chuva.c_str(), pressaoStatus.c_str(),
      trendPressao.c_str(), trendUmidade.c_str(), trendCeu.c_str(),
      condicaoSeguranca == 0 ? "SAFE" : "UNSAFE",
      falhasBME, falhasMLX   // contadores de falha para diagnóstico
    );
  }
}

// ─────────────────────────────────────────────────────────────────────────────
//  LOGGING 
// ─────────────────────────────────────────────────────────────────────────────
String formatUptime() {
  unsigned long totalSec = millis() / 1000;
  unsigned long d = totalSec / 86400; totalSec %= 86400;
  unsigned long h = totalSec / 3600;  totalSec %= 3600;
  unsigned long m = totalSec / 60;
  unsigned long s = totalSec % 60;
  char buf[32];
  snprintf(buf, sizeof(buf), "%lu d %02lu:%02lu:%02lu", d, h, m, s);
  return String(buf);
}

void handleLogging() {
  time_t epoch      = time(nullptr);
  time_t localEpoch = epoch + TZ_OFFSET;
  struct tm ti;
  localtime_r(&localEpoch, &ti);

  int totalMins = ti.tm_hour * 60 + ti.tm_min;
  static int lastLoggedTotalMins = -1;

  if ((totalMins % LOG_INTERVAL_MINUTES) == 0 && totalMins != lastLoggedTotalMins) {
    lastLoggedTotalMins = totalMins;
    logData();
  }
}

void logData() {
  cleanupOldLogs(MIN_FREE_SPIFFS);

  time_t tnow = time(nullptr) + TZ_OFFSET;
  struct tm ti;
  localtime_r(&tnow, &ti);

  char dateBuf[16], timeBuf[16];
  snprintf(dateBuf, sizeof(dateBuf), "%04d.%02d.%02d",
           ti.tm_year + 1900, ti.tm_mon + 1, ti.tm_mday);
  snprintf(timeBuf, sizeof(timeBuf), "%02d:%02d:%02d",
           ti.tm_hour, ti.tm_min, ti.tm_sec);

  String fname = String("/") + dateBuf + "_Log_met.csv";
  bool exists = LittleFS.exists(fname);
  File f = LittleFS.open(fname, "a");
  if (!f) return;

  if (!exists) {
    f.println("DATA,HORA,TEMPERATURA,UMIDADE,TEMP_ORVALHO,PRESSAO,TIRCEU,"
              "ESTADO_NUVENS,ESTADO_CHUVA,CONDICAO_SEGURANCA,FALHAS_BME,FALHAS_MLX,FALHAS_BMP");
  }
  f.printf("%s,%s,%.1f,%.1f,%.1f,%.1f,%.1f,%s,%s,%s,%d,%d,%d\n",
           dateBuf, timeBuf,
           t, u, to, p, tIRceu,
           estado_nuvens.c_str(), estado_chuva.c_str(),
           condicaoSeguranca == 0 ? "SAFE" : "UNSAFE",
           falhasBME, falhasMLX);
  f.close();
}

void cleanupOldLogs(size_t minFree) {
  FSInfo fs_info;
  if (!LittleFS.info(fs_info)) return;
  size_t freeBytes = fs_info.totalBytes - fs_info.usedBytes;

  while (freeBytes < minFree) {
    Dir dir = LittleFS.openDir("/");
    String oldest = "";
    while (dir.next()) {
      String fn = dir.fileName();
      if (fn.endsWith("_Log_met.csv")) {
        if (oldest == "" || fn < oldest) oldest = fn;
      }
    }
    if (oldest == "") break;
    Serial.print("Removendo log antigo: "); Serial.println(oldest);
    LittleFS.remove(oldest);
    if (!LittleFS.info(fs_info)) break;
    freeBytes = fs_info.totalBytes - fs_info.usedBytes;
  }
}

// ─────────────────────────────────────────────────────────────────────────────
//  SERVIDOR WEB  (mantido integral — apenas o scan é novo)
// ─────────────────────────────────────────────────────────────────────────────
void iniciarServidor() {
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *req) {
    String html = R"rawliteral(
  <!DOCTYPE html>
  <html lang="pt-BR">
  <head>
    <meta charset="UTF-8">
    <meta http-equiv="refresh" content="10">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Estação Meteorológica</title>
    <style>
      body { font-family: Arial, Helvetica, sans-serif; background: #f4f7fb; color: #1f2937; margin: 0; padding: 0; }
      .container { max-width: 1200px; margin: 0 auto; padding: 20px; }
      .header { background: linear-gradient(135deg, #0f172a, #1e3a8a); color: white; padding: 24px; border-radius: 14px; box-shadow: 0 4px 14px rgba(0,0,0,0.15); margin-bottom: 20px; }
      .header h1 { margin: 0 0 8px 0; font-size: 28px; }
      .header p { margin: 4px 0; font-size: 15px; opacity: 0.95; }
      .summary-grid { display: grid; grid-template-columns: repeat(auto-fit, minmax(220px, 1fr)); gap: 16px; margin: 20px 0; }
      .summary-card { border-radius: 14px; padding: 18px; color: white; box-shadow: 0 4px 14px rgba(0,0,0,0.12); }
      .summary-card h3 { margin: 0 0 10px 0; font-size: 16px; opacity: 0.95; }
      .summary-card .value { font-size: 26px; font-weight: bold; margin-bottom: 6px; }
      .summary-card .sub { font-size: 14px; opacity: 0.95; }
      .safe-card   { background: linear-gradient(135deg, #16a34a, #15803d); }
      .unsafe-card { background: linear-gradient(135deg, #dc2626, #991b1b); }
      .rain-ok-card  { background: linear-gradient(135deg, #2563eb, #1d4ed8); }
      .rain-bad-card { background: linear-gradient(135deg, #b91c1c, #7f1d1d); }
      .sky-ok-card   { background: linear-gradient(135deg, #0891b2, #155e75); }
      .sky-bad-card  { background: linear-gradient(135deg, #6d28d9, #4c1d95); }
      .press-ok-card { background: linear-gradient(135deg, #0f766e, #115e59); }
      .press-bad-card{ background: linear-gradient(135deg, #ca8a04, #a16207); }
      .grid { display: grid; grid-template-columns: repeat(auto-fit, minmax(340px, 1fr)); gap: 20px; }
      .card { background: white; border-radius: 14px; padding: 18px; box-shadow: 0 4px 14px rgba(0,0,0,0.08); }
      .card h2 { margin-top: 0; margin-bottom: 14px; font-size: 20px; color: #111827; border-bottom: 2px solid #e5e7eb; padding-bottom: 8px; }
      table { width: 100%; border-collapse: collapse; margin-top: 10px; font-size: 15px; }
      th { background: #e5e7eb; text-align: left; padding: 10px; border-bottom: 1px solid #d1d5db; }
      td { padding: 10px; border-bottom: 1px solid #e5e7eb; }
      tr:hover { background: #f9fafb; }
      .txt-good { font-weight: bold; color: #166534; }
      .txt-bad  { font-weight: bold; color: #991b1b; }
      .txt-warn { font-weight: bold; color: #92400e; }
      .footer-link { margin-top: 20px; display: inline-block; background: #2563eb; color: white; text-decoration: none; padding: 10px 16px; border-radius: 10px; font-weight: bold; }
      .footer-link:hover { background: #1d4ed8; }
      .header-grid { display: grid; grid-template-columns: 2fr 1fr; gap: 16px; margin-bottom: 20px; }
      .header { margin-bottom: 0; }
      .header-stats { background: white; border-radius: 14px; padding: 18px; box-shadow: 0 4px 14px rgba(0,0,0,0.12); display: flex; flex-direction: column; justify-content: center; }
      .header-stats h3 { margin: 0 0 12px 0; color: #1e3a8a; font-size: 17px; font-weight: bold; }
      .stats-tbl { width: 100%; border-collapse: collapse; margin-top: 0; font-size: 14px; }
      .stats-tbl th { background: #f1f5f9; padding: 7px 10px; font-size: 12px; color: #6b7280; }
      .stats-tbl td { padding: 7px 10px; border-bottom: 1px solid #f1f5f9; }
      .th-max { color: #dc2626; text-align: center; }
      .th-min { color: #2563eb; text-align: center; }
      .td-max { color: #dc2626; font-weight: bold; text-align: center; }
      .td-min { color: #2563eb; font-weight: bold; text-align: center; }
      .mini-note { font-size: 13px; color: #6b7280; margin-top: 8px; }
    </style>
  </head>
  <body><div class="container">
    )rawliteral";

    time_t now = time(nullptr) + TZ_OFFSET;
    struct tm ti;
    localtime_r(&now, &ti);
    char buf[32];
    snprintf(buf, sizeof(buf), "%04d-%02d-%02d %02d:%02d:%02d",
             ti.tm_year + 1900, ti.tm_mon + 1, ti.tm_mday,
             ti.tm_hour, ti.tm_min, ti.tm_sec);

    bool isSafe       = (condicaoSeguranca == 0);
    bool rainDetected = (media_sens_chuva <= params.limite_chuva);
    bool cloudy       = (tIRceu > params.limite_IR_ceu);

    String statusClass   = isSafe ? "safe-card"     : "unsafe-card";
    String statusText    = isSafe ? "SAFE"           : "UNSAFE";
    String rainClass     = rainDetected ? "rain-bad-card" : "rain-ok-card";
    String skyClass      = cloudy       ? "sky-bad-card"  : "sky-ok-card";
    String pressureClass = (pressaoStatus == "Normal") ? "press-ok-card" : "press-bad-card";
    String rainTextClass = rainDetected ? "txt-bad"  : "txt-good";
    String skyTextClass  = cloudy       ? "txt-warn" : "txt-good";
    String safeTextClass = isSafe       ? "txt-good" : "txt-bad";
    String sensorRainClass = rainDetected ? "txt-bad" : "txt-good";

    html += "<div class='header-grid'>";
    html += "<div class='header'><h1>" + String(stationName) + "</h1>";
    html += "<p><strong>Data / Hora:</strong> " + String(buf) + "</p>";
    html += "<p><strong>Atualizacao automatica:</strong> a cada 10 segundos</p></div>";
    // Card de extremos do dia
    String tMaxStr = (tMax > -90.0f) ? String(tMax, 1) + " C" : "---";
    String tMinStr = (tMin <  90.0f) ? String(tMin, 1) + " C" : "---";
    String uMaxStr = (uMax >   0.0f) ? String(uMax, 1) + " %" : "---";
    String uMinStr = (uMin < 100.0f) ? String(uMin, 1) + " %" : "---";
    html += "<div class='header-stats'>";
    html += "<h3>&#128197; Extremos do Dia</h3>";
    html += "<table class='stats-tbl'>";
    html += "<tr><th></th><th class='th-max'>&#9650; Max</th><th class='th-min'>&#9660; Min</th></tr>";
    html += "<tr><td>Temperatura</td><td class='td-max'>" + tMaxStr + "</td><td class='td-min'>" + tMinStr + "</td></tr>";
    html += "<tr><td>Umidade</td><td class='td-max'>" + uMaxStr + "</td><td class='td-min'>" + uMinStr + "</td></tr>";
    html += "</table></div></div>";

    html += "<div class='summary-grid'>";
    html += "<div class='summary-card " + statusClass + "'><h3>Seguranca</h3><div class='value'>" + statusText + "</div><div class='sub'>Condicao geral do observatorio</div></div>";
    html += "<div class='summary-card " + rainClass + "'><h3>Chuva</h3><div class='value'>" + estado_chuva + "</div><div class='sub'>Sensor bruto: " + String(media_sens_chuva) + " | Limite: " + String(params.limite_chuva) + "</div></div>";
    html += "<div class='summary-card " + skyClass + "'><h3>Ceu</h3><div class='value'>" + estado_nuvens + "</div><div class='sub'>IR ceu: " + String(tIRceu, 1) + " C | Limite: " + String(params.limite_IR_ceu, 1) + " C</div></div>";
    html += "<div class='summary-card " + pressureClass + "'><h3>Pressao</h3><div class='value'>" + pressaoStatus + "</div><div class='sub'>Delta P: " + String(delta_p, 1) + " hPa</div></div>";
    html += "</div>";

    html += "<div class='grid'>";

    html += "<div class='card'><h2>Leituras Atuais</h2><table>";
    html += "<tr><th>Parametro</th><th>Valor</th></tr>";
    html += "<tr><td>Temperatura</td><td>" + String(t, 1) + " C</td></tr>";
    html += "<tr><td>Umidade</td><td>" + String(u, 1) + " %</td></tr>";
    html += "<tr><td>Ponto de Orvalho</td><td>" + String(to, 1) + " C</td></tr>";
    html += "<tr><td>Pressao Nominal</td><td>" + String(p_nominal, 1) + " hPa</td></tr>";
    html += "<tr><td>Pressao Medida</td><td>" + String(p, 1) + " hPa</td></tr>";
    html += "<tr><td>Delta P</td><td>" + String(delta_p, 1) + " hPa</td></tr>";
    html += "<tr><td>Temperatura Fundo do Ceu</td><td>" + String(tIRceu, 1) + " C</td></tr>";
    html += "<tr><td>Sensor de Chuva (bruto)</td><td><span class='" + sensorRainClass + "'>" + String(media_sens_chuva) + "</span></td></tr>";
    html += "<tr><td>Falhas BME / MLX</td><td>" + String(falhasBME) + " / " + String(falhasMLX) + "</td></tr>";
    html += "</table></div>";

    // ── Card saúde do barramento I²C ──────────────────────────────────────
    String i2cBadge, i2cClass;
    if (totalRecuperacoes == 0) {
      i2cBadge = "<span style='color:#166534;font-weight:bold'>&#10003; Estavel</span>";
    } else if (tentativasRecuperacao < 5) {
      i2cBadge = "<span style='color:#9a3412;font-weight:bold'>&#9888; Instavel</span>";
    } else {
      i2cBadge = "<span style='color:#991b1b;font-weight:bold'>&#9888; CRITICO</span>";
    }
    html += "<div class='card'><h2>Saude do Barramento I&#178;C</h2><table>";
    html += "<tr><th>Parametro</th><th>Valor</th></tr>";
    html += "<tr><td>Status</td><td>" + i2cBadge + "</td></tr>";
    html += "<tr><td>Recuperacoes desde boot</td><td>" + String(totalRecuperacoes) + "</td></tr>";
    html += "<tr><td>Tentativas na sessao atual</td><td>" + String(tentativasRecuperacao) + " / " + String(MAX_TENTATIVAS_RESTART) + "</td></tr>";
    html += "<tr><td>Ultima recuperacao</td><td>" + ultimaRecuperacaoHora + "</td></tr>";
    html += "<tr><td>Sensores na ultima falha</td><td>" + sensoresNaUltimaFalha + "</td></tr>";
    html += "<tr><td>Velocidade I2C</td><td>50 kHz</td></tr>";
    html += "<tr><td>Clock stretch limit</td><td>25 ms</td></tr>";
    html += "</table></div>";

    html += "<div class='card'><h2>Interpretacao do Sistema</h2><table>";
    html += "<tr><th>Parametro</th><th>Valor</th></tr>";
    html += "<tr><td>Estado do Ceu</td><td><span class='" + skyTextClass + "'>" + estado_nuvens + "</span></td></tr>";
    html += "<tr><td>Estado de Chuva</td><td><span class='" + rainTextClass + "'>" + estado_chuva + "</span></td></tr>";
    html += "<tr><td>Status da Pressao</td><td>" + pressaoStatus + "</td></tr>";
    html += "<tr><td>Tendencia Umidade</td><td>" + trendUmidade + "</td></tr>";
    html += "<tr><td>Tendencia Pressao</td><td>" + trendPressao + "</td></tr>";
    html += "<tr><td>Tendencia Ceu</td><td>" + trendCeu + "</td></tr>";
    html += "<tr><td><strong>Condicao de Seguranca</strong></td><td><span class='" + safeTextClass + "'><strong>" + statusText + "</strong></span></td></tr>";
    html += "</table><div class='mini-note'>Interpretacao baseada nos sensores e nos limites da EEPROM.</div></div>";

    html += "<div class='card'><h2>Parametros Configurados</h2><table>";
    html += "<tr><th>Parametro</th><th>Valor</th></tr>";
    html += "<tr><td>Timeout Nublado</td><td>" + String(params.timeout_nublado) + " min</td></tr>";
    html += "<tr><td>Limite Chuva</td><td>" + String(params.limite_chuva) + "</td></tr>";
    html += "<tr><td>Limite IR</td><td>" + String(params.limite_IR_ceu, 1) + " C</td></tr>";
    html += "<tr><td>Limite Umidade</td><td>" + String(params.limite_u, 1) + " %</td></tr>";
    html += "<tr><td>Limite Delta P</td><td>" + String(params.limite_p, 1) + " hPa</td></tr>";
    html += "<tr><td>Altitude</td><td>" + String(params.h, 0) + " m</td></tr>";
    html += "</table></div>";

    html += "</div>";
    html += "<div style='margin-top:20px;'>";
    html += "<a class='footer-link' href='/logs'>Ver arquivos de log</a> ";
    html += "<a class='footer-link' href='/i2clog' style='margin-left:10px;'>Log I2C</a> ";
    html += "<a class='footer-link' href='/espstatus' style='margin-left:10px;'>Status do ESP</a>";
    html += "</div></div></body></html>";

    req->send(200, "text/html", html);
  });

  server.on("/espstatus", HTTP_GET, [](AsyncWebServerRequest *request) {
    FSInfo fs_info;
    bool fsOk = LittleFS.info(fs_info);
    size_t fsFree = fsOk ? (fs_info.totalBytes - fs_info.usedBytes) : 0;

    uint32_t heapLivre   = ESP.getFreeHeap();
    uint32_t heapMaxBloco= ESP.getMaxFreeBlockSize();
    uint8_t  fragmentacao= ESP.getHeapFragmentation();
    long rssi   = WiFi.RSSI();
    String ipStr = WiFi.isConnected() ? WiFi.localIP().toString() : "offline";

    auto classeHeap  = [](uint32_t v)->String { return v>20000?"ok":v>12000?"warn":"crit"; };
    auto classeBloco = [](uint32_t v)->String { return v>12000?"ok":v>6000?"warn":"crit"; };
    auto classeFrag  = [](uint8_t  v)->String { return v<=20?"ok":v<=40?"warn":"crit"; };
    auto classeRSSI  = [](long     v)->String { return v>-70?"ok":v>-80?"warn":"crit"; };
    auto classeFSFree= [](size_t   v)->String { return v>200000?"ok":v>50000?"warn":"crit"; };
    auto badge = [](String cls)->String {
      if(cls=="ok")   return "<span class='badge ok'>OK</span>";
      if(cls=="warn") return "<span class='badge warn'>ATENCAO</span>";
      return "<span class='badge crit'>CRITICO</span>";
    };

    String page = "<!DOCTYPE html><html><head><meta charset='UTF-8'><title>Status ESP</title>";
    page += "<meta name='viewport' content='width=device-width,initial-scale=1'>";
    page += "<style>body{font-family:Arial,sans-serif;background:#f4f7fb;margin:0;padding:20px}";
    page += ".container{max-width:900px;margin:0 auto}.card{background:#fff;border-radius:14px;padding:18px;box-shadow:0 4px 14px rgba(0,0,0,.08);margin-bottom:20px}";
    page += "table{width:100%;border-collapse:collapse;font-size:15px}th{background:#e5e7eb;text-align:left;padding:10px;border-bottom:1px solid #d1d5db}td{padding:10px;border-bottom:1px solid #e5e7eb}tr:hover{background:#f9fafb}";
    page += ".badge{display:inline-block;padding:4px 10px;border-radius:999px;font-weight:bold;font-size:13px}.ok{background:#dcfce7;color:#166534}.warn{background:#ffedd5;color:#9a3412}.crit{background:#fee2e2;color:#991b1b}";
    page += ".btn{display:inline-block;background:#2563eb;color:#fff;text-decoration:none;padding:10px 14px;border-radius:10px;font-weight:bold}</style></head><body>";
    page += "<div class='container'><div class='card'><h1>Status do ESP8266</h1><a class='btn' href='/'>Voltar</a></div>";
    page += "<div class='card'><h2>Saude do Sistema</h2><table><tr><th>Parametro</th><th>Valor</th><th>Status</th></tr>";
    page += "<tr><td>Uptime</td><td>"        + formatUptime()          + "</td><td><span class='badge ok'>INFO</span></td></tr>";
    page += "<tr><td>Heap livre</td><td>"    + String(heapLivre)       + " bytes</td><td>" + badge(classeHeap(heapLivre))   + "</td></tr>";
    page += "<tr><td>Maior bloco</td><td>"   + String(heapMaxBloco)    + " bytes</td><td>" + badge(classeBloco(heapMaxBloco))+ "</td></tr>";
    page += "<tr><td>Fragmentacao</td><td>"  + String(fragmentacao)    + " %</td><td>"    + badge(classeFrag(fragmentacao)) + "</td></tr>";
    page += "<tr><td>Wi-Fi RSSI</td><td>"    + String(rssi)            + " dBm</td><td>"  + badge(classeRSSI(rssi))         + "</td></tr>";
    page += "<tr><td>IP</td><td>"            + ipStr                   + "</td><td><span class='badge ok'>INFO</span></td></tr>";
    page += "<tr><td>LittleFS livre</td><td>"+ String(fsFree)          + " bytes</td><td>"+ badge(classeFSFree(fsFree))     + "</td></tr>";
    page += "<tr><td>Falhas BME280</td><td>"   + String(falhasBME) + "</td><td>" + badge(falhasBME==0?"ok":falhasBME<5?"warn":"crit") + "</td></tr>";
    page += "<tr><td>Falhas MLX90614</td><td>"  + String(falhasMLX) + "</td><td>" + badge(falhasMLX==0?"ok":falhasMLX<3?"warn":"crit") + "</td></tr>";
    page += "<tr><td>Falhas BME280</td><td>"    + String(falhasBME) + "</td><td>" + badge(falhasBME==0?"ok":falhasBME<5?"warn":"crit") + "</td></tr>";
    page += "<tr><td>Recuperacoes I2C (boot)</td><td>" + String(totalRecuperacoes) + "</td><td>" + badge(totalRecuperacoes==0?"ok":totalRecuperacoes<5?"warn":"crit") + "</td></tr>";
    page += "<tr><td>Ultima recuperacao</td><td>" + ultimaRecuperacaoHora + "</td><td><span class='badge ok'>INFO</span></td></tr>";
    page += "<tr><td>Sensores na ultima falha</td><td>" + sensoresNaUltimaFalha + "</td><td><span class='badge ok'>INFO</span></td></tr>";
    page += "</table></div></div></body></html>";

    request->send(200, "text/html", page);
  });

  server.on("/i2clog", HTTP_GET, [](AsyncWebServerRequest *request) {
    String page = "<!DOCTYPE html><html><head><meta charset='UTF-8'>";
    page += "<title>Log I2C</title>";
    page += "<meta name='viewport' content='width=device-width,initial-scale=1'>";
    page += "<meta http-equiv='refresh' content='15'>";
    page += "<style>body{font-family:Arial,sans-serif;background:#f4f7fb;margin:0;padding:20px}";
    page += ".container{max-width:1000px;margin:0 auto}";
    page += ".card{background:#fff;border-radius:14px;padding:18px;box-shadow:0 4px 14px rgba(0,0,0,.08);margin-bottom:20px}";
    page += "table{width:100%;border-collapse:collapse;font-size:13px}";
    page += "th{background:#1e3a8a;color:#fff;text-align:left;padding:8px}";
    page += "td{padding:7px 8px;border-bottom:1px solid #e5e7eb}tr:hover{background:#f9fafb}";
    page += ".ok{color:#166534;font-weight:bold}.warn{color:#9a3412;font-weight:bold}.crit{color:#991b1b;font-weight:bold}";
    page += ".btn{display:inline-block;background:#2563eb;color:#fff;text-decoration:none;padding:8px 14px;border-radius:8px;font-weight:bold;margin-right:8px}</style></head><body>";
    page += "<div class='container'><div class='card'>";
    page += "<h1>Log de Eventos I&#178;C</h1>";
    page += "<a class='btn' href='/'>Voltar</a>";
    page += "<a class='btn' href='/i2clog'>Atualizar</a>";
    page += "<p style='margin-top:12px;color:#6b7280;font-size:13px'>Atualiza automaticamente a cada 15s. ";
    page += "Total de recuperacoes desde o boot: <strong>" + String(totalRecuperacoes) + "</strong></p>";
    page += "</div>";

    page += "<div class='card'><h2>Eventos registrados</h2>";

    File f = LittleFS.open("/i2c_eventos.csv", "r");
    if (!f || f.size() == 0) {
      page += "<p style='color:#166534;font-weight:bold'>&#10003; Nenhum evento de recuperacao registrado — barramento estavel!</p>";
    } else {
      page += "<table><tr><th>Data</th><th>Hora</th><th>Tentativa</th>";
      page += "<th>HTU falhas</th><th>MLX falhas</th><th>BMP falhas</th>";
      page += "<th>Sensores</th><th>Resultado</th></tr>";
      f.readStringUntil('\n'); // pula header
      int count = 0;
      while (f.available()) {
        String linha = f.readStringUntil('\n');
        linha.trim();
        if (linha.length() == 0) continue;
        // Parseia CSV
        int c[7];
        int pos = 0;
        for (int i = 0; i < 7; i++) {
          c[i] = linha.indexOf(',', pos);
          if (c[i] < 0) c[i] = linha.length();
          pos = c[i] + 1;
        }
        // Extrai campos simples
        String cols[8];
        pos = 0;
        for (int i = 0; i < 8; i++) {
          int next = linha.indexOf(',', pos);
          if (next < 0) next = linha.length();
          cols[i] = linha.substring(pos, next);
          pos = next + 1;
        }
        String resClass = cols[7] == "RESTART_ESP" ? "crit" : "warn";
        page += "<tr><td>" + cols[0] + "</td><td>" + cols[1] + "</td>";
        page += "<td>" + cols[2] + "</td><td>" + cols[3] + "</td>";
        page += "<td>" + cols[4] + "</td><td>" + cols[5] + "</td>";
        page += "<td>" + cols[6] + "</td>";
        page += "<td class='" + resClass + "'>" + cols[7] + "</td></tr>";
        count++;
      }
      page += "</table>";
      page += "<p style='color:#6b7280;font-size:12px;margin-top:8px'>" + String(count) + " eventos registrados.</p>";
    }
    if (f) f.close();
    page += "</div></div></body></html>";
    request->send(200, "text/html", page);
  });

  server.on("/i2clog", HTTP_DELETE, [](AsyncWebServerRequest *request) {
    LittleFS.remove("/i2c_eventos.csv");
    request->send(200, "text/plain", "Log I2C apagado.");
  });
  server.on("/logs", HTTP_GET, [](AsyncWebServerRequest *request) {
    const int logsPorPagina = 20;
    int pagina = 1;
    if (request->hasParam("page")) {
      pagina = request->getParam("page")->value().toInt();
      if (pagina < 1) pagina = 1;
    }

    int totalLogs = 0;
    Dir dirCount = LittleFS.openDir("/");
    while (dirCount.next()) {
      if (dirCount.fileName().endsWith("_Log_met.csv")) totalLogs++;
    }

    if (totalLogs == 0) {
      request->send(200, "text/html",
        "<html><head><meta charset='UTF-8'><title>Logs</title></head><body>"
        "<h2>Logs</h2><p>Nenhum log encontrado.</p><a href='/'>Voltar</a></body></html>");
      return;
    }

    String *logs = new String[totalLogs];
    int idx = 0;
    Dir dirFill = LittleFS.openDir("/");
    while (dirFill.next()) {
      String fn = dirFill.fileName();
      if (fn.endsWith("_Log_met.csv")) logs[idx++] = fn;
    }
    for (int i = 0; i < totalLogs - 1; i++)
      for (int j = i + 1; j < totalLogs; j++)
        if (logs[j] > logs[i]) { String tmp = logs[i]; logs[i] = logs[j]; logs[j] = tmp; }

    int totalPaginas = (totalLogs + logsPorPagina - 1) / logsPorPagina;
    if (pagina > totalPaginas) pagina = totalPaginas;
    int inicio = (pagina - 1) * logsPorPagina;
    int fim    = min(inicio + logsPorPagina, totalLogs);

    String page = "<html><head><meta charset='UTF-8'><title>Logs</title></head><body>";
    page += "<h2>Arquivos de log</h2><p>Pagina " + String(pagina) + " de " + String(totalPaginas) + "</p>";
    page += "<form method='POST' action='/delete_logs?page=" + String(pagina) + "'>";
    for (int i = inicio; i < fim; i++)
      page += "<input type='checkbox' name='log' value='" + logs[i] + "'> <a href='/log?f=" + logs[i] + "' target='_blank'>" + logs[i] + "</a><br>";
    page += "<br><input type='submit' value='Apagar selecionados'></form>";
    page += "<hr><p>";
    if (pagina > 1) page += "<a href='/logs?page=1'>&lt;&lt;</a> <a href='/logs?page=" + String(pagina-1) + "'>&lt;</a> ";
    for (int p = max(1,pagina-2); p <= min(totalPaginas,pagina+2); p++)
      page += (p==pagina) ? "<strong>"+String(p)+"</strong> " : "<a href='/logs?page="+String(p)+"'>"+String(p)+"</a> ";
    if (pagina < totalPaginas) page += "<a href='/logs?page=" + String(pagina+1) + "'>&gt;</a> <a href='/logs?page=" + String(totalPaginas) + "'>&gt;&gt;</a>";
    page += "</p><a href='/'>Voltar</a></body></html>";

    delete[] logs;
    request->send(200, "text/html", page);
  });

  server.on("/delete_logs", HTTP_POST, [](AsyncWebServerRequest *request) {
    int deleted = 0;
    int pagina  = 1;
    if (request->hasParam("page")) {
      pagina = request->getParam("page")->value().toInt();
      if (pagina < 1) pagina = 1;
    }
    int count = request->params();
    for (int i = 0; i < count; i++) {
      const AsyncWebParameter *pp = request->getParam(i);
      if (pp->isPost() && pp->name() == "log") {
        String filename = pp->value();
        if (!filename.startsWith("/")) filename = "/" + filename;
        if (LittleFS.exists(filename)) { LittleFS.remove(filename); deleted++; }
      }
    }
    String page = "<html><head><meta charset='UTF-8'></head><body>";
    page += "<p>" + String(deleted) + " arquivo(s) apagado(s).</p>";
    page += "<a href='/logs?page=" + String(pagina) + "'>Voltar</a></body></html>";
    request->send(200, "text/html", page);
  });

  server.on("/log", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (!request->hasParam("f")) { request->send(400, "text/plain", "Parametro 'f' ausente"); return; }
    String filename = request->getParam("f")->value();
    if (!filename.startsWith("/")) filename = "/" + filename;
    if (!LittleFS.exists(filename)) { request->send(404, "text/plain", "Arquivo nao encontrado"); return; }
    request->send(LittleFS, filename, "text/csv");
  });

  server.begin();
}
