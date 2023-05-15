#include "Timer.h"

int a = 3;
int b = 2;
int c = 4;
int d = 5;
int e = 6;
int f = 7;
int g = 8;
int dp = 9;
int sw = 10;

int LEDS[7] {a, b, c, d, e, f, g};
int nums[11][7] {
  {1, 1, 1, 1, 1, 1, 0}, //0
  {0, 1, 1, 0, 0, 0, 0}, //1
  {1, 1, 0, 1, 1, 0, 1}, //2
  {1, 1, 1, 1, 0, 0, 1}, //3
  {0, 1, 1, 0, 0, 1, 1}, //4
  {1, 0, 1, 1, 0, 1, 1}, //5
  {1, 0, 1, 1, 1, 1, 1}, //6
  {1, 1, 1, 0, 0, 0, 0}, //7
  {1, 1, 1, 1, 1, 1, 1}, //8
  {1, 1, 1, 1, 0, 1, 1}, //9
  {0, 0, 0, 0, 0, 0, 0}, //off
};

void outNum(int x) { //outputs number x on the display
  for (int i = 0; i < 7; i++) {
    if (nums[x][i] == 1) {
      digitalWrite(LEDS[i], HIGH);
    }
    else {
      digitalWrite(LEDS[i], LOW);
    }
  }
}

int readStick() { //returns 1 if the joystick was up, 2 if it is down, 0 for anything else
  // you may have to read from A0 instead of A1 depending on how you orient the joystick
  if (analogRead(A0) > 800) {
    return 2; // down
  }
  else if (analogRead(A0) < 200) {
    return 1; // up
  }
  else {
    return 0;
  }

}

unsigned char cflag = 0; // count flag
unsigned int count = 0;  // display count
unsigned long tcnt = 0; // tick count
unsigned int speed[3] = {4,2,1}; // array of speeds
unsigned int val = 1; // value to determine speed

// write state machine here
enum states { INIT, WAIT_PRESS, COUNT, PAUSE, ADJUST_SPEED  } state;

void Tick() {
  int buttonPress = !digitalRead(sw);
  int stickDirection = readStick(); 
  // Transitions
  switch (state) {
    case INIT:
      state = WAIT_PRESS;
      break;

    case WAIT_PRESS:
      if (buttonPress) {
        state = COUNT;
      }
      break;

    case COUNT:
      if (buttonPress) {
        state = PAUSE;
      } else if (stickDirection != 0) {  // Use the stored joystick direction
        state = ADJUST_SPEED;
      }
      break;

    case PAUSE:
      if (buttonPress) {
        state = COUNT;
      }
      break;

    case ADJUST_SPEED:
      if (stickDirection == 0) {  // If joystick is in neutral position, go back to COUNT state
        state = COUNT;
      }
      break;

    default:
      state = INIT;
      break;      
  }

  // State Actions
  switch (state) {
    case INIT:
      Serial.println("INIT");
      outNum(0);
      break;

    case WAIT_PRESS:
      Serial.println("WAIT_PRESS");
      break;

    case COUNT:
      Serial.println("COUNT");
      tcnt++;
      if (tcnt >= speed[val]) {
        tcnt = 0;
        count = (count + 1) % 10;
        outNum(count);
      }
      break;

    case PAUSE:
      Serial.println("PAUSE");
      break;

    case ADJUST_SPEED:
      Serial.println("ADJUST_SPEED");
      if (stickDirection == 1 && val < 2) { // Up
        val++;
      } else if (stickDirection == 2 && val > 0) { // Down
        val--;
      }
      break;

    default:
      Serial.println("default");
      break;      
  }
}



void setup() {
  // put your setup code here, to run once:
  for(int i = 0; i < 7; i++) {
    pinMode(LEDS[i], OUTPUT);
  }
  pinMode(A1, INPUT);
  pinMode(sw, INPUT_PULLUP);
  TimerSet(500);
  TimerOn();
  Serial.begin(9600);
  state = INIT; 
  outNum(0);
}

void loop() {
  // put your main code here, to run repeatedly:
  // call tick function here
  Tick();
  while (!TimerFlag){}  // Wait for BL's period
  TimerFlag = 0;  
}

