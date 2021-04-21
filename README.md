# ESP32-Webradio
Webradio reciever based on ESP32, using VS1003 as decoder

Uses dependency VS1053 library by baldram (https://github.com/baldram/ESP_VS1053_Library) [Copyright (C) 2018 Vince Gellár (github.com/vincegellar)  Licensed under GNU GPL v3]
https://github.com/baldram/ESP_VS1053_Library/blob/master/README.md
Based on the example of the library, but adapted to have perfect audio quality as output

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
  
  Inspired by https://electroniqueamateur.blogspot.com/2021/03/esp32-et-vs1053-ecouter-les-fichiers.html
  
