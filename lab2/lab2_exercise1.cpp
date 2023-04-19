//VIDEO: https://www.youtube.com/watch?v=lbn5jL3cWuc
int B[4] = {2, 3, 4, 5};
int x = 6;
int y = 7;

int num = 750;
void setup() {
  // put your setup code here, to run once:
  pinMode(x, INPUT);
  pinMode(y, INPUT_PULLUP);
  for (int i = 0; i < 4; i++) {
    pinMode(B[i], OUTPUT);
 }
  Serial.begin(9600);
}

void loop() {
  if (digitalRead(y)==LOW) {
    digitalWrite(B[0],HIGH);
    digitalWrite(B[1],HIGH);
    digitalWrite(B[2],HIGH);
    digitalWrite(B[3],HIGH);
  }

  if (digitalRead(x)==HIGH) {
    digitalWrite(B[0],LOW);
    digitalWrite(B[1],LOW);
    digitalWrite(B[2],LOW);
    digitalWrite(B[3],LOW);
  }

}