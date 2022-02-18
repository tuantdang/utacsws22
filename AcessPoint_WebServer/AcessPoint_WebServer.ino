#include <SPI.h>
#include <WiFi101.h>
#include "arduino_secrets.h" 

char ssid[] = "Jerry";        // your network SSID (name)
char pass[] = "";             // your network password
int keyIndex = 0;             // your network key Index number (needed only for WEP)

int led =  LED_BUILTIN;
int status = WL_IDLE_STATUS;
WiFiServer server(80);

void setup() {
  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // Wait for serial port to connect. Needed for native USB port only
  }

  Serial.println("Access Point Web Server");

  pinMode(led, OUTPUT);      // Set the LED pin mode

  // Check for the presence of the shield:
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi 101 Shield not present");
    // Do not continue
    while (true);
  }

  // By default the local IP address of will be 192.168.1.1
  // you can override it with the following:
  // WiFi.config(IPAddress(10, 0, 0, 1));

  // Print the network name (SSID);
  Serial.print("Creating access point named: ");
  Serial.println(ssid);

  // Create open network. Change this line if you want to create a WEP network:
  status = WiFi.beginAP(ssid);
  if (status != WL_AP_LISTENING) {
    Serial.println("Creating access point failed");
    // don't continue
    while (true);
  }

  // Wait 10 seconds for connection:
  delay(10000);

  // Start the web server on port 80
  server.begin();

  // You're connected now, so print out the status
  printWiFiStatus();
}


void loop() {
  // Compare the previous status to the current status
  if (status != WiFi.status()) {
    // It has changed, so update the variable
    status = WiFi.status();

    if (status == WL_AP_CONNECTED) {
      byte remoteMac[6];

      // A device has connected to the AP
      Serial.print("Device connected to AP, MAC address: ");
      WiFi.APClientMacAddress(remoteMac);
      printMacAddress(remoteMac);
    } else {
      // A device has disconnected from the AP, and we are back in listening mode
      Serial.println("Device disconnected from AP");
    }
  }
  
  WiFiClient client = server.available();   // Listen for incoming clients

  if (client) {                             // If you get a client,
    Serial.println("new client");           // print a message out the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there are bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        if (c == '\n') {                    // if the byte is a newline character

          // If the current line is blank, you got two newline characters in a row.
          // That's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();

            // The content of the HTTP response follows the header:
            client.print("Click <a href=\"/H\">here</a> turn the LED on<br>");
            client.print("Click <a href=\"/L\">here</a> turn the LED off<br>");

            // The HTTP response ends with another blank line:
            client.println();
            // Break out of the while loop:
            break;
          }
          else {      // If you got a newline, then clear currentLine:
            currentLine = "";
          }
        }
        else if (c != '\r') {    // If you got anything else but a carriage return character,
          currentLine += c;      // Add it to the end of the currentLine
        }

        // Check to see if the client request was "GET /H" or "GET /L":
        if (currentLine.endsWith("GET /H")) {
          digitalWrite(led, HIGH);               // GET /H turns the LED on
        }
        if (currentLine.endsWith("GET /L")) {
          digitalWrite(led, LOW);                // GET /L turns the LED off
        }
      }
    }
    // Close the connection:
    client.stop();
    Serial.println("client disconnected");
  }
}

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
  
  // Print where to go in a browser:
  Serial.print("To see this page in action, open a browser to http://");
  Serial.println(ip);

}

void printMacAddress(byte mac[]) {
  for (int i = 5; i >= 0; i--) {
    if (mac[i] < 16) {
      Serial.print("0");
    }
    Serial.print(mac[i], HEX);
    if (i > 0) {
      Serial.print(":");
    }
  }
  Serial.println();
}
