int red_pin = 9;
int blue_pin = 10;
int green_pin = 11;

//you will need to determine the value for dark
//its unique to the lighting in your room
int dark_threshold = 80;

bool isDark(){
  if (analogRead(A1)<dark_threshold) {
    return true;    
  }
  else {
    return false;
  }
}

void turnoff(){
  analogWrite(red_pin, 0);
  analogWrite(blue_pin, 0);
  analogWrite(green_pin, 0);
}

enum ButtonStates{INIT,off, on} current_state;
void SM0_Tick() {
   // State Transistions
   switch(current_state) {
       case INIT:
         current_state = off;
         break;
       case off:
        if (isDark()) {
         current_state = on;
        }
          break;
       case on:
         if (!isDark()) {
         current_state = off;
        }
         break;
       default:
         current_state = off;
         break;
   }
   // State Actions
   switch(current_state) {
   	case INIT:
      break;
    case off:
      turnoff();
      break;
    case on:
   //displays the color white
      if (analogRead(A0)<200){
        analogWrite(red_pin, 0);
        analogWrite(green_pin, 206);
        analogWrite(blue_pin, 209);
      }
      if (analogRead(A0)>=200 && analogRead(A0)<400){
        analogWrite(red_pin, 255);
        analogWrite(green_pin, 165);
        analogWrite(blue_pin, 0);
      }
      if (analogRead(A0)>=400 && analogRead(A0)<600){
        analogWrite(red_pin, 0);
        analogWrite(green_pin, 255);
        analogWrite(blue_pin, 0);
      }
      if (analogRead(A0)>=600 && analogRead(A0)<800){
        analogWrite(red_pin, 0);
        analogWrite(green_pin, 0);
        analogWrite(blue_pin, 255);
      }
      if (analogRead(A0)>=800){
        analogWrite(red_pin, 128);
        analogWrite(green_pin, 0);
        analogWrite(blue_pin, 0);
      }
      break;
   	default:
      break;
   }
}

void setup() {
  // put your setup code here, to run once:
  pinMode(red_pin, OUTPUT);
  pinMode(green_pin, OUTPUT);
  pinMode(blue_pin, OUTPUT);
  Serial.begin(9600);
  current_state = INIT;
}
void loop() {
  // put your main code here, to run repeatedly:
  SM0_Tick();
  //you can delete these prints if you want they are 
  //just here to show you the values of the 
  //potentiometer(A0) and the photoresistor(A1)
  Serial.print(analogRead(A0));
  Serial.print(' ');
  Serial.println(analogRead(A1));
  //DELETE THIS LINE OF CODE WHEN YOU MAKE YOUR SOLUTION
  delay(500); //DELETE ME!!!!!
  //PLEASE DONT FORGET TO DELETE THE DELAY FUNCTION
  //its only there so you can see the transition from states
  //and so that your serial monitor is not spammed with values
}