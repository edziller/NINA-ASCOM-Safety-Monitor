// Ziller Safety Monitor
// Eduardo Ziller, Belo Horizonte, Brasil
// Entusiasta e astrônomo amador
// IG: @eduardoziller
//
// Correções aplicadas (v3):
//  - Tendência: substituído endpoint comparison por média suavizada de 6 amostras
//    comparada com janela de 15 minutos atrás (TREND_SMOOTH=6, TREND_LAG=180)
//    Elimina falsas tendências causadas por picos isolados do DHT11/MLX90614
//  - Thresholds distintos por grandeza: P=1.5hPa, U=5%, IR=3°C
//  - Web page: "Tendencia Ceu" renomeado para "Tend. Nuvens"
//  - (v2) Watchdog hardware, EEPROM magic byte, validação NaN, I2C 50kHz,
//    scan WiFi assíncrono, ponto de orvalho clampado

#include <Wire.h>
#include <Adafruit_BMP280.h>
#include <Adafruit_MLX90614.h>
#include <DHTesp.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <time.h>
#include <EEPROM.h>
#include <LittleFS.h>

Adafruit_MLX90614 mlx;
Adafruit_BMP280 bmp;
DHTesp dht;
AsyncWebServer server(80);

// ── Pinos ─────────────────────────────────────────────────────────────────────
#define SENSOR_DHT           0
#define PIN_SENSOR_CHUVA     A0

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
#define TREND_THRESH_P   1.5f   // hPa — limiar para considerar tendência de pressão
#define TREND_THRESH_U   5.0f   // %   — variações < 5% são ruído do DHT11
#define TREND_THRESH_IR  3.0f   // °C  — limiar para tendência de cobertura de nuvens

// ── Parâmetros do cálculo suavizado de tendência ──────────────────────────────
// Compara média de TREND_SMOOTH amostras recentes vs média de TREND_SMOOTH
// amostras de TREND_LAG posições atrás no buffer circular.
// TREND_SMOOTH=6 → janela de 30s de suavização (elimina picos do DHT11)
// TREND_LAG=180  → compara com 15 minutos atrás (180 × 5s)
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
// Se um sensor falhar repetidamente, mantemos o último valor válido mas
// registramos para diagnóstico.
uint8_t falhasDHT = 0, falhasMLX = 0, falhasBMP = 0;

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

  // FIX: I2C a 50kHz para estabilidade com cabo > 10cm
  Wire.begin();
  Wire.setClock(50000);

  // Inicializa sensores
  if (!mlx.begin()) {
    Serial.println("AVISO: MLX90614 nao encontrado. Verifique o I2C.");
  }
  if (!bmp.begin(0x76)) {
    Serial.println("AVISO: BMP280 nao encontrado. Verifique o I2C.");
  } else {
    bmp.setSampling(
      Adafruit_BMP280::MODE_NORMAL,
      Adafruit_BMP280::SAMPLING_X2,
      Adafruit_BMP280::SAMPLING_X16,
      Adafruit_BMP280::FILTER_X16,
      Adafruit_BMP280::STANDBY_MS_500
    );
  }
  dht.setup(SENSOR_DHT, DHTesp::DHT11);

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
//  LEITURA DOS SENSORES  (com validação de NaN)
// ─────────────────────────────────────────────────────────────────────────────
void lerSensores() {
  // DHT11: valida NaN, mantém último valor válido
  float novo_u = dht.getHumidity();
  float novo_t = dht.getTemperature();
  if (!isnan(novo_u) && !isnan(novo_t) && novo_u >= 0.0f && novo_u <= 100.0f) {
    u = novo_u;
    t = novo_t;
    falhasDHT = 0;
  } else {
    falhasDHT = min((int)falhasDHT + 1, 255);
    // mantém u e t anteriores
  }

  // BMP280 
  float novo_p = bmp.readPressure() / 100.0f;
  if (!isnan(novo_p) && novo_p > 800.0f && novo_p < 1100.0f) {
    p = novo_p;
    falhasBMP = 0;
  } else {
    falhasBMP = min((int)falhasBMP + 1, 255);
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

  // Sensor de chuva analógico — lê DEPOIS das transações I2C para evitar ruído
  int soma = 0;
  for (int i = 0; i < NUMSAMPLES; i++) {
    soma += analogRead(PIN_SENSOR_CHUVA);
    yield();
  }
  media_sens_chuva = soma / NUMSAMPLES;
}

// ─────────────────────────────────────────────────────────────────────────────
//  CÁLCULO DE SEGURANÇA
// ─────────────────────────────────────────────────────────────────────────────
void calcularSeguranca() {

  // ── Tendência com suavização por média de janela deslizante ──────────────────
  // Compara média de TREND_SMOOTH amostras recentes vs TREND_SMOOTH amostras
  // de TREND_LAG posições atrás. Elimina picos isolados do DHT11/MLX.
  float sumP_now = 0, sumP_old = 0;
  float sumU_now = 0, sumU_old = 0;
  float sumI_now = 0, sumI_old = 0;

  for (int i = 0; i < TREND_SMOOTH; i++) {
    int ni = (histIndex - i + TREND_WINDOW) % TREND_WINDOW;
    int oi = (histIndex - TREND_LAG - i + TREND_WINDOW) % TREND_WINDOW;
    sumP_now += histPressure[ni];  sumP_old += histPressure[oi];
    sumU_now += histHumidity[ni];  sumU_old += histHumidity[oi];
    sumI_now += histIR[ni];        sumI_old += histIR[oi];
  }
  float deltaP  = (sumP_now - sumP_old) / TREND_SMOOTH;
  float deltaU  = (sumU_now - sumU_old) / TREND_SMOOTH;
  float deltaIR = (sumI_now - sumI_old) / TREND_SMOOTH;

  trendPressao = deltaP  >  TREND_THRESH_P  ? "Aumento" : deltaP  < -TREND_THRESH_P  ? "Queda" : "Estavel";
  trendUmidade = deltaU  >  TREND_THRESH_U  ? "Aumento" : deltaU  < -TREND_THRESH_U  ? "Queda" : "Estavel";
  trendCeu     = deltaIR >  TREND_THRESH_IR ? "Aumento" : deltaIR < -TREND_THRESH_IR ? "Queda" : "Estavel";

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
      "FD:%d;FM:%d;FB:%d\r\n",
      params.h, t, u, to, p_nominal, p, delta_p, tIRceu,
      media_sens_chuva, params.limite_chuva,
      estado_nuvens.c_str(), estado_chuva.c_str(), pressaoStatus.c_str(),
      trendPressao.c_str(), trendUmidade.c_str(), trendCeu.c_str(),
      condicaoSeguranca == 0 ? "SAFE" : "UNSAFE",
      falhasDHT, falhasMLX, falhasBMP   // contadores de falha para diagnóstico
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
              "ESTADO_NUVENS,ESTADO_CHUVA,CONDICAO_SEGURANCA,FALHAS_DHT,FALHAS_MLX,FALHAS_BMP");
  }
  f.printf("%s,%s,%.1f,%.1f,%.1f,%.1f,%.1f,%s,%s,%s,%d,%d,%d\n",
           dateBuf, timeBuf,
           t, u, to, p, tIRceu,
           estado_nuvens.c_str(), estado_chuva.c_str(),
           condicaoSeguranca == 0 ? "SAFE" : "UNSAFE",
           falhasDHT, falhasMLX, falhasBMP);
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

    html += "<div class='header'><h1>" + String(stationName) + "</h1>";
    html += "<p><strong>Data / Hora:</strong> " + String(buf) + "</p>";
    html += "<p><strong>Atualizacao automatica:</strong> a cada 10 segundos</p></div>";

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
    html += "<tr><td>Falhas DHT / MLX / BMP</td><td>" + String(falhasDHT) + " / " + String(falhasMLX) + " / " + String(falhasBMP) + "</td></tr>";
    html += "</table></div>";

    html += "<div class='card'><h2>Interpretacao do Sistema</h2><table>";
    html += "<tr><th>Parametro</th><th>Valor</th></tr>";
    html += "<tr><td>Estado do Ceu</td><td><span class='" + skyTextClass + "'>" + estado_nuvens + "</span></td></tr>";
    html += "<tr><td>Estado de Chuva</td><td><span class='" + rainTextClass + "'>" + estado_chuva + "</span></td></tr>";
    html += "<tr><td>Status da Pressao</td><td>" + pressaoStatus + "</td></tr>";
    html += "<tr><td>Tendencia Umidade</td><td>" + trendUmidade + "</td></tr>";
    html += "<tr><td>Tendencia Pressao</td><td>" + trendPressao + "</td></tr>";
    html += "<tr><td>Tend. Nuvens</td><td>" + trendCeu + "</td></tr>";
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
    page += "<tr><td>Falhas DHT11</td><td>"  + String(falhasDHT)       + "</td><td>"      + badge(falhasDHT==0?"ok":falhasDHT<5?"warn":"crit") + "</td></tr>";
    page += "<tr><td>Falhas MLX90614</td><td>"+ String(falhasMLX)      + "</td><td>"      + badge(falhasMLX==0?"ok":falhasMLX<3?"warn":"crit") + "</td></tr>";
    page += "<tr><td>Falhas BMP280</td><td>" + String(falhasBMP)       + "</td><td>"      + badge(falhasBMP==0?"ok":falhasBMP<5?"warn":"crit") + "</td></tr>";
    page += "</table></div></div></body></html>";

    request->send(200, "text/html", page);
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
