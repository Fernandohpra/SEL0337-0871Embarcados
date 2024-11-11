#include <Wire.h>
int pot_pin = A0;
int n = 0;
void setup () {
  Serial.begin(9600);
  Wire.onRequest(requestEvent);

}
void requestEvent () {
  n = analogRead(pot_pin);
  Serial.println(n);
  Wire.write(highByte(n));
  Wire.write(lowByte(n));
}
void receiveEvent(int number) {
}
void loop() { // aguarda 100 ms antes de continuar a execução em loop
delay(100);
}
//