#include <WiFi.h>
#include <TFT_eSPI.h>  // Biblioteca para o display ST7789
#include <SPI.h>

// Configurações do display
TFT_eSPI tft = TFT_eSPI(); 

// Configurações Wi-Fi
const char* ssid = "ifce-alunos";        // Coloque o nome da sua rede Wi-Fi
const char* password = "ifce4lun0s";   // Coloque a senha da sua rede Wi-Fi
const char* serverIP = "10.16.1.40";  // IP ao qual o ESP32 se conectará
const int serverPort = 80;  // Porta do servidor

void setup() {
  // Inicializa o display
  tft.init();
  tft.setRotation(1);  // Altera a orientação do display, se necessário
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextSize(2);
  
  // Exibe a mensagem de inicialização
  tft.setCursor(0, 10);
  tft.println("Inicializando...");

  // Conectar ao Wi-Fi
  WiFi.begin(ssid, password);
  tft.println("Conectando ao WiFi...");

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    tft.print(".");
  }

  // Wi-Fi conectado
  tft.fillScreen(TFT_BLACK);
  tft.setCursor(0, 10);
  tft.println("WiFi Conectado!");
  tft.println(WiFi.localIP());

  // Tenta se conectar ao IP do servidor
  if (connectToServer(serverIP, serverPort)) {
    tft.fillScreen(TFT_GREEN);
    tft.setCursor(0, 30);
    tft.println("Conectado ao servidor!");
  } else {
    tft.fillScreen(TFT_RED);
    tft.setCursor(0, 30);
    tft.println("Falha ao conectar!");
  }
}

void loop() {
  // Pode ser usado para monitorar a conexão ou realizar novas tarefas
}

bool connectToServer(const char* ip, int port) {
  WiFiClient client;
  if (client.connect(ip, port)) {
    return true;
  } else {
    return false;
  }
}