/*
 File: Sketch_feb21a
 Author: Erfan Rahimi
 Date 2024-02-21
 Description: This is an ESP weather Station project that uses IoT(Internet Of Things)
 applications in order to design and monitor enviromental conditions such as temperature and 
 humidity. By leveraging the ESPAsyncWebServer library, the ESP8266 D1 mini creates a web interfaces
 that is hosted on SPIFFS(SPI Flash File System) of the D1 mini that allows the user to remotly 
 access real-time data of the temperature and himidty data that is captured by the DHT11 sensor.
 */

//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX

/*
 This part includes all the different libraries depending on wether the user is using a ESP32 or ESP8266 board.
 For the ESP32 board, it includes the following: WiFi.h, AsyncWebServer.h and the SPIFFS.h. For Esp8266, it includes Arduino
 core, ESP8266WiFi.h, Hash.h, AsyncTCP.h, AsyncWebServer.h, and the FS.h.
*/

#ifdef ESP32
  #include <WiFi.h>
  #include <ESPAsyncWebServer.h>
  #include <SPIFFS.h>
#else
  #include <Arduino.h>
  #include <ESP8266WiFi.h>
  #include <Hash.h>
  #include <ESPAsyncTCP.h>
  #include <ESPAsyncWebServer.h>
  #include <FS.h>
#endif
#include <DHT.h> // Include the DHT library

/*
 In this part, DHTPIN is defines as pin 2, which is the digital pin that is connected to the DHT sensor.
 The DHTTYPE is defines as DHT11 which indicates the type of DHT sensor that is being used. And at last
, an instance of the DHT class is created with DHTPIN and DHTTYPE named dht.
*/

#define DHTPIN 2     // Digital pin connected to the DHT sensor
#define DHTTYPE DHT11   // DHT 11

DHT dht(DHTPIN, DHTTYPE); // Initialize DHT sensor

/*
 These two lines define the SSID and Password for the local Wi-Fi credential of the user. The 
 ESP device at hand will connect to this WiFi.
*/
const char* ssid = "TN-BZ1772";
const char* password = "wadHyWyohof5";

/*
 This line here creates an instance of the AsyncWebServer class on port 80, 
 which is the default port for HTTP communication.
*/
AsyncWebServer server(80);

/*
 These are two variables that are declared in order to store tempreture and humidity reading from the sensor at hand
 You can add on this and edit depending on your sensor.
 */

float temp = 0; 
float hum = 0;  

/*
 This function here reads the temperature and the humidity values from the DHT sensor and 
 stores it in the temp and hum variables that we declared before.
 */

void storeSensorValues() {
  // Read temperature and humidity from DHT sensor
  temp = dht.readTemperature();
  hum = dht.readHumidity();
}

xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

/*
 The setup initializes all the necessary components and configurations which are needed for the 
 sketch to run. This includes setting up the serial communication, initializes sensors, connects to the 
 WiFi, sets up routes for serving web content and starts the web server.
 */

void setup(){

  /*
   Here we are initializing the serial communication with a baud of 115200. This is very useful 
   for debugging, as it allows us to print on to the serial monitor to understand what is going on in your code 
   and diagnose issues. You will want to have to do the same.
   */
  
  Serial.begin(115200);
  
  /*
   This initilizes the DHT sensor. This allows us to prepare the sensor for reading 
   the temperature and humidity.
   */
  dht.begin();

  /*
   This block of code initializes the SPIFFS (SPI Flash File System). SPIFFS allows the 
   ESP boards to store and access files in its flash memory. If there is any error while 
   mounting the SPIFFS, the error message will be printed on the serial monitor.
   */
  if(!SPIFFS.begin()){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  /*
   In this block of code, we are attemping to connect to the WiFi using the given and provided 
   SSID and Password. This is a loop that waits until the board is connected to the WiFi. I constantly
   checks the WiFi status until the board is connected, printing the "Connecting to WiFi..." on the 
   serial monitor.
   */
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  /*
   Once connected to the WiFi, this line will print the locap IP adress of the device 
   on the serial monitor. This is crutial and you will need this in order to identify 
   the IP address assigned to the device on the network.
   */
  Serial.println(WiFi.localIP());

  /*
   This block of code defines routes for data in response to HTTP GET requests to one particular URLs.
   When a client accesses these URLs, the given server will respond with the appropriate content or data.
   In this case, serving the main webpage, temperature data and humidity data.These are lambda functions and when a 
   client accesses the root URL, the provided lambda function is invoked.
   */

   /*
    This line sets a route for the rott URL. When a client makes a GET request to the root URL, it serves the 
    "index.html" files from the stored file in the SPIFFS. We store the html file with the built in ESP8266 sketch data upload
    that you will have to download.The request line sends the content 
   of the saved html file as the response to the clients request. This will later be the main webpage when 
   the root URL is reached and accessed.
    */
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html");
  });

  /*
   This line sets a route for temperature data. When the client makes a GET request to this URL, it serves the 
   live and current temperature values. Although there is some delay. The request line sends the current temperature as the 
   response to the clients request. 
   */
  server.on("/temperature", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", String(temp).c_str());
  });

  /*
   This is very similiar to the previous line of code and sets a route for humidity.
   When a client makes a GET request to this URL, it serves the current humidity data.
   This line sends the current humidity as the response to the clients request.
   */
  server.on("/humidity", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", String(hum).c_str());
  });

  /*
   This little line starts the web server, enabling the server to listen to incoming HTTP requests.
   The reason to why we have it here is so that we can initialize the server before the program starts.
   */
  server.begin();
}

xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

/*
 This is where we continuosly execute the code. Here, we call the readSensorValues function, which reads the current 
 temperature and humidity.
 */

void loop(){
  // Read sensor values every 2 seconds (adjust as needed)
  storeSensorValues();
  delay(2000);
}
