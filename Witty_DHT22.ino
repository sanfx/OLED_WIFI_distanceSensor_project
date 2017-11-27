#include <Adafruit_Sensor.h>
#include <ArduinoJson.h>
#include <DHT.h>
#include <DHT_U.h>
#include "Page_Index.h"
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>

const char* host = "wittycloud";

// needed to avoid link error on ram check
// include is a plain C, not C++
extern "C"
{
#include "user_interface.h"
}
ADC_MODE(ADC_VCC);

ESP8266WebServer server (80);

WiFiClient client;
const char* ssid = "Vikas_PC_Network";
const char* password = "9815610902";

float hic, pfDew, pfHum, pfTemp, pfVcc;

//#define LED  D4  //Define connection of LED

// HC-SR04 GPIO mapping
#define echoPin D7 // Echo Pin i.e the pingPin 
#define trigPin D6 // Trigger Pin i.e. the inPin

#define LDR_PIN      A0 // LDR 
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
DHT dhthi(DHTPIN, DHTTYPE);

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

void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += ( server.method() == HTTP_GET ) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";

  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName ( i ) + ": " + server.arg ( i ) + "\n";
  }
  server.send ( 404, "text/plain", message );
}


String createJsonResponse() {
  StaticJsonBuffer<500> jsonBuffer;

  JsonObject &root = jsonBuffer.createObject();
  JsonArray &tempValues = root.createNestedArray("temperature");
  tempValues.add(t);
  JsonArray &humiValues = root.createNestedArray("humidity");
  humiValues.add(h);
  JsonArray &dewpValues = root.createNestedArray("dewpoint");
  dewpValues.add(pfDew);
  JsonArray &heindValues = root.createNestedArray("heatindex");
  heindValues.add(hic);
  JsonArray &EsPvValues = root.createNestedArray("systemv");
  EsPvValues.add(pfVcc / 1000, 3);

  String json;
  root.prettyPrintTo(json);
  return json;
}

void outputJson() {
  server.send(200, "text/json", createJsonResponse());
}

void setup() {
  Serial.begin(115200);
  //  pinMode(LED, OUTPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(LDR_PIN, INPUT);

  // inital connect
  //  WiFi.mode(WIFI_STA);
  //  delay(1000);
  // config static IP
  IPAddress myIp(192, 168, 1, 89);
  IPAddress gateway(192, 168, 1, 1);
  IPAddress subnet(255, 255, 255, 0);
  IPAddress dns(192, 168, 1, 1);
  WiFi.config(myIp, dns, gateway, subnet);
  Serial.print("AP IP address: ");
  Serial.println(myIp);
  Serial.print("Mac Address:: ");
  Serial.println(WiFi.macAddress());
  // Connect to WiFi network
  WiFi.begin(ssid, password); // WiFi.begin() requires 2 strings as arguments.
  // You need to pass the SSID and the password of the Access point you wish to
  // connect.You need to pass the arguments as character arrays or strings with a lower case s.


  // The status function in the WiFi class, doesn’t take any arguments but it returns
  // stuff depending on the status of the network that you’re connected to.
  // Usually, first, you call WiFi.begin, you pass the SSID and the password because you’re
  // trying to establish a connection with the network. Then, what you do is you wait in a loop
  // until WiFi.status returns the value WL_CONNECTED.
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
  }

  if ( MDNS.begin ( host ) ) {
    Serial.println ( "MDNS responder started" );
  }
  MDNS.addService("http", "tcp", 80);
//  server.on ( "/", handleRoot );
  server.on ( "/", []() { Serial.println("opened root page"); server.send( 200, "text/html", PAGE_Index ); });
  server.onNotFound (handleNotFound);
  server.on("/json", outputJson);
  server.begin();
  Serial.println("HTTP server started");

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
  delay(400);
  display.invertDisplay(false);
  delay(800);
  display.display();
  display.clearDisplay();
}


//void handleRoot() {
//  const int nsize = 3000;
//  char temp[nsize];
//  snprintf ( temp, nsize,
//             "%s\n\
//  </\div></body>\n\
//</html>", index::html
//           );
//
//  server.send ( 200, "text/html", temp );
//}


void loop() {
  distance = getDistance();
  pfVcc = ESP.getVcc();
  distance = getDistance();

  Serial.println(distance);
  // Delay between measurements.
  delay(delayMS);
  //  digitalWrite(LED, HIGH);
  // Get temperature event and print its value.
  sensors_event_t event;
  dht.temperature().getEvent(&event);
  if (isnan(event.temperature)) {
    // draw a bitmap icon and 'animate' movement
    // testdrawbitmap(logo16_glcd_bmp, LOGO16_GLCD_HEIGHT, LOGO16_GLCD_WIDTH);
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
  // Compute heat index in Celsius (isFahreheit = false)
  hic = dhthi.computeHeatIndex(t, h, false);
  Serial.println(analogRead(LDR_PIN));
  server.handleClient();
//  client_status();
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

void client_status() {

  unsigned char number_client;
  struct station_info *stat_info;

  struct ip_addr *IPaddress;
  IPAddress address;
  int i = 1;

  number_client = wifi_softap_get_station_num(); // Count of stations which are connected to ESP8266 soft-AP
  stat_info = wifi_softap_get_station_info();

  Serial.print(" Total connected_client are = ");
  Serial.println(number_client);

  while (stat_info != NULL) {

    IPaddress = &stat_info->ip;
    address = IPaddress->addr;

    Serial.print("client= ");

    Serial.print(i);
    Serial.print(" ip adress is = ");
    Serial.print((address));
    Serial.print(" with mac adress is = ");

    Serial.print(stat_info->bssid[0], HEX);
    Serial.print(stat_info->bssid[1], HEX);
    Serial.print(stat_info->bssid[2], HEX);
    Serial.print(stat_info->bssid[3], HEX);
    Serial.print(stat_info->bssid[4], HEX);
    Serial.print(stat_info->bssid[5], HEX);

    stat_info = STAILQ_NEXT(stat_info, next);
    i++;
    Serial.println();

  }
  delay(500);
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
