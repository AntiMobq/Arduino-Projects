#include <Wire.h>

void setup() {
  Wire.begin(8);
  Wire.onReceive(receber);
  for(int i = 2; i <= 9; i++) pinMode(i, OUTPUT);
}

void receber(int quantos) {
  if (Wire.available()) {
    byte dado = Wire.read();
    for(int i = 0; i < 8; i++) {
      digitalWrite(i + 2, bitRead(dado, i));
    }
  }
}

void loop() { delay(1); }