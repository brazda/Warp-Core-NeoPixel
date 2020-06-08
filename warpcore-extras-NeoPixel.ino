#include <Adafruit_NeoPixel.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

ESP8266WebServer server;
uint8_t pin_led = 16;
char* ssid = "SSID";
char* password = "password";
#define PIN D4
// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0;        
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;

Adafruit_NeoPixel strip = Adafruit_NeoPixel(300, PIN, NEO_GRB + NEO_KHZ800);
//Setup Wifi
void setup() {
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  Serial.begin(115200);
  WiFi.begin(ssid,password);
  while(WiFi.status()!=WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
//Webserver Links
  server.on("/",[](){server.send(200,"text/plain","Hello");});
  server.on("/warp",setwarp);
  server.on("/rainbow",setrainbow);
  server.on("/chaserainbow",setchaserainbow);
  server.on("/cyclerainbow",setcyclerainbow);
  server.on("/reset",resetPixelColor);
  server.begin();  
}
void setwarp() {
     server.send(200,"text/plain","Warp");
     warpChase(strip.Color(  0,   0, 255), 50); // Blu 
}
void setcyclerainbow() {
     server.send(200,"text/plain","Cycle Rainbow");
     rainbowCycle(20);
}
void setchaserainbow() {
     server.send(200,"text/plain","Chase Rainbow");
     theaterChaseRainbow(20);
}
void setrainbow() {
     server.send(200,"text/plain","Rainbow");
     rainbow(20);
}
void colorWipe() {
     server.send(200,"text/plain","Color Wipe");
//     colorWipe;
}
void resetPixelColor(){
     server.send(200,"text/plain","Color Wipe");
     resetColor();
}
void loop() {
  server.handleClient();
     currentTime = millis();
     previousTime = currentTime;    
}
// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, c);
      strip.show();
      delay(wait);
  }
}
void rainbow(uint8_t wait) {
  uint16_t i, j;
  for(j=0; j<256; j++) {
    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i+j) & 255));
    }
    strip.show();
    delay(wait);
  }
}
// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256*20; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
   strip.show();
   delay(wait);
  }
}
//Theatre-style crawling lights.
void warpChase(uint32_t c, uint8_t wait) {
 String dist = server.arg("distance");
 String warp1 = server.arg("warp");
 String num = server.arg("num");
  for (int j=0; j<dist.toInt(); j++) {  //do 10 cycles of chasing
    for (int q=0; q < dist.toInt(); q++) {
      for (int i=0; i < strip.numPixels(); i=i+dist.toInt()) {
        strip.setPixelColor(i+q, c);    //turn every third pixel on
      }
      strip.show(); 
      delay(warp1.toInt());
          delay(warp1.toInt());      
      for (int i=0; i < strip.numPixels(); i=i+dist.toInt()) {
        strip.setPixelColor(i+q+num.toInt(), 0);        //turn every third pixel off
      }
    }
  }
}
//Theatre-style crawling lights with rainbow effect
 void theaterChaseRainbow(uint8_t wait) {
  for (int j=0; j < 256; j++) {     // cycle all 256 colors in the wheel
    for (int q=0; q < 3; q++) {
        for (int i=0; i < strip.numPixels(); i=i+3) {
          strip.setPixelColor(i+q, Wheel( (i+j) % 255));    //turn every third pixel on
        }
        strip.show();
        delay(wait); 
        for (int i=0; i < strip.numPixels(); i=i+3) {
          strip.setPixelColor(i+q, 0);        //turn every third pixel off
        }
    }
  }
}
void resetColor(){
  strip.clear();
  strip.show();
  server.send(200,"text/plain","All LEDs are off!");
}
// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
   return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else if(WheelPos < 170) {
    WheelPos -= 85;
   return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  } else {
   WheelPos -= 170;
   return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  }
}
