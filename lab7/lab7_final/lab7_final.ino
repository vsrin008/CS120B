#include "Timer.h"
#include "LiquidCrystal.h"
// Depending on the LiquidCrystal library you are able to install, it might be:
// #include "LiquidCrystal.h"
#include "pitches.h"

// Sound Variables  
int buzzer = 8;

// == Song 1 ==
int song1[] = {
NOTE_E4, NOTE_C5, NOTE_B1, NOTE_F3, NOTE_C4, 
NOTE_A4, NOTE_A4, NOTE_GS5, NOTE_C5, NOTE_CS4, 
NOTE_AS4, NOTE_C5, NOTE_DS4, NOTE_CS5, NOTE_GS4, 
NOTE_C3, NOTE_E3, NOTE_DS5, NOTE_D4, NOTE_D3, -1
};
int song1_time[] = {
2, 1, 2, 1, 1, 4, 8, 16, 8, 4, 4, 1, 8, 4, 2, 4, 4, 16, 4, 2, 1
};

// == Song 2 ==

int song2[] = {
  NOTE_FS5, NOTE_D2, NOTE_DS5, NOTE_G2, NOTE_B3, 
  NOTE_C2, NOTE_G5, NOTE_D6, NOTE_CS5, NOTE_AS4, 
  NOTE_DS6, NOTE_D3, NOTE_CS4, NOTE_E5, NOTE_DS6,
   NOTE_E4, NOTE_B4, NOTE_F4, NOTE_E6, NOTE_DS4, -1
};

int song2_time[] = {
  2, 2, 4, 8, 1, 8, 4, 4, 16, 8, 2, 4, 16, 8, 2, 4, 16, 4, 8, 1, 1
};

// == Song 3 == 

int song3[] = {
  NOTE_A5, NOTE_D4, NOTE_D6, NOTE_DS3, NOTE_G4, 
  NOTE_B2, NOTE_F2, NOTE_A3, NOTE_AS2, NOTE_B5, 
  NOTE_C6, NOTE_C3, NOTE_GS3, NOTE_G2, NOTE_FS5, 
  NOTE_AS4, NOTE_GS2, NOTE_CS3, NOTE_C3, NOTE_AS2, -1
};

int song3_time[] = {
  1, 2, 16, 4, 16, 2, 16, 1, 1, 2, 1, 8, 2, 16, 8, 1, 16, 4, 1, 2, 1
};


// LCD variables
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
// Depending on the LiquidCrystal library you are able to install, it might be:
// LiquidCrystal lcd(rs, en, d4, d5, d6, d7);



// Task Structure Definition
typedef struct task {
   int state;                  // Tasks current state
   unsigned long period;       // Task period
   unsigned long elapsedTime;  // Time elapsed since last task tick
   int (*TickFct)(int);        // Task tick function
} task;


const unsigned char tasksNum = 4;
task tasks[tasksNum]; // We have 4 tasks

// Task Periods

const unsigned long periodLCDOutput = 100;
const unsigned long periodJoystickInput = 100;
const unsigned long periodSoundOutput = 100;
const unsigned long periodController = 500;


// GCD 
const unsigned long tasksPeriodGCD = 100;

// Task Function Definitions
int TickFct_LCDOutput(int state);
int TickFct_JoystickInput(int state);
int TickFct_SoundOutput(int state);
int TickFct_Controller(int state);

// Task Enumeration Definitions
enum LO_States {LO_init, LO_MenuOptionA, LO_MenuOptionB};
enum JI_States {JI_init, JI_Sample};
enum SO_States {SO_init, SO_SoundOn, SO_SoundOff};
enum C_States {C_init, C_T1};



void TimerISR() { // TimerISR actually defined here
  unsigned char i;
  for (i = 0; i < tasksNum; ++i) { // Heart of the scheduler code
     if ( tasks[i].elapsedTime >= tasks[i].period ) { // Ready
        tasks[i].state = tasks[i].TickFct(tasks[i].state);
        tasks[i].elapsedTime = 0;
     }
     tasks[i].elapsedTime += tasksPeriodGCD;
  }
}


void LCDWriteLines(String line1, String line2, String line3, String line4) {
  lcd.clear();          
  lcd.setCursor(0, 0);
  lcd.print(line1);
  lcd.setCursor(0,1);
  lcd.print(line2);
  lcd.setCursor(8, 0);
  lcd.print(line3);
  lcd.setCursor(8, 1);
  lcd.print(line4);
}

// Task Function Definitions

int direction = 0;
int cursorPosition = 1;
int selectedSong = -1;
int sw = 10;
const unsigned long debounceDelay = 200; //200 ms debouncing
unsigned long lastDebounceTime = 0; 
unsigned long lastButtonPress = 0; 
int bPos = 1;
int paused = 1;
int songEnd = 0;





int readButton() { //returns 1 if the joystick button is pressed, 0 otherwise
  if (!digitalRead(sw)) { 
    Serial.println("PRESS");
    return 1; // button pressed
  }
  else {
    return 0; // button not pressed
  }
}

/// Task 1
int TickFct_LCDOutput(int state) {
  switch (state) { // State Transitions
    case LO_init:
      LCDWriteLines("Song 1", "Song 2", "Song 3", "Start");
      state = LO_MenuOptionA;
    break;
    case LO_MenuOptionA:
      if (selectedSong !=-1 && cursorPosition == 4 && readButton()) {
        state = LO_MenuOptionB;
        Serial.print("Going to menu B with selection of Song ");
        Serial.println(selectedSong);
        paused=0;
      if (selectedSong==1) {
        LCDWriteLines("Playing", "Pause", "Song 1", "Play");
      }
      if (selectedSong==2) {
        LCDWriteLines("Playing", "Pause", "Song 2", "Play");
      }

      if (selectedSong==3) {
        LCDWriteLines("Playing", "Pause", "Song 3", "Play");
      }

      }
    break;
    case LO_MenuOptionB:
      if (songEnd == 1) {
        songEnd=0;
        selectedSong = -1;
        LCDWriteLines("Song 1", "Song 2", "Song 3", "Start");
        state = LO_MenuOptionA;
      }
    break;
  }

  switch (state) { // State Actions
    case LO_init:
    break;
    case LO_MenuOptionA:
      if (readButton()) { // This will clear selection when a new song is chosen
        lcd.setCursor(6, 0);
        lcd.print(" "); 
        lcd.setCursor(6, 1);
        lcd.print(" "); 
        lcd.setCursor(7, 0);
        lcd.print(" "); 
      }
    
      if (cursorPosition == 1) {
        lcd.setCursor(6, 0); // Align with "Song 1"
        lcd.blink();
        if(readButton()) {
          lcd.print("*");
          selectedSong = 1;
          Serial.println("Song1 selected");
        }
      } else if (cursorPosition == 2) {
        lcd.setCursor(6, 1); // Align with "Song 2"
        lcd.blink();
        if(readButton()) {
          lcd.print("*");
          selectedSong = 2;
          Serial.println("Song2 selected");
        }
      } else if (cursorPosition == 3) {
        lcd.setCursor(7, 0); // Align with "Song 3"
        lcd.blink();
        if(readButton()) {
          lcd.print("*");
          selectedSong = 3;
          Serial.println("Song3 selected");
        }
      } else if (cursorPosition == 4) {
        lcd.setCursor(7, 1); // Align with "Start"
        lcd.blink();
        if(readButton() && selectedSong != -1) {
          paused = 0;
          selectedSong = -1;
        }
      }
    break;
    case LO_MenuOptionB:
      if (readButton()) { // This will clear selection when a new song is chosen
        lcd.setCursor(5, 1);
        lcd.print(" "); 
        lcd.setCursor(12, 1);
        lcd.print(" ");
      }

      if (bPos==1) {
        lcd.setCursor(5, 1); 
        lcd.blink();
        if(readButton()) {
          lcd.print("*"); 
          paused = 1;
          Serial.println("PAUSE");          
        }       
      }

      if (bPos==2) {
        lcd.setCursor(12, 1); 
        lcd.blink();
        if(readButton()) {
          lcd.print("*"); 
          paused = 0;
          Serial.println("UNPAUSE");
        }
        
      }
      

    break;
  }
  return state;
}


// Task 2
int TickFct_JoystickInput(int state) {
  switch (state) { // State Transitions
    case JI_init:
      state = JI_Sample;
    break;
    case JI_Sample:
    break;
  }

  switch (state) { // State Actions
    case JI_Sample:
      unsigned long currentTime = millis(); // get the current time

      // Check if debounce period has passed since last button press
      if (currentTime - lastButtonPress > debounceDelay) {
        int xinput = analogRead(A0);
        int yinput = analogRead(A1);
        if (xinput < 400) { //left
          cursorPosition = cursorPosition - 2;
          bPos = 1;
          Serial.println("Left");
          lastButtonPress = currentTime;
        }
        if (xinput > 600) { //right
          cursorPosition = cursorPosition + 2;
          bPos = 2;
          Serial.println("Right");
          lastButtonPress = currentTime;
        }
        if (yinput > 600) { //up
          cursorPosition--; // Moves cursor up
          Serial.println("Up");
          lastButtonPress = currentTime;
        }
        if (yinput < 400) { //down
          cursorPosition++; // Moves cursor down
          Serial.println("Down");
          lastButtonPress = currentTime;
        }
      }

      if (readButton()) {
        if (currentTime - lastDebounceTime > debounceDelay) {
          if (cursorPosition !=4) {
            selectedSong = cursorPosition;
          }
          lastDebounceTime = currentTime;
        }
      }      

      cursorPosition = constrain(cursorPosition, 1, 4); // Ensures cursor stays within menu options (1-4 for 4 options).
    break;
  }
  return state;
}


// Sound Output
int counter = 0;
int note = 0;
int TickFct_SoundOutput(int state) {
  switch (state) { // State Transitions
    case SO_init:
      state = SO_SoundOn;
    break;
    case SO_SoundOn:
      if (selectedSong != -1 && !paused) {
        if(song1[note] == -1 || song2[note] == -1 || song3[note] == -1) { // Song ended
            note = 0; // Reset note counter
            Serial.print("End of Song ");
            Serial.println(selectedSong);
            selectedSong = -1; // Deselect the song
            paused = 1; // Ensure pause state
            songEnd = 1;
            state = SO_init; // Go back to initial state
            break;
        }
        if(counter >= song1_time[note]) {
          state = SO_SoundOff;
          note++;
          counter = 0;
        }
      }
    break;
    case SO_SoundOff:
      state = SO_SoundOn;
    break;
    
  }
   switch (state) { // State Actions
    case SO_SoundOn:
      if (selectedSong != -1 && !paused) {
        switch (selectedSong) {
          case 1: 
            tone(buzzer, song1[note], periodSoundOutput * song1_time[note]);
            Serial.println("Playing song1");
            break;
          case 2:
            tone(buzzer, song2[note], periodSoundOutput * song2_time[note]);
            Serial.println("Playing song2");
            break;
          case 3:
            tone(buzzer, song3[note], periodSoundOutput * song3_time[note]);
            Serial.println("Playing song3");
            break;
        }
        counter++;
      }
    break;
    case SO_SoundOff:
      noTone(buzzer);
    break;
  }
  return state;
}

// Task 4 (Unused)
int TickFct_Controller(int state) {
  switch (state) { // State Transitions
    case 0:
    break;
  }

   switch (state) { // State Actions
    case 0:
    break;
  }
  return state;
}



void InitializeTasks() {
  unsigned char i=0;
  tasks[i].state = LO_init;
  tasks[i].period = periodLCDOutput;
  tasks[i].elapsedTime = tasks[i].period;
  tasks[i].TickFct = &TickFct_LCDOutput;
  ++i;
  tasks[i].state = JI_init;
  tasks[i].period = periodJoystickInput;
  tasks[i].elapsedTime = tasks[i].period;
  tasks[i].TickFct = &TickFct_JoystickInput;
  ++i;
  tasks[i].state = SO_init;
  tasks[i].period = periodSoundOutput;
  tasks[i].elapsedTime = tasks[i].period;
  tasks[i].TickFct = &TickFct_SoundOutput;
  ++i;
  tasks[i].state = C_init;
  tasks[i].period = periodController;
  tasks[i].elapsedTime = tasks[i].period;
  tasks[i].TickFct = &TickFct_Controller;

}

void setup() {
  // put your setup code here, to run once:
  InitializeTasks();
  pinMode(sw, INPUT_PULLUP);
  pinMode(buzzer, OUTPUT);
  TimerSet(tasksPeriodGCD);
  TimerOn();
  Serial.begin(9600);
  // Initialize Outputs
  lcd.begin(16, 2);
  // Initialize Inputs

}

void loop() {
  // put your main code here, to run repeatedly:
  // Task Scheduler with Timer.h
}