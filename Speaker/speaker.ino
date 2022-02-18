// Configure pin 3 for speaker 
#define SPEAKER 3

// Function initiating speaker
void pinInit() {
    pinMode(SPEAKER, OUTPUT);
    digitalWrite(SPEAKER, LOW);
}

// Initailize speaker when Arduino runs
void setup() {
    pinInit();
}

// Generate the sound iteratively
void loop() {
    tone(SPEAKER, 100, 500);
}
