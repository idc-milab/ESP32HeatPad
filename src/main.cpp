#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>

#include <Arduino.h>
#include <DHT.h>
#include <Adafruit_Sensor.h>

const char* ssid = "milab_idc";
const char* password = "milabspirit";

WebServer server(80);
DHT dht(26, DHT22);

const int mosfetPin = 25; // GPIO connected to the SIG pin of the MOS module

float readDHTTemperature();
float readDHTHumidity();

void handleRoot() {
  char msg[1500];
  snprintf(msg, 1500,
           "<html>\
<head>\
  <meta http-equiv='refresh' content='4'/>\
  <meta name='viewport' content='width=device-width, initial-scale=1'>\
  <link rel='stylesheet' href='https://use.fontawesome.com/releases/v5.7.2/css/all.css' integrity='sha384-fnmOCqbTlWIlj8LyTjo7mOUStjsKC4pOpQbqyi7RrhN7udi9RwhKkMHpvLbHG9Sr' crossorigin='anonymous'>\
  <title>ESP32 DHT Server</title>\
  <style>\
  html { font-family: Arial; display: inline-block; margin: 0px auto; text-align: center;}\
  h2 { font-size: 3.0rem; }\
  p { font-size: 3.0rem; }\
  .units { font-size: 1.2rem; }\
  .dht-labels{ font-size: 1.5rem; vertical-align:middle; padding-bottom: 15px;}\
  </style>\
</head>\
<body>\
  <h2>ESP32 DHT Server!</h2>\
  <p>\
    <i class='fas fa-thermometer-half' style='color:#ca3517;'></i>\
    <span class='dht-labels'>Temperature</span>\
    <span>%.2f</span>\
    <sup class='units'>&deg;C</sup>\
  </p>\
  <p>\
    <i class='fas fa-tint' style='color:#00add6;'></i>\
    <span class='dht-labels'>Humidity</span>\
    <span>%.2f</span>\
    <sup class='units'>&percnt;</sup>\
  </p>\
  <p>\
    <a href=\"/heatpad/on\">Turn Heat Pad On</a>\
  </p>\
  <p>\
    <a href=\"/heatpad/off\">Turn Heat Pad Off</a>\
  </p>\
</body>\
</html>",
           readDHTTemperature(), readDHTHumidity()
          );
  server.send(200, "text/html", msg);
}

void handleHeatPadOn() {
  digitalWrite(mosfetPin, HIGH); // Turn on the heat pad
  server.send(200, "text/plain", "Heat pad turned on");
}

void handleHeatPadOff() {
  digitalWrite(mosfetPin, LOW); // Turn off the heat pad
  server.send(200, "text/plain", "Heat pad turned off");
}

void setup(void) {
  Serial.begin(115200);
  pinMode(2, OUTPUT); // Initialize the built-in LED pin as an output
  digitalWrite(2, HIGH); // Turn on the built-in LED

  pinMode(mosfetPin, OUTPUT); // Initialize the MOSFET control pin as an output
  digitalWrite(mosfetPin, LOW); // Ensure the heat pad is off initially

  dht.begin();
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp32")) {
    Serial.println("MDNS responder started");
  }
  
  server.on("/", handleRoot);
  server.on("/heatpad/on", handleHeatPadOn);
  server.on("/heatpad/off", handleHeatPadOff);
  
  server.begin();
  Serial.println("HTTP server started");
}

void loop(void) {
  server.handleClient();
  delay(2); //allow the cpu to switch to other tasks
}

float readDHTTemperature() {
  float t = dht.readTemperature();
  if (isnan(t)) {    
    Serial.println("Failed to read from DHT sensor!");
    return -1;
  } else {
    return t;
  }
}

float readDHTHumidity() {
  float h = dht.readHumidity();
  if (isnan(h)) {
    Serial.println("Failed to read from DHT sensor!");
    return -1;
  } else {
    return h;
  }
}
