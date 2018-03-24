/*  
 *   Play pong on an 8x8 matrix - project from itopen.it
 */
 
#include "Timer.h"
 
#define POTPIN A5 // Potentiometer
#define PADSIZE 3
#define BALL_DELAY 400
#define GAME_DELAY 10
#define BOUNCE_VERTICAL 1
#define BOUNCE_HORIZONTAL -1
#define NEW_GAME_ANIMATION_SPEED 15
#define HIT_NONE 0
#define HIT_CENTER 1
#define HIT_LEFT 2
#define HIT_RIGHT 3
 
//#define DEBUG 1

int clockPin = 10;
int latchPin = 9;
int dataPin = 8;

int clockPinx = 13;
int latchPinx = 12;
int dataPinx = 11;

Timer timer;
 
byte ledStates[8] = {B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111};
byte GroundLEDs[8] = {B01111111, B10111111, B11011111, B11101111, B11110111, B11111011, B11111101, B11111110};

const byte rightUpPin = 7, leftDownPin = 4, rightDownPin = 6, leftUpPin = 5, mspmPin = A1;
const byte rightInterrupt = 2, leftInterrupt = 3;
static unsigned long last_interrupt_time_right = 0;
static unsigned long last_interrupt_time_left = 0;

byte direction; // Wind rose, 0 is north
int xball;
int yball;
int yball_prev;
byte rightpad;
byte leftpad;
int ball_timer;

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
    //int del = map (0, 0, 1023, 0, 100);
    //delay(del);
    delayMicroseconds(10);
    digitalWrite(latchPin, LOW);
    shiftOut(dataPin, clockPin, LSBFIRST, B11111111);  
    digitalWrite(latchPin, HIGH);
    delayMicroseconds(10);
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
 
void newGame() {
    //lc.clearDisplay(0);
    setLedStates(LOW);
    refresh();
    // initial position
    xball = random(1, 7);
    yball = 2;
    direction = random(3, 6); // Go south
    for(int r = 0; r < 8; r++){
        for(int c = 0; c < 8; c++){
            //lc.setLed(0, r, c, HIGH);
            bitSet(ledStates[r], c);
            refresh();
            delay(NEW_GAME_ANIMATION_SPEED);
        }
    }
    
    delay(1500);
    //lc.clearDisplay(0);
    setLedStates(LOW);
    refresh();
}

void setRightPad() {
    rightpad = random(0,5);
}
void setLeftPad() {
    leftpad = random(0,5);
}
 
void debug(const char* desc){
#ifdef DEBUG
    Serial.print(desc);
    Serial.print(" XY: ");
    Serial.print(xball);
    Serial.print(", ");
    Serial.print(yball);
    Serial.print(" rightpad: ");
    Serial.print(rightpad);
    Serial.print(" DIR: ");
    Serial.println(direction);
#endif
}
 
int checkBounce() {
    if(!xball || yball == 1 || xball == 7 || yball == 6){
        int bounce = (yball == 1 || yball == 6) ? BOUNCE_HORIZONTAL : BOUNCE_VERTICAL;
#ifdef DEBUG
        debug(bounce == BOUNCE_HORIZONTAL ? "HORIZONTAL" : "VERTICAL");
#endif
        return bounce;
    }
    return 0;
}
 
int getHit() {
    if(2 < direction && direction < 6) {
      if(yball != 6 || xball < rightpad || xball > rightpad + PADSIZE - 1){
        return HIT_NONE;
      }
      if(xball == rightpad + (PADSIZE - 1) / 2){
          return HIT_CENTER;
      }
      return xball < rightpad + (PADSIZE - 1) / 2 ? HIT_LEFT : HIT_RIGHT;
    }
    else if(direction != 6 && direction != 2) {
      if(yball != 1 || xball < leftpad || xball > leftpad + PADSIZE - 1){
        return HIT_NONE;
      }
      if(xball == leftpad + (PADSIZE - 1) / 2){
        return HIT_CENTER;
      }
      return xball < leftpad + (PADSIZE - 1) / 2 ? HIT_LEFT : HIT_RIGHT;
    }
}
 
bool checkLoose() {//if(2 < direction && direction < 6) {
    return ((yball == 6 && 2 < direction && direction < 6) || (yball == 1 && direction != 6 && direction != 2)) && getHit() == HIT_NONE;
}
 
void moveBall() {
    debug("MOVE");
    int bounce = checkBounce();
    int hit = getHit();
    int up = (2 < direction && direction < 6);
    if(bounce) {
        switch(direction){
            case 0:
                direction = 4;
            break;
            case 1:
                direction = (bounce == BOUNCE_VERTICAL) ? 7 : 3;
            break;
            case 2:
                direction = 6;
            break;
            case 6:
                direction = 2;
            break;
            case 7:
                direction = (bounce == BOUNCE_VERTICAL) ? 1 : 5;
            break;
            case 5:
                direction = (bounce == BOUNCE_VERTICAL) ? 3 : 7;
            break;
            case 3:
                direction = (bounce == BOUNCE_VERTICAL) ? 5 : 1;
            break;
            case 4:
                direction = 0;
            break;
        }
        debug("->");
    }
 
    // Check hit: modify direction is left or right
    if(up) {
      switch(hit){
        case HIT_LEFT:
            if(direction == 0){
                direction =  7;
            } else if (direction == 1){
                direction = 0;
            }
        break;
        case HIT_RIGHT:
            Serial.println("right");
            Serial.println(direction);
            Serial.println();
            if(direction == 0){
                direction = 1;
            } else if(direction == 7){
                direction = 0;
            }
        break;
      }
    }
    else {
      switch(hit){
        case HIT_LEFT:
            Serial.println("left");
            Serial.println(direction);
            Serial.println();
            if(direction == 4){
                direction =  5;
            } else if (direction == 3){
                direction = 4;
            }
        break;
        case HIT_RIGHT:
            Serial.println("right");
            Serial.println(direction);
            Serial.println();
            if(direction == 4){
                direction = 3;
            } else if(direction == 5){
                direction = 4;
            }
        break;
      }
    }
 
    // Check orthogonal directions and borders ...
    if((direction == 0 && xball == 0) || (direction == 4 && xball == 7)){
        direction++;
    }
    if(direction == 0 && xball == 7){
        direction = 7;
    }
    if(direction == 4 && xball == 0){
        direction = 3;
    }
    if(direction == 2 && yball == 0){
        direction = 3;
    }
    if(direction == 2 && yball == 6){
        direction = 1;
    }
    if(direction == 6 && yball == 0){
        direction = 5;
    }
    if(direction == 6 && yball == 6){
        direction = 7;
    }
    
    // "Corner" case
    if(xball == 0 && yball == 0){
        direction = 3;
    }
    if(xball == 0 && yball == 6){
        direction = 1;
    }
    if(xball == 7 && yball == 6){
        direction = 7;
    }
    if(xball == 7 && yball == 0){
        direction = 5;
    }
 
    yball_prev = yball;
    if(2 < direction && direction < 6) {
        yball++;
    } else if(direction != 6 && direction != 2) {
        yball--;
    }
    if(0 < direction && direction < 4) {
        xball++;
    } else if(direction != 0 && direction != 4) {
        xball--;
    }
    xball = max(0, min(7, xball));
    yball = max(0, min(6, yball));
    debug("AFTER MOVE");
}
 
void gameOver() {
    //setLedStates(HIGH);
    //delay(GAME_DELAY);
    long cur_time = millis();
    while(cur_time + BALL_DELAY > millis()) {
      refresh();
      delayMicroseconds(GAME_DELAY);
    }
    delay(1500);
    //lc.clearDisplay(0);
    setLedStates(LOW);
    refresh();
}
 
void drawGame() {
    if(yball_prev != yball){
        //lc.setRow(0, yball_prev, 0);
        ledStates[yball_prev] = B00000000;
    }
    ledStates[yball] = byte(1 << (xball));
    //lc.setRow(0, yball, byte(1 << (xball)));
    byte padmapRight = byte(0xFF >> (8 - PADSIZE) << rightpad);
    byte padmapLeft = byte(0xFF >> (8 - PADSIZE) << leftpad);
#ifdef DEBUG
    //Serial.println(padmap, BIN);
#endif
    //lc.setRow(0, 7, padmap);
    ledStates[7] = padmapRight;
    ledStates[0] = padmapLeft;
    refresh();
}
 
void setup() {
  // The MAX72XX is in power-saving mode on startup,
  // we have to do a wakeup call
  //pinMode(POTPIN, INPUT);
  setRightPad();
  setLeftPad();
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
  pinMode(rightUpPin, INPUT);
  pinMode(rightDownPin, INPUT);
  pinMode(leftUpPin, INPUT);
  pinMode(leftDownPin, INPUT);
  pinMode(leftInterrupt, INPUT);
  pinMode(rightInterrupt, INPUT);

  // enable pull-up resistor
  /*digitalWrite(rightUpPin, LOW);
  digitalWrite(rightDownPin, LOW);
  digitalWrite(leftUpPin, LOW);
  digitalWrite(leftDownPin, LOW);
  digitalWrite(leftInterrupt, LOW);
  digitalWrite(rightInterrupt, LOW);*/

  setLedStates(LOW);
  attachInterrupt(digitalPinToInterrupt(rightInterrupt), rightInterruptFn, RISING);
  attachInterrupt(digitalPinToInterrupt(leftInterrupt), leftInterruptFn, RISING);

  //lc.shutdown(0,false);
  // Set the brightness to a medium values
  //lc.setIntensity(0, 8);
  // and clear the display
  //lc.clearDisplay(0);
  randomSeed(analogRead(0));
#ifdef DEBUG
  Serial.begin(9600);
  Serial.println("Pong");
#endif
  newGame();
  ball_timer = timer.every(BALL_DELAY, moveBall);
}
 
void loop() {
    timer.update();
    // Move pad
    
#ifdef DEBUG
    Serial.println(rightpad);
#endif
    // Update screen
    drawGame();
    if(checkLoose()) {
        debug("LOOSE");
        gameOver();
        newGame();
    }
    delay(GAME_DELAY);
}

void rightInterruptFn() {
  
  unsigned long interrupt_time_right = millis();
  // If interrupts come faster than 200ms, assume it's a bounce and ignore
  if (interrupt_time_right - last_interrupt_time_right > 200) 
  {
    if(digitalRead(rightUpPin) == HIGH) {
      if(rightpad < 5) {
        rightpad++;
      }
    }
    else if(digitalRead(rightDownPin) == HIGH) {
      if(rightpad > 0) {
        rightpad--;
      }
    }
  }
  last_interrupt_time_right = interrupt_time_right;
}

void rightDown() {
  unsigned long interrupt_time_right = millis();
  // If interrupts come faster than 200ms, assume it's a bounce and ignore
  if (interrupt_time_right - last_interrupt_time_right > 200) 
  {
    if(rightpad > 0) {
      rightpad--;
    }
  }
  last_interrupt_time_right = interrupt_time_right; 

  Serial.println("RIGHTDOWN" + leftpad);
}

void leftInterruptFn() {
  unsigned long interrupt_time_left = millis();
  // If interrupts come faster than 200ms, assume it's a bounce and ignore
  if (interrupt_time_left - last_interrupt_time_left > 100) 
  {
    if(digitalRead(leftUpPin) == HIGH) {
      if(leftpad < 5) {
        leftpad++;
      }
    }
    else if(digitalRead(leftDownPin) == HIGH) {
      if(leftpad > 0) {
        leftpad--;
      }
    }
  }
  last_interrupt_time_left = interrupt_time_left;
}

void leftDown() {
  unsigned long interrupt_time_left = millis();
  // If interrupts come faster than 200ms, assume it's a bounce and ignore
  if (interrupt_time_left - last_interrupt_time_left > 100) 
  {
    if(leftpad >= 0) {
      leftpad--;
    }
  }
  last_interrupt_time_left = interrupt_time_left; 
  Serial.println("LEFTDOWN" + leftpad);
}

