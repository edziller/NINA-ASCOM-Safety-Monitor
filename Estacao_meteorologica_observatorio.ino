/****************************************************************************** 
Sistema de Monitor de Segurança para observatório remoto
por Eduardo Ziller
******************************************************************************/
#include <Wire.h> 
#include <Adafruit_BMP280.h>
#include <SparkFunMLX90614.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DHTesp.h> 

IRTherm therm; // Cria um objeto para o sensor de temperatura
Adafruit_BMP280 bmp; // I2C - Cria um objeto para o Sensor de Pressão

#define SENSOR_DHT 0 //Define a ligação ao pino de dados do sensor de umidade
#define PIN_teto 2 //Define a saida para comando do relé pra fechar o teto em caso de condições atmosfericas adversas
#define PIN_SENSOR_CHUVA A0 //Definindo a entrada do sensor de chuva

DHTesp dht; //Define o tipo de sensor DHT utilizado
float u; // valor da umidade lido pelo DHT
float t; // valor da temperatura lido pelo DHT
float to; // temperatura do ponto de orvalho
float p; // valor da pressão lido pelo BMP280
float tIRamb; // valor da temperatura lido pelo MLX90614 do ambiente
float tIRceu; // // valor da temperatura lido pelo MLX90614 do céu
float tavg; //  valor medio da temperatura entre MLX90614 e o DHT11
String estado_nuvens; // descreve se está nublado ou ceu claro
String estado_chuva; // descreve se esta chovendo ou não
String estado_teto; // descreve se o estado do teto é abrindo, fechando, aberto ou fechado
byte nuvens;
byte chuva;


//Conexão com a rede WiFi
const char* ssid = "ssid";  // Rede WiFi
const char* password = "psswd";  //Senha da Rede WiFi

ESP8266WebServer server(80); //server na porta 80

void setup() 
{
  pinMode(PIN_teto, OUTPUT);
  pinMode(PIN_SENSOR_CHUVA, INPUT);
  Serial.begin(115200); // Inicializa a comunicação serial para o ESP8266
  Wire.begin(); //Inicializa o barramento I2C
  //Inicialização do sensor IR MLX90614
  therm.begin();
  if (therm.begin() == false){ // inicia o sensor IR
    Serial.println("Sensor de temperatura de fundo do céu não detectado! Parando! Verifique as conexões");
    while(1);
  }
  Serial.println();
  therm.setUnit(TEMP_C); //definindo as unidades em Celsius
  //Inicialização do sensor de pressão BMP280
  bmp.begin(0x76);
  //Definições padrão do datasheet
  bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,     // Modo de operação
                  Adafruit_BMP280::SAMPLING_X2,     // Sobreamostragem de temperatura
                  Adafruit_BMP280::SAMPLING_X16,    // Sobreamostragem de pressão Pressure oversampling
                  Adafruit_BMP280::FILTER_X16,      // Filtro
                  Adafruit_BMP280::STANDBY_MS_500); // Tempo de espera
  
  dht.setup(SENSOR_DHT, DHTesp::DHT11);//inciailização do DHT com ESP
  Serial.println();
  Serial.println("Conectando a Rede: "); //Imprime na serial a mensagem
  Serial.println(ssid); //Imprime na serial o nome da Rede Wi-Fi
  WiFi.begin(ssid, password); //Inicialização da comunicação Wi-Fi
  //Verificação da conexão
  while (WiFi.status() != WL_CONNECTED) { //Enquanto estiver aguardando status da conexão
    delay(500);
    Serial.print("."); //Imprime pontos
  }
  Serial.println("");
  Serial.println("WiFi Conectado");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP()); //Função para exibir o IP da ESP
  server.on("/", handle_OnConnect); //Servidor recebe uma solicitação HTTP - chama a função handle_OnConnect
  server.onNotFound(handle_NotFound); //Servidor recebe uma solicitação HTTP não especificada - chama a função handle_NotFound
  server.begin(); //Inicializa o servidor
  Serial.println("Servidor HTTP inicializado");
  delay(500);
 }

void loop() 
{
  server.handleClient(); //Chama o método handleClient()
  float a = 17.27;
  float b = 237.7;
  u = dht.getHumidity(); //Le o valor da umidade ESP
  t = dht.getTemperature(); //Le o valor da temperatura ESP
  float perc_h = u/100;
  p = bmp.readPressure()/100; // Valor da pressão em hPa
  //Chama a leitura do sensor IR
  if (therm.read()) //caso sucesso, retorna 0, caso contrario, 1
    {
      tIRamb = therm.ambient();
      tIRceu = therm.object();
      Serial.println();
      Serial.print("---------------------------------------------------------------------");
      Serial.println();
      Serial.print("Temp. fundo do céu: " + String(tIRceu, 2));
      Serial.println(" C");
      //Serial.print("Temp. ambiente: " + String(tIRamb, 2));
      //Serial.println(" C");
    }
    else {
      Serial.print("---------------------------------------------------------------------");
      Serial.println("Falha na leitura da tempertatura do sensor infravermelho");
    }
    //Rotina para determiação se temos ou nao nuvens no ceu 
    if(tIRceu > 0 ){
      estado_nuvens = "Nublado";
      Serial.print("Condição do Ceu: ");
      Serial.println(estado_nuvens);
     
    }
    else{
      estado_nuvens = "Ceu limpo";
      Serial.print("Condição do Ceu: ");
      Serial.println(estado_nuvens);
   
    }
    tavg = (tIRamb+t)/2;
    to = (b*(((a*tavg)/(b+tavg))+log(u/100)))/(a-((a*tavg)/(b+tavg)+log(u/100)));
    //leitura do sensor de pressão
    Serial.print(F("Pressão atmosférica = "));
    Serial.print(p);
    Serial.println(" hPa");
    //temperatura DHT
    Serial.print(F("Temperatura media = "));
    Serial.print(tavg);
    Serial.println(" C");
    //umidade DHT
    Serial.print(F("Umidade = "));
    Serial.print(u);
    Serial.println(" %");
    //temperatura de orvalho
    Serial.print(F("T. de orvalho = "));
    Serial.print(to);
    Serial.println(" C");
    Serial.println();
    //Leitura do sensor de chuva
   Serial.print("Sensor de Chuva: ");
   Serial.print(analogRead(PIN_SENSOR_CHUVA)); 
   Serial.println("  ");
   if (analogRead(PIN_SENSOR_CHUVA) > 890)
   {
    estado_chuva = "Nao esta chovendo";
    Serial.println(estado_chuva);
    digitalWrite(PIN_teto, HIGH);
    }
   else
   {
    estado_chuva = "Chovendo";
    Serial.println(estado_chuva);
    digitalWrite(PIN_teto, LOW);
   }  
   delay(2000);
}

void handle_OnConnect() {
  Serial.print("Temperatura: ");
  Serial.print(tavg); //Imprime no monitor serial o valor da temperatura media entre o DHT e o IR
  Serial.println(" ºC");
  Serial.print("Umidade: ");
  Serial.print(u); //Imprime no monitor serial o valor da umidade lida
  Serial.println(" %");
  Serial.print("Temperatura do ponto de orvalho: ");
  Serial.print(to); //Imprime no monitor serial o valor da temperatura de orvalho calculada
  Serial.println(" ºC");
  Serial.print("Pressão: ");
  Serial.print(p); //Imprime no monitor serial o valor da pressão lida
  Serial.println(" hPa");
  Serial.print("Condição do Ceu: ");
  Serial.print(estado_nuvens); //Imprime no monitor serial o estado do ceu
  Serial.print("Temperatura do Ceu: ");
  Serial.print(tIRceu); //Imprime no monitor serial o estado do ceu
  Serial.print("Condição de chuva: ");
  Serial.print(estado_chuva); //Imprime no monitor serial o estado do ceu  
  server.send(200, "text/html", EnvioHTML(tavg, u, to, p, estado_nuvens, tIRceu, estado_chuva)); //Envia as informações usando o código 200, especifica o conteúdo como "text/html" e chama a função EnvioHTML
}

void handle_NotFound() { //Função para lidar com o erro 404
  server.send(404, "text/plain", "Não encontrado"); //Envia o código 404, especifica o conteúdo como "text/pain" e envia a mensagem "Não encontrado"
}

String EnvioHTML(float Tstat, float Ustat, float Tostat, float Pstat, String Nstat, float irceustat, String Cstat) { //Exibindo a página da web em HTML
  String ptr = "<!DOCTYPE html> <html>\n"; //Indica o envio do código HTML
  ptr += "<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n"; //Torna a página da Web responsiva em qualquer navegador Web
  ptr += "<meta http-equiv='refresh' content='2'>";//Atualizar browser a cada 2 segundos
  ptr += "<link href=\"https://fonts.googleapis.com/css?family=Open+Sans:300,400,600\" rel=\"stylesheet\">\n";
  ptr += "<title>Monitor de Seguranca</title>\n"; //Define o título da página

  //Configurações de fonte do título e do corpo do texto da página web
  ptr += "<style>html { font-family: 'Open Sans', sans-serif; display: block; margin: 0px auto; text-align: center;color: #000000;}\n";
  ptr += "body{margin-top: 50px;}\n";
  ptr += "h1 {margin: 50px auto 30px;}\n";
  ptr += "h2 {margin: 40px auto 20px;}\n";
  ptr += "p {font-size: 24px;color: #000000;margin-bottom: 10px;}\n";
  ptr += "</style>\n";
  ptr += "</head>\n";
  ptr += "<body>\n";
  ptr += "<div id=\"webpage\">\n";
  ptr += "<h1>Monitor de Seguranca Meteorologico do Observatorio</h1>\n";
  ptr += "<h2>NODEMCU ESP8266 Web Server</h2>\n";

  //Exibe as informações de temperatura e umidade na página web
  ptr += "<p><b>Temperatura: </b>";
  ptr += (float)Tstat;
  ptr += " graus Celsius</p>";
  ptr += "<p><b>Umidade: </b>";
  ptr += (float)Ustat;
  ptr += " %</p>";
  ptr += "<p><b>Temperatura do ponto de orvalho: </b>";
  ptr += (float)Tostat;
  ptr += " graus Celsius</p>";
  ptr += "<p><b>Pressao atmosferica: </b>";
  ptr += (float)Pstat;
  ptr += " hPa</p>";
  ptr += "<p><b>Condicao do ceu: </b>";
  ptr += (String)Nstat;
  ptr += "</p>";
  ptr += "<p><b>Temperatura do ceu: </b>";
  ptr += (String)irceustat;
  ptr += "</p>";
  ptr += "<p><b>Condicao de precipitacao: </b>";
  ptr += (String)Cstat;
  ptr += "</p>";
  
  ptr += "</div>\n";
  ptr += "</body>\n";
  ptr += "</html>\n";
  return ptr;

}
