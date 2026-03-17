//Ziller Safety Monitor
//Eduardo Ziller, Belo Horizonte, Brasil
//Entusiasta e astronomo amador
//IG: @eduardoziller
#include <Wire.h>
#include <Adafruit_BMP280.h>
#include <Adafruit_MLX90614.h>
#include <DHTesp.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <Ticker.h>
#include <time.h>
#include <EEPROM.h>
#include <LittleFS.h>

Adafruit_MLX90614 mlx;
Adafruit_BMP280 bmp;
DHTesp dht;
AsyncWebServer server(80);
Ticker watchdog;

#define SENSOR_DHT 0
#define PIN_SENSOR_CHUVA A0
#define LOG_INTERVAL_MINUTES 5
#define WIFI_RECONNECT_INTERVAL 10000
#define numsamples 20
#define TZ_OFFSET -3L * 3600L
#define MIN_FREE_SPIFFS 51200
// --- para tendência nas últimas 10 minutos (amostra a cada 5 s = 120 pontos) ---
#define TREND_WINDOW 360

float histPressure[TREND_WINDOW];
float histHumidity[TREND_WINDOW];
float histIR[TREND_WINDOW];
int histIndex = 0;
bool histInit = false;

const int ADDR_ALTITUDE = 0;          // 2 bytes (uint16)
const int ADDR_LIM_IR = 2;            // 2 bytes (uint16 = IR*10)
const int ADDR_LIM_CHUVA = 4;         // 2 bytes (uint16)
const int ADDR_LIM_DELTA_P = 6;       // 2 bytes (uint16 = ΔP*10 + offset)
const int ADDR_LIM_UMIDADE = 8;       // 2 bytes (uint16 = U*10)
const int ADDR_TIMEOUT_NUBLADO = 10;  // uso de 2 bytes (uint16 = minutos)

struct Parametros {
  float h;                   // altitude (m)
  int limite_chuva;          // ex: 800
  float limite_p;            // ex: –3.0
  float limite_u;            // ex: 75.0
  float limite_IR_ceu;       // limite IR inferior (ex: 2.0)
  uint16_t timeout_nublado;  // timeout em minutos para “nublado” (ex: 10)
} params;

float u, t, to, p, p_nominal, tIRceu, delta_p;
int media_sens_chuva = 0;
int condicaoSeguranca = 0;
String estado_chuva, estado_nuvens, pressaoStatus;
String trendPressao = "---";
String trendUmidade = "---";
String trendCeu = "---";
unsigned long agora, lastRead = 0, wifiLastCheck = 0;
static int8_t lastLoggedMinute = -1;
static int8_t lastLoggedHour = -1;

String ssid = "DEFAULT";
String password = "DEFAULT";
const char *stationName = "Estacao Meteorologica Observatorio Eureka";

void setDefaultParameters() {
  params.h = 0;
  params.limite_chuva = 850;
  params.limite_p = 0;
  params.limite_u = 65.0;
  params.limite_IR_ceu = 8.0;
  params.timeout_nublado = 10;
}

bool watchdogReset = false;
void alimentarWatchdog() {
  watchdogReset = true;
}
void verificarWatchdog() {
  if (!watchdogReset) ESP.restart();
  watchdogReset = false;
}

void carregarParametrosEEPROM() {
  // 1) ALTITUDE
  uint16_t a = EEPROM.read(ADDR_ALTITUDE) | (EEPROM.read(ADDR_ALTITUDE + 1) << 8);
  params.h = constrain((float)a, 0.0f, 10000.0f);
  // 2) Limite IR (inferior) ×10
  uint16_t ir10 = EEPROM.read(ADDR_LIM_IR) | (EEPROM.read(ADDR_LIM_IR + 1) << 8);
  params.limite_IR_ceu = ir10 / 10.0f;
  // 3) Limite Chuva
  uint16_t c = EEPROM.read(ADDR_LIM_CHUVA) | (EEPROM.read(ADDR_LIM_CHUVA + 1) << 8);
  params.limite_chuva = c;
  // 4) Limite ΔP +offset(1000) e ×10
  uint16_t dp10 = EEPROM.read(ADDR_LIM_DELTA_P) | (EEPROM.read(ADDR_LIM_DELTA_P + 1) << 8);
  params.limite_p = ((float)dp10 - 1000.0f) / 10.0f;
  // 5) Limite Umidade ×10
  uint16_t u10 = EEPROM.read(ADDR_LIM_UMIDADE) | (EEPROM.read(ADDR_LIM_UMIDADE + 1) << 8);
  params.limite_u = u10 / 10.0f;
  // 6) Timeout para tempo nublado
  uint16_t tout = EEPROM.read(ADDR_TIMEOUT_NUBLADO) | (EEPROM.read(ADDR_TIMEOUT_NUBLADO + 1) << 8);
  params.timeout_nublado = tout;  // já em minutos
}

void salvarParametrosEEPROM() {
  // grava ALTITUDE
  uint16_t a = (uint16_t)params.h;
  EEPROM.write(ADDR_ALTITUDE, a & 0xFF);
  EEPROM.write(ADDR_ALTITUDE + 1, (a >> 8) & 0xFF);
  // grava IR ×10
  uint16_t ir10 = (uint16_t)(params.limite_IR_ceu * 10.0f);
  EEPROM.write(ADDR_LIM_IR, ir10 & 0xFF);
  EEPROM.write(ADDR_LIM_IR + 1, (ir10 >> 8) & 0xFF);
  // grava Chuva
  uint16_t c = (uint16_t)params.limite_chuva;
  EEPROM.write(ADDR_LIM_CHUVA, c & 0xFF);
  EEPROM.write(ADDR_LIM_CHUVA + 1, (c >> 8) & 0xFF);
  // grava ΔP +offset1000 ×10
  uint16_t dp10 = (uint16_t)(params.limite_p * 10.0f + 1000.0f);
  EEPROM.write(ADDR_LIM_DELTA_P, dp10 & 0xFF);
  EEPROM.write(ADDR_LIM_DELTA_P + 1, (dp10 >> 8) & 0xFF);
  // grava Umidade ×10
  uint16_t u10 = (uint16_t)(params.limite_u * 10.0f);
  EEPROM.write(ADDR_LIM_UMIDADE, u10 & 0xFF);
  EEPROM.write(ADDR_LIM_UMIDADE + 1, (u10 >> 8) & 0xFF);
  // Grava Timeout Nublado (em minutos, inteiro)
  uint16_t tout = params.timeout_nublado;
  EEPROM.write(ADDR_TIMEOUT_NUBLADO, tout & 0xFF);
  EEPROM.write(ADDR_TIMEOUT_NUBLADO + 1, (tout >> 8) & 0xFF);

  EEPROM.commit();
}

void salvarCredenciaisSPIFFS(const String &ssid, const String &senha) {
  File f = LittleFS.open("/wifi.txt", "w");
  if (!f) {
    Serial.println("Erro ao abrir arquivo de credenciais");
    return;
  }
  f.println(ssid);
  f.println(senha);
  f.close();
  Serial.println("Credenciais Wi-Fi salvas com sucesso.");
}

void carregarCredenciaisWiFi() {
  File f = LittleFS.open("/wifi.txt", "r");
  if (f) {
    ssid = f.readStringUntil('\n');
    ssid.trim();
    password = f.readStringUntil('\n');
    password.trim();
    f.close();

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid.c_str(), password.c_str());
    Serial.print("⏳ Conectando ao Wi-Fi");
    int result = WiFi.waitForConnectResult();

    Serial.println();  // pular linha após os pontos
    if (result == WL_CONNECTED) {
      Serial.println("\n✅ Wi-Fi conectado com sucesso.");
      Serial.print("📡 IP: ");
      Serial.println(WiFi.localIP());
    } else {
      Serial.println("⚠ Não foi possível conectar. Operando offline.");
    }
  }
}

void setup() {
  delay(200);
  Serial.begin(115200);
  delay(100);

  EEPROM.begin(512);

  // 1) carrega TUDO da EEPROM
  carregarParametrosEEPROM();

  if (!LittleFS.begin()) {
    Serial.println("ERRO: falha ao montar LittleFS");
  } else {
    Serial.println("LittleFS montado com sucesso.");
  }
  carregarCredenciaisWiFi();  // Agora tenta conectar ou segue offline

  configTime(0, 0, "pool.ntp.org", "time.nist.gov");
  unsigned long tStart = millis();
  while (time(nullptr) < 24 * 3600 && millis() - tStart < 3000) {
    delay(100);
  }

  Wire.begin();
  mlx.begin();
  bmp.begin(0x76);
  bmp.setSampling(Adafruit_BMP280::MODE_NORMAL, Adafruit_BMP280::SAMPLING_X2, Adafruit_BMP280::SAMPLING_X16, Adafruit_BMP280::FILTER_X16, Adafruit_BMP280::STANDBY_MS_500);
  dht.setup(SENSOR_DHT, DHTesp::DHT11);

  watchdog.attach(30, alimentarWatchdog);
  iniciarServidor();
}

void loop() {
  alimentarWatchdog();
  handleLogging();
  agora = millis();
  tratarSerial();

  if (agora - lastRead >= 5000) {
    lastRead = agora;
    lerSensores();
    calcularSeguranca();
  }

  if (WiFi.status() != WL_CONNECTED && millis() - wifiLastCheck > WIFI_RECONNECT_INTERVAL) {
    wifiLastCheck = millis();
    WiFi.disconnect();
    WiFi.begin(ssid, password);
  }
}

void lerSensores() {
  u = dht.getHumidity();
  t = dht.getTemperature();
  p = bmp.readPressure() / 100.0;
  tIRceu = mlx.readObjectTempC();

  if (!histInit) {
    // preenche o buffer = valor inicial, garante delta=0
    for (int i = 0; i < TREND_WINDOW; i++) {
      histPressure[i] = p;
      histHumidity[i] = u;
      histIR[i] = tIRceu;
    }
    histInit = true;
  }
  // avança o índice circular
  histIndex = (histIndex + 1) % TREND_WINDOW;
  histPressure[histIndex] = p;
  histHumidity[histIndex] = u;
  histIR[histIndex] = tIRceu;

  float a = 17.27, b = 237.7;
  to = (b * (((a * t) / (b + t)) + log(u / 100))) / (a - ((a * t) / (b + t) + log(u / 100)));

  p_nominal = 1013.25 * exp(-0.0289644 * 9.81 * params.h / (8.31432 * (t + 272.15)));
  delta_p = p - p_nominal;

  int soma = 0;
  for (int i = 0; i < numsamples; i++) {
    soma += analogRead(PIN_SENSOR_CHUVA);
    yield();
  }
  media_sens_chuva = soma / numsamples;
}

void calcularSeguranca() {
  // calcula diferença com 10 min atrás (índice circular)
  int oldIdx = (histIndex + 1) % TREND_WINDOW;
  float deltaP = p - histPressure[oldIdx];
  float deltaU = u - histHumidity[oldIdx];
  float deltaIR = tIRceu - histIR[oldIdx];

  // threshold de 2 unidades
  auto trendOf = [&](float d) -> int {
    if (d > 2.0f) return 1;
    if (d < -2.0f) return -1;
    return 0;
  };

  int tP = trendOf(deltaP);
  int tU = trendOf(deltaU);
  int tI = trendOf(deltaIR);

  const char *txtT[3] = { "Queda", "Estável", "Aumento" };
  trendPressao = String(txtT[tP + 1]);
  trendUmidade = String(txtT[tU + 1]);
  trendCeu = String(txtT[tI + 1]);

  // defaults
  estado_nuvens = "Céu limpo";
  estado_chuva = "Sem chuva";

  // flags
  bool chovendo = media_sens_chuva <= params.limite_chuva;
  bool ir_alta = tIRceu > params.limite_IR_ceu;
  bool umid_alta = u >= params.limite_u;
  bool press_baixa = delta_p <= params.limite_p;

  static unsigned long inicio = 0;
  unsigned long agora = millis();

  if (chovendo) {
    // 1ª condição: chuva UNSAFE imediato
    condicaoSeguranca = 100;
    estado_chuva = "Chovendo";
    estado_nuvens = "Nublado";
    inicio = 0;
  } else if (ir_alta && umid_alta && press_baixa) {
    // 2ª condição (estática): IR>limiteIR + umidade + pressão baixa = UNSAFE
    condicaoSeguranca = 100;
    estado_chuva = "Risco alto de chuva: umid. alta, pressão baixa e nublado";
    estado_nuvens = "Nublado";
    inicio = 0;
  } else if (ir_alta && umid_alta) {
    // 3ª condição (dinâmica): IR>limiteIR + umidade alta por timeout
    if (inicio == 0) {
      inicio = agora;         // marca o início do “persistente”
      condicaoSeguranca = 0;  // por enquanto SAFE
      estado_chuva = "Sem chuva";
      estado_nuvens = "Nublado";
    } else if (agora - inicio >= (unsigned long)params.timeout_nublado * 60UL * 1000UL) {
      condicaoSeguranca = 100;
      estado_chuva = "Risco de chuva: tempo nubl e umid. alta por tempo maior que Timeout";
      estado_nuvens = "Nublado";
      // mantém inicio para não resetar até sair de ir_alta||umid_alta
    } else {
      // ainda dentro do timeout
      condicaoSeguranca = 0;
      estado_chuva = "Sem chuva";
      estado_nuvens = "Nublado";
    }
  } else if (ir_alta) {
    // opcional: só IR>limiteIR sem umidade alta = SAFE, mas mostra “Nublado”
    condicaoSeguranca = 0;
    estado_chuva = "Sem chuva";
    estado_nuvens = "Nublado";
    inicio = 0;
  } else {
    // IR≤limiteIR ou umid baixa = SAFE e reseta contador
    condicaoSeguranca = 0;
    estado_chuva = "Sem chuva";
    estado_nuvens = "Céu limpo";
    inicio = 0;
  }

  // Atualização de status de pressão (mesmo do código anterior)
  if (delta_p <= params.limite_p) pressaoStatus = "Baixa";
  else if (delta_p > params.limite_p && delta_p < 2.0) pressaoStatus = "Normal";
  else pressaoStatus = "Alta";
  /*
  //=====================DEBUG========================
  
  bool ndCondIr_Umid_PressBaixa = (ir_alta && umid_alta && press_baixa);
  bool rdCondIr_Umidade_timeout = (ir_alta && umid_alta) && (agora - inicio >= (unsigned long)params.timeout_nublado * 60UL * 1000UL);

  Serial.printf(
    "DBG conds → 1a Cond: chuva:%d 2a Cond: IR,Umid e Press Bx:%d 3a Cond IR e Umid timeout:%d\n",
    chovendo ? 1 : 0,
    ndCondIr_Umid_PressBaixa ? 1 : 0,
    rdCondIr_Umidade_timeout ? 1 : 0
  );
  */
}


void tratarSerial() {
  if (Serial.available()) {
    String linha = Serial.readStringUntil('\n');
    linha.trim();

    if (linha.startsWith("ALT:")) {
      float h_recebida = linha.substring(4).toFloat();
      if (h_recebida > 0) {
        params.h = h_recebida;
        salvarParametrosEEPROM();
        Serial.printf("Altitude atualizada: %.1f\n", params.h);
      }
    } else if (linha.startsWith("LIMITE_IR:")) {
      float v = linha.substring(10).toFloat();
      params.limite_IR_ceu = v;
      salvarParametrosEEPROM();
      Serial.println("Limite IR atualizado");
    } else if (linha.startsWith("LIMITE_CHUVA:")) {
      int v = linha.substring(13).toInt();
      params.limite_chuva = v;
      salvarParametrosEEPROM();
      Serial.println("Limite Chuva atualizado");
    } else if (linha.startsWith("LIMITE_P:")) {
      float v = linha.substring(9).toFloat();
      params.limite_p = v;
      salvarParametrosEEPROM();
      Serial.println("Limite ΔP atualizado");
    } else if (linha.startsWith("LIMITE_U:")) {
      float v = linha.substring(9).toFloat();
      params.limite_u = v;
      salvarParametrosEEPROM();
      Serial.println("Limite Umidade atualizado");
    } else if (linha.startsWith("TIMEOUT_NUBLADO:")) {
      uint16_t v = (uint16_t)linha.substring(String("TIMEOUT_NUBLADO:").length()).toInt();
      params.timeout_nublado = v;
      salvarParametrosEEPROM();
      Serial.println("Timeout nublado atualizado");
    } else if (linha.startsWith("WIFI:")) {
      String payload = linha.substring(5);
      int sep = payload.indexOf(',');
      String ssid = payload.substring(0, sep);
      String pass = payload.substring(sep + 1);
      salvarCredenciaisSPIFFS(ssid, pass);
      Serial.println("OK");
      delay(1000);
      ESP.restart();
    } else if (linha == "SCAN_WIFI") {
      WiFi.mode(WIFI_STA);
      int n = WiFi.scanNetworks();
      if (n == 0) {
        Serial.println("Nenhuma rede encontrada.");
      } else {
        for (int i = 0; i < n; ++i) {
          Serial.print("SSID:");
          Serial.println(WiFi.SSID(i));
        }
      }
      Serial.println("#");
    }
  }

  static unsigned long ultimaPublicacaoSerial = 0;
  unsigned long agorapub = millis();

  if (agorapub - ultimaPublicacaoSerial >= 5000) {
    ultimaPublicacaoSerial = agorapub;
    Serial.printf("ALT:%.1f;T:%.1f;U:%.1f;TO:%.1f;PN:%.1f;P:%.1f;DP:%.1f;IR:%.1f;SENSCH:%d;LIMCH:%d;CEU:%s;CHUVA:%s;SP:%s;TP:%s;TU:%s;TC:%s;SEG:%s\r\n",
                  params.h,
                  t,
                  u,
                  to,
                  p_nominal,
                  p,
                  delta_p,
                  tIRceu,
                  media_sens_chuva,
                  params.limite_chuva,
                  estado_nuvens.c_str(),
                  estado_chuva.c_str(),
                  pressaoStatus.c_str(),
                  trendPressao.c_str(),
                  trendUmidade.c_str(),
                  trendCeu.c_str(),
                  condicaoSeguranca == 0 ? "SAFE" : "UNSAFE");
    //Serial.println();
  }
}

String formatUptime() {
  unsigned long totalSeconds = millis() / 1000;
  unsigned long days = totalSeconds / 86400;
  totalSeconds %= 86400;
  unsigned long hours = totalSeconds / 3600;
  totalSeconds %= 3600;
  unsigned long minutes = totalSeconds / 60;
  unsigned long seconds = totalSeconds % 60;

  char buf[32];
  snprintf(buf, sizeof(buf), "%lu d %02lu:%02lu:%02lu", days, hours, minutes, seconds);
  return String(buf);
}

void iniciarServidor() {
  /*
  //++++++++DEBUG DOS LOGS++++++++++++++
  server.on("/logs", HTTP_GET, [](AsyncWebServerRequest *request) {
    Serial.println("ENTROU NA ROTA /logs");
    request->send(200, "text/plain", "Rota /logs OK");
  });

  server.on("/log", HTTP_GET, [](AsyncWebServerRequest *request) {
    Serial.println("ENTROU NA ROTA /log");
    request->send(200, "text/plain", "Rota /log OK");
  });

  server.onNotFound([](AsyncWebServerRequest *request) {
    Serial.print("404 em: ");
    Serial.println(request->url());
    request->send(404, "text/plain", "Nao encontrado");
  });
  //++++++++FIM DEBUG DOS LOGS++++++++++++++
 
 //+++++++++DEBUG DO SISTEMA LITTLEFS E ARQUIVOS NA EEPROM++++++++++++++
 server.on("/fsdebug", HTTP_GET, [](AsyncWebServerRequest *request) {
    String out = "";

    FSInfo fs_info;
    if (!LittleFS.info(fs_info)) {
      request->send(500, "text/plain", "Erro ao ler informacoes do LittleFS");
      return;
    }

    out += "LittleFS montado.\n";
    out += "Total: " + String(fs_info.totalBytes) + "\n";
    out += "Usado: " + String(fs_info.usedBytes) + "\n";
    out += "Livre: " + String(fs_info.totalBytes - fs_info.usedBytes) + "\n";
    out += "\nArquivos:\n";

    Dir dir = LittleFS.openDir("/");
    int count = 0;
    while (dir.next()) {
      count++;
      out += dir.fileName();
      out += "\n";
    }

    if (count == 0) {
      out += "(nenhum arquivo encontrado)\n";
    }

    request->send(200, "text/plain", out);
  });
 //+++++++++FIM DEBUG DO SISTEMA LITTLEFS E ARQUIVOS NA EEPROM++++++++++++++
*/

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
      body {
        font-family: Arial, Helvetica, sans-serif;
        background: #f4f7fb;
        color: #1f2937;
        margin: 0;
        padding: 0;
      }

      .container {
        max-width: 1200px;
        margin: 0 auto;
        padding: 20px;
      }

      .header {
        background: linear-gradient(135deg, #0f172a, #1e3a8a);
        color: white;
        padding: 24px;
        border-radius: 14px;
        box-shadow: 0 4px 14px rgba(0,0,0,0.15);
        margin-bottom: 20px;
      }

      .header h1 {
        margin: 0 0 8px 0;
        font-size: 28px;
      }

     .header p {
        margin: 4px 0;
        font-size: 15px;
        opacity: 0.95;
      }

      .summary-grid {
        display: grid;
        grid-template-columns: repeat(auto-fit, minmax(220px, 1fr));
        gap: 16px;
        margin: 20px 0;
      }

      .summary-card {
        border-radius: 14px;
        padding: 18px;
        color: white;
        box-shadow: 0 4px 14px rgba(0,0,0,0.12);
      }

      .summary-card h3 {
        margin: 0 0 10px 0;
        font-size: 16px;
        opacity: 0.95;
      }

      .summary-card .value {
        font-size: 26px;
        font-weight: bold;
        margin-bottom: 6px;
      }

      .summary-card .sub {
        font-size: 14px;
        opacity: 0.95;
      }

      .safe-card { background: linear-gradient(135deg, #16a34a, #15803d); }
      .unsafe-card { background: linear-gradient(135deg, #dc2626, #991b1b); }
      .rain-ok-card { background: linear-gradient(135deg, #2563eb, #1d4ed8); }
      .rain-bad-card { background: linear-gradient(135deg, #b91c1c, #7f1d1d); }
      .sky-ok-card { background: linear-gradient(135deg, #0891b2, #155e75); }
      .sky-bad-card { background: linear-gradient(135deg, #6d28d9, #4c1d95); }
      .press-ok-card { background: linear-gradient(135deg, #0f766e, #115e59); }
      .press-bad-card { background: linear-gradient(135deg, #ca8a04, #a16207); }

      .grid {
        display: grid;
        grid-template-columns: repeat(auto-fit, minmax(340px, 1fr));
        gap: 20px;
      }

      .card {
        background: white;
        border-radius: 14px;
        padding: 18px;
        box-shadow: 0 4px 14px rgba(0,0,0,0.08);
      }

      .card h2 {
        margin-top: 0;
        margin-bottom: 14px;
        font-size: 20px;
        color: #111827;
        border-bottom: 2px solid #e5e7eb;
        padding-bottom: 8px;
      }

      table {
        width: 100%;
        border-collapse: collapse;
        margin-top: 10px;
        font-size: 15px;
      }

      th {
        background: #e5e7eb;
        text-align: left;
        padding: 10px;
        border-bottom: 1px solid #d1d5db;
      }

      td {
        padding: 10px;
        border-bottom: 1px solid #e5e7eb;
      }

      tr:hover {
        background: #f9fafb;
      }

      .txt-good {
        font-weight: bold;
        color: #166534;
      }

      .txt-bad {
        font-weight: bold;
        color: #991b1b;
      }

      .txt-warn {
        font-weight: bold;
        color: #92400e;
      }

      .footer-link {
        margin-top: 20px;
        display: inline-block;
        background: #2563eb;
        color: white;
        text-decoration: none;
        padding: 10px 16px;
        border-radius: 10px;
        font-weight: bold;
      }

      .footer-link:hover {
        background: #1d4ed8;
      }

      .mini-note {
        font-size: 13px;
        color: #6b7280;
        margin-top: 8px;
      }
    </style>
  </head>
  <body>
    <div class="container">
  )rawliteral";

    time_t now = time(nullptr) + TZ_OFFSET;
    struct tm ti;
    localtime_r(&now, &ti);
    char buf[32];
    snprintf(buf, sizeof(buf), "%04d-%02d-%02d %02d:%02d:%02d",
             ti.tm_year + 1900, ti.tm_mon + 1, ti.tm_mday,
             ti.tm_hour, ti.tm_min, ti.tm_sec);

    bool isSafe = (condicaoSeguranca == 0);
    bool rainDetected = (media_sens_chuva <= params.limite_chuva);
    bool cloudyDetected = (tIRceu > params.limite_IR_ceu);

    String statusClass = isSafe ? "safe-card" : "unsafe-card";
    String statusText = isSafe ? "SAFE" : "UNSAFE";

    String rainClass = rainDetected ? "rain-bad-card" : "rain-ok-card";
    String skyClass = cloudyDetected ? "sky-bad-card" : "sky-ok-card";
    String pressureClass = (pressaoStatus == "Normal") ? "press-ok-card" : "press-bad-card";

    String rainTextClass = rainDetected ? "txt-bad" : "txt-good";
    String skyTextClass = cloudyDetected ? "txt-warn" : "txt-good";
    String safeTextClass = isSafe ? "txt-good" : "txt-bad";
    String sensorRainClass = rainDetected ? "txt-bad" : "txt-good";

    html += "<div class='header'>";
    html += "<h1>" + String(stationName) + "</h1>";
    html += "<p><strong>Data / Hora:</strong> " + String(buf) + "</p>";
    html += "<p><strong>Atualização automática:</strong> a cada 10 segundos</p>";
    html += "</div>";

    // RESUMO RÁPIDO
    html += "<div class='summary-grid'>";

    html += "<div class='summary-card " + statusClass + "'>";
    html += "<h3>Segurança</h3>";
    html += "<div class='value'>" + statusText + "</div>";
    html += "<div class='sub'>Condição geral do observatório</div>";
    html += "</div>";

    html += "<div class='summary-card " + rainClass + "'>";
    html += "<h3>Chuva</h3>";
    html += "<div class='value'>" + estado_chuva + "</div>";
    html += "<div class='sub'>Sensor bruto: " + String(media_sens_chuva) + " | Limite: " + String(params.limite_chuva) + "</div>";
    html += "</div>";

    html += "<div class='summary-card " + skyClass + "'>";
    html += "<h3>Céu</h3>";
    html += "<div class='value'>" + estado_nuvens + "</div>";
    html += "<div class='sub'>IR céu: " + String(tIRceu, 1) + " &deg;C | Limite: " + String(params.limite_IR_ceu, 1) + " &deg;C</div>";
    html += "</div>";

    html += "<div class='summary-card " + pressureClass + "'>";
    html += "<h3>Pressão</h3>";
    html += "<div class='value'>" + pressaoStatus + "</div>";
    html += "<div class='sub'>Delta P: " + String(delta_p, 1) + " hPa</div>";
    html += "</div>";

    html += "</div>";

    html += "<div class='grid'>";

    // CARD 1 - LEITURAS
    html += "<div class='card'>";
    html += "<h2>Leituras Atuais</h2>";
    html += "<table>";
    html += "<tr><th>Parâmetro</th><th>Valor</th></tr>";
    html += "<tr><td>Temperatura</td><td>" + String(t, 1) + " &deg;C</td></tr>";
    html += "<tr><td>Umidade</td><td>" + String(u, 1) + " %</td></tr>";
    html += "<tr><td>Ponto de Orvalho</td><td>" + String(to, 1) + " &deg;C</td></tr>";
    html += "<tr><td>Pressão Nominal</td><td>" + String(p_nominal, 1) + " hPa</td></tr>";
    html += "<tr><td>Pressão Medida</td><td>" + String(p, 1) + " hPa</td></tr>";
    html += "<tr><td>Delta P</td><td>" + String(delta_p, 1) + " hPa</td></tr>";
    html += "<tr><td>Temperatura Fundo do Céu</td><td>" + String(tIRceu, 1) + " &deg;C</td></tr>";
    html += "<tr><td>Sensor de Chuva (bruto)</td><td><span class='" + sensorRainClass + "'>" + String(media_sens_chuva) + "</span></td></tr>";
    html += "</table>";
    html += "</div>";

    // CARD 2 - INTERPRETAÇÃO
    html += "<div class='card'>";
    html += "<h2>Interpretação do Sistema</h2>";
    html += "<table>";
    html += "<tr><th>Parâmetro</th><th>Valor</th></tr>";
    html += "<tr><td>Estado do Céu</td><td><span class='" + skyTextClass + "'>" + estado_nuvens + "</span></td></tr>";
    html += "<tr><td>Estado de Chuva</td><td><span class='" + rainTextClass + "'>" + estado_chuva + "</span></td></tr>";
    html += "<tr><td>Status da Pressão</td><td>" + pressaoStatus + "</td></tr>";
    html += "<tr><td>Tendência Umidade</td><td>" + trendUmidade + "</td></tr>";
    html += "<tr><td>Tendência Pressão</td><td>" + trendPressao + "</td></tr>";
    html += "<tr><td>Tendência Céu</td><td>" + trendCeu + "</td></tr>";
    html += "<tr><td><strong>Condição de Segurança</strong></td><td><span class='" + safeTextClass + "'><strong>" + statusText + "</strong></span></td></tr>";
    html += "</table>";
    html += "<div class='mini-note'>A interpretação é baseada nos sensores e nos limites configurados na EEPROM.</div>";
    html += "</div>";

    // CARD 3 - PARÂMETROS
    html += "<div class='card'>";
    html += "<h2>Parâmetros Configurados</h2>";
    html += "<table>";
    html += "<tr><th>Parâmetro</th><th>Valor</th></tr>";
    html += "<tr><td>Timeout Nublado</td><td>" + String(params.timeout_nublado) + " min</td></tr>";
    html += "<tr><td>Limite Chuva</td><td>" + String(params.limite_chuva) + "</td></tr>";
    html += "<tr><td>Limite IR</td><td>" + String(params.limite_IR_ceu, 1) + " &deg;C</td></tr>";
    html += "<tr><td>Limite Umidade</td><td>" + String(params.limite_u, 1) + " %</td></tr>";
    html += "<tr><td>Limite Delta P</td><td>" + String(params.limite_p, 1) + " hPa</td></tr>";
    html += "</table>";
    html += "</div>";

    html += "</div>";

    html += "<div style='margin-top:20px;'>";
    html += "<a class='footer-link' href='/logs'>📂 Ver arquivos de log</a> ";
    html += "<a class='footer-link' href='/espstatus' style='margin-left:10px;'>🛠 Status do ESP</a>";
    html += "</div>";

    html += R"rawliteral(
    </div>
  </body>
  </html>
  )rawliteral";

    req->send(200, "text/html", html);
  });

  server.on("/espstatus", HTTP_GET, [](AsyncWebServerRequest *request) {
    FSInfo fs_info;
    bool fsOk = LittleFS.info(fs_info);

    size_t fsTotal = fsOk ? fs_info.totalBytes : 0;
    size_t fsUsed = fsOk ? fs_info.usedBytes : 0;
    size_t fsFree = fsOk ? (fs_info.totalBytes - fs_info.usedBytes) : 0;

    uint32_t heapLivre = ESP.getFreeHeap();
    uint32_t heapMaxBloco = ESP.getMaxFreeBlockSize();
    uint8_t fragmentacao = ESP.getHeapFragmentation();

    long rssi = WiFi.RSSI();
    String ipStr = WiFi.isConnected() ? WiFi.localIP().toString() : "offline";

    const int EEPROM_SIZE_BYTES = 512;
    const int EEPROM_USED_BYTES = 12;
    const int EEPROM_FREE_BYTES = EEPROM_SIZE_BYTES - EEPROM_USED_BYTES;

    auto classeHeap = [&](uint32_t v) -> String {
      if (v > 20000) return "ok";
      if (v > 12000) return "warn";
      return "crit";
    };

    auto classeBloco = [&](uint32_t v) -> String {
      if (v > 12000) return "ok";
      if (v > 6000) return "warn";
      return "crit";
    };

    auto classeFrag = [&](uint8_t v) -> String {
      if (v <= 20) return "ok";
      if (v <= 40) return "warn";
      return "crit";
    };

    auto classeRSSI = [&](long v) -> String {
      if (v > -70) return "ok";
      if (v > -80) return "warn";
      return "crit";
    };

    auto classeFSFree = [&](size_t v) -> String {
      if (v > 200000) return "ok";
      if (v > 50000) return "warn";
      return "crit";
    };

    String page = "<!DOCTYPE html><html><head><meta charset='UTF-8'><title>Status do ESP</title>";
    page += "<meta name='viewport' content='width=device-width, initial-scale=1.0'>";
    page += "<style>";
    page += "body{font-family:Arial,Helvetica,sans-serif;background:#f4f7fb;color:#1f2937;margin:0;padding:20px;}";
    page += ".container{max-width:900px;margin:0 auto;}";
    page += ".card{background:#ffffff;border-radius:14px;padding:18px;box-shadow:0 4px 14px rgba(0,0,0,0.08);margin-bottom:20px;}";
    page += "h1{margin-top:0;margin-bottom:10px;}";
    page += "h2{margin-top:0;margin-bottom:14px;font-size:20px;border-bottom:2px solid #e5e7eb;padding-bottom:8px;}";
    page += "table{width:100%;border-collapse:collapse;font-size:15px;}";
    page += "th{background:#e5e7eb;text-align:left;padding:10px;border-bottom:1px solid #d1d5db;}";
    page += "td{padding:10px;border-bottom:1px solid #e5e7eb;}";
    page += "tr:hover{background:#f9fafb;}";
    page += ".badge{display:inline-block;padding:4px 10px;border-radius:999px;font-weight:bold;font-size:13px;}";
    page += ".ok{background:#dcfce7;color:#166534;}";
    page += ".warn{background:#ffedd5;color:#9a3412;}";
    page += ".crit{background:#fee2e2;color:#991b1b;}";
    page += ".muted{font-size:12px;color:#6b7280;margin-top:12px;}";
    page += ".topbar{display:flex;justify-content:space-between;align-items:center;gap:12px;flex-wrap:wrap;}";
    page += ".btn{display:inline-block;background:#2563eb;color:#fff;text-decoration:none;padding:10px 14px;border-radius:10px;font-weight:bold;}";
    page += ".btn:hover{background:#1d4ed8;}";
    page += "</style></head><body>";

    page += "<div class='container'>";
    page += "<div class='card'>";
    page += "<div class='topbar'>";
    page += "<div><h1>Status do ESP8266</h1><div>Diagnóstico rápido do firmware e do hardware</div></div>";
    page += "<div><a class='btn' href='/'>Voltar</a></div>";
    page += "</div>";
    page += "</div>";

    page += "<div class='card'>";
    page += "<h2>Saúde do Sistema</h2>";
    page += "<table>";
    page += "<tr><th>Parâmetro</th><th>Valor</th><th>Status</th></tr>";

    page += "<tr><td>Uptime</td><td>" + formatUptime() + "</td><td><span class='badge ok'>INFO</span></td></tr>";

    page += "<tr><td>Heap livre</td><td>" + String(heapLivre) + " bytes</td><td><span class='badge " + classeHeap(heapLivre) + "'>" + (classeHeap(heapLivre) == "ok" ? "OK" : (classeHeap(heapLivre) == "warn" ? "ATENÇÃO" : "CRÍTICO")) + "</span></td></tr>";

    page += "<tr><td>Maior bloco livre</td><td>" + String(heapMaxBloco) + " bytes</td><td><span class='badge " + classeBloco(heapMaxBloco) + "'>" + (classeBloco(heapMaxBloco) == "ok" ? "OK" : (classeBloco(heapMaxBloco) == "warn" ? "ATENÇÃO" : "CRÍTICO")) + "</span></td></tr>";

    page += "<tr><td>Fragmentação do heap</td><td>" + String(fragmentacao) + " %</td><td><span class='badge " + classeFrag(fragmentacao) + "'>" + (classeFrag(fragmentacao) == "ok" ? "OK" : (classeFrag(fragmentacao) == "warn" ? "ATENÇÃO" : "CRÍTICO")) + "</span></td></tr>";

    page += "<tr><td>Wi-Fi RSSI</td><td>" + String(rssi) + " dBm</td><td><span class='badge " + classeRSSI(rssi) + "'>" + (classeRSSI(rssi) == "ok" ? "OK" : (classeRSSI(rssi) == "warn" ? "ATENÇÃO" : "CRÍTICO")) + "</span></td></tr>";

    page += "<tr><td>IP</td><td>" + ipStr + "</td><td><span class='badge ok'>INFO</span></td></tr>";

    page += "<tr><td>LittleFS total</td><td>" + String(fsTotal) + " bytes</td><td><span class='badge ok'>INFO</span></td></tr>";
    page += "<tr><td>LittleFS usado</td><td>" + String(fsUsed) + " bytes</td><td><span class='badge ok'>INFO</span></td></tr>";

    page += "<tr><td>LittleFS livre</td><td>" + String(fsFree) + " bytes</td><td><span class='badge " + classeFSFree(fsFree) + "'>" + (classeFSFree(fsFree) == "ok" ? "OK" : (classeFSFree(fsFree) == "warn" ? "ATENÇÃO" : "CRÍTICO")) + "</span></td></tr>";

    page += "<tr><td>EEPROM reservada</td><td>" + String(EEPROM_SIZE_BYTES) + " bytes</td><td><span class='badge ok'>INFO</span></td></tr>";
    page += "<tr><td>EEPROM usada (mapa atual)</td><td>" + String(EEPROM_USED_BYTES) + " bytes</td><td><span class='badge ok'>INFO</span></td></tr>";
    page += "<tr><td>EEPROM livre estimada</td><td>" + String(EEPROM_FREE_BYTES) + " bytes</td><td><span class='badge ok'>INFO</span></td></tr>";

    page += "</div></body></html>";

    request->send(200, "text/html", page);
  });

  server.on("/logs", HTTP_GET, [](AsyncWebServerRequest *request) {
    const int logsPorPagina = 20;

    int pagina = 1;
    if (request->hasParam("page")) {
      pagina = request->getParam("page")->value().toInt();
      if (pagina < 1) pagina = 1;
    }

    // 1) contar logs
    int totalLogs = 0;
    Dir dirCount = LittleFS.openDir("/");
    while (dirCount.next()) {
      String fn = dirCount.fileName();
      if (fn.endsWith("_Log_met.csv")) {
        totalLogs++;
      }
    }

    if (totalLogs == 0) {
      String page = "<html><head><meta charset='UTF-8'><title>Logs</title></head><body>";
      page += "<h2>Arquivos de log</h2>";
      page += "<p>Nenhum log encontrado.</p>";
      page += "<br><a href='/'>Voltar</a></body></html>";
      request->send(200, "text/html", page);
      return;
    }

    // 2) carregar nomes em array
    String *logs = new String[totalLogs];
    int idx = 0;

    Dir dirFill = LittleFS.openDir("/");
    while (dirFill.next()) {
      String fn = dirFill.fileName();
      if (fn.endsWith("_Log_met.csv")) {
        logs[idx++] = fn;
      }
    }

    // 3) ordenar do mais novo para o mais antigo
    for (int i = 0; i < totalLogs - 1; i++) {
      for (int j = i + 1; j < totalLogs; j++) {
        if (logs[j] > logs[i]) {
          String tmp = logs[i];
          logs[i] = logs[j];
          logs[j] = tmp;
        }
      }
    }

    // 4) calcular paginação
    int totalPaginas = (totalLogs + logsPorPagina - 1) / logsPorPagina;
    if (pagina > totalPaginas) pagina = totalPaginas;

    int inicio = (pagina - 1) * logsPorPagina;
    int fim = inicio + logsPorPagina;
    if (fim > totalLogs) fim = totalLogs;

    // 5) montar página
    String page = "<html><head><meta charset='UTF-8'><title>Logs</title></head><body>";
    page += "<h2>Arquivos de log</h2>";
    page += "<p>Página " + String(pagina) + " de " + String(totalPaginas) + "</p>";

    page += "<form method='POST' action='/delete_logs?page=" + String(pagina) + "'>";

    for (int i = inicio; i < fim; i++) {
      page += "<input type='checkbox' name='log' value='" + logs[i] + "'> ";
      page += "<a href='/log?f=" + logs[i] + "' target='_blank'>" + logs[i] + "</a><br>";
    }

    page += "<br><input type='submit' value='Apagar selecionados'>";
    page += "</form>";

    // navegação
    page += "<hr><p>";

    if (pagina > 1) {
      page += "<a href='/logs?page=1'>&lt;&lt;</a> ";
      page += "<a href='/logs?page=" + String(pagina - 1) + "'>&lt;</a> ";
    }

    int primeiraPaginaVisivel = pagina - 2;
    int ultimaPaginaVisivel = pagina + 2;

    if (primeiraPaginaVisivel < 1) primeiraPaginaVisivel = 1;
    if (ultimaPaginaVisivel > totalPaginas) ultimaPaginaVisivel = totalPaginas;

    for (int p = primeiraPaginaVisivel; p <= ultimaPaginaVisivel; p++) {
      if (p == pagina) {
        page += "<strong>" + String(p) + "</strong> ";
      } else {
        page += "<a href='/logs?page=" + String(p) + "'>" + String(p) + "</a> ";
      }
    }

    if (pagina < totalPaginas) {
      page += "<a href='/logs?page=" + String(pagina + 1) + "'>&gt;</a> ";
      page += "<a href='/logs?page=" + String(totalPaginas) + "'>&gt;&gt;</a>";
    }

    page += "</p>";
    page += "<br><a href='/'>Voltar</a></body></html>";

    delete[] logs;

    request->send(200, "text/html", page);
  });

  server.on("/delete_logs", HTTP_POST, [](AsyncWebServerRequest *request) {
    int deleted = 0;
    int pagina = 1;

    if (request->hasParam("page")) {
      pagina = request->getParam("page")->value().toInt();
      if (pagina < 1) pagina = 1;
    }

    int count = request->params();
    for (int i = 0; i < count; i++) {
      const AsyncWebParameter *p = request->getParam(i);
      if (p->isPost() && p->name() == "log") {
        String filename = p->value();
        if (!filename.startsWith("/")) {
          filename = "/" + filename;
        }
        if (LittleFS.exists(filename)) {
          LittleFS.remove(filename);
          deleted++;
        }
      }
    }

    String page = "<html><head><meta charset='UTF-8'><title>Logs apagados</title></head><body>";
    page += "<p>" + String(deleted) + " arquivo(s) apagado(s).</p>";
    page += "<a href='/logs?page=" + String(pagina) + "'>Voltar para os logs</a>";
    page += "</body></html>";

    request->send(200, "text/html", page);
  });

  server.on("/log", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (!request->hasParam("f")) {
      request->send(400, "text/plain", "Parâmetro 'f' ausente");
      return;
    }

    String filename = request->getParam("f")->value();

    if (!filename.startsWith("/")) {
      filename = "/" + filename;
    }

    if (!LittleFS.exists(filename)) {
      request->send(404, "text/plain", "Arquivo não encontrado: " + filename);
      return;
    }

    request->send(LittleFS, filename, "text/csv");
  });

  server.begin();
}

void handleLogging() {
  // obtém hora local
  time_t epoch = time(nullptr);
  time_t localEpoch = epoch + TZ_OFFSET;
  struct tm ti;
  localtime_r(&localEpoch, &ti);
  int currentMinute = ti.tm_min;
  int currentHour = ti.tm_hour;

  // converte tudo em minutos desde meia-noite
  int totalMins = currentHour * 60 + currentMinute;

  // variáveis estáticas para saber quando já loguei
  static int lastLoggedTotalMins = -1;

  // se for múltiplo do intervalo E ainda não foi logado neste minuto
  if ((totalMins % LOG_INTERVAL_MINUTES) == 0 && totalMins != lastLoggedTotalMins) {
    lastLoggedTotalMins = totalMins;
    logData();
  }
}

void logData() {
  cleanupOldLogs(MIN_FREE_SPIFFS);

  time_t tnow = time(nullptr);
  tnow += TZ_OFFSET;  // Corrige para UTC-3
  struct tm ti;
  localtime_r(&tnow, &ti);

  char dateBuf[16];
  snprintf(dateBuf, sizeof(dateBuf), "%04d.%02d.%02d", ti.tm_year + 1900, ti.tm_mon + 1, ti.tm_mday);
  char timeBuf[16];
  snprintf(timeBuf, sizeof(timeBuf), "%02d:%02d:%02d", ti.tm_hour, ti.tm_min, ti.tm_sec);

  String fname = String("/") + dateBuf + "_Log_met.csv";
  bool exists = LittleFS.exists(fname);
  File f = LittleFS.open(fname, "a");
  if (!f) return;

  if (!exists) {
    f.println("DATA,HORA,TEMPERATURA,UMIDADE,TEMP_ORVALHO,PRESSAO,TIRCEU,ESTADO_NUVENS,ESTADO_CHUVA,CONDICAO_SEGURANCA");
  }

  f.printf("%s,%s,%.1f,%.1f,%.1f,%.1f,%.1f,%s,%s,%s\n",
           dateBuf,
           timeBuf,
           t,
           u,
           to,
           p,
           tIRceu,
           estado_nuvens.c_str(),
           estado_chuva.c_str(),
           condicaoSeguranca == 0 ? "SAFE" : "UNSAFE");
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
        if (oldest == "" || fn < oldest) {
          oldest = fn;
        }
      }
    }

    if (oldest == "") break;

    Serial.print("Apagando log antigo para liberar espaço: ");
    Serial.println(oldest);

    LittleFS.remove(oldest);

    if (!LittleFS.info(fs_info)) break;
    freeBytes = fs_info.totalBytes - fs_info.usedBytes;
  }
}
