#include "Timer.h"

const int roomLightPin = 12;
const int switchIndicatorPin = 13;
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

void clearDisplay() {
  for (int i = 0; i < 7; i++) {
    digitalWrite(LEDS[i], LOW);
  }
}


int readStick() { //returns 1 if the joystick was up, 2 if it is down, 0 for anything else
  // you may have to read from A0 instead of A1 depending on how you orient the joystick
  if (analogRead(A1) > 800) {
    return 2; // down
  }
  else if (analogRead(A1) < 200) {
    return 1; // up
  }
  else {
    return 0;
  }

}

int readButton() { //returns 1 if the joystick button is pressed, 0 otherwise
  if (!digitalRead(sw)) {
    return 1; // button pressed
  }
  else {
    return 0; // button not pressed
  }
}



unsigned char cflag = 0; // count flag
unsigned int count = 0;  // display count
unsigned long tcnt = 0; // tick count
unsigned int speed[3] = {4,2,1}; // array of speeds
unsigned int val = 1; // value to determine speed
int count9 = 0;
int count5 = 0;
bool counting = 0;


// write state machine here
enum Light_States {RL_INIT, RL_OFF, RL_ON, RL_DISABLED} Light_State;
void LightTick() {
  switch (Light_State) { // Transitions
    case RL_INIT:
      Light_State = RL_OFF;
      break;

    case RL_OFF:
      if (readButton() || readStick()) {
        Light_State = RL_ON;
      }
      break;
    
    case RL_ON:
      if (readButton()) {
        Light_State = RL_DISABLED;
        count5 = 5;
      }
      else if (count9 == 0 && count5 == 0) {
        Light_State = RL_OFF;
      }

      if (counting == 1 && readButton()){
          Light_State = RL_OFF;
      }
      break;

    case RL_DISABLED:
      if (count5 == 0) {
        Light_State = RL_OFF;
      }
      break;

    default:
      Light_State = RL_OFF;
      break;      
  }


  switch (Light_State) {   // State Actions
    case RL_INIT:
      Serial.println("RL_INIT");
      break;

    case RL_OFF:
      Serial.println("RL_OFF");
      digitalWrite(roomLightPin, LOW);
      break;

    case RL_ON:
      Serial.println("RL_ON");
      digitalWrite(roomLightPin, HIGH);
      break;

    default:
      Serial.println("default");
      digitalWrite(roomLightPin, LOW);
      break;      
  }
}


enum Led_States {LED_INIT, LED_IDLE, LED_COUNTDOWN, LED_OFF, LED_FINAL_COUNTDOWN} Led_State;
int blinkCount = 0;
void LedTick() {
  blinkCount++;
  switch (Led_State) { // Transitions
    case LED_INIT:
      Led_State = LED_IDLE;
      break;

    case LED_IDLE:
      if (readButton() || readStick()!=0) {
        Led_State = LED_COUNTDOWN;
        count9 = 9; 
      }
      else if (!readButton() && blinkCount >= 10) {
        blinkCount = 0;
      }
      break;

    case LED_COUNTDOWN:
      if (readButton()) {
        Led_State = LED_OFF;
        count5 = 5; 
      }
      else if (count9 == 0) {
        Led_State = LED_IDLE;
      }
      else if (readStick() != 0) {
        count9 = 9; 
      }
      if (blinkCount >= 10) {
        blinkCount = 0;
        count9--; /
      }
      count = count9; 
      break;
    
    case LED_OFF:
      if (!readButton()) {
        Led_State = LED_FINAL_COUNTDOWN;
        count = 5;
      }
      break;

    case LED_FINAL_COUNTDOWN:
      if (count5 == 0) {
        Led_State = LED_IDLE;
      }
      if (blinkCount >= 10) {
        blinkCount = 0;
        count5--; 
      }
      count = count5; 
      break;

    default:
      Led_State = LED_IDLE;
      break;
  }


  switch (Led_State) {   
    case LED_INIT:
      Serial.println("LED_INIT");
      digitalWrite(switchIndicatorPin, blinkCount % 10 == 0);
      break;

    case LED_IDLE:
      Serial.println("LED_IDLE");
      digitalWrite(switchIndicatorPin, blinkCount % 10 == 0);
      clearDisplay();
      break;

    case LED_COUNTDOWN:
      Serial.println("LED_COUNTDOWN");
      counting = 1;
      outNum(count9);
      break;

    case LED_OFF:
      Serial.println("LED_OFF");
      clearDisplay();
      digitalWrite(switchIndicatorPin, LOW);
      break;

    case LED_FINAL_COUNTDOWN:
      Serial.println("FINAL_COUNTDOWN");
      outNum(count5);
      digitalWrite(switchIndicatorPin, LOW);
      break;

    default:
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
  TimerSet(100);
  TimerOn();
  Serial.begin(9600);
}


void loop() {
  // put your main code here, to run repeatedly:
  // call tick function here
  LightTick();
  LedTick();
  while (!TimerFlag){}  // Wait for BL's period
  TimerFlag = 0;  
}

