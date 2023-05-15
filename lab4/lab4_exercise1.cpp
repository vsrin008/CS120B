#include "Timer.h"

const int roomLightPin = 3;
const int switchIndicatorPin = 4;
const int buttonPin = 5;
const int motionSensorPin = A0;

enum BL_States { BL_SMStart, BL_LedOff, BL_LedOn } BL_State;

unsigned int motionDetectedCounter = 0;
unsigned int buttonPressedCounter = 0;
bool buttonPressed = false;

void TickFct_Blink() {
  int buttonState = digitalRead(buttonPin);
  int motionSensorValue = analogRead(motionSensorPin);

  switch (BL_State) { // Transitions
    case BL_SMStart:
      BL_State = BL_LedOff; // Initial state
      break;
    case BL_LedOff:
      if (!buttonPressed && buttonState == HIGH) {
        buttonPressed = true;
        BL_State = BL_LedOn;
        buttonPressedCounter = 0;
      } else if (motionSensorValue < 900 && buttonPressedCounter >= 50) {
        BL_State = BL_LedOn;
      }
      break;
    case BL_LedOn:
      if (!buttonPressed && buttonState == HIGH) {
        buttonPressed = true;
        BL_State = BL_LedOff;
        buttonPressedCounter = 0;
        motionDetectedCounter = 0;
      } else if (motionDetectedCounter >= 100) { // 20 seconds (100 * 200 ms)
        BL_State = BL_LedOff;
        motionDetectedCounter = 0;
      }
      
      if (motionSensorValue < 900) {
        motionDetectedCounter = 0;
      } else {
        motionDetectedCounter++;
      }

      if (buttonState == HIGH) {
        buttonPressed = true;
      } else {
        buttonPressed = false;
      }
      break;
    default:
      BL_State = BL_SMStart;
      break;
  }

  switch (BL_State) { // State actions
    case BL_LedOff:
      Serial.print("OFF ");
      Serial.println(motionSensorValue);
        digitalWrite(roomLightPin, LOW);
        digitalWrite(switchIndicatorPin, HIGH);
        buttonPressedCounter++;
      break;
    case BL_LedOn:
      Serial.print("ON ");
      Serial.println(motionSensorValue);
      digitalWrite(roomLightPin, HIGH);
      digitalWrite(switchIndicatorPin, LOW);
      break;
    default:
      break;
  }
}

void setup() {
  pinMode(roomLightPin, OUTPUT);
  pinMode(switchIndicatorPin, OUTPUT);
  pinMode(buttonPin, INPUT);
  pinMode(motionSensorPin, INPUT);

  TimerSet(200); // Set the period to 200 ms
  TimerOn();
  Serial.begin(9600);
}

void loop() {
  TickFct_Blink();
  while (!TimerFlag) {}  // Wait for BL's period
  TimerFlag = 0;
}
