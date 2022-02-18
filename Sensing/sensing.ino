#include <SparkFun_Bio_Sensor_Hub_Library.h>
#include <Wire.h>

int resPin = 4;
int mfioPin = 5;
int stat = 100;

SparkFun_Bio_Sensor_Hub bioHub(resPin, mfioPin); 
bioData body;  

// Function inferring values of sensor's status to spoken language
void check_stat(int stat) {
    if(stat == 0)
        Serial.println("Sensor configured successfully!");
    else if(stat == 1)
         Serial.println("Sensor is not ready.");
    else if(stat == -1)
        Serial.println("Object detected.");
    else if(stat == -2)
        Serial.println("Too many device's vibration.");
    else if(stat == -3)
        Serial.println("No objects detected.");
    else if(stat == -4)
        Serial.println("Pressing too hard!");
    else if(stat == -5)
        Serial.println("Objects dectected not finger.");    
    else 
        Serial.println("Too many finger's vibration.");
}

// Function setting-up the wires for sensors and Arduino communication
void setup(){
    Serial.begin(115200);

    Wire.begin();
    int result = bioHub.begin();
    if (result == 0) //Zero errors!
        Serial.println("Sensor started!");
    else
        Serial.println("Could not communicate with the sensor!!!");
  
    Serial.println("Configuring Sensor...."); 
    int error = bioHub.configBpm(MODE_TWO); // Configuring just the BPM settings. 
    if(error == 0){ // Zero errors
        Serial.println("Sensor configured.");
    }
    else {
        Serial.println("Error configuring sensor.");
        Serial.print("Error: "); 
        Serial.println(error); 
    }

    Serial.println("Loading up the buffer with data....");
    delay(4000); 
}

// Function iteratively sensing biometrics
void loop(){
    // Load bio-data into "body" variable
    body = bioHub.readBpm();
    
    // Check sensor's status
    stat = body.extStatus;
    check_stat(stat);
    
    // Print heart rate and SpO2 level data
    Serial.print("Heartrate: ");
    Serial.println(body.heartRate);  
    Serial.print("Oxygen: ");
    Serial.println(body.oxygen); 
    
    // Delay between two consecutive measurements
    delay(250); 
}
