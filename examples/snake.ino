int clockPin = 10;
int latchPin = 9;
int dataPin = 8;

int clockPinx = 13;
int latchPinx = 12;
int dataPinx = 11;

int potPin = 2;
int val = 0;
//int mspm = 1000;
//int lastMove = millis();

class Coord {
public:
  Coord () {
    x = 255;
    y = 255; 
  }
  Coord (byte pX, byte pY) {
    x = pX;
    y = pY; 
  }
  byte x;
  byte y;
};

const byte topDirPin = 7, rightDirPin = 4, bottomDirPin = 6, leftDirPin = 5, mspmPin = A1;

byte snakeDirection = 0, snakeLength = 1; // 0 = up, 1 = right, 2 = bottom, 3 = left
Coord snakePosition (0,7), foodPosition;
Coord * snakeSegments = new Coord[64];
int totalRefreshes = 0;
long mspm = 1000, lastMove = 0; // milliseconds per move
boolean gameRunning = true; 

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

  // set direction pins as input
  pinMode(topDirPin, INPUT);
  pinMode(rightDirPin, INPUT);
  pinMode(bottomDirPin, INPUT);
  pinMode(leftDirPin, INPUT);

  // enable pull-up resistor
  digitalWrite(topDirPin, LOW);
  digitalWrite(rightDirPin, LOW);
  digitalWrite(bottomDirPin, LOW);
  digitalWrite(leftDirPin, LOW);

  randomSeed(analogRead(0));
  
  snakeSegments[0].x = snakePosition.x;
  snakeSegments[0].y = snakePosition.y;

  setLedStates(LOW); 

  spawnFood();
}

void loop() {
  if (digitalRead(topDirPin) == HIGH && digitalRead(rightDirPin) == LOW && digitalRead(bottomDirPin) == LOW && digitalRead(leftDirPin) == LOW && snakeDirection != 2)
    snakeDirection = 0;
  else if (digitalRead(topDirPin) == LOW && digitalRead(rightDirPin) == HIGH && digitalRead(bottomDirPin) == LOW && digitalRead(leftDirPin) == LOW && snakeDirection != 3)
    snakeDirection = 1;
  else if (digitalRead(topDirPin) == LOW && digitalRead(rightDirPin) == LOW && digitalRead(bottomDirPin) == HIGH && digitalRead(leftDirPin) == LOW && snakeDirection != 0)
    snakeDirection = 2;
  else if (digitalRead(topDirPin) == LOW && digitalRead(rightDirPin) == LOW && digitalRead(bottomDirPin) == LOW && digitalRead(leftDirPin) == HIGH && snakeDirection != 1)
    snakeDirection = 3;


  if (mspm + lastMove < millis()) { // time to move the snake
    lastMove = millis();
    //change();
    //state++;
    //mspm = analogRead(mspmPin);
    totalRefreshes++;

    if (snakeDirection == 0)
      snakePosition.y++;
    else if (snakeDirection == 1)
      snakePosition.x++;
    else if (snakeDirection == 2)
      snakePosition.y--;
    else if (snakeDirection == 3)
      snakePosition.x--;

    // boundary conditions
    if (snakePosition.x == 255)
      snakePosition.x = 7;
    if (snakePosition.x == 8)
      snakePosition.x = 0;
    if (snakePosition.y == 255)
      snakePosition.y = 7;
    if (snakePosition.y == 8)
      snakePosition.y = 0;

    // check if snake eats itself
    if (segmentExists(snakePosition.x, snakePosition.y))
      gameRunning = false; 

    // check if snake eats food
    if (snakePosition.x != foodPosition.x || snakePosition.y != foodPosition.y) 
      removeLastSegment();
    else 
      spawnFood();

    addNewSegment();

    //zeroLedArray();
    setLedStates(LOW);
    refreshLedArray();
    

    Serial.println(String(snakeDirection) + " (" + snakePosition.x + "|" + snakePosition.y + ")");
  }

  if (gameRunning) {  
    /*// refresh led matrix
    for (int i = 0; i < 8; i++) {
      row.setAll(led[i]);
      col.set(i, LOW);
      delayMicroseconds(50);
      col.setAllHigh();
    }*/
    refresh();
    //delayMicroseconds(50);
    //clearBoard();
    //refresh();
  }

  // check if game is over
  if (!gameRunning) {
    clearBoard();
    byte snakeDirection = 0, snakeLength = 1; // 0 = up, 1 = right, 2 = bottom, 3 = left
    Coord snakePosition (0,7), foodPosition;
    Coord * snakeSegments = new Coord[64];
    int totalRefreshes = 0;
    long mspm = 1000, lastMove = 0; // milliseconds per move
    boolean gameRunning = true; 
    randomSeed(analogRead(0));
  
    snakeSegments[0].x = snakePosition.x;
    snakeSegments[0].y = snakePosition.y;

    setLedStates(LOW); 

    spawnFood();
    gameRunning = true;
  }
  
  
  
  //byte ledStates[8] = {B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111};
  //byte GroundLEDs [8] = {B01111111, B10111111, B11011111, B11101111, B11110111, B11111011, B11111101, B11111110};
}

void setLedStates(boolean power) {
  if(power == LOW) {
    byte setList[8] = {B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000};
    setByteArray(ledStates, setList);
  }
  else {
    byte setList[8] = {B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111};
    setByteArray(ledStates, setList);
  }
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

boolean segmentExists(byte x, byte y) {
  for (int i = 0; i < snakeLength; i++) 
    if (snakeSegments[i].x == x && snakeSegments[i].y == y)
      return true;
  return false;
}

void addNewSegment() {
  snakeSegments[snakeLength].x = snakePosition.x;
  snakeSegments[snakeLength].y = snakePosition.y;

  snakeLength++;
}

void removeLastSegment() {
  int i = -1;
  for (i = 1; i < snakeLength; i++) {
    snakeSegments[i-1].x = snakeSegments[i].x; 
    snakeSegments[i-1].y = snakeSegments[i].y;
  }
  if (i != -1) {
    snakeSegments[i].x = 255;
    snakeSegments[i].y = 255;
  }

  snakeLength--; 
}

void refreshLedArray() {
  int i = 0;
  while (snakeSegments[i].x != 255) {
    bitSet(ledStates[snakeSegments[i].x],snakeSegments[i].y);
    //led[snakeSegments[i].x][snakeSegments[i].y] = 1;
    i++;
  }

  bitSet(ledStates[foodPosition.x],foodPosition.y);
  //led[foodPosition.x][foodPosition.y] = 1;
}

/*void zeroLedArray() {
  for (int i = 0; i < 8; i++) 
    for (int j = 0; j < 8; j++) 
      led[i][j] = 0; 
}*/

void spawnFood() {
  byte x, y;

  // Yes! This is the first time I actually use a do {} while (); loop!
  do {
    x = random(8);
    y = random(8);
  } 
  while (segmentExists(x, y));
  foodPosition.x = x;
  foodPosition.y = y;
}
