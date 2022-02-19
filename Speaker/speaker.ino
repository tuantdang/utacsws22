int iStart = 0;
int iEnd = 3;
int BassTab[]={1911, 1702, 1516, 1431, 1275, 1136, 1012};
 
void setup() {
    pinInit();
}

void loop() {
    for(int note_index = 0; note_index < 7; note_index++) {
        sound(note_index);
        delay(500);
    }
}
void pinInit() {
    for(int i = iStart; i <= iEnd; i++) {
       pinMode(i, INPUT);
    }
    //pinMode(SPEAKER,OUTPUT);

    for(int i = iStart; i <= iEnd; i++) {
       digitalWrite(i, LOW);
    }
    //digitalWrite(SPEAKER,LOW);
}

void sound(uint8_t note_index) {
    for(int i = 0; i < 100; i++) {
        for(int i = iStart; i <= iEnd; i++) {
           digitalWrite(i, HIGH);
        }
        //digitalWrite(SPEAKER,HIGH);
        delayMicroseconds(BassTab[note_index]);
        
        for(int i = iStart; i <= iEnd; i++) {
           digitalWrite(i, LOW);
        }
        //digitalWrite(SPEAKER,LOW);
        delayMicroseconds(BassTab[note_index]);
    }
}
