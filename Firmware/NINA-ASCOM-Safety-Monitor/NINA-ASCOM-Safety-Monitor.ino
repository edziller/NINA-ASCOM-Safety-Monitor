// Versão otimizada do firmware NINA-ASCOM-Safety-Monitor.ino
// Organizado para fluidez, resposta serial rápida e parametrização flexível
// Versão otimizada com log, web, watchdog e limpeza de SPIFFS e download dos logs
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
#define LOG_INTERVAL_MINUTES 30
#define WIFI_RECONNECT_INTERVAL 10000
#define numsamples 20
#define TZ_OFFSET -3L * 3600L
#define MIN_FREE_SPIFFS 20480
// --- para tendência nas últimas 10 minutos (amostra a cada 5 s = 120 pontos) ---
#define TREND_WINDOW 360

float histPressure[TREND_WINDOW];
float histHumidity[TREND_WINDOW];
float histIR[TREND_WINDOW];
int   histIndex = 0;
bool  histInit = false;

const int ADDR_ALTITUDE       = 0;  // 2 bytes (uint16)
const int ADDR_LIM_IR         = 2;  // 2 bytes (uint16 = IR*10)
const int ADDR_LIM_CHUVA      = 4;  // 2 bytes (uint16)
const int ADDR_LIM_DELTA_P    = 6;  // 2 bytes (uint16 = ΔP*10 + offset)
const int ADDR_LIM_UMIDADE    = 8;  // 2 bytes (uint16 = U*10)
const int ADDR_TIMEOUT_NUBLADO  = 10;  // uso de 2 bytes (uint16 = minutos)

struct Parametros {
  float h;                 // altitude (m)
  int   limite_chuva;      // ex: 890
  float limite_p;          // ex: –3.0
  float limite_u;          // ex: 75.0
  float limite_IR_ceu;     // limite IR inferior (ex: 2.0)
  uint16_t timeout_nublado;// timeout em minutos para “nublado” (ex: 10)
} params;

float u, t, to, p, p_nominal, tIRceu, delta_p;
int media_sens_chuva = 0;
int condicaoSeguranca = 0;
String estado_chuva, estado_nuvens, pressaoStatus;
String trendPressao = "---";
String trendUmidade = "---";
String trendCeu     = "---";
unsigned long agora, lastRead = 0, wifiLastCheck = 0;
static int8_t lastLoggedMinute = -1;
static int8_t lastLoggedHour = -1;

String ssid = "DEFAULT";
String password = "DEFAULT";
const char* stationName = "Estacao Meteorologica Observatorio Eureka";

void setDefaultParameters() {
  params.h = 0;
  params.limite_chuva = 890;
  params.limite_p = -3.0;
  params.limite_u = 75.0;
  params.limite_IR_ceu = 2.0;
  params.timeout_nublado = 10;         
}

bool watchdogReset = false;
void alimentarWatchdog() { watchdogReset = true; }
void verificarWatchdog() {
  if (!watchdogReset) ESP.restart();
  watchdogReset = false;
}

void carregarParametrosEEPROM() {
  // 1) ALTITUDE
  uint16_t a = EEPROM.read(ADDR_ALTITUDE) | (EEPROM.read(ADDR_ALTITUDE+1) << 8);
  params.h = constrain((float)a, 0.0f, 10000.0f);
  // 2) Limite IR (inferior) ×10
  uint16_t ir10 = EEPROM.read(ADDR_LIM_IR) | (EEPROM.read(ADDR_LIM_IR+1) << 8);
  params.limite_IR_ceu = ir10 / 10.0f;
  // 3) Limite Chuva
  uint16_t c = EEPROM.read(ADDR_LIM_CHUVA) | (EEPROM.read(ADDR_LIM_CHUVA+1) << 8);
  params.limite_chuva = c;
  // 4) Limite ΔP +offset(1000) e ×10
  uint16_t dp10 = EEPROM.read(ADDR_LIM_DELTA_P) | (EEPROM.read(ADDR_LIM_DELTA_P+1) << 8);
  params.limite_p = ( (float)dp10 - 1000.0f ) / 10.0f;
  // 5) Limite Umidade ×10
  uint16_t u10 = EEPROM.read(ADDR_LIM_UMIDADE) | (EEPROM.read(ADDR_LIM_UMIDADE+1) << 8);
  params.limite_u = u10 / 10.0f;
  // 6) Timeout para tempo nublado
  uint16_t tout = EEPROM.read(ADDR_TIMEOUT_NUBLADO) | (EEPROM.read(ADDR_TIMEOUT_NUBLADO+1) << 8);
  params.timeout_nublado = tout; // já em minutos
}

void salvarParametrosEEPROM() {
  // grava ALTITUDE
  uint16_t a = (uint16_t)params.h;
  EEPROM.write(ADDR_ALTITUDE,       a & 0xFF);
  EEPROM.write(ADDR_ALTITUDE + 1,  (a >> 8) & 0xFF);
  // grava IR ×10
  uint16_t ir10 = (uint16_t)(params.limite_IR_ceu * 10.0f);
  EEPROM.write(ADDR_LIM_IR,       ir10 & 0xFF);
  EEPROM.write(ADDR_LIM_IR + 1,  (ir10 >> 8) & 0xFF);
  // grava Chuva
  uint16_t c = (uint16_t)params.limite_chuva;
  EEPROM.write(ADDR_LIM_CHUVA,       c & 0xFF);
  EEPROM.write(ADDR_LIM_CHUVA + 1,  (c >> 8) & 0xFF);
  // grava ΔP +offset1000 ×10
  uint16_t dp10 = (uint16_t)(params.limite_p * 10.0f + 1000.0f);
  EEPROM.write(ADDR_LIM_DELTA_P,      dp10 & 0xFF);
  EEPROM.write(ADDR_LIM_DELTA_P + 1, (dp10 >> 8) & 0xFF);
  // grava Umidade ×10
  uint16_t u10 = (uint16_t)(params.limite_u * 10.0f);
  EEPROM.write(ADDR_LIM_UMIDADE,       u10 & 0xFF);
  EEPROM.write(ADDR_LIM_UMIDADE + 1,  (u10 >> 8) & 0xFF);
  // Grava Timeout Nublado (em minutos, inteiro)
  uint16_t tout = params.timeout_nublado;
  EEPROM.write(ADDR_TIMEOUT_NUBLADO,       tout & 0xFF);
  EEPROM.write(ADDR_TIMEOUT_NUBLADO + 1,  (tout >> 8) & 0xFF);

  EEPROM.commit();
}

void salvarCredenciaisSPIFFS(const String &ssid, const String &senha) {
  if (!LittleFS.begin()) {
    Serial.println("Erro ao montar SPIFFS");
    return;
  }
  File f = LittleFS.open("/wifi.txt", "w");
  if (!f) {
    Serial.println("Erro ao abrir arquivo de credenciais");
    return;
  }
  f.println(ssid);
  f.println(senha);
  f.close();
  LittleFS.end();
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

    Serial.println(); // pular linha após os pontos
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

  LittleFS.begin();

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
    
    //Serial.print(media_sens_chuva);
    //Serial.println();
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
      histIR[i]       = tIRceu;
    }
    histInit = true;
  }
  // avança o índice circular
  histIndex = (histIndex + 1) % TREND_WINDOW;
  histPressure[histIndex] = p;
  histHumidity[histIndex] = u;
  histIR[histIndex]       = tIRceu;

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
  float deltaP  = p      - histPressure[oldIdx];
  float deltaU  = u      - histHumidity[oldIdx];
  float deltaIR = tIRceu - histIR[oldIdx];

  // threshold de 2 unidades
  auto trendOf = [&](float d)->int {
    if (d >  2.0f) return  1;
    if (d < -2.0f) return -1;
    return  0;
  };

  int tP = trendOf(deltaP);
  int tU = trendOf(deltaU);
  int tI = trendOf(deltaIR);

  const char* txtT[3] = {"Queda","Estável","Aumento"};
  trendPressao = String(txtT[tP + 1]);
  trendUmidade = String(txtT[tU + 1]);
  trendCeu     = String(txtT[tI + 1]);

  // defaults
  estado_nuvens = "Céu limpo";
  estado_chuva  = "Sem chuva";
  
  // flags
  bool chovendo    = media_sens_chuva <= params.limite_chuva;
  bool ir_alta     = tIRceu       > params.limite_IR_ceu; 
  bool umid_alta   = u            >= params.limite_u;
  bool press_baixa = delta_p <= params.limite_p;

  static unsigned long inicio = 0;
  unsigned long agora = millis();
  
  if (chovendo) {
    // 1ª condição: chuva → UNSAFE imediato
    condicaoSeguranca = 100;
    estado_chuva      = "Chovendo";
    estado_nuvens     = "Nublado";
    inicio            = 0;
  }
  else if (ir_alta && umid_alta && press_baixa) {
    // 2ª condição (estática): IR>limiteIR + umidade + pressão baixa → UNSAFE
    condicaoSeguranca = 100;
    estado_chuva      = "Risco alto de chuva: umid. alta, pressão baixa e nublado";
    estado_nuvens     = "Nublado";
    inicio            = 0;
  }
  else if (ir_alta && umid_alta) {
    // 3ª condição (dinâmica): IR>limiteIR + umidade alta por timeout
    if (inicio == 0) {
      inicio            = agora;             // marca o início do “persistente”
      condicaoSeguranca = 0;                 // por enquanto SAFE
      estado_chuva      = "Sem chuva";
      estado_nuvens     = "Nublado";
    }
    else if (agora - inicio >= (unsigned long)params.timeout_nublado * 60UL * 1000UL) {
      condicaoSeguranca = 100;
      estado_chuva      = "Risco de chuva: tempo nubl e umid. alta por tempo maior que Timeout";
      estado_nuvens     = "Nublado";
      // mantém inicio para não resetar até sair de ir_alta||umid_alta
    }
    else {
      // ainda dentro do timeout
      condicaoSeguranca = 0;
      estado_chuva      = "Sem chuva";
      estado_nuvens     = "Nublado";
    }
  }
  else if (ir_alta) {
    // opcional: só IR>limiteIR sem umidade alta → SAFE, mas mostra “Nublado”
    condicaoSeguranca = 0;
    estado_chuva      = "Sem chuva";
    estado_nuvens     = "Nublado";
    inicio            = 0;
  }
  else {
    // IR≤limiteIR ou umid baixa → SAFE e reseta contador
    condicaoSeguranca = 0;
    estado_chuva      = "Sem chuva";
    estado_nuvens     = "Céu limpo";
    inicio            = 0;
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
    }
    else if (linha.startsWith("LIMITE_IR:")) {
      float v = linha.substring(10).toFloat();
      params.limite_IR_ceu = v;
      salvarParametrosEEPROM();
      Serial.println("Limite IR atualizado");
    }
    else if (linha.startsWith("LIMITE_CHUVA:")) {
      int v = linha.substring(13).toInt();
      params.limite_chuva = v;
      salvarParametrosEEPROM();
      Serial.println("Limite Chuva atualizado");
    }
    else if (linha.startsWith("LIMITE_P:")) {
      float v = linha.substring(9).toFloat();
      params.limite_p = v;
      salvarParametrosEEPROM();
      Serial.println("Limite ΔP atualizado");
    }
    else if (linha.startsWith("LIMITE_U:")) {
      float v = linha.substring(9).toFloat();
      params.limite_u = v;
      salvarParametrosEEPROM();
      Serial.println("Limite Umidade atualizado");
    }
     else if (linha.startsWith("TIMEOUT_NUBLADO:")) {
      uint16_t v = (uint16_t)linha.substring(String("TIMEOUT_NUBLADO:").length()).toInt();
      params.timeout_nublado = v;
      salvarParametrosEEPROM();
      Serial.println("Timeout nublado atualizado");
    }
    else if (linha.startsWith("WIFI:")) {
      String payload = linha.substring(5);
      int sep = payload.indexOf(',');
      String ssid = payload.substring(0, sep);
      String pass = payload.substring(sep + 1);
      salvarCredenciaisSPIFFS(ssid, pass);
      Serial.println("OK");
      delay(1000);
      ESP.restart();
      } 
      else if (linha == "SCAN_WIFI") {
        WiFi.mode(WIFI_STA);    
        int n = WiFi.scanNetworks();
        if (n == 0) {
          Serial.println("Nenhuma rede encontrada.");
        } 
        else {
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
    Serial.printf("ALT:%.1f;T:%.1f;U:%.1f;TO:%.1f;PN:%.1f;P:%.1f;DP:%.1f;IR:%.1f;CEU:%s;CHUVA:%s;SP:%s;TP:%s;TU:%s;TC:%s;SEG:%s\n",
    params.h,
    t, 
    u, 
    to, 
    p_nominal, 
    p, 
    delta_p, 
    tIRceu, 
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

void iniciarServidor() {
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *req) {
    String html = "<!DOCTYPE html><html><head><meta charset='UTF-8'><meta http-equiv='refresh' content='10'><style>body{font-family:sans-serif;}</style></head><body>";
    html += "<h1>" + String(stationName) + "</h1>";
    time_t now = time(nullptr) + TZ_OFFSET;
    struct tm ti;
    localtime_r(&now, &ti);
    char buf[32];
    snprintf(buf, sizeof(buf), "%04d-%02d-%02d %02d:%02d:%02d", ti.tm_year + 1900, ti.tm_mon + 1, ti.tm_mday, ti.tm_hour, ti.tm_min, ti.tm_sec);
    html += String("<p><strong>Data/Hora:</strong> ") + buf + "</p>";
    html += "<p>Altitude: " + String(params.h, 1) + " m</p>";
    html += "<p>Temperatura: " + String(t, 1) + " &deg;C</p>";
    html += "<p>Umidade: " + String(u, 1) + " %</p>";
    html += "<p>Tendência Umidade: " + trendUmidade + "</p>";
    html += "<p>Ponto de orvalho: " + String(to, 1) + " &deg;C</p>";
    html += "<p>Pressão nominal: " + String(p_nominal, 1) + " hPa</p>";
    html += "<p>Pressão medida: " + String(p, 1) + " hPa</p>";
    html += "<p>Delta P: " + String(delta_p, 1) + " hPa</p>";
    html += "<p>Status da pressão: " + pressaoStatus + "</p>";
    html += "<p>Tendência Pressão: " + trendPressao + "</p>";
    html += "<p>Temp. fundo do céu: " + String(tIRceu, 1) + " &deg;C</p>";
    html += "<p>Estado do céu: " + estado_nuvens + "</p>";
    html += "<p>Tendência Céu: "      + trendCeu + "</p>";
    html += "<p>Chuva: " + estado_chuva + "</p>";
    html += "<p>Segurança: " + String(condicaoSeguranca == 0 ? "SAFE" : "UNSAFE") + "</p>";
    html += "<p><a href='/logs'>📂 Ver arquivos de log</a></p>";
    html += "</body></html>";
    req->send(200, "text/html", html);
  });

  server.on("/logs", HTTP_GET, [](AsyncWebServerRequest *request) {
    String page = "<html><head><meta charset='UTF-8'><title>Logs</title></head><body><h2>Arquivos de log</h2><form method='POST' action='/delete_logs'>";
    Dir dir = LittleFS.openDir("/");
    while (dir.next()) {
      String fn = dir.fileName();
      if (fn.endsWith("_Log_met.csv")) {
        page += "<input type='checkbox' name='log' value='" + fn + "'> <a href='/log?f=" + fn + "' target='_blank'>" + fn + "</a><br>";
      }
    }
    page += "<br><input type='submit' value='Apagar Selecionados'></form><br><a href='/'>Voltar</a></body></html>";
    request->send(200, "text/html", page);
  });

  server.on("/delete_logs", HTTP_POST, [](AsyncWebServerRequest *request) {
    int deleted = 0;
    if (request->hasParam("log", true)) {
      int count = request->params();
      for (int i = 0; i < count; i++) {
        const AsyncWebParameter* p = request->getParam(i);
        if (p->isPost() && p->name() == "log") {
          String filename = p->value();
          if (LittleFS.exists(filename)) {
            LittleFS.remove(filename);
            deleted++;
          }
        }
      }
    }
    request->send(200, "text/html", "<html><body><p>" + String(deleted) + " arquivo(s) apagado(s).</p><a href='/logs'>Voltar</a></body></html>");
  });

  server.on("/log", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (!request->hasParam("f")) {
      request->send(400, "text/plain", "Parâmetro 'f' ausente");
      return;
    }
    String filename = request->getParam("f")->value();
    if (!LittleFS.exists(filename)) {
      request->send(404, "text/plain", "Arquivo não encontrado");
      return;
    }
    request->send(LittleFS, filename, "text/csv");
  });
  
  server.on("/debug", HTTP_GET, [](AsyncWebServerRequest *req) {
    String dbg = "";
    dbg += "Altitude (params.h): "      + String(params.h, 1)              + "\n";
    dbg += "Limite IR (inferior): "     + String(params.limite_IR_ceu, 1)  + "\n";
    dbg += "Limite Chuva: "             + String(params.limite_chuva)      + "\n";
    dbg += "Limite ΔP: "                + String(params.limite_p, 1)       + "\n";
    dbg += "Limite Umidade: "           + String(params.limite_u, 1)       + "\n";
    dbg += "Timeout Nublado (min): "    + String(params.timeout_nublado)   + "\n";
    dbg += "\n";
    dbg += "Última leitura serial:\n";
    dbg += "  ALT:"  + String(params.h,1) + "; ";
    dbg += "TIR:"   + String(tIRceu,1)     + "; ";
    dbg += "UMID:"  + String(u,1)          + "; ";
    dbg += "PRESS:" + String(p,1)          + "; ";
    dbg += "\n";
    req->send(200, "text/plain", dbg);
  });

  server.begin();
}

void handleLogging() {
  // obtém hora local
  time_t epoch      = time(nullptr);
  time_t localEpoch = epoch + TZ_OFFSET;
  struct tm ti;
  localtime_r(&localEpoch, &ti);
  int currentMinute = ti.tm_min;
  int currentHour   = ti.tm_hour;

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
      if (fn.endsWith("_Log_met.txt")) {
        if (oldest == "" || fn < oldest) oldest = fn;
      }
    }
    if (oldest == "") break;
    LittleFS.remove(oldest);
    if (!LittleFS.info(fs_info)) break;
    freeBytes = fs_info.totalBytes - fs_info.usedBytes;
  }
}

