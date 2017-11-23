#include <Adafruit_Sensor.h>
#include <ArduinoJson.h>
#include <DHT.h>
#include <DHT_U.h>
#include <ESP8266WiFi.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>

// needed to avoid link error on ram check
extern "C"
{
#include "user_interface.h"
}
ADC_MODE(ADC_VCC);

WiFiServer server(80);
WiFiClient client;
const char* ssid = "Vikas_PC_Network";
const char* password = "9815610902";

float pfDew, pfHum, pfTemp, pfVcc;

//#define LED  D4  //Define connection of LED

// HC-SR04 GPIO mapping
#define echoPin D7 // Echo Pin i.e the pingPin 
#define trigPin D6 // Trigger Pin i.e. the inPin

long duration, distance; // Duration used to calculate distance

// OLED display TWI address
#define OLED_ADDR   0x3C

// reset pin not used on 4-pin OLED module
Adafruit_SSD1306 display(-1);  // -1 = no reset pin
// 128 x 64 pixel display
#if (SSD1306_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

#define DHTPIN D5
#define DHTTYPE DHT22

DHT_Unified dht(DHTPIN, DHTTYPE);

uint32_t delayMS;

#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2

#define LOGO16_GLCD_HEIGHT 16
#define LOGO16_GLCD_WIDTH  16

static const unsigned char PROGMEM logo16_glcd_bmp[] =
{ B00000000, B11000000,
  B00000001, B11000000,
  B00000001, B11000000,
  B00000011, B11100000,
  B11110011, B11100000,
  B11111110, B11111000,
  B01111110, B11111111,
  B00110011, B10011111,
  B00011111, B11111100,
  B00001101, B01110000,
  B00011011, B10100000,
  B00111111, B11100000,
  B00111111, B11110000,
  B01111100, B11110000,
  B01110000, B01110000,
  B00000000, B00110000
};

float t ;
float h ;

/*
  This function returns the distance to the object
  in front of the HC-SR04 sensor with Temperature
  compensation depending of FIXEDSPEED setting
*/
unsigned long getDistance() {

  /* The following trigPin/echoPin cycle is used to determine the
    distance of the nearest object by bouncing soundwaves off of it. */

  long duration;

  // The sensor is triggered by a HIGH pulse of 15 or more microseconds.
  // Give a short LOW pulse beforehand to ensure a clean HIGH pulse:
  pinMode(trigPin, OUTPUT);
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(15);
  digitalWrite(trigPin, LOW);

  // Read the signal from the sensor: a HIGH pulse whose
  // duration is the time (in microseconds) from the sending
  // of the ping to the reception of its echo off of an object.
  pinMode(echoPin, INPUT);
  duration = pulseIn(echoPin, HIGH);

  //Calculate the distance (in cm) based on the speed of sound.
  return distance = duration / 58.2;
}

bool readRequest(WiFiClient& client) {
  bool currentLineIsBlank = true;
  while (client.connected()) {
    if (client.available()) {
      char c = client.read();
      if (c == '\n' && currentLineIsBlank) {
        return true;
      } else if (c == '\n') {
        currentLineIsBlank = true;
      } else if (c != '\r') {
        currentLineIsBlank = false;
      }
    }
  }
  return false;
}

JsonObject& prepareResponse(JsonBuffer& jsonBuffer) {
  JsonObject& root = jsonBuffer.createObject();
  JsonArray& tempValues = root.createNestedArray("temperature");
  tempValues.add(t);
  JsonArray& humiValues = root.createNestedArray("humidity");
  humiValues.add(h);
  JsonArray& dewpValues = root.createNestedArray("dewpoint");
  dewpValues.add(pfDew);
  JsonArray& EsPvValues = root.createNestedArray("Systemv");
  EsPvValues.add(pfVcc / 1000, 3);
  return root;
}

void writeResponse(WiFiClient& client, JsonObject& json) {
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: application/json");
  client.println("Access-Control-Allow-Origin: *");
  client.println("Connection: close");
  client.println();

  json.prettyPrintTo(client);
}


void setup() {
  Serial.begin(115200);
  //  pinMode(LED, OUTPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  // inital connect
  //  WiFi.mode(WIFI_STA);
  //  delay(1000);
  // config static IP
  IPAddress ip(192, 168, 1, 89);
  IPAddress gateway(192, 168, 1, 1);
  IPAddress subnet(255, 255, 255, 0);
  IPAddress dns(192, 168, 1, 1);
  WiFi.config(ip, dns, gateway, subnet);
  // Connect to WiFi network
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
  }
  server.begin();

  dht.begin();
  sensor_t sensor;
  dht.temperature().getSensor(&sensor);
  dht.humidity().getSensor(&sensor);
  // Set delay between sensor readings based on sensor details.
  delayMS = sensor.min_delay / 800;
  pinMode (DHTPIN, OUTPUT);
  /// initialize and clear display
  display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR);
  display.clearDisplay();
  display.drawRect(0, 0, display.width() - 1, display.height() - 1, WHITE); // draws the outer rectangular boundary on the screen
  display.display();

  // display a pixel in each corner of the screen
  display.drawPixel(0, 0, WHITE);
  display.drawPixel(127, 0, WHITE);
  display.drawPixel(0, 63, WHITE);
  display.drawPixel(127, 63, WHITE);

  // display a line of text
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(20, 16);
  display.print("Welcome");
  display.setCursor(25, 40);
  display.print("San");

  // update display with all of the above graphics
  display.display();
  display.drawBitmap(75, 40,  logo16_glcd_bmp, 16, 16, 1);
  display.display();
  delay(1);

  // invert the display
  display.invertDisplay(true);
  delay(500);
  display.invertDisplay(false);
  delay(1000);
  display.display();
  display.clearDisplay();
}

void loop() {
  distance = getDistance();
  WiFiClient client = server.available();
  if (client) {
    bool success = readRequest(client);
    if (success) {
      delay(1000);

      //      delay(500);
      pfVcc = ESP.getVcc();
      StaticJsonBuffer<500> jsonBuffer;
      JsonObject& json = prepareResponse(jsonBuffer);
      writeResponse(client, json);
    }
    delay(1);
    client.stop();
  }

  distance = getDistance();
  //  if (distance <= 20) {
  //    digitalWrite(LED, HIGH);
  //  }
  //  else {
  //    digitalWrite(LED, LOW);
  //  }
  Serial.println(distance);
  // Delay between measurements.
  delay(delayMS);
  //  digitalWrite(LED, HIGH);
  // Get temperature event and print its value.
  sensors_event_t event;
  dht.temperature().getEvent(&event);
  if (isnan(event.temperature)) {
    // draw a bitmap icon and 'animate' movement
    testdrawbitmap(logo16_glcd_bmp, LOGO16_GLCD_HEIGHT, LOGO16_GLCD_WIDTH);
    Serial.println("Error reading temperature!");
  }
  else {
    Serial.print("Temperature: ");
    t = event.temperature;
    Serial.print(t);
    Serial.println(" *C");
  }
  // Get humidity event and print its value.
  dht.humidity().getEvent(&event);
  if (isnan(event.relative_humidity)) {
    Serial.println("Error reading humidity!");
  }
  else {
    Serial.print("Humidity: ");
    h = event.relative_humidity;
    Serial.print(h);
    Serial.println("%");
  }

  float a = 17.67;
  float b = 243.5;
  float alpha = (a * t) / (b + t) + log(h / 100);
  pfDew = (b * alpha) / (a - alpha);

  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 1);
  display.print("Temperature/Humidity");
  // display Temperature
  display.setTextSize(4);
  display.setTextColor(WHITE);
  display.setCursor(0, 16);
  //  display.print("T:");
  display.print(t);
  display.setTextSize(1);
  display.print((char)247);
  //  display.setTextSize(2);
  //  display.print("C");
  display.setCursor(0, 50);
  display.print("Humidity : ");
  display.print(h);
  //  display.setTextSize(2);
  display.print(" %");
  //  display.setTextSize(2);
  //  display.setCursor(0, 48);
  //  display.print("Dist:");
  //  display.print(distance);
  //  //  display.setTextSize(1);
  //  display.print("cm");
  // update display with all of the above graphics
  display.display();

}

void testdrawbitmap(const uint8_t *bitmap, uint8_t w, uint8_t h) {
  uint8_t icons[NUMFLAKES][3];

  // initialize
  for (uint8_t f = 0; f < NUMFLAKES; f++) {
    icons[f][XPOS] = random(display.width());
    icons[f][YPOS] = 0;
    icons[f][DELTAY] = random(5) + 1;

    Serial.print("x: ");
    Serial.print(icons[f][XPOS], DEC);
    Serial.print(" y: ");
    Serial.print(icons[f][YPOS], DEC);
    Serial.print(" dy: ");
    Serial.println(icons[f][DELTAY], DEC);
  }

  //  while (1) {
  if (isnan(h) || isnan(t)) {
    // draw each icon
    for (uint8_t f = 0; f < NUMFLAKES; f++) {
      display.drawBitmap(icons[f][XPOS], icons[f][YPOS], bitmap, w, h, WHITE);
    }
    display.display();

    // then erase it + move it
    for (uint8_t f = 0; f < NUMFLAKES; f++) {
      display.drawBitmap(icons[f][XPOS], icons[f][YPOS], bitmap, w, h, BLACK);
      // move it
      icons[f][YPOS] += icons[f][DELTAY];
      // if its gone, reinit
      if (icons[f][YPOS] > display.height()) {
        icons[f][XPOS] = random(display.width());
        icons[f][YPOS] = 0;
        icons[f][DELTAY] = random(5) + 1;
      }
    }
  }
}
