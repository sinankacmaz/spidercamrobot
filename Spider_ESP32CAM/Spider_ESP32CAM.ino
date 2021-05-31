#include "esp_camera.h"
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#define CAMERA_MODEL_AI_THINKER 
#include "camera_pins.h"
#define PHOTO 14     
#define LED 4


const char* ssid = "TD-W8901G_1";       
const char* password = "64486Df0";       
char auth[] = "NbzRiEa859RtFKRVq7dA1QHxhZ3VHwME";//Auth Blynk

String local_IP;
int count = 0;
void startCameraServer();

void takePhoto()
{
  digitalWrite(LED, HIGH); 
  delay(200);
  uint32_t randomNum = random(50000);
  Serial.println("http://"+local_IP+"/capture?_cb="+ (String)randomNum);
  Blynk.setProperty(V1, "urls", "http://"+local_IP+"/capture?_cb="+(String)randomNum);
  digitalWrite(LED, LOW); 
  delay(1000);
}

int Value_V2=0;                                           //Global değişkenler.
int Value_V3=0;                                           //Global değişkenler.
int Value_V6=0;                                           //Global değişkenler.
int Value_V7=0;                                           //Global değişkenler.

BLYNK_WRITE(V2){                                          //V2 okunup belirlenen değişkene atanılıyor.
  Value_V2 = param.asInt();
  if(Value_V2){
  Serial.println('Z');//Forward     
  }
  }              
BLYNK_WRITE(V3){                                          //V3 okunup belirlenen değişkene atanılıyor.
Value_V3 = param.asInt();
  if(Value_V3){
  Serial.println('K');//Back  
  }
}
BLYNK_WRITE(V6){                                         //V6 okunup belirlenen değişkene atanılıyor.
  Value_V6 = param.asInt();
    if(Value_V6){
  Serial.println('L');//Left  
  }
  }              
BLYNK_WRITE(V7){                                         //V7 okunup belirlenen değişkene atanılıyor.    
  Value_V7 = param.asInt();
    if(Value_V7){
  Serial.println('R');//Right  
  }
  }                        

   
int X_Data;
int Y_Data;

void Arduino_Data_Alma(){
if (Serial.available() > 0) {
 int m = Serial.read();
 int x = 16;
 int a = m % x;
 m = m - a;
 int b = m / x;
 
 a = a * 15;
 b = b * 15;

 X_Data = a;
 Y_Data = b;

 m = 0;
 a = 0;
 b = 0;
}  
}

void setup() {
  Serial.begin(9600);
  pinMode(LED,OUTPUT);
  pinMode(PHOTO,OUTPUT);
  digitalWrite(PHOTO,LOW);
  Serial.setDebugOutput(true);
  Serial.println();
  
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
  
  // if PSRAM IC present, init with UXGA resolution and higher JPEG quality
  //                      for larger pre-allocated frame buffer.
  if(psramFound()){
    config.frame_size = FRAMESIZE_UXGA;
    config.jpeg_quality = 10;
    config.fb_count = 2;
  } else {
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }

  // camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }

  sensor_t * s = esp_camera_sensor_get();
  // initial sensors are flipped vertically and colors are a bit saturated
  if (s->id.PID == OV3660_PID) {
    s->set_vflip(s, 1); // flip it back
    s->set_brightness(s, 1); // up the brightness just a bit
    s->set_saturation(s, -2); // lower the saturation
  }
  // drop down frame size for higher initial frame rate
  s->set_framesize(s, FRAMESIZE_QVGA);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    //Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  startCameraServer();

  Serial.print("Camera Ready! Use 'http://");
  Serial.print(WiFi.localIP());
  local_IP = WiFi.localIP().toString();
  Serial.println("' to connect");
  Blynk.begin(auth, ssid, password);
  
  Serial.println('S');//Stop  
}

void loop() {
  Arduino_Data_Alma();
  Blynk.run();
   
  if(digitalRead(PHOTO) == HIGH){
    takePhoto();
    }
    
    Blynk.virtualWrite(V4,X_Data);                        //X_Data verisi bulut ortamına taşınıyor.
    Blynk.virtualWrite(V5,Y_Data);                        //Y_Data verisi bulut ortamına taşınıyor.
}
