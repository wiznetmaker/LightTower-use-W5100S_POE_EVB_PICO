 /* Fill-in information from Blynk Device Info here */
#define BLYNK_TEMPLATE_ID "**********"
#define BLYNK_TEMPLATE_NAME "Quickstart Template"
#define BLYNK_AUTH_TOKEN "*************"

/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial

#include <SPI.h>
#include <Ethernet.h>
#include <BlynkSimpleEthernet.h>
#include <EEPROM.h>
#include <WS2812FX.h>

#define LED_COUNT 36
#define LED_PIN 28
WS2812FX ws2812fx = WS2812FX(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

// start reading from the first byte (address 0) of the EEPROM
byte value;

#include "TouchyTouch.h"

const int touch_threshold_adjust = 300;
const int touch_pins[] = {27};
const int touch_count = sizeof(touch_pins) / sizeof(int);
TouchyTouch touches[touch_count];
uint32_t time_now = 0;

BlynkTimer timer;

int light_mode = 0;

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};
IPAddress ip(10, 0, 1, 226);

int r;
int g;
int b;  

BLYNK_WRITE(V5) {   
  // Called when virtual pin V2 is updated from the Blynk.App
  // V2 is a datastream of data type String assigned to a   
  // Blynk.App ZeRGBa widget.
  r = param[0].asInt();
  g = param[1].asInt();
  b = param[2].asInt();
  Serial.print("V2: r = ");
  Serial.print(r);
  Serial.print("\t g=");
  Serial.print(g);
  Serial.print("\t b=");
  Serial.println(b);
  ws2812fx.setColor(((uint32_t)r << 16 | (uint32_t)g << 8 | (uint32_t)b));
  light_mode = 1;
} // BLYNK_WRITE(V5)

void setup() {  

  for (int i = 0; i < touch_count; i++) {
    touches[i].begin( touch_pins[i] );
    touches[i].threshold += touch_threshold_adjust; // make a bit more noise-proof
  }  

  ws2812fx.init();
  ws2812fx.setBrightness(64);
  ws2812fx.setSpeed(1000);
  ws2812fx.setColor(0x007BFF);
  ws2812fx.setMode(FX_MODE_STATIC);
  ws2812fx.start();  
  
  // Initialize components
  Serial.begin(115200);
//   while (!Serial) {
//     ; // wait for serial port to connect. Needed for native USB port only
//   }

  Ethernet.init(17);  // WIZnet W5100S-EVB-Pico
  // start the Ethernet connection and the server:
  Ethernet.begin(mac, ip);
  
  if (Ethernet.linkStatus() == LinkOFF) {
    Serial.println("Ethernet cable is not connected.");
  }

  Blynk.begin(BLYNK_AUTH_TOKEN);
}

void loop() {
  
  ws2812fx.service();  
  
  Blynk.run();
  timer.run();
 
  // Check for touch button input
  // Add your touch button logic here
  Touch_handling();

  if(light_mode)
  {
    ws2812fx.setColor(((uint32_t)r << 16 | (uint32_t)g << 8 | (uint32_t)b));
  }
//  delay(20);            //don't ned to run this at full speed.
}

void Touch_handling()
{
  // key handling
  for ( int i = 0; i < touch_count; i++) {
    touches[i].update();
    if ( touches[i].rose() ) {
      Serial.print("Button:");
      Serial.println(i);
      ws2812fx.setMode((ws2812fx.getMode() + 1) % ws2812fx.getModeCount());
      time_now = millis();
    }
    if ( touches[i].fell() ) {
     Serial.printf("Release:");
     Serial.println(i);
     if((millis()-time_now)>500)
     {
      if(light_mode)
      {
      light_mode = 0;
      }else{
        light_mode = 1;
      }
      Serial.print("Time_enough");
     }
    }
  }
}
