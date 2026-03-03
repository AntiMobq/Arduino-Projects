#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>

#define HARDWARE_TYPE MD_MAX72XX::FC16_HW 
#define MAX_DEVICES 4  
#define CS_PIN      10

MD_Parola P = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  lcd.init();
  lcd.backlight();
  
  P.begin();
  P.setIntensity(5); 
  P.displayClear();
}

void loop() {
  lcd.clear();
  lcd.print("MODO: SCROLL");
  P.displayText("SOCORROOOO", PA_CENTER, 60, 1000, PA_SCROLL_LEFT, PA_SCROLL_LEFT);
  while (!P.displayAnimate());

  lcd.clear();
  lcd.print("MODO: SLICE");
  P.displayText("FLASH", PA_CENTER, 13, 1000, PA_SLICE, PA_SLICE);
  while (!P.displayAnimate());

  lcd.clear();
  lcd.print("MODO: BLINDS");
  P.displayText("BUHH", PA_CENTER, 60, 1000, PA_BLINDS, PA_BLINDS);
  while (!P.displayAnimate());

  lcd.clear();
  lcd.print("MODO: WIPE");
  P.displayText("WIPE", PA_CENTER, 60, 1000, PA_WIPE, PA_WIPE);
  while (!P.displayAnimate());

  lcd.clear();
  lcd.print("MODO: MESH");
  P.displayText("MESH", PA_CENTER, 60, 1000, PA_MESH, PA_MESH);
  while (!P.displayAnimate());

  lcd.clear();
  lcd.print("REINICIANDO...");
  P.displayClear();
  delay(1000);
}