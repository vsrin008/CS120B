//Video: https://www.youtube.com/watch?v=c3m_Jz0msq4
int pPin = A0;
int LEDs[4] = {2, 3, 4, 5}; 

void setup() {
  for (int i = 0; i < 4; i++) {
    pinMode(LEDs[i], OUTPUT);
  }
}

void loop() {
  int pVal = analogRead(pPin);
  int LEDVAL = map(pVal, 0, 950, 0, 15);
  
  for (int i = 0; i < 4; i++) {
    digitalWrite(LEDs[i], bitRead(LEDVAL, i));
  }
}
