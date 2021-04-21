/**
  A simple stream handler to play web radio stations using ESP8266

  Copyright (C) 2018 Vince Gellár (github.com/vincegellar)
  Licensed under GNU GPL v3

  Wiring:
  -----------------------
  | VS1053  ||  ESP32   |
  -----------------------
  |   SCK   ||   IO18   |
  |   MISO  ||   IO19   |
  |   MOSI  ||   IO23   |
  |   XRST  ||   EN     |
  |   CS    ||   IO5    |
  |   DCS   ||   IO16   |
  |   DREQ  ||   IO4    |
  |   5V    ||   5V     |
  |   GND   ||   GND    |
  -----------------------

  Dependencies:
  -VS1053 library by baldram (https://github.com/baldram/ESP_VS1053_Library)
  -ESP8266Wifi/WiFi

  To run this example define the platformio.ini as below.

  [env:nodemcuv2]
  platform = espressif8266
  board = nodemcuv2
  framework = arduino
  build_flags = -D PIO_FRAMEWORK_ARDUINO_LWIP2_HIGHER_BANDWIDTH
  lib_deps =
    ESP_VS1053_Library

  [env:esp32dev]
  platform = espressif32
  board = esp32dev
  framework = arduino
  lib_deps =
    ESP_VS1053_Library

  Instructions:
  -Build the hardware
    (please find an additional description and Fritzing's schematic here:
     https://github.com/vincegellar/Simple-Radio-Node#wiring)
  -Set the station in this file
  -Upload the program

  IDE Settings (Tools):
  -IwIP Variant: v1.4 Higher Bandwidth
  -CPU Frequency: 160Hz
*/

#include <VS1053.h>


#include <WiFi.h>
#include <HTTPClient.h>
#define VS1053_CS     5
#define VS1053_DCS    16
#define VS1053_DREQ   4


// Default volume
float volume = 80;
int potPin = 2;
int highPin = 25;
int lowPin = 22;
VS1053 player(VS1053_CS, VS1053_DCS, VS1053_DREQ);
WiFiClient client;

// WiFi settings example, substitute your own
const char *ssid = "YOUR SSDI";
const char *password = "YOUR PASSWORD";

//  https://listen.radioking.com/radio/372772/stream/422929
const char *host = "https://listen.radioking.com/radio/372772/stream/422929";
const char *path = "/422929";
int httpPort = 8563;

// The buffer size 64 seems to be optimal. At 32 and 128 the sound might be brassy.
uint8_t mp3buff[64];

void setup() {
  pinMode(potPin, INPUT);
  pinMode(highPin, OUTPUT);
  digitalWrite(highPin, HIGH);
  
  pinMode(lowPin, OUTPUT);
  digitalWrite(lowPin, LOW);
  Serial.begin(115200);

  // Wait for VS1053 and PAM8403 to power up
  // otherwise the system might not start up correctly
  delay(3000);

  // This can be set in the IDE no need for ext library
  // system_update_cpu_freq(160);

  Serial.println("\n\nSimple Radio Node WiFi Radio");

  //Begin SPI connection with VS1003
  SPI.begin();
  //Start decoder
  player.begin();
  //Not necessary (needed in case it's set on midi player by default)
  player.switchToMp3Mode();
  //Default volume
  player.setVolume(volume);
  //While chip isn't connected, say it
  while (!player.isChipConnected()) {
    Serial.println("VS1003 not connected (or poorly)");
    delay(100);
  }

  //Connect Wifi with specified SSID
  Serial.print("Connecting to SSID ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  Serial.print("connecting to ");
  Serial.println(host);

  // Ask for data
  Serial.print("Requesting stream: ");
  Serial.println(path);

  client.print(String("GET ") + path + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "Connection: close\r\n\r\n");
}

void loop() {

  HTTPClient http;
  http.begin("https://listen.radioking.com/radio/372772/stream/422929"); //Specify the URL https://listen.radioking.com/radio/372772/stream/422929
  int httpCode = http.GET();                                 //Make the request
  Serial.printf("[HTTP] GET... code: %d\n", httpCode); //Expected code 200
  uint8_t buff[32] = { 0 };  //Create the bufferto get infos. Size 32 works very well
  while (httpCode > 0) { //While there's data
    float val = analogRead(potPin);   //Read the potentiometer value
    volume = constrain(map(val, 0, 1023, 0, 100),0,100);  //And constrain it to get a volume
    player.setVolume(volume); //Set the volume
    
    WiFiClient * stream = http.getStreamPtr(); //Create the stream pointer to stream music

    size_t size = stream->available();
    int c = stream->readBytes(buff, ((size > sizeof(buff)) ? sizeof(buff) : size));

    player.playChunk(buff, c);//Play the buffer on VS1003 via SPI
  }
  //When no more data, end httpclient
  http.end();
}
