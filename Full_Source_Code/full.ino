#include <SPI.h>
#include <WiFi101.h>
#include <SparkFun_Bio_Sensor_Hub_Library.h>
#include <Wire.h>
#define SPEAKER 3
#define LOW_HEARTRATE 60
#define HIGH_HEARTRATE 100
#define LOW_SPO2LEVEL 95

int stat = 100;

SparkFun_Bio_Sensor_Hub bioHub(resPin, mfioPin);
bioData body;

char ssid[] = "Jerry";            // your network SSID (name)
char pass[] = "46840574";         // your network password
int keyIndex = 0;                 // your network key Index number (needed only for WEP)

int status = WL_IDLE_STATUS;
WiFiServer server(80);

void printWiFiStatus() {
  // Print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // Print your WiFi 101 Shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // Print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}

// Function inferring values of sensor's status to spoken language
void check_stat(int stat) {
  if (stat == 0)
    Serial.println("Sensing your biometrics...");
  else if (stat == 1)
    Serial.println("Sensor is not ready.");
  else if (stat == -1)
    Serial.println("Object detected.");
  else if (stat == -2)
    Serial.println("Too many device's vibration.");
  else if (stat == -3)
    Serial.println("No objects detected.");
  else if (stat == -4)
    Serial.println("Pressing too hard!");
  else if (stat == -5)
    Serial.println("Objects dectected not finger.");
  else
    Serial.println("Too many finger's vibration.");
}

// Function initiating speaker
void pinInit() {
  pinMode(SPEAKER, OUTPUT);
  digitalWrite(SPEAKER, LOW);
}

void setup() {
  // Initialize serial and wait for port to open:
  Serial.begin(9600);
  
  //while (!Serial) {
    //; // Wait for serial port to connect. Needed for native USB port only
  //}

  // Check for the presence of the shield:
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi 101 Shield not present");
    // Don't continue:
    while (true);
  }

  // Attempt to connect to WiFi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);

    // Wait 10 seconds for connection:
    delay(10000);
  }
  
  server.begin();
  printWiFiStatus();

  // Initialie sensor and speaker 
  Wire.begin();
  int result = bioHub.begin();
  if (result == 0)
    Serial.println("Sensor started!");
  else
    Serial.println("Could not communicate with the sensor!!!");

  Serial.println("Configuring Sensor....");
  int error = bioHub.configBpm(MODE_TWO);
  if (error == 0) {
    Serial.println("Sensor configured.");
  }
  else {
    Serial.println("Error configuring sensor.");
    Serial.print("Error: ");
    Serial.println(error);
  }

  Serial.println("Loading up the buffer with data....");
  delay(4000);

  pinInit();
}

void loop() {
  // Listen for incoming clients
  WiFiClient client = server.available();
  
  if (client) {
    Serial.println("new client");
    bool currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        if (c == '\n' && currentLineIsBlank) {

          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close"); 
          client.println("Refresh: 5");
          client.println();
          client.println("<!DOCTYPE HTML>");
          client.println("<html>");
          
          //Read biometrics data
          body = bioHub.readBpm();
          stat = body.extStatus;
          check_stat(stat);

          // Retreive heart rate and SpO2 level from the sensor
          int heart_rate = body.heartRate;
          int spo2_level = body.oxygen;

          // Set the flag value as 0, will be changed to 1 if any abnormality is detected
          int flag = 0;

          // Play sound if there is any abnormality detected
          if (heart_rate != 0 && spo2_level != 0) {
            if (heart_rate < LOW_HEARTRATE || heart_rate > HIGH_HEARTRATE || spo2_level <= LOW_SPO2LEVEL) {
              tone(SPEAKER, 100, 500);
              flag = 1;
            }
          }

          // Print heart rate together with checking abnormalities
          Serial.print("Heart rate: ");
          if (heart_rate < LOW_HEARTRATE && flag == 1) {
            Serial.print(heart_rate);
            Serial.println(" <-- Your heart rate is low.");
          }
          else Serial.println(heart_rate);

          if (heart_rate > HIGH_HEARTRATE && flag == 1) {
            Serial.println(" <-- Your heart rate is high.");
          }

          // Print SpO2 level together with checking abnormalities
          Serial.print("SpO2 level: ");
          if (spo2_level <= LOW_SPO2LEVEL && flag == 1) {
            Serial.print(spo2_level);
            Serial.println(" <-- Your SpO2 level is low.");
          }
          else Serial.println(spo2_level);

          client.print("<center><h1 style=\"background-color:powderblue;\">Welcome to Computer Science @ UT Arlington</h1></center>");
          client.print("<center><h2 style=\"background-color:#E6B0C5;\">[CPS-Health] Building Cyber-Physical Systems for Healthcare</h2></center>");
          client.print("<center><h3 style=\"background-color:#E6B0C5;\">");

          // Print status values of sensor's status to spoken language to client
          if (stat == 0)
            client.print("Sensing your biometrics...</h3></center>");
          else if (stat == 1)
            client.print("Sensor is not ready.</h3></center>");
          else if (stat == -1)
            client.print("Object detected.</h3></center>");
          else if (stat == -2)
            client.print("Too many device's vibration.</h3></center>");
          else if (stat == -3)
            client.print("No objects detected.</h3></center>");
          else if (stat == -4)
            client.print("Pressing too hard!</h3></center>");
          else if (stat == -5)
            client.print("Objects dectected not finger.</h3></center>");
          else
            client.print("Too many finger's vibration.</h3></center>");

          // Print heart rate together with checking abnormalities
          if (heart_rate < LOW_HEARTRATE && flag == 1) {
            client.print("<center><h4 style=\"background-color:#B0C5E6;\"> Heart Rate: ");
            client.print(heart_rate); 
            client.print(" <-- Your heart rate is low. </h4></center>");
          } 
          else if (heart_rate > HIGH_HEARTRATE && flag == 1) {
            client.print("<center><h4 style=\"background-color:#B0C5E6;\"> Heart Rate: ");
            client.print(heart_rate); 
            client.print(" <-- Your heart rate is high. </h4></center>");
          } else {
            client.print("<center><h4 style=\"background-color:#B0C5E6;\"> Heart Rate: ");
            client.print(heart_rate); 
            client.print("</h4></center>");
          }

          // Print SpO2 level together with checking abnormalities
          if (spo2_level <= LOW_SPO2LEVEL && flag == 1) {
            client.print("<center><h4 style=\"background-color:#B0C5E6;\"> SpO2: ");  
            client.print(spo2_level); 
            client.print(" <-- Your SpO2 level is low. </h4></center>");
          }
          else {
            client.print("<center><h4 style=\"background-color:#B0C5E6;\"> SpO2: ");  
            client.print(spo2_level); 
            client.print("</h4></center>");
          }

          client.print("<h2 align=\"right\">Wireless and Sensor Systems Lab &copy;</h2>");
          client.println("</html>");
          break;
        }
        if (c == '\n') {
          currentLineIsBlank = true;
        }
        else if (c != '\r') {
          currentLineIsBlank = false;
        }
      }
    }

    delay(1);
    client.stop();
    Serial.println("Client disconnected.");
  }
}
