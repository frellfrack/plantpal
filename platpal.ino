#include <U8g2lib.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include "PlantFacts.h"
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>

#include "EEPROM.h"

const int moistureSensorPin = 34; 
const int led = 13;
const int relayPin = 12;

const String hostname = "plantpal";

float temp =0;
float hum =0;
int moisture=0;

const int AirValue = 2650;   //you need to replace this value with Value_1
const int WaterValue = 1210;  //you need to replace this value with Value_2

const char* ssid = "europa";
const char* password = "VvahTUMdwv5RJDQW";

//const char* ssid = "SKYTQBQV";
//const char* password = "EmpyQQyAawtw";

const char DEGREE_SYMBOL[] = { 0xB0, '\0' };

unsigned long previousMillis=0;  
const unsigned long loopInterval = 1000000;

//U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(0x3C, /* clock=*/ SCL, /* data=*/ SDA, /* reset=*/ U8X8_PIN_NONE);   // All Boards without Reset of the Display

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

Adafruit_BME280 bme; 

WebServer server(80);


void handleReadings() {
  String message ="\n{\"hum\":";
  message += hum;
  message +=",\"temp\":";
  message += temp;
  message +=",\"moisture\":";
  message += moisture;
  message +="}\n";  
  server.send(200, "application/json", message);
}

void randomFact() {
  String message ="\n{\"randomfact\":\"";
  message +=getRandomPlantFact();
  message +="\"}\n";
  server.send(200, "application/json", message);
}

String returnHeader(String Title)
{
  String html = "<!DOCTYPE html>\n<html lang=\"enGB\">\n";
  html += "<head>\n";
  html += "<meta charset=\"UTF-8\">\n";
  html += "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  html += "<title> Plant Pal V0.2 - ";
  html += Title;
  html += "</title>\n";
  html += "<script src=\"https://code.jquery.com/jquery-3.7.0.min.js\" integrity=\"sha256-2Pmvv0kuTBOenSvLm6bvfBSSHrUJ+3A7x6P5Ebd07/g=\" crossorigin=\"anonymous\"></script>\n"; // Add jQuery library
  html += "<script src=\"https://cdn.jsdelivr.net/npm/chart.js\"></script>\n"; // Add Chart.js library  
  html += "<link rel=\"stylesheet\" href=\"/style.css\">\n";
  html += "</head>\n";
  html += "<body>\n";
  html += "<header><h1>Plant Pal V0.2</h1></header>\n";
  html += "<nav>\n";
  html += "<a href=\"/\">Dashboard</a>\n";
  html += "<a href=\"/settings/\">Settings</a>\n";
  html += "</nav>\n";
  html += "<p id=\"randomFact\">\n";
  html += getRandomPlantFact();
  html += "</p>\n";
  html += "<main>\n";
  return html;  
}

void handleRoot() {
  digitalWrite(led, 1);
  String html = returnHeader("Dashboard");
  html += "<main>\n";
  html += "<ul>\n";
  html += "<li id=\"temp\"></li>\n";
  html += "<li id=\"hum\"></li>\n";
  html += "<li id=\"moist\"></li>\n";
  html += "<ul>\n";
  html += "<canvas id=\"chart\"></canvas>\n"; // Canvas element for the chart
  html += "</main>\n";
  html += "<script src=\"/app.js\"></script>\n"; // include app logic
  html += "</body>\n";
  html += "</html>\n";

  server.send(200, "text/html", html);
  
  digitalWrite(led, 0);
}

void handleCSS() 
{
  digitalWrite(led, 1);
  
  String css = "body {\n";
  css += "  font-family: Arial, sans-serif;\n";
  css += "  margin: 0;\n";
  css += "  padding: 0;\n";
  css += "  display: flex;\n";
  css += "  flex-direction: column;\n";
  css += "  justify-content: center;\n";
  css += "  align-items: center;\n";
  css += "}\n";
  css += "header {\n";
  css += "  background-color: #333;\n";
  css += "  width: 100%;\n";
  css += "  text-align: center;\n";
  css += "}\n";
  css += "#chart {\n";
  css += "  width: 100%;\n";
  css += "  height:400px;\n";
  css += "  display:block ;\n";
  css += "}\n";
  css += "nav {\n";
  css += "  display: flex;\n";
  css += "  justify-content: center;\n";
  css += "  margin-top: 20px;\n";
  css += "}\n";
  css += "nav a {\n";
  css += "  display: inline-block;\n";
  css += "  padding: 10px 20px;\n";
  css += "  margin: 0 5px;\n";
  css += "  background-color: #333;\n";
  css += "  color: #FFF;\n";
  css += "  text-decoration: none;\n";
  css += "  border-radius: 5px;\n";
  css += "}\n";
  css += "nav a:hover {\n";
  css += "  background-color: #555;\n";
  css += "}\n";
  css += "h1 {\n";
  css += "  font-size: 24px;\n";
  css += "  color: #FFF;\n";
  css += "  text-transform: uppercase;\n";
  css += "  padding: 10px;\n";
  css += "}\n";
  css += "main {\n";
  css += "  padding: 10px;\n";
  css += "}\n";
  css += "ul {\n";
  css += "  list-style-type: none;\n";
  css += "  padding: 0;\n";
  css += "  margin: 0;\n";
  css += "  text-align:center;\n";
  css += "}\n";
  css += "li {\n";
  css += "  font-size: 18px;\n";
  css += "  margin-bottom: 10px;\n";
  css += "  text-align: center;\n";
  css += "}\n";
  css += "#temp, #hum {\n";
  css += "  display: inline-block;\n";
  css += "  margin-right: 10px;\n";
  css += "}\n";
  css += "#temp:before {\n";
  css += "  content: \"Temperature: \";\n";
  css += "}\n";
  css += "#hum:before {\n";
  css += "  content: \"Humidity: \";\n";
  css += "}\n";
  css += "#moist:before {\n";
  css += "  content: \"Soil Moisture: \";\n";
  css += "}\n";
  server.send(200, "text/css", css);
  digitalWrite(led, 0);
}

void handleSettings()
{
  if (server.method() == HTTP_POST) 
  {
    storeSettings(); // Call the function to store the settings
  }
  digitalWrite(led, 1);
  String html = returnHeader("Settings");
  html += "<form method=\"post\" action=\"/store-settings/\">\n";
  html += "<label>Water % Level: <input type=\"number\" name=\"waterlevel\" min=\"0\" max=\"100\"/></label>\n";
  html += "<input type=\"submit\" value=\"Update\"></form>\n"; 
  html += "<script src=\"/app.js\"></script>\n"; // include app logic
  html += "</body>\n";
  html += "</html>\n";

  server.send(200, "text/html", html);
  digitalWrite(led, 0);
}

void storeSettings()
{
 if (!server.hasArg("waterlevel")) {
    server.send(400, "text/plain", "Bad Request");
    return;
  }
  
  int waterLevel = server.arg("waterlevel").toInt();
  
  if (waterLevel < 0 || waterLevel > 100) {
    server.send(400, "text/plain", "Invalid water level");
    return;
  }

  EEPROM.begin(2); // Initialize EEPROM with 2 bytes of storage

  // Store the water level in EEPROM
  EEPROM.write(0, waterLevel);
  EEPROM.commit();

}

void handleJS() {
  digitalWrite(led, 1); // Turns on an LED (assuming it's connected to a pin named 'led')

  // Initializing a JavaScript string variable 'javaScript' with initial code
  String javaScript = "var chartData = [];\n";
  javaScript += "var myChart = null;\n";
  javaScript += "function timer() {\n";
  javaScript += "  $.getJSON(\"/readings.json\", function(data) {\n";
  javaScript += "    console.log(data);\n";
  javaScript += "    $(\"#temp\").html(data.temp+\"<sup>o</sup>\");\n";
  javaScript += "    $(\"#hum\").html(data.hum+\"%\");\n";
  javaScript += "    $(\"#moist\").html(data.moisture+\"%\");\n";

  // Code to retain only the last 8 readings
  javaScript += "    if (chartData.length >= 8) {\n";
  javaScript += "      chartData.shift();\n"; // Remove the oldest reading
  javaScript += "    }\n";

  javaScript += "    var currentTime = new Date().toLocaleTimeString();\n";
  javaScript += "    chartData.push({\n";
  javaScript += "      time: currentTime,\n";
  javaScript += "      temp: data.temp,\n";
  javaScript += "      hum: data.hum,\n";
  javaScript += "      moisture: data.moisture\n";
  javaScript += "    });\n";
  javaScript += "    updateChart();\n";

  javaScript += "  });\n";

  javaScript += "function updateChart() {\n";
  javaScript += "  var chartCanvas = document.getElementById('chart');\n";
  javaScript += "  if (!chartCanvas) {\n"; // Check if the chart canvas is present
  javaScript += "    return; // Exit the function if the chart is not present\n";
  javaScript += "  }\n";
  javaScript += "  if (myChart) {\n";
  javaScript += "    myChart.destroy();\n";
  javaScript += "  }\n";
  javaScript += "  var timeData = chartData.map(function(entry) {\n";
  javaScript += "    return entry.time;\n";
  javaScript += "  });\n";
  javaScript += "  var ctx = chartCanvas.getContext('2d');\n";
  javaScript += "  myChart = new Chart(ctx, {\n";
  javaScript += "    type: 'line',\n";
  javaScript += "    data: {\n";
  javaScript += "      labels: timeData,\n";
  javaScript += "      datasets: [\n";
  javaScript += "        {\n";
  javaScript += "          label: 'Temperature',\n";
  javaScript += "          data: chartData.map(function(entry) { return entry.temp; }),\n";
  javaScript += "          borderColor: 'rgba(255, 99, 132, 1)',\n";
  javaScript += "          backgroundColor: 'rgba(255, 99, 132, 0.2)',\n";
  javaScript += "          borderWidth: 1\n";
  javaScript += "        },\n";
  javaScript += "        {\n";
  javaScript += "          label: 'Humidity',\n";
  javaScript += "          data: chartData.map(function(entry) { return entry.hum; }),\n";
  javaScript += "          borderColor: 'rgba(54, 162, 235, 1)',\n";
  javaScript += "          backgroundColor: 'rgba(54, 162, 235, 0.2)',\n";
  javaScript += "          borderWidth: 1\n";
  javaScript += "        },\n";
  javaScript += "        {\n";
  javaScript += "          label: 'Moisture',\n";
  javaScript += "          data: chartData.map(function(entry) { return entry.moisture; }),\n";
  javaScript += "          borderColor: 'rgba(75, 192, 192, 1)',\n";
  javaScript += "          backgroundColor: 'rgba(75, 192, 192, 0.2)',\n";
  javaScript += "          borderWidth: 1\n";
  javaScript += "        }\n";
  javaScript += "      ]\n";
  javaScript += "    },\n";
  javaScript += "    options: {\n";
  javaScript += "      animation: {\n";
  javaScript += "        duration: 0\n";
  javaScript += "      },\n";
  javaScript += "      scales: {\n";
  javaScript += "        y: {\n";
  javaScript += "          beginAtZero: true\n";
  javaScript += "        }\n";
  javaScript += "      }\n";
  javaScript += "    }\n";
  javaScript += "  });\n";
  javaScript += "}\n";

  javaScript += "}\n";

  javaScript += "timer();\n";
  javaScript += "setInterval(timer, 10000);\n";
  
  javaScript += "function getRandomFact() {\n";
  javaScript += "  $.getJSON(\"/randomfact.json\", function(data) {\n";
  javaScript += "    console.log(data);\n";
  javaScript += "    $(\"#randomFact\").text(data.randomfact);\n";
  javaScript += "  });\n";
  javaScript += "}\n";
  javaScript += "setInterval(getRandomFact, 100000);\n";

  server.send(200, "text/javascript", javaScript); // Sends the JavaScript response to the client

  digitalWrite(led, 0); // Turns off the LED
}

void handleNotFound() {
  digitalWrite(led, 1);
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
  digitalWrite(led, 0);
}

// Function to return a random plant fact
const char* getRandomPlantFact() {
  int index = random(0, 100);
  return plantFacts[index];
}

void u8g2_prepare(void) {
  u8g2.enableUTF8Print();
  u8g2.setFont(u8g2_font_9x18B_tf);
  u8g2.setFontRefHeightExtendedText();
  u8g2.setDrawColor(1);
  u8g2.setFontPosTop();
  u8g2.setFontDirection(0);
}

void setup() {
  pinMode(led, OUTPUT);
  pinMode(relayPin, OUTPUT);
  
  digitalWrite(relayPin, 1);
  digitalWrite(led, 0);
    
  // put your setup code here, to run once:
  Serial.begin(115200);  
  
  Wire.begin();
  bme.begin(0x76);
  u8g2.begin();
  u8g2_prepare();
 
  WiFi.mode(WIFI_STA);
  WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE, INADDR_NONE);
  WiFi.setHostname(hostname.c_str()); //define hostname

  WiFi.begin(ssid, password);
  Serial.println("");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
 // Initialize mDNS
 
  if (MDNS.begin("plantpal")) {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);
  server.on("/readings.json", handleReadings);
  server.on("/randomfact.json", randomFact);
  server.on("/style.css",handleCSS);
  server.on("/settings/",handleSettings);
  server.on("/store-settings/", HTTP_POST, handleSettings);
  server.on("/app.js",handleJS);
  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("HTTP server started");
  
  String ipAddressStr = WiFi.localIP().toString();
  const char* ipAddress = ipAddressStr.c_str();

  u8g2.clearBuffer(); // clear the internal memory
  u8g2.drawStr(6, 9, ssid);
  u8g2.drawStr(6, 39, ipAddress);
  u8g2.drawLine(0,32, 128,32);
  u8g2.sendBuffer();
  delay(5000);
}


void printValues() 
{
    char str[80];
    char buff[7];
    char buff2[7];
    char buff3[7];

    temp = bme.readTemperature();
    hum = bme.readHumidity();

    int soilMoistureValue = analogRead(moistureSensorPin);
    moisture  = map(soilMoistureValue, AirValue, WaterValue, 0, 100);
    if(moisture > 100)
    {
      moisture =100;
    }
    if(moisture<0)
    {
      moisture =0;
    }
    
    sprintf(str, "%s,%s,%i", dtostrf(temp,4,2,buff),dtostrf(hum,4,2,buff2),moisture);
    Serial.println(str);
    u8g2.clearBuffer(); // clear the internal memory
    sprintf(str, "%s C", buff);    
    u8g2.drawStr(32,9, str);
    u8g2.drawUTF8(82,6, DEGREE_SYMBOL);


    u8g2.drawLine(0,32, 128,32);
    sprintf(str, "%s%% %i%% ", buff2, moisture);
    u8g2.drawStr(20,42,str);
    u8g2.sendBuffer();
}  

int readWaterLevelFromEEPROM()
{
  EEPROM.begin(2); // Initialize EEPROM with 2 bytes of storage
  
  // Read the water level from EEPROM
  int storedWaterLevel = EEPROM.read(0);

  return storedWaterLevel;
}

void loop() 
{  
  unsigned long currentTime = micros();
  unsigned long interval = currentTime - previousMillis;
  server.handleClient();
 
  if (interval >=  loopInterval) 
  {
  printValues();
  previousMillis=  currentTime;
  } 
}
