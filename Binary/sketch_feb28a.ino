  #include <Wire.h>

  uint16_t memoria = 0;

  void setup() {
    Wire.begin();
    Serial.begin(9600);
    for(int i = 2; i <= 9; i++) pinMode(i, OUTPUT);
  }

  void loop() {
    if (Serial.available() > 0) {
      long valor = Serial.parseInt();
      if (valor >= 0) {
        memoria = (uint16_t)valor;
        
        for(int i = 0; i < 8; i++) {
          digitalWrite(i + 2, bitRead(memoria, i));
        }

        byte msb = highByte(memoria);
        Wire.beginTransmission(8);
        Wire.write(msb);
        Wire.endTransmission();
      }
      while(Serial.available()) Serial.read(); 
    }
  }