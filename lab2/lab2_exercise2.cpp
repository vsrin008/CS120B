//Video: https://www.youtube.com/watch?v=CeH0hLxk3aU

int B[4] = {2, 3, 4, 5};
int x = 6;
int y = 7;
char Patterns[9] = {0x00, 0x08, 0x0C, 0x0E, 0x0F, 0x0E, 0x0C, 0x08, 0x00};
int pattern_index = 0;


void clearLEDs() {
  for(int i = 0; i < 4; i++) {
     digitalWrite(B[i], LOW);
  }
}

void writeLEDs(char pattern) {
  for(int i = 0; i < 4; i++) {
     digitalWrite(B[i], (pattern & (1 << i)) > 0);
  }
}


enum states {START, IDLE, X_PRESSED, Y_PRESSED, X_RELEASED, Y_RELEASED} state;

void setup() {
  pinMode(x, INPUT);
  pinMode(y, INPUT_PULLUP);
  for (int i = 0; i < 4; i++) {
    pinMode(B[i], OUTPUT);
 }
  Serial.begin(9600);
}

void Tick() {
  switch(state) { //transitions
    case START:
      state = IDLE;
      break;
    
    case IDLE:
      if (digitalRead(x)==HIGH) {
        state = X_PRESSED;
      }
      else if (digitalRead(y)==LOW) {
        state = Y_PRESSED;
      }
      break;

    case X_PRESSED:
      if (digitalRead(x)==LOW) {
        state = X_RELEASED;
      }
      break;

    case Y_PRESSED:
      if (digitalRead(y)==HIGH) {
        state = Y_RELEASED;
      }
      break;

    case X_RELEASED:
      state=IDLE;
      break;
    
    case Y_RELEASED:
      state=IDLE;
      break;
  }

  switch(state) { //state actions
    case START:
      Serial.print("START");
      break;

    case IDLE:
      Serial.print("IDLE");
      break;

    case X_PRESSED:
      Serial.print("X_PRESSED");
      break;

    case Y_PRESSED:
      Serial.print("Y_PRESSED");
      break;

    case X_RELEASED:
      Serial.print("X_RELEASED");
      pattern_index++;
      if (pattern_index > 7) {
        pattern_index = 7;
      }
      clearLEDs();
      writeLEDs(Patterns[pattern_index]);
      break;

    case Y_RELEASED:
      Serial.print("Y_RELEASED");
      pattern_index--;
      if (pattern_index < 0) {
        pattern_index = 0;
      }
      clearLEDs();
      writeLEDs(Patterns[pattern_index]);
      break;


  }
}


void loop() {
  Serial.print("MAIN");
   state = START; // Indicates initial call
   while(1) {
      Tick();
   }
}



