#include <U8g2lib.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
int moistureSensorPin = 34; // GPIO 34 for analog input


float temp =0;
float hum =0;
int moisture=0;

const int AirValue = 2800;   //you need to replace this value with Value_1
const int WaterValue = 1100;  //you need to replace this value with Value_2

const char* ssid = "europa";
const char* password = "VvahTUMdwv5RJDQW";
WebServer server(80);
const int led = 13;



void handleReadings() {
  String ptr ="\n{\"hum\":";
  ptr+= hum;
  ptr +=",\"temp\":";
  ptr+= temp;
  
  ptr +=",\"moisture\":";
  ptr+= moisture;
  ptr +="}\n";
  
  server.send(200, "application/json", ptr);

}
void handleRoot() {
  digitalWrite(led, 1);

  String ptr = "<!DOCTYPE html>\n<html lang=\"enGB\">\n";
  ptr += "<head>\n";
    ptr += "<meta charset=\"UTF-8\">\n";
  ptr += "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  ptr += "<title>Plant Pal V1</title>\n";

 
  ptr += "<script src=\"https://code.jquery.com/jquery-3.7.0.min.js\" integrity=\"sha256-2Pmvv0kuTBOenSvLm6bvfBSSHrUJ+3A7x6P5Ebd07/g=\" crossorigin=\"anonymous\"></script>\n"; // Add jQuery library
  ptr += "<script src=\"https://cdn.jsdelivr.net/npm/chart.js\"></script>\n"; // Add Chart.js library
  ptr += "</head>\n";
  ptr += "<body>\n";
  ptr += "<header><h1>Plant Pal V1</h1></header>\n";
 
  ptr += "<link rel=\"stylesheet\" href=\"/style.css\">\n";

  ptr += "<main>\n";
  ptr += "<ul>\n";
  
  ptr += "<li id=\"temp\"></li>\n";
  ptr += "<li id=\"hum\"></li>\n";
  ptr += "<li id=\"moist\"></li>\n";
  ptr += "<ul>\n";
  
  ptr += "<canvas id=\"chart\"></canvas>\n"; // Canvas element for the chart
  ptr += "</main>\n";
  ptr += "<script src=\"/app.js\"></script>\n"; // include app logic
  ptr += "</body>\n";
  ptr += "</html>\n";

  server.send(200, "text/html", ptr);
  digitalWrite(led, 0);
}



void handleCSS() {
  digitalWrite(led, 1);
  String ptr = "body {\n";
  ptr += "  font-family: Arial, sans-serif;\n";
  ptr += "  margin: 0;\n";
  ptr += "  padding: 0;\n";
  ptr += "  display: flex;\n";
  ptr += "  flex-direction: column;\n";
  ptr += "  justify-content: center;\n";
  ptr += "  align-items: center;\n";
  ptr += "}\n";

  
  ptr += "header {\n";
  ptr += "  background-color: #333;\n";
  ptr += "  width: 100%;\n";
  ptr += "  text-align: center;\n";
  ptr += "}\n";


  
  ptr += "#chart {\n";
  ptr += "  width: 100%;\n";
  ptr += "  height:400px;\n";
  ptr += "  display:block ;\n";
  ptr += "}\n";
  
  ptr += "h1 {\n";
  ptr += "  font-size: 24px;\n";
  ptr += "  color: #FFF;\n";
  ptr += "  text-transform: uppercase;\n";
   ptr += "  padding: 10px;\n";
  ptr += "}\n";
  ptr += "main {\n";
  ptr += "  padding: 10px;\n";
  ptr += "}\n";
  ptr += "ul {\n";
  ptr += "  list-style-type: none;\n";
  ptr += "  padding: 0;\n";
  ptr += "  margin: 0;\n";
  ptr += "  text-align:center;\n";
  ptr += "}\n";
  ptr += "li {\n";
  ptr += "  font-size: 18px;\n";
  ptr += "  margin-bottom: 10px;\n";
  ptr += "  text-align: center;\n";
  ptr += "}\n";
  ptr += "#temp, #hum {\n";
  ptr += "  display: inline-block;\n";
  ptr += "  margin-right: 10px;\n";
  ptr += "}\n";
  ptr += "#temp:before {\n";
  ptr += "  content: \"Temperature: \";\n";
  ptr += "}\n";
  ptr += "#hum:before {\n";
  ptr += "  content: \"Humidity: \";\n";
  ptr += "}\n";
  ptr += "#moist:before {\n";
  ptr += "  content: \"Soil Moisture: \";\n";
  ptr += "}\n";

  server.send(200, "text/css", ptr);
  digitalWrite(led, 0);
}
void handleJS() {
  digitalWrite(led, 1);
  String ptr = "var chartData = [];\n"; // Array to store the historical data

  ptr += "var myChart = null;\n"; // Variable to store the chart instance

  ptr += "function timer() {\n";
  ptr += "  $.getJSON(\"/readings.json\", function(data) {\n";
  ptr += "    console.log(data);\n";
  ptr += "    $(\"#temp\").html(data.temp+\"<sup>o</sup>\");\n";
  ptr += "    $(\"#hum\").html(data.hum+\"%\");\n";
  ptr += "    $(\"#moist\").html(data.moisture+\"%\");\n";

  ptr += "    chartData.push({\n"; // Push the new data to the array
  ptr += "      temp: data.temp,\n";
  ptr += "      hum: data.hum,\n";
  ptr += "      moisture: data.moisture\n";
  ptr += "    });\n";
  ptr += "    updateChart();\n"; // Call the function to update the chart
  ptr += "  });\n";
  ptr += "}\n";

  ptr += "function updateChart() {\n";
  ptr += "  if (myChart) {\n";
  ptr += "    myChart.destroy();\n"; // Destroy the existing chart instance if it exists
  ptr += "  }\n";

  ptr += "  var labels = chartData.map(function(entry) {\n"; // Extract labels from the array
  ptr += "    return '';\n"; // You can customize the labels here
  ptr += "  });\n";
  ptr += "  var tempData = chartData.map(function(entry) {\n"; // Extract temperature data from the array
  ptr += "    return entry.temp;\n";
  ptr += "  });\n";
  ptr += "  var humData = chartData.map(function(entry) {\n"; // Extract humidity data from the array
  ptr += "    return entry.hum;\n";
  ptr += "  });\n";
  ptr += "  var moistData = chartData.map(function(entry) {\n"; // Extract moisture data from the array
  ptr += "    return entry.moisture;\n";
  ptr += "  });\n";

  ptr += "  var ctx = document.getElementById('chart').getContext('2d');\n"; // Get the chart canvas context
  ptr += "  myChart = new Chart(ctx, {\n";
  ptr += "    type: 'line',\n";
  ptr += "    data: {\n";
  ptr += "      labels: labels,\n"; // Use the extracted labels
  ptr += "      datasets: [\n";
  ptr += "        {\n";
  ptr += "          label: 'Temperature',\n";
  ptr += "          data: tempData,\n"; // Use the extracted temperature data
  ptr += "          borderColor: 'rgba(255, 99, 132, 1)',\n";
  ptr += "          backgroundColor: 'rgba(255, 99, 132, 0.2)',\n";
  ptr += "          borderWidth: 1\n";
  ptr += "        },\n";
  ptr += "        {\n";
  ptr += "          label: 'Humidity',\n";
  ptr += "          data: humData,\n"; // Use the extracted humidity data
  ptr += "          borderColor: 'rgba(54, 162, 235, 1)',\n";
  ptr += "          backgroundColor: 'rgba(54, 162, 235, 0.2)',\n";
  ptr += "          borderWidth: 1\n";
  ptr += "        },\n";
  ptr += "        {\n";
  ptr += "          label: 'Moisture',\n";
  ptr += "          data: moistData,\n"; // Use the extracted moisture data
  ptr += "          borderColor: 'rgba(75, 192, 192, 1)',\n";
  ptr += "          backgroundColor: 'rgba(75, 192, 192, 0.2)',\n";
  ptr += "          borderWidth: 1\n";
  ptr += "        }\n";
  ptr += "      ]\n";
  ptr += "    },\n";
  ptr += "    options: {\n";
    ptr += "      animation: {\n";
  ptr += "        duration: 0 // Set animation duration to 0 to disable animation\n";
  ptr += "      },\n";

  ptr += "      scales: {\n";
  ptr += "        y: {\n";
  ptr += "          beginAtZero: true\n";
  ptr += "        }\n";
  ptr += "      }\n";
  ptr += "    }\n";
  ptr += "  });\n";
  ptr += "}\n";

  ptr += "setInterval(timer, 1500);\n";

  server.send(200, "text/javascript", ptr);
  digitalWrite(led, 0);
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





const char DEGREE_SYMBOL[] = { 0xB0, '\0' };

unsigned long previousMillis=0;
  
const unsigned long loopInterval = 1000000;


//U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(0x3C, /* clock=*/ SCL, /* data=*/ SDA, /* reset=*/ U8X8_PIN_NONE);   // All Boards without Reset of the Display

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);



Adafruit_BME280 bme; 

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
  digitalWrite(led, 0);

  
  // put your setup code here, to run once:
  Serial.begin(115200);  
  Wire.begin();
  bme.begin(0x76);
  u8g2.begin();
  u8g2_prepare();
  
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
  server.on("/readings.json", handleReadings);
  server.on("/style.css",handleCSS);
  server.on("/app.js",handleJS);
  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("HTTP server started");
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

    sprintf(str, "%s,%s,%i", dtostrf(temp,4,2,buff),dtostrf(hum,4,2,buff2),moisture);
    Serial.println(str);
    u8g2.clearBuffer(); // clear the internal memory
    sprintf(str, "%s C", buff);    
    u8g2.drawStr(32,9, str);
    u8g2.drawUTF8(82,6, DEGREE_SYMBOL);

    //u8g2.drawFrame(1,1,127,63);
    u8g2.drawLine(0,32, 128,32);

    sprintf(str, "%s%% %i%% ", buff2, moisture);
    u8g2.drawStr(8,42,str);

    u8g2.sendBuffer();
}  



void loop() {
  
  unsigned long currentTime = micros();
  unsigned long interval = currentTime - previousMillis;
  server.handleClient();
 
  if (interval >=  loopInterval) 
  {
  printValues();
  previousMillis=  currentTime;
  }
  
}
