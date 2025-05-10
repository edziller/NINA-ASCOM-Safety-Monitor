/****************************************************************************** 
Sistema de Monitor de Segurança para observatório remoto
por Eduardo Ziller
******************************************************************************/
#include <Wire.h> 
#include <Adafruit_BMP280.h>
#include <Adafruit_MLX90614.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <DHTesp.h> 
#include <Ticker.h>
#include <FS.h>
#include <time.h>
#include <ArduinoJson.h>

//IRTherm therm; // Cria um objeto para o sensor de temperatura
Adafruit_MLX90614 mlx;
Adafruit_BMP280 bmp; // I2C - Cria um objeto para o Sensor de Pressão
#define FILE_READ "r"
#define SENSOR_DHT 0 //Define a ligação ao pino de dados do sensor de umidade
DHTesp dht; //Define o tipo de sensor DHT utilizado
#define PIN_SENSOR_CHUVA A0 //Definindo a entrada do sensor de chuva
#define numsamples 20 //Número de amostras para médias
#define LOG_INTERVAL_MINUTES  60   // coloque 60 em produção

//Declaração de variáveis
//Limites e dados calibráveis
int amostra[numsamples];
int media_sens_chuva = 0; //variável para calculo da media lida pelo sensor analogico de chuva
int limite_IR_ceu = 2; // valor limite superior de leitura do sensor IR a partir do qual sera considerada um ceu nublado
int limite_chuva = 890; // valor do limite infeior lido no sensor de chuva a partir do qual será considerado que ha precipitação
float limite_u = 85.00; // valor minimo de umidade a ser usado no calculo combinado de condições para SAFE/UNSAFE
int leitura_sensor_chuva; // valor analogico da leitura do sensor de chuva
float limite_p = 5.00; // valor minimo de diferença entre a pressão nominal e a pressão lida para ser considerada uma baixa pressão, e portanto indicativo de chuvas
float h = 1000; //altitude em m
const size_t MIN_FREE_SPIFFS = 20 * 1024;
const unsigned long WIFI_RECONNECT_INTERVAL = 10000;  // 10 s
unsigned long wifiLastCheck = 0;
unsigned long lastRead = 0;
const unsigned long readInterval = 5000;

//Variaveis
int condicaoSeguranca = 0; // 0 = SAFE; 100 = UNSAFE
float u; // valor da umidade lido pelo DHT
float t; // valor da temperatura lido pelo DHT
float to; // temperatura do ponto de orvalho
float p; // valor da pressão lido pelo BMP280
float tIRamb; // valor da temperatura lido pelo MLX90614 do ambiente
float tIRceu; // // valor da temperatura lido pelo MLX90614 do céu
float delta_p;// diferença entre a pressão atmosferica nominal e a atual
float p_nominal; //pressão nominal calculada para a altitude definida em h
float p0 = 1013.25; //hPa ao nivel do mar
float G = 9.81; // aceleração da gravidade m/s^2
float M = 0.0289644; // massa molar do ar
float R = 8.31432; // constante universal dos gases
const char* stationName      = "Estacao Meteorologica Observatorio Eureka";
const float stationAltitude  = h; // metros
static int8_t lastLoggedMinute = -1;
static int8_t lastLoggedHour   = -1;
unsigned long agora;
const long TZ_OFFSET = -3L * 3600L;  // UTC-3 em segundos

//Conexão com a rede WiFi
const char* ssid = "SSID";  // Rede WiFi
const char* password = "PSWD";  //Senha da Rede WiFi

//Watchdog
Ticker watchdog;
bool watchdogReset = false;
void alimentarWatchdog() {
  watchdogReset = true;
}
void verificarWatchdog() {
  if (!watchdogReset) {
    ESP.restart();
  }
  watchdogReset = false;
}

AsyncWebServer server(80);

// protótipos
void handleLogging();
void logData();
void readSensors();
void printValores();
void calculoSeguranca();

void setup() 
{
  pinMode(PIN_SENSOR_CHUVA, INPUT);
  Serial.begin(115200); // Inicializa a comunicação serial para o ESP8266
  delay(100);
  Serial.printf("===Boot reset reason: %s\n", ESP.getResetReason().c_str());
  Serial.printf("Free heap at boot: %u\n\n", ESP.getFreeHeap());
  Wire.begin(); //Inicializa o barramento I2C
  watchdog.attach(30, alimentarWatchdog);
   // —– conecta Wi-Fi —–
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(250);
    Serial.print(".");
  }
  Serial.println("\nWi-Fi conectado, IP = " + WiFi.localIP().toString());
  // —– sincroniza relógio (para não ficar em 1970!) —–
  configTime(0, 0, "pool.ntp.org", "time.nist.gov");
  // espera até termos um timestamp válido
  while (time(nullptr) < 24*3600) {
    delay(200);
  }
   mlx.begin();  // Inicializa o sensor IR
  //Inicialização do sensor de pressão BMP280
  bmp.begin(0x76);
  //Definições padrão do datasheet
  bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,     // Modo de operação
                  Adafruit_BMP280::SAMPLING_X2,     // Sobreamostragem de temperatura
                  Adafruit_BMP280::SAMPLING_X16,    // Sobreamostragem de pressão Pressure oversampling
                  Adafruit_BMP280::FILTER_X16,      // Filtro
                  Adafruit_BMP280::STANDBY_MS_500); // Tempo de espera
  
  dht.setup(SENSOR_DHT, DHTesp::DHT11);//inciailização do DHT com ESP
  // 1) monta o SPIFFS
  if (!SPIFFS.begin()) {
    Serial.println(F("❌ Erro ao montar SPIFFS!"));
  } else {
    Serial.println(F("✅ SPIFFS montado com sucesso, arquivos:"));
    // 2) abre a raiz
    Dir dir = SPIFFS.openDir("/");
    // 3) percorre e lista
    while (dir.next()) {
      Serial.print("  • ");
      Serial.print(dir.fileName());
      Serial.print("  (");
      Serial.print(dir.fileSize());
      Serial.println(" bytes)");
    }
    Serial.println();  // só pra dar um espaçamento
  }
  // —– registra rotas do web-service —–
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *req){
    handlePaginaGrafico(req);    // página principal
  });
  server.on("/history", HTTP_GET, handleHistory);
  //Inicializa o servidor
  server.begin(); 
  Serial.println("AsyncWebServer iniciado");
  delay(2000);
 }

void loop() { 
  alimentarWatchdog(); // SW Watchdog
  handleLogging(); // trata log de dados em SPIFFS
  // leitura dos sensores
  agora = millis();
  if (agora - lastRead >= readInterval) {
    lastRead = agora;
    readSensors();
    calculoSeguranca();
    //printValores(); // descomente se quiser ver na serial os valores para ajustes do codigo
    }    
  if (WiFi.status() != WL_CONNECTED && millis() - wifiLastCheck > WIFI_RECONNECT_INTERVAL) {
  wifiLastCheck = millis();
  WiFi.disconnect();
  WiFi.begin(ssid, password);
  }
}

void readSensors(){
  // leituras dos sensores
  //DHT11
  u = dht.getHumidity(); //Le o valor da umidade DHT
  t = dht.getTemperature(); //Le o valor da temperatura DHT
  float perc_h = u/100;
  //Leitura do sensor de pressao BMP280
  p = bmp.readPressure()/100; // hPa
  //Chama a leitura do sensor IR
  tIRceu = mlx.readObjectTempC();  
  //Calculo da temperatura do ponto de orvalho
  float a = 17.27;
  float b = 237.7;
  to = (b*(((a*t)/(b+t))+log(u/100)))/(a-((a*t)/(b+t)+log(u/100)));
  //Calculo da pressão atmosférica local
  p_nominal=p0 * exp(-M * G * h/(R * (t+272.15)));
  delta_p = p_nominal - p;
  //media leitura do sensor de chuva, pra evitar flutuações rapidas
  leitura_sensor_chuva = analogRead(PIN_SENSOR_CHUVA);
    for (int i=0; i< numsamples; i++) {
     amostra[i] = leitura_sensor_chuva;
     delay(10);
     }
    //quantidade de amostras
    for (int i=0; i< numsamples; i++) {
     media_sens_chuva += amostra[i];
    }
    media_sens_chuva /= numsamples;
}

 void calculoSeguranca(){
  // Lógica de decisão de segurança com tempo mínimo para condição de céu nublado
  if (media_sens_chuva <= limite_chuva) {
     condicaoSeguranca = 100;
     estado_chuva = "Chovendo";
     estado_nuvens = "Nublado";
    }
  else if (tIRceu > limite_IR_ceu && u > limite_u && delta_p >= limite_p) {
     condicaoSeguranca = 100;
     estado_chuva = "Risco alto de chuva";
     estado_nuvens = "Nublado";
    }     
  else if (tIRceu > limite_IR_ceu && u < limite_u && delta_p <= limite_p){
    condicaoSeguranca = 0;
    estado_chuva = "Sem chuva";
    estado_nuvens = "Nublado";
  }
  else {
     condicaoSeguranca = 0;
     estado_chuva = "Sem chuva";
     estado_nuvens = "Céu limpo";
    }
  if (condicaoSeguranca == 0) {
    Serial.println("SAFE");
   } 
  else {
    Serial.println("UNSAFE");
   }
 }

 void printValores(){
   //Plotagem de dados na serial para testes
   //temperatura Ambiente
  Serial.println();
  Serial.print("---------------------------------------------------------------------");
  Serial.println();
  Serial.print(F("Temperatura: "));
  Serial.print(t);
  Serial.println(" C");
  //Umidade
  Serial.print(F("Umidade: "));
  Serial.print(u);
  Serial.println(" %");  
  //Temperatura do ponto de orvalho
  Serial.print(F("T. de orvalho: "));
  Serial.print(to);
  Serial.println(" C");   
  //Pressão atm. nominal
  Serial.print(F("Pressão atm. nominal no observatorio = "));
  Serial.print(p_nominal);
  Serial.println(" hPa"); 
  //Pressão medida
  Serial.print(F("Pressão atmosférica medida: "));
  Serial.print(p);
  Serial.println(" hPa"); 
  //Calculo do delta P (P nominal - P atmosferica medida)
  Serial.print(F("Delta P: "));
  Serial.print(delta_p);
  Serial.println(" hPa"); 
  // Temperatura de fundo do céu
  Serial.print("Temp. fundo do céu: ");
  Serial.print(tIRceu);
  Serial.println(" C");
  //Presença de nuvens
  Serial.print(F("Condição do céu: "));
  Serial.println(estado_nuvens);
  //Leitura do sensor de chuva
  Serial.print("Sensor de Chuva: ");
  Serial.print(media_sens_chuva);
  Serial.println("  ");
 }

 void handleHistory(AsyncWebServerRequest *request) {
  // 1) determina qual arquivo de log abrir (o de hoje)
  time_t now = time(nullptr);
  time_t local = now + TZ_OFFSET;
  struct tm ti;
  localtime_r(&local, &ti);
  char dateBuf[16];
  snprintf(dateBuf, sizeof(dateBuf), "%04d.%02d.%02d",
           ti.tm_year + 1900, ti.tm_mon + 1, ti.tm_mday);
  String fname = String("/") + dateBuf + "_Log_met.txt";

  // 2) abre o stream de resposta
  AsyncResponseStream *res = request->beginResponseStream("application/json");
  res->print('[');

  File f = SPIFFS.open(fname, "r");
  if (!f || !f.available()) {
    // Se não existir ou estiver vazio, retorna vazio
    res->print(']');
    request->send(res);
    return;
  }

  bool first = true;
  // 3) lê linha a linha, cada linha tem "timestamp,t,u,p,..." 
  while (f.available()) {
    String line = f.readStringUntil('\n');
    line.trim();
    if (line.length() == 0) continue;

    // separa pelos campos
    // ex: "2025-05-01T12:00:00,23.4,55.1,1013.2,..." 
    int idx = 0;
    String parts[7];
    while (idx < 6) {
      int c = line.indexOf(',', 0);
      if (c < 0) break;
      parts[idx++] = line.substring(0, c);
      line = line.substring(c + 1);
    }
    parts[idx++] = line;

    if (!first) res->print(',');
    first = false;

    // 4) constrói o objeto JSON
    res->print('{');
    res->printf("\"ts\":\"%s\",", parts[0].c_str());
    res->printf("\"t\":%s,", parts[1].c_str());
    res->printf("\"u\":%s,",    parts[2].c_str());
    res->printf("\"p\":%s,",    parts[3].c_str());
    res->printf("\"tIR\":%s,",  parts[4].c_str());
    res->printf("\"rain\":%s,", parts[5].c_str());
    res->printf("\"safe\":%s",  parts[6].c_str());
    res->print('}');

    // 5) alimenta o watchdog
    yield();
  }
  f.close();

  res->print(']');
  request->send(res);

  //Serial.printf("Free heap after /history: %u\n", ESP.getFreeHeap());
}

void handlePaginaGrafico(AsyncWebServerRequest *request) {
  // --- monta data ---
  time_t now = time(nullptr);
  time_t local = now + TZ_OFFSET;
  struct tm ti;
  gmtime_r(&local, &ti);
  char dateBuf[16];
  snprintf(dateBuf, sizeof(dateBuf), "%04d.%02d.%02d",
           ti.tm_year + 1900, ti.tm_mon + 1, ti.tm_mday);

  // --- calcula tendência como antes ---
  float T_k      = t + 273.15;
  float p_nom    = 1013.25 * expf(-0.0289644 * 9.81 * stationAltitude / (8.31423 * T_k));
  float delta_p  = p_nom - p;
  String tendencia;
  if      (delta_p < 0)   tendencia = "Alta";
  else if (delta_p < 5)  tendencia = "Normal";
  else                    tendencia = "Baixa";

  // --- cria o stream de resposta ---
  AsyncResponseStream *res = request->beginResponseStream("text/html");

  // --- escreve cabeçalho e estilos (tudo em PROGMEM com F()) ---
  res->print(F(
    "<!DOCTYPE html><html><head>"
      "<meta charset='utf-8'>"
      "<meta http-equiv='refresh' content='60'>"
      "<title>"));
  res->print(stationName);
  res->print(F(
      "</title><style>"
        "body{font-family:Arial,sans-serif;text-align:center;} "
      "</style></head><body>"));

  // --- escreve dados principais ---
  res->print(F("<h1>")); res->print(stationName); res->print(F("</h1>"));
  res->printf("<h2>Altitude: %.1f m</h2>", stationAltitude);
  res->printf("<h2>Data: %s</h2>", dateBuf);

  // --- caixa com leituras ---
  res->print(F("<div class='container'><div class='box'>"));
  res->printf("<p><strong>Temperatura ambiente:</strong> %.1f °C</p>", t);
  res->printf("<p><strong>Umidade:</strong> %.1f %%</p>", u);
  res->printf("<p><strong>Temperatura de orvalho:</strong> %.1f °C</p>", to);
  res->printf("<p><strong>Pressão Atmosférica:</strong> %.1f hPa</p>", p);
  res->printf("<p><strong>Tendência pressão:</strong> %s</p>", tendencia.c_str());
  res->printf("<p><strong>Temp. fundo do céu:</strong> %.1f °C</p>", tIRceu);
  res->printf("<p><strong>Condição do céu:</strong> %s</p>", estado_nuvens);
  res->printf("<p><strong>Chuva:</strong> %s</p>", estado_chuva);
  res->printf("<p><strong>Segurança:</strong> %s</p>", condicaoSeguranca==0?"SAFE":"UNSAFE");
  res->print(F("</div></div>"));

  // --- envia e mede o heap final ---
  request->send(res);
  //Serial.printf("Free heap after stream: %u\n", ESP.getFreeHeap());
}

// executa a cada iteração, decide se grava agora
void handleLogging() {
  time_t epoch = time(nullptr);
  time_t localEpoch = epoch + TZ_OFFSET;
  struct tm ti;
  localtime_r(&localEpoch, &ti);
  int currentMinute = ti.tm_min;
  int currentHour   = ti.tm_hour;

  // modo de teste: a cada minuto diferente
  if (LOG_INTERVAL_MINUTES == 1) {
    if (currentMinute != lastLoggedMinute) {
      lastLoggedMinute = currentMinute;
      logData();
    }
  }
  // modo produção: uma vez por hora no minuto zero
  else {
    if (currentMinute == 0 && currentHour != lastLoggedHour) {
      lastLoggedHour = currentHour;
      logData();
    }
  }
}

// monta nome do arquivo e grava uma linha com timestamp + leituras
void logData() {
  // 0) limpa se estiver sem espaço suficiente
  cleanupOldLogs(MIN_FREE_SPIFFS); 

  // 1) Obter tempo via NTP (já rodou configTime() no setup)
  time_t t = time(nullptr);
  struct tm ti;
  localtime_r(&t, &ti);

  // 2) Formatar data para nome de arquivo "AAAA.MM.DD"
  char dateBuf[16];
  snprintf(dateBuf, sizeof(dateBuf),
           "%04d.%02d.%02d",
           ti.tm_year + 1900,
           ti.tm_mon  + 1,
           ti.tm_mday);

  // 3) Formatar hora para registro "HH:MM:SS"
  char timeBuf[16];
  snprintf(timeBuf, sizeof(timeBuf),
           "%02d:%02d:%02d",
           ti.tm_hour,
           ti.tm_min,
           ti.tm_sec);

  // 4) Abrir (ou criar) o log do dia em modo append
  String fname = String("/") + dateBuf + "_Log_met.txt";
  File f = SPIFFS.open(fname.c_str(), "a");
  if (!f) {
    Serial.println("❌ Falha ao abrir log: " + fname);
    return;
  }

  // 5) Montar a linha de log
  //    Ajuste as variáveis t,u,to,p,tIRceu, estado_nuvens, estado_chuva, condicaoSeguranca
  char buf[128];
  snprintf(buf, sizeof(buf),
           "%s, Tamb:%.1fC, Umid:%.1f%%, Tdew:%.1fC, P:%.1fhPa, TIR:%.1fC, Ceu:%s, Chuva:%s, %s\n",
           timeBuf,
           t,
           u,
           to,
           p,
           tIRceu,
           estado_nuvens.c_str(),
           estado_chuva.c_str(),
           (condicaoSeguranca == 0) ? "SAFE" : "UNSAFE"
  );

  // 6) Gravar e fechar
  f.print(buf);
  f.close();

  // 7) (Opcional) debug no Serial
  //Serial.print("✅ Log gravado em ");
  //Serial.print(fname);
  //Serial.print(" -> ");
  //Serial.print(buf);
}
// ----------------------------------------------------------------------------
// Apaga arquivos de log mais antigos até que reste pelo menos `minFree` bytes
// ----------------------------------------------------------------------------
void cleanupOldLogs(size_t minFree) {
  // 1) obtém estatísticas do SPIFFS
  FSInfo fs_info;
  if (!SPIFFS.info(fs_info)) {
    Serial.println("! SPIFFS.info() falhou");
    return;
  }
  size_t freeBytes = fs_info.totalBytes - fs_info.usedBytes;

  // 2) enquanto não tiver espaço suficiente, apaga o log mais antigo
  while (freeBytes < minFree) {
    Dir dir = SPIFFS.openDir("/");
    String oldestName;
    // encontra o nome do arquivo mais antigo (lexico)
    while (dir.next()) {
      String fn = dir.fileName();
      if (fn.endsWith("_Log_met.txt")) {
        if (oldestName.length() == 0 || fn < oldestName) {
          oldestName = fn;
        }
      }
    }
    // se não achar nenhum, sai
    if (oldestName.length() == 0) {
      Serial.println("! Nenhum log para apagar, mas ainda sem espaço!");
      break;
    }
    // remove o arquivo
    Serial.printf("Removendo log antigo: %s\n", oldestName.c_str());
    SPIFFS.remove(oldestName);
    // atualiza freeBytes para checar de novo
    if (!SPIFFS.info(fs_info)) {
      Serial.println("! SPIFFS.info() falhou após remoção");
      break;
    }
    freeBytes = fs_info.totalBytes - fs_info.usedBytes;
  }
}
