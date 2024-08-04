#include "FS.h"
#include "SD.h"
#include "SPI.h"
#include "esp_camera.h"
#include <WiFi.h>
#include <ESPAsyncWebServer.h>

// Definindo os pinos para o cartão SD
#define SD_CS_PIN 5

// Definindo os pinos da câmera
#define PWDN_GPIO_NUM     -1
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      4
#define SIOD_GPIO_NUM     18
#define SIOC_GPIO_NUM     23
#define Y9_GPIO_NUM       36
#define Y8_GPIO_NUM       37
#define Y7_GPIO_NUM       38
#define Y6_GPIO_NUM       39
#define Y5_GPIO_NUM       35
#define Y4_GPIO_NUM       14
#define Y3_GPIO_NUM       13
#define Y2_GPIO_NUM       34
#define VSYNC_GPIO_NUM    5
#define HREF_GPIO_NUM     27
#define PCLK_GPIO_NUM     25

const char* ssid = "your_SSID";
const char* password = "your_PASSWORD";

AsyncWebServer server(80);

void setup() {
  Serial.begin(115200);

  // Conectando ao Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando ao WiFi...");
  }
  Serial.println("Conectado ao WiFi");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if(!SD.begin(SD_CS_PIN)){
    Serial.println("Erro ao iniciar o cartão SD");
    return;
  }
  Serial.println("Cartão SD iniciado");

  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;

  if(psramFound()){
    config.frame_size = FRAMESIZE_UXGA;
    config.jpeg_quality = 10;
    config.fb_count = 2;
  } else {
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }

  // Inicializando a câmera
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Erro ao iniciar a câmera: 0x%x", err);
    return;
  }

  // Tirar foto
  camera_fb_t * fb = esp_camera_fb_get();
  if (!fb) {
    Serial.println("Erro ao capturar a foto");
    return;
  }

  // Salvar a foto no cartão SD
  File file = SD.open("/foto.jpg", FILE_WRITE);
  if (!file) {
    Serial.println("Erro ao abrir o arquivo no cartão SD");
    return;
  }
  file.write(fb->buf, fb->len);
  file.close();

  Serial.println("Foto salva no cartão SD");

  // Liberar a memória da foto
  esp_camera_fb_return(fb);

  // Configurar rotas do servidor
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/plain", "Foto tirada e salva no cartão SD");
  });

  server.on("/foto", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SD, "/foto.jpg", "image/jpeg");
  });

  server.begin();
}

void loop() {
  // Deixe o loop vazio
}
