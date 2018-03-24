int clockPin = 10;
int latchPin = 9;
int dataPin = 8;

int clockPinx = 13;
int latchPinx = 12;
int dataPinx = 11;

int potPin = 2;
int val = 0;
int mspm = 1000;
long lastMove = millis();

byte charBlank[8] = {B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000};

byte heart0[8] = {B00000000, B00000000, B00000000, B00000000, B00011000, B00011000, B00000000, B00000000};

byte heart1[8] = {B00000000, B00000000, B00000000, B00011000, B00111100, B00111100, B00011000, B00000000};

byte heart2[8] = {B00000000, B00000000, B01100110, B11111111, B11111111, B01111110, B00111100, B00011000};

byte heart3[8] = {B00000000, B01100110, B11111111, B11111111, B01111110, B00111100, B00011000, B00000000};

byte heart4[8] = {B01100110, B11111111, B11111111, B01111110, B00111100, B00011000, B00000000, B00000000};

byte ledStates[8] = {B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111};
byte GroundLEDs[8] = {B01111111, B10111111, B11011111, B11101111, B11110111, B11111011, B11111101, B11111110};
int state = 1;
//byte ledStates[8] = {B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111};
//byte GroundLEDs [8] = {B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000};
void setup() {
  pinMode(latchPinx, OUTPUT);
  pinMode(clockPinx, OUTPUT);
  pinMode(dataPinx, OUTPUT);
  
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);

  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, LSBFIRST, B11111111);  
  digitalWrite(latchPin, HIGH);
  Serial.begin(9600);
}

void loop() {

  if (mspm + lastMove < millis()) { // time to move the snake
    lastMove = millis();
    change();
    state++;
  }
  refresh();
  
  
  //byte ledStates[8] = {B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111};
  //byte GroundLEDs [8] = {B01111111, B10111111, B11011111, B11101111, B11110111, B11111011, B11111101, B11111110};
}

void change() {
  if(state == 1) {
    clearBoard();
  }
  else if (state == 2) {
    setGround();
    byte setList[8] = {B00000000, B00000000, B00000000, B00001100, B00001100, B00000000, B00000000, B00000000};
    setByteArray(ledStates, setList);
  }
  else if (state == 3) {
    setGround();
    byte setList[8] = {B00000000, B00000000, B00001100, B00011110, B00011110, B00001100, B00000000, B00000000};
    setByteArray(ledStates, setList);
  }
  else if (state == 4) {
    setGround();
    byte setList[8] = {B00011000, B00111100, B00111110, B00011111, B00011111, B00111110, B00111100, B00011000};
    setByteArray(ledStates, setList);
  }
  else if (state == 5) {
    setGround();
    byte setList[8] = {B00110000, B01111000, B01111100, B00111110, B00111110, B01111100, B01111000, B00110000};
    setByteArray(ledStates, setList);
  }
  else if (state == 6) {
    setGround();
    byte setList[8] = {B01100000, B11110000, B11111000, B01111100, B01111100, B11111000, B11110000, B01100000};
    setByteArray(ledStates, setList);
  }
  else {
    fillBoard();
    state = 0;
  }
}

void setByteArray(byte states[], byte setters[]) {
  for(int i=0;i<8;i++){
    states[i] = setters[i];
  }
}

void setGround() {
  for(int i=0;i<8;i++){
    for(int j=0;j<8;j++){
      if(i == j) {
        bitClear(GroundLEDs[i],j);
      }
      else {
        bitSet(GroundLEDs[i],j);
      }
    }
  }
}



void clearBoard() {
  for(int i=0;i<8;i++){
    for(int j=0;j<8;j++){
      bitClear(ledStates[i],j);
      bitClear(GroundLEDs[i],j);
    }
  }
}

void fillBoard() {
  for(int i=0;i<8;i++){
    for(int j=0;j<8;j++){
      bitSet(ledStates[i],j);
      if(i == j) {
        bitClear(GroundLEDs[i],j);
      }
      else {
        bitSet(GroundLEDs[i],j);
      }
    }
  }
}

void refresh() {
  for(int i=0;i<8;i++){
    SetStates(ledStates[i]);
    GroundCorrectLED (GroundLEDs[i]);
    //val = analogRead(potPin);
    val = 0;
    int del = map (val, 0, 1023, 0, 100);
    delay(del);
    digitalWrite(latchPin, LOW);
    shiftOut(dataPin, clockPin, LSBFIRST, B11111111);  
    digitalWrite(latchPin, HIGH);
    delay(del);
  }
}

void GroundCorrectLED (byte states){
  
    digitalWrite(latchPin, LOW);
    shiftOut(dataPin, clockPin, LSBFIRST, states);  
    digitalWrite(latchPin, HIGH);
    
}

void SetStates (byte statesx){
       
    digitalWrite(latchPinx, LOW);
    shiftOut(dataPinx, clockPinx, LSBFIRST, statesx);  
    digitalWrite(latchPinx, HIGH);
    
}
     

