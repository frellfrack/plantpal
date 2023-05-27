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

String returnHeader()
{
  String ptr = "<!DOCTYPE html>\n<html lang=\"enGB\">\n";
  ptr += "<head>\n";
    ptr += "<meta charset=\"UTF-8\">\n";
  ptr += "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  ptr += "<title>Plant Pal V1</title>\n";

 
  ptr += "<script src=\"https://code.jquery.com/jquery-3.7.0.min.js\" integrity=\"sha256-2Pmvv0kuTBOenSvLm6bvfBSSHrUJ+3A7x6P5Ebd07/g=\" crossorigin=\"anonymous\"></script>\n"; // Add jQuery library
  ptr += "<script src=\"https://cdn.jsdelivr.net/npm/chart.js\"></script>\n"; // Add Chart.js library
  ptr += "</head>\n";
  ptr += "<body>\n";
  ptr += "<header><h1>Plant Pal V0.1</h1></header>\n";
 
  ptr += "<link rel=\"stylesheet\" href=\"/style.css\">\n";

  ptr += "<nav>\n";
  
  ptr += "<a href=\"/\">Dashboard</a>\n";
    
  ptr += "<a href=\"/settings/\">Settings</a>\n";
  ptr += "</nav>\n";
   ptr += "<p>\n";
ptr+=getRandomPlantFact();
   ptr += "</p>\n";
  ptr += "<main>\n";
  return ptr;  
}

void handleRoot() {
  digitalWrite(led, 1);

  String ptr = returnHeader();
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
    // Navigation buttons
  ptr += "nav {\n";
  ptr += "  display: flex;\n";
  ptr += "  justify-content: center;\n";
  ptr += "  margin-top: 20px;\n";
  ptr += "}\n";
  ptr += "nav a {\n";
  ptr += "  display: inline-block;\n";
  ptr += "  padding: 10px 20px;\n";
  ptr += "  margin: 0 5px;\n";
  ptr += "  background-color: #333;\n";
  ptr += "  color: #FFF;\n";
  ptr += "  text-decoration: none;\n";
  ptr += "  border-radius: 5px;\n";
  ptr += "}\n";
  ptr += "nav a:hover {\n";
  ptr += "  background-color: #555;\n";
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

void displaySettings()
{
  
  digitalWrite(led, 1);

  String ptr = returnHeader();
  ptr += "<form method=\"post\" action=\"/storesettings/\">\n";
  ptr += "<label>Water % Level: <input type=\"number\" name=\"waterlevel\" min=\"0\" max=\"100\"/></label>\n";
  ptr += "<input type=\"submit\" value=\"Update\"></form>\n"; 
  
  ptr += "<script src=\"/app.js\"></script>\n"; // include app logic
  ptr += "</body>\n";
  ptr += "</html>\n";

  server.send(200, "text/html", ptr);
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

// Array of 100 plant facts
const char* plantFacts[] = {
  "Plants use photosynthesis to convert sunlight into energy.",
  "Some plants can live for hundreds or even thousands of years.",
  "Bamboo is the fastest-growing plant in the world.",
  "The world's largest flower is the Rafflesia arnoldii.",
  "Cacti are succulent plants adapted to dry environments.",
  "Plants release oxygen as a byproduct of photosynthesis.",
  "Plants have been used for medicinal purposes throughout history.",
  "The Venus flytrap is a carnivorous plant that catches insects for nutrients.",
  "Seeds are nature's way of allowing plants to reproduce and spread.",
  "Plants can communicate with each other through chemical signals.",
  "Plants can adapt to different environmental conditions.",
  "Certain plants have evolved thorns or prickles for protection.",
  "The largest trees in the world are giant sequoias and coast redwoods.",
  "Plants provide habitats and food for many animal species.",
  "The colors of flowers are often used to attract pollinators.",
  "Plants convert carbon dioxide into oxygen through photosynthesis.",
  "The oldest known tree is estimated to be over 5,000 years old.",
  "Certain plants can survive in extreme temperatures, such as deserts or the Arctic.",
  "The Amazon rainforest is home to an incredible diversity of plant species.",
  "Plants play a crucial role in maintaining the Earth's ecosystems.",
  "Some plants have evolved to live in symbiotic relationships with other organisms.",
  "Plants can reproduce through asexual methods like vegetative propagation.",
  "The tallest flowering plant is the Eucalyptus regnans.",
  "Plant roots help anchor the plant and absorb water and nutrients from the soil.",
  "The study of plants is called botany.",
  "Plants can undergo dormancy during unfavorable conditions.",
  "The Baobab tree can store thousands of liters of water in its trunk.",
  "Plants have been used for dye production throughout history.",
  "The Corpse flower is known for its foul odor.",
  "Certain plants have natural insect-repellent properties.",
  "Plants can sense and respond to changes in light direction.",
  "Some plants have evolved mechanisms to catch and digest insects.",
  "The ancient Egyptians used plants in the mummification process.",
  "Plant hormones regulate various growth processes in plants.",
  "Certain plants have evolved to thrive in aquatic environments.",
  "The lotus flower is considered a symbol of purity and enlightenment.",
  "Plants can regenerate damaged or lost parts.",
  "The Titan arum produces the largest unbranched inflorescence in the world.",
  "Plants can be categorized into different types, such as trees, shrubs, and herbs.",
  "The Saguaro cactus can live for over 150 years.",
  "The rubber tree is a major source of natural rubber.",
  "Plants have been used for making paper for centuries.",
  "The Pitcher plant traps insects in its pitcher-shaped leaves.",
  "Certain plants have thigmotropism, where they respond to touch or contact.",
  "Plants have different strategies for seed dispersal, such as wind, water, or animals.",
  "The California redwood is the tallest tree species in the world.",
  "Plants have developed defense mechanisms to protect against herbivores.",
  "The Aloe vera plant is known for its medicinal properties for skin ailments.",
  "Certain plants have adapted to grow in acidic or alkaline soil conditions.",
  "Plants have complex root systems that provide support and nutrient absorption.",
  "The Corpse Lily has the largest individual flower in the world.",
  "Plants have evolved various methods of pollination, including wind, insects, and birds.",
  "The Bristlecone pine is one of the oldest known tree species.",
  "Plants can undergo seasonal changes, such as shedding leaves in autumn.",
  "Certain plants have natural antifungal or antibacterial properties.",
  "The Prickly Pear cactus produces edible fruits.",
  "Plants have been used for making various types of fibers, such as cotton and linen.",
  "The Baobab tree is also known as the \"Tree of Life\".",
  "Plants can release chemical signals to attract beneficial insects for pollination.",
  "Certain plants have evolved to live in symbiotic relationships with fungi.",
  "The Water Lily has floating leaves and flowers.",
  "Plants play a vital role in the carbon cycle by absorbing carbon dioxide.",
  "The Agave plant is used to make tequila.",
  "Plants have different methods of reproduction, including sexual and asexual.",
  "Certain plants have adapted to survive in nutrient-poor soil.",
  "The Flowering Dogwood is a popular ornamental tree.",
  "Plants can exhibit heliotropism, where they move or grow towards the sun.",
  "Certain plants have thorns or spines as a defense mechanism against herbivores.",
  "The Aloe vera plant is native to the Arabian Peninsula.",
  "Plants have been used for making various types of beverages, such as tea and coffee.",
  "The Saguaro cactus is native to the Sonoran Desert.",
  "Plants have different types of leaf arrangements, such as alternate, opposite, or whorled.",
  "Certain plants have evolved to attract specific pollinators, such as bees or butterflies.",
  "The Bonsai technique involves growing miniature trees in containers.",
  "Plants have adapted to survive in extreme habitats, such as deserts, tundra, or mountains.",
  "The Lotus plant can survive in muddy or aquatic environments.",
  "Plants have different methods of storing food reserves, such as bulbs, rhizomes, or tubers.",
  "Certain plants have symbiotic relationships with nitrogen-fixing bacteria.",
  "The Peony is a popular flowering plant in many gardens.",
  "Plants can produce scented compounds to attract pollinators.",
  "The Joshua tree is native to the Mojave Desert.",
  "Plants have evolved various mechanisms for seed dispersal, such as burrs or hooks.",
  "Certain plants have evolved to survive in low-light conditions, such as the understory of forests.",
  "The Rose is a classic symbol of love and beauty.",
  "Plants can respond to environmental stimuli, such as temperature or touch.",
  "Certain plants have evolved to thrive in salty or brackish water.",
  "The Pitcher plant is native to swampy or boggy areas.",
  "Plants have different types of root systems, such as taproots or fibrous roots.",
  "The Baobab tree is known for its unique bottle-shaped trunk.",
  "Plants can reproduce through fragmentation, where a piece of the plant can grow into a new individual.",
  "Certain plants have evolved to store water in their leaves, such as succulents.",
  "The Marigold is a common flower used in gardens for pest control.",
  "Plants have been used for making various types of oils, such as lavender or peppermint oil.",
  "The Cacao tree is native to the tropical regions of the Americas.",
  "Plants have adapted to attract different types of pollinators, including bats and moths.",
  "Certain plants have evolved to tolerate fire and even rely on it for seed germination.",
  "The Banyan tree has aerial roots that grow downwards to the ground.",
  "Plants can exhibit nastic movements, where they respond to stimuli but without directional growth.",
  "Certain plants have evolved to attract ants for defense against herbivores.",
  "The Hoya plant is known for its waxy and fragrant flowers.",
  "Plants have been used for making natural dyes for fabrics and textiles.",
  "The Durian fruit is notorious for its strong odor.",
  "Plants have different types of specialized cells, such as guard cells for stomatal regulation.",
  "Certain plants have evolved to be wind-pollinated, producing large amounts of lightweight pollen.",
  "The Coconut Palm is known for its versatile uses, including food, water, and construction materials.",
  "Plants have adapted to resist or tolerate various types of pests and diseases.",
  "Certain plants have evolved to produce toxins as a defense mechanism against herbivores.",
  "The Wisteria is a climbing vine known for its cascading clusters of flowers.",
  "Plants have been used for making herbal remedies and traditional medicines.",
  "The Baobab tree is culturally significant in many African societies.",
  "Plants can exhibit nyctinasty, where they open or close their flowers or leaves in response to light.",
  "Certain plants have evolved to live in symbiotic relationships with animals, such as pollinators or seed dispersers.",
  "The Cactus family includes various species with different shapes and sizes.",
  "Plants have different methods of storing water, such as succulent leaves or swollen stems.",
  "Certain plants have evolved to have deep taproots to access water deep underground.",
  "The Cherry Blossom is a symbol of beauty and transience in Japanese culture.",
  "Plants have been used for making natural fibers for clothing and textiles.",
  "The Baobab tree has traditional uses in African folklore and rituals.",
  "Plants can respond to gravity through gravitropism, where roots grow downward and shoots grow upward.",
  "Certain plants have evolved to be self-pollinating, reducing the reliance on external pollinators.",
  "The Bird of Paradise flower has vibrant and exotic blooms.",
  "Plants have different methods of defense against herbivores, including thorns, spines, or toxic compounds.",
  "Certain plants have evolved to live in symbiotic relationships with fungi, forming mycorrhizae.",
  "The Bristlecone pine can withstand harsh weather conditions and has a twisted and gnarled appearance.",
  "Plants have adapted to attract specific types of birds for pollination or seed dispersal.",
  "The Orchid family is one of the largest plant families, known for its diverse and intricate flowers.",
  "Plants can undergo senescence, where they age and eventually die after reproductive maturity.",
  "Certain plants have evolved to have brightly colored fruits to attract animals for seed dispersal.",
  "The Venus Flytrap has specialized leaves with trigger-like mechanisms to catch insects.",
  "Plants have different methods of vegetative propagation, such as runners, bulbs, or cuttings.",
  "Certain plants have evolved to have serrated or toothed leaves for defense or increased surface area.",
  "The Sunflower is known for its large and distinctive flower head.",
  "Plants have been used for making natural pigments for art and dyeing.",
  "The Baobab tree is considered a symbol of longevity and strength.",
  "Plants can exhibit heliotropism, where leaves or flowers track the movement of the sun.",
  "Certain plants have evolved to have water-repellent leaves or surfaces to prevent water loss.",
  "The Poinsettia is a popular plant associated with Christmas and winter holidays.",
  "Plants have different mechanisms of allelopathy, where they release chemicals to inhibit the growth of nearby plants.",
  "Certain plants have evolved to have symbiotic relationships with nitrogen-fixing bacteria, enriching the soil with nitrogen.",
  "The Maple tree is known for its vibrant autumn foliage.",
  "Plants have been used for making natural cosmetics and skincare products.",
  "The Baobab tree can store large amounts of water to survive drought conditions.",
  "Plants can respond to touch or contact through thigmotropism, where they change their growth direction.",
  "Certain plants have evolved to have hairy or fuzzy leaves as a defense mechanism against herbivores.",
  "The Sunflower follows the movement of the sun during the day, a phenomenon known as heliotropism.",
  "Plants have different types of inflorescence, such as spikes, racemes, or panicles.",
  "Certain plants have adapted to live in acidic or alkaline soil conditions, such as heathlands or alkaline flats.",
  "The Bristlecone pine is known for its long lifespan and resilience in harsh environments.",
  "Plants can exhibit nocturnal blooming, where their flowers open during the night.",
  "Certain plants have evolved to have silvery or hairy leaves to reflect sunlight and reduce water loss.",
  "The Mosses are primitive plants that reproduce by spores.",
  "Plants have been used for making various types of alcoholic beverages, such as wine or beer.",
  "The Baobab tree has cultural and symbolic significance in African folklore and traditions.",
  "Plants can exhibit thermogenesis, where they generate heat to attract pollinators or aid in seed dispersal.",
  "Certain plants have evolved to have deep taproots to access water in arid environments.",
  "The Dandelion has a fluffy seed head that disperses in the wind.",
  "Plants have different methods of defense against pathogens, including the production of antimicrobial compounds.",
  "Certain plants have evolved to have brightly colored leaves for visual attraction or camouflage.",
  "The Lotus plant has cultural and religious significance in many Asian countries.",
  "Plants can exhibit nyctinasty, where leaves fold or close at night or during unfavorable conditions.",
  "Certain plants have adapted to live in aquatic environments, such as ponds, rivers, or wetlands.",
  "The Baobab tree is nicknamed the \"Upside-Down Tree\" due to its unique appearance when leafless.",
  "Plants can reproduce through apomixis, where seeds are produced without fertilization.",
  "Certain plants have evolved to have deep root systems to stabilize soil and prevent erosion.",
  "The Sunflower is a popular source of edible seeds and oil.",
  "Plants have different types of reproductive structures, such as flowers, cones, or spores.",
  "Certain plants have evolved to have serrated leaves to deter herbivores or increase surface area for photosynthesis.",
  "The Baobab tree has been used for various purposes, including food, medicine, and shelter, by indigenous cultures.",
  "Plants can exhibit dormancy during unfavorable seasons or conditions, conserving energy and resources.",
  "Certain plants have evolved to have brightly colored stems or bark for visual attraction or protection.",
  "The Truffle is a rare and highly prized edible fungus associated with tree roots.",
  "Plants have been used for making natural inks and dyes for artistic and writing purposes.",
  "The Baobab tree has a unique growth pattern, with a thick trunk and sparse branches.",
  "Plants can respond to touch or stimuli through thigmonasty, where leaves or flowers fold or collapse upon contact.",
  "Certain plants have adapted to live in arctic or alpine environments, enduring freezing temperatures and strong winds.",
  "The Orchid family is known for its intricate and diverse flowers, attracting pollinators with elaborate adaptations.",
  "Plants have different methods of nutrient acquisition, including symbiotic relationships with fungi or nitrogen-fixing bacteria.",
  "Certain plants have evolved to have specialized structures for water storage, such as succulent stems or leaves.",
  "The Baobab tree has a fibrous and nutritious fruit, rich in vitamin C.",
  "Plants can exhibit phototropism, where stems or leaves bend or grow towards a light source.",
  "Certain plants have evolved to have thorns, spines, or prickles for defense against herbivores.",
  "The Tulip is a popular spring flower associated with the Netherlands.",
  "Plants have been used for making natural fragrances and perfumes.",
  "The Baobab tree is often considered a symbol of resilience and survival in challenging environments.",
  "Plants can undergo secondary growth, resulting in increased stem thickness and woody tissues.",
  "Certain plants have evolved to have toxic or bitter-tasting compounds to deter herbivores.",
  "The Baobab tree has an impressive girth, with some individuals having trunk diameters of several meters.",
  "Plants can exhibit leaf movements, such as opening and closing of leaves in response to light or temperature.",
  "Certain plants have adapted to live in nutrient-poor environments, relying on symbiotic relationships or specialized nutrient acquisition mechanisms.",
  "The Baobab tree has been used for making traditional crafts, such as baskets, ropes, and musical instruments.",
  "Plants can produce secondary metabolites with medicinal properties, used in traditional and modern medicine.",
  "Certain plants have evolved to have succulent leaves or stems for water storage in dry climates.",
  "The Baobab tree is often a prominent feature in African landscapes and savannahs.",
  "Plants can exhibit different forms of reproductive structures, such as cones in conifers or catkins in some trees.",
  "Certain plants have adapted to live in intertidal zones, with the ability to tolerate saltwater and fluctuating water levels.",
  "The Baobab tree is known for its resilience and ability to survive in harsh and drought-prone environments."
};

// Function to return a random plant fact
const char* getRandomPlantFact() {
  int index = random(0, 100);
  return plantFacts[index];
}


String hostname = "plantpal";
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
  WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE, INADDR_NONE);
  WiFi.setHostname(hostname.c_str()); //define hostname

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
 // Initialize mDNS
 
  if (MDNS.begin("plantpal")) {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);
  server.on("/readings.json", handleReadings);
  server.on("/style.css",handleCSS);
  server.on("/settings/",displaySettings);
  server.on("/app.js",handleJS);
  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("HTTP server started");

  String ipAddressStr = WiFi.localIP().toString();
  const char* ipAddress = ipAddressStr.c_str();

  u8g2.clearBuffer(); // clear the internal memory
  u8g2.drawStr(6, 9, ssid);
  u8g2.drawStr(6, 39, ipAddress);
  u8g2.drawFrame(1,1,127,63);
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

    sprintf(str, "%s,%s,%i", dtostrf(temp,4,2,buff),dtostrf(hum,4,2,buff2),moisture);
    Serial.println(str);
    u8g2.clearBuffer(); // clear the internal memory
    sprintf(str, "%s C", buff);    
    u8g2.drawStr(32,9, str);
    u8g2.drawUTF8(82,6, DEGREE_SYMBOL);

    //u8g2.drawFrame(1,1,127,63);
    u8g2.drawLine(0,32, 128,32);

    sprintf(str, "%s%% %i%% ", buff2, moisture);
    u8g2.drawStr(20,42,str);

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
