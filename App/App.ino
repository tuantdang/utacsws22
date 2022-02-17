#include <SPI.h>
#include <WiFi101.h>

#include <SparkFun_Bio_Sensor_Hub_Library.h>
#include <Wire.h>
#define SPEAKER 3
#define LOW_HEARTRATE 60
#define HIGH_HEARTRATE 100
#define LOW_SPO2LEVEL 95

int resPin = 4;
int mfioPin = 5;
int stat = 100;
int BassTab[] = { 500, 500, 500, 500, 500, 500, 500 };

SparkFun_Bio_Sensor_Hub bioHub(resPin, mfioPin);
bioData body;


///////please enter your sensitive data in the Secret tab/arduino_secrets.h
char ssid[] = "Jerry";        // your network SSID (name)
char pass[] = "46840574";    // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;                 // your network key Index number (needed only for WEP)

int status = WL_IDLE_STATUS;
WiFiServer server(80);

void setup() {
  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // check for the presence of the shield:
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi 101 Shield not present");
    // don't continue:
    while (true);
  }

  // attempt to connect to WiFi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);
  }
  server.begin();
  // you're connected now, so print out the status:
  printWiFiStatus();


  /*Sensor and speaker initialzation*/
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
  // listen for incoming clients
  WiFiClient client = server.available();
  if (client) {



    Serial.println("new client");
    // an HTTP request ends with a blank line
    bool currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the HTTP request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) {
          // send a standard HTTP response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close");  // the connection will be closed after completion of the response
          client.println("Refresh: 5");  // refresh the page automatically every 5 sec
          client.println();
          client.println("<!DOCTYPE HTML>");
          client.println("<html>");
          // output the value of each analog input pin

          /*
          for (int analogChannel = 0; analogChannel < 6; analogChannel++) {
            int sensorReading = analogRead(analogChannel);
            client.print("analog input ");
            client.print(analogChannel);
            client.print(" is ");
            client.print(sensorReading);
            client.println("<br />");
          }*/

          //Read bio-data
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
              for (int note_index = 0; note_index < 7; note_index++) {
                sound(note_index);
                delay(250);
              }
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

          // Delay between two consecutive measurements
          //delay(250);

          client.print("Heart rate: ");
          client.print(heart_rate);
          client.println("<br />");
          client.print("SpO2 level: ");
          client.print(spo2_level);          

          client.println("</html>");
          break;
        }
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        }
        else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    // give the web browser time to receive the data
    delay(1);

    // close the connection:
    client.stop();
    Serial.println("client disconnected");
  }
}


void printWiFiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi 101 Shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
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

// Function transmitting sound from Arduino to speaker
void sound(uint8_t note_index) {
  for (int i = 0; i < 100; i++) {
    digitalWrite(SPEAKER, HIGH);
    delayMicroseconds(BassTab[note_index]);
    digitalWrite(SPEAKER, LOW);
    delayMicroseconds(BassTab[note_index]);
  }
}
