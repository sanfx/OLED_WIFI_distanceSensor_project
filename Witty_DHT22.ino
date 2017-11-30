#include <Adafruit_Sensor.h>
#include <ArduinoJson.h>
#include <DHT.h>
#include <DHT_U.h>
#include "Page_Index.h"
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <Wire.h>

const char* host = "nitin";

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

#define LDR_PIN      A0 // LDR 

#define DHTOUTPIN D7


// http://www.instructables.com/id/How-to-Use-a-Magnetic-Door-Switch-Sensor-With-Ardu/
int state; // 0 close - 1 open wwitch
#define DOORSENSE D6
uint32_t doorOpen;


#define DHTPIN 14
#define DHTTYPE DHT22

DHT_Unified dht(DHTPIN, DHTTYPE);
DHT dhthi(DHTPIN, DHTTYPE);

DHT_Unified dhtOut(DHTOUTPIN, DHTTYPE);

uint32_t delayMS;

float ot ;
float oh ;
float t ;
float h ;


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
  JsonArray &outtempValues = root.createNestedArray("Outtemperature");
  outtempValues.add(ot);
  JsonArray &outhumiValues = root.createNestedArray("Outhumidity");
  outhumiValues.add(oh);
  JsonArray &humiValues = root.createNestedArray("humidity");
  humiValues.add(h);
  JsonArray &dewpValues = root.createNestedArray("dewpoint");
  dewpValues.add(pfDew);
  JsonArray &heindValues = root.createNestedArray("heatindex");
  heindValues.add(hic);
  JsonArray &oorSenseValue = root.createNestedArray("doorOpen");
  oorSenseValue.add(doorOpen);
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
  pinMode (DHTPIN, OUTPUT);
  pinMode (DHTOUTPIN, OUTPUT);
  pinMode(LDR_PIN, INPUT);
  pinMode(DOORSENSE, INPUT_PULLUP);

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
  MDNS.addService("ssh", "tcp", 22);
  server.on ( "/", []() {
    Serial.println("opened root page");
    server.send( 200, "text/html", PAGE_Index );
  });
  server.onNotFound (handleNotFound);
  server.on("/json", outputJson);
  server.begin();
  Serial.println("HTTP server started");

  dht.begin();
  sensor_t sensor;
  dht.temperature().getSensor(&sensor);
  dht.humidity().getSensor(&sensor);
  sensor_t sensor2;
  dhtOut.temperature().getSensor(&sensor2);
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
  pfVcc = ESP.getVcc();

  // Delay between measurements.
  delay(delayMS);
  //  digitalWrite(LED, HIGH);
  // Get temperature event and print its value.
  sensors_event_t event;
  dht.temperature().getEvent(&event);
  if (isnan(event.temperature)) {
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

  sensors_event_t event2;
  dhtOut.temperature().getEvent(&event2);
  if (isnan(event2.temperature)) {
    Serial.println("Error reading Outside temperature!");
  }
  else {
    Serial.print("Outside Temperature: ");
    ot = event2.temperature;
    Serial.print(ot);
    Serial.println(" *C");
  }

  dhtOut.humidity().getEvent(&event2);
  if (isnan(event2.relative_humidity)) {
    Serial.println("Error reading outdoor humidity!");
  }
  else {
    Serial.print("Outdoor Humidity: ");
    oh = event2.relative_humidity;
    Serial.print(oh);
    Serial.println("%");
  }

  float a = 17.67;
  float b = 243.5;
  float alpha = (a * t) / (b + t) + log(h / 100);
  pfDew = (b * alpha) / (a - alpha);
  // Compute heat index in Celsius (isFahreheit = false)
  hic = dhthi.computeHeatIndex(t, h, false);
  Serial.println(analogRead(LDR_PIN));

  state = digitalRead(DOORSENSE);
  Serial.print("Door Status: ");
  Serial.println(state);
  if (state == HIGH) {
    doorOpen = 1 ;
  }
  else {
    doorOpen = 0 ;
  }


  server.handleClient();


}

