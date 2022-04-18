#include <WiFi.h>
#include <WiFiUdp.h>
#include <ArtnetWifi.h>
#include <FastLED.h>
//TFT LCD libraries
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <Wire.h>
#include <SPI.h>

#define TFT_CS         5
#define TFT_RST        22 
#define TFT_DC         21
// define not needed for all pins; reference for ESP32 physical pins connections to VSPI:
// SDA  GPIO23 aka VSPI MOSI
// SCLK GPIO18 aka SCK aka VSPI SCK
// D/C  GPIO21 aka A0 (also I2C SDA)
// RST  GPIO22 aka RESET (also I2C SCL)
// CS   GPIO5  aka chip select
// LED  3.3V
// VCC  5V
// GND - GND
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

const char* ssid = "yourssid";
const char* password = "yourpassword";

// LED Strip
const int numLeds = 96; 
const int numberOfChannels = numLeds * 3; // Total number of DMX channels to receive (1 led = 3 channels)
#define DATA_PIN_A 17 //TX2 pin
#define DATA_PIN_B 16 //RX2 pin
CRGB leds[numLeds];
CRGB ledsB[numLeds];

// Artnet settings
ArtnetWifi artnet;
const int startUniverse = 0;

int previousDataLength = 0;
uint16_t packetsReceived = 0;
bool packetsColorFlip;


//Forward function declarations
boolean ConnectWifi(void);
void onDmxFrame(uint16_t universe, uint16_t length, uint8_t sequence, uint8_t* data);

void setup()
{
  tft.initR(INITR_MINI160x80_DKOFFSET); // special init for ebay tft
  tft.setRotation(3);
  tft.invertDisplay(true);
  
  tft.setTextWrap(false);
  tft.fillScreen(ST77XX_BLACK);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(1);

  FastLED.addLeds<WS2812, DATA_PIN_A, RGB>(leds, numLeds);
  FastLED.addLeds<WS2812, DATA_PIN_B, RGB>(ledsB, numLeds);
 
  //Power bottleneck on final board:
  //1000 = 350mA, 1500 = 500mA, 2000 = 600mA actual tested via USB (w/ 180leds), 
  //2300+ too much for USB, external power only
  FastLED.setMaxPowerInMilliWatts(2000); // 2000 - smoother visible lcd power performance

  // Warm white OK on USB at 450mA (@2500)
  fill_solid( leds, numLeds, CHSV(25, 200, 85)); // HSV value of warm white DK fairy lights
  fill_solid( ledsB, numLeds, CHSV(25, 200, 85));
  FastLED.show();

  ConnectWifi();
  
  artnet.begin();
  artnet.setArtDmxCallback(onDmxFrame);
}

void loop()
{
  // we call the read function inside the loop
  artnet.read();
}

boolean ConnectWifi(void)
{
  boolean state = true;
  int i = 0;

  tft.print(F("Connecting to "));
  tft.setTextColor(ST77XX_CYAN);
  tft.println(ssid);
  tft.println();
  WiFi.begin(ssid, password);

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    //Serial.print(".");
    if (i > 20){
      state = false;
      break;
    }
    i++;
  }

  if (state){
    tft.setTextSize(2);
    tft.setTextColor(ST77XX_GREEN);
    tft.println(WiFi.localIP());
    tft.setTextSize(1);
    tft.setTextColor(ST77XX_BLUE);
    tft.println();
    tft.println(F("Streaming DMX over ArtNet"));
  } else {
    tft.setTextColor(ST77XX_RED);
    tft.println(F("Connection failed"));
  }

  return state;
}

void onDmxFrame(uint16_t universe, uint16_t length, uint8_t sequence, uint8_t* data)
{
  packetsReceived++;
  if(packetsReceived % 160 == 0) packetsColorFlip = !packetsColorFlip;
  tft.drawLine(packetsReceived % 160, 52, packetsReceived % 160, 54, packetsColorFlip ? ST77XX_BLACK : ST77XX_ORANGE);

  // set brightness of the whole strip 
  if (universe == 15)
  {
    FastLED.setBrightness(data[0]);
  }
  // read universe and put into the right part of the display buffer
  for (int i = 0; i < length / 3; i++)
  {
    int led = i + (universe - startUniverse) * (previousDataLength / 3);
    if (led < numLeds)
    {
      leds[led] = CRGB(data[i * 3], data[i * 3 + 1], data[i * 3 + 2]);
    } else {
      ledsB[led-numLeds] = CRGB(data[i * 3], data[i * 3 + 1], data[i * 3 + 2]); 
    }
  }
  previousDataLength = length;     
  FastLED.show();
}