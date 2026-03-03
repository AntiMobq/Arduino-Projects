#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 4
#define CS_PIN 10

MD_Parola P = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);
MD_MAX72XX *mx;
LiquidCrystal_I2C lcd(0x27, 16, 2); 

int p1Y = 2, p2Y = 5;
const int pX = 31; 
int score1 = 0, score2 = 0;
int maxScore = 25; 

#define MAX_BULLETS 6
int bX[MAX_BULLETS], bY[MAX_BULLETS], bOwner[MAX_BULLETS];
bool bActive[MAX_BULLETS];

#define MAX_ENEMIES 4
int eX[MAX_ENEMIES], eY[MAX_ENEMIES];
bool eActive[MAX_ENEMIES];
unsigned long lastEnemyMove = 0;
int enemySpeed = 110;

const char* effects[] = {"OUCH!", "UGH!", "WAIT!", "OPS!"};

void updateLCD() {
  lcd.setCursor(0, 0);
  lcd.print("P1: "); lcd.print(score1);
  lcd.print("    "); 
  lcd.setCursor(0, 1);
  lcd.print("P2: "); lcd.print(score2);
  lcd.print("    ");
}

void screenFlash() {
  const char* msg = effects[random(0, 4)];
  P.setTextAlignment(PA_CENTER);
  for(int i = 0; i < 3; i++) {
    P.print(msg); 
    mx->control(MD_MAX72XX::INTENSITY, 15);
    delay(40);
    mx->control(MD_MAX72XX::INTENSITY, 0);
    delay(30);
  }
  mx->control(MD_MAX72XX::INTENSITY, 5);
  P.displayClear();
  updateLCD();
}

void resetGame(String winner) {
  P.displayClear();
  P.setTextAlignment(PA_CENTER);
  P.print(winner);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("FIM DE JOGO");
  lcd.setCursor(0, 1);
  lcd.print(winner);
  delay(5000);
  score1 = 0; score2 = 0;
  for(int i=0; i<MAX_ENEMIES; i++) eActive[i] = false;
  for(int i=0; i<MAX_BULLETS; i++) bActive[i] = false;
  lcd.clear();
  updateLCD();
}

void setup() {
  pinMode(2, INPUT_PULLUP); pinMode(4, INPUT_PULLUP); pinMode(3, INPUT_PULLUP); 
  pinMode(7, INPUT_PULLUP); pinMode(9, INPUT_PULLUP); pinMode(6, INPUT_PULLUP); 
  P.begin();
  mx = P.getGraphicObject();
  mx->control(MD_MAX72XX::INTENSITY, 5);
  P.setTextAlignment(PA_CENTER);
  lcd.init();
  lcd.backlight();
  updateLCD();
  randomSeed(analogRead(0));
}

void loop() {
  mx->clear();
  if (digitalRead(2) == LOW && p1Y > 0) p1Y--;
  if (digitalRead(4) == LOW && p1Y < 7) p1Y++;
  if (digitalRead(7) == LOW && p2Y > 0) p2Y--;
  if (digitalRead(9) == LOW && p2Y < 7) p2Y++;
  if (digitalRead(3) == LOW) {
    for(int i=0; i<MAX_BULLETS; i++) {
      if(!bActive[i]) { bX[i] = pX - 1; bY[i] = p1Y; bOwner[i] = 1; bActive[i] = true; break; }
    }
  }
  if (digitalRead(6) == LOW) {
    for(int i=0; i<MAX_BULLETS; i++) {
      if(!bActive[i]) { bX[i] = pX - 1; bY[i] = p2Y; bOwner[i] = 2; bActive[i] = true; break; }
    }
  }
  for(int i=0; i<MAX_BULLETS; i++) {
    if(bActive[i]) {
      bX[i]--;
      if(bX[i] < 0) { bActive[i] = false; continue; }
      for(int j=0; j<MAX_ENEMIES; j++) {
        if(eActive[j] && bX[i] <= eX[j] && bY[i] == eY[j]) {
          eActive[j] = false;
          bActive[i] = false;
          if(bOwner[i] == 1) score1 += 2; else score2 += 2;
          updateLCD();
        }
      }
      if(bActive[i]) mx->setPoint(bY[i], bX[i], true);
    }
  }
  if (millis() - lastEnemyMove > enemySpeed) {
    lastEnemyMove = millis();
    for(int i=0; i<MAX_ENEMIES; i++) {
      if(eActive[i]) {
        eX[i]++;
        if(eX[i] > 31) { 
           eActive[i] = false;
           score1 = (score1 >= 10) ? score1 - 10 : 0;
           score2 = (score2 >= 10) ? score2 - 10 : 0;
           screenFlash(); 
        }
      } else if(random(0, 100) > 96) {
        eX[i] = 0; eY[i] = random(0, 8);
        eActive[i] = true;
      }
    }
  }
  mx->setPoint(p1Y, pX, true);
  mx->setPoint(p2Y, pX, true);
  for(int i=0; i<MAX_ENEMIES; i++) if(eActive[i]) mx->setPoint(eY[i], eX[i], true);
  if(score1 >= maxScore) resetGame("P1 WIN");
  if(score2 >= maxScore) resetGame("P2 WIN");
  delay(35);
}