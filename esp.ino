#include <WiFi.h>
#include <time.h>
#include <WebServer.h>
#include "esp_camera.h"
#include "esp_timer.h"
#include "img_converters.h"
#include "Arduino.h"
#include "soc/soc.h"           // Disable brownour problems
#include "soc/rtc_cntl_reg.h"  // Disable brownour problems
#include "driver/rtc_io.h"
#include <SPIFFS.h>
#include <FS.h>


#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27
#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22


String timez; 
String plantsER;
String rows; 
String travel_distance;

const char* ssid = "Sparklight";
const char* password = "6017867082";

WebServer server(80);

void setup() {
  Serial.begin(115200);
  Serial1.begin(9600, SERIAL_8N1, 16, 13);


  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Trying to connect....");
  }

  
  configTime(-5 * 3600, 0, "pool.ntp.org", "time.nist.gov");
  
  Serial.print("Fetching time from NTP...");
  struct tm timeinfo;
  int retry = 0;
  const int maxRetry = 20;

  while (!getLocalTime(&timeinfo) && retry < maxRetry) {
    Serial.print(".");
    delay(500);
    retry++;
  }

  if (retry == maxRetry) {
    Serial.println("\nFailed to obtain time. Check your internet connection or UDP port 123.");
  } else {
    Serial.println("\nSuccess!");
    Serial.println(&timeinfo, "%A, %B %d %Y %H:%M");
  }
   
    Serial.print("IP Address: http://");
  Serial.println(WiFi.localIP());

  // Turn-off the 'brownout detector'
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);

  // OV2640 camera module
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
  config.pin_sccb_sda = SIOD_GPIO_NUM;
  config.pin_sccb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;

  if (psramFound()) {
    config.frame_size = FRAMESIZE_UXGA;
    config.jpeg_quality = 10;
    config.fb_count = 2;
  } else {
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }
  // Cam init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    ESP.restart();
  }


  Serial.println(&timeinfo, "Time set: %Y-%m-%d %H:%M");
  server.on("/data-bus", HTTP_ANY, get_Data);
  server.begin();
  Serial.println(WiFi.localIP());

  server.on("/capture", giveData);
  server.begin();
  Serial.println("Web Server Started!");
}

void loop() {
server.handleClient();

}

void get_Data(){
  server.sendHeader( "Access-Control-Allow-Headers", "Content-Type");  
  server.sendHeader("Access-Control-Allow-Origin", "*"); 
  server.sendHeader("Access-Control-Allow-Methods", "OPTIONS,POST,GET");

  server.send(200, "text/plain", "OK"); 
  Serial.println("Data received and 'OK' sent back to browser.");
  travel_distance = server.arg("distance");
  timez = server.arg("time");
  rows = server.arg("columns");
  plantsER = server.arg("rows");
  
  delay(1000);
  Serial.print("Distance:");
  Serial.print(travel_distance);
  Serial.println(";");
  Serial.print("Rows:");
  Serial.print(rows);
  Serial.println(";");
  Serial.print("Plants:");
  Serial.print(plantsER);
  Serial.println(";");

  Serial1.print("Distance:");
  Serial1.print(travel_distance);
  Serial1.println(";");
  Serial1.print("Rows:");
  Serial1.print(rows);
  Serial1.println(";");
  Serial1.print("Plants:");
  Serial1.print(plantsER);
  Serial1.println(";");
  }


  void giveData() {

 camera_fb_t * fb = esp_camera_fb_get();
  if (!fb) {
    server.send(500, "text/plain", "Camera capture failed");
    return;
  }

  WiFiClient client = server.client();
  
  // Set HTTP headers so the browser sees it as an image
  server.sendHeader("Content-Disposition", "inline; filename=capture.jpg");
  server.sendHeader("Access-Control-Allow-Origin", "*"); 
  server.send_P(200, "image/jpeg", (const char *)fb->buf, fb->len);
  esp_camera_fb_return(fb);
}


