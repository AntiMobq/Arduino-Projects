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
int maxScore = 40;

// variaveis do Boss ESTER EGG
const int BOSS_MAX_HP = 90;
int bossHP; 
int bossX = 0;
unsigned long bossStartTime;
bool bossActive = false;

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

void startBossBattle() {
  bossActive = true;
  bossHP = BOSS_MAX_HP;
  bossStartTime = millis();
  
  for(int i=0; i<MAX_BULLETS; i++) bActive[i] = false;

  while (bossHP > 0) {
    unsigned long elapsed = millis() - bossStartTime;
    int timeLeft = 15 - (elapsed / 1000);

    if (timeLeft <= 0) {
      resetGame("MORTO");
      return;
    }

    mx->clear();
    
    bossX = map(elapsed, 0, 15000, 0, 26);

    for(int i = bossX; i < bossX + 6; i++) {
      mx->setPoint(3, i, true);
      mx->setPoint(4, i, true);
      mx->setPoint(5, i, true);
    }

    if (digitalRead(2) == LOW && p1Y > 0) p1Y--;
    if (digitalRead(4) == LOW && p1Y < 7) p1Y++;
    if (digitalRead(7) == LOW && p2Y > 0) p2Y--;
    if (digitalRead(9) == LOW && p2Y < 7) p2Y++;
    
    mx->setPoint(p1Y, pX, true);
    mx->setPoint(p2Y, pX, true);

    if (digitalRead(3) == LOW) {
      for(int i=0; i<MAX_BULLETS; i++) {
        if(!bActive[i]) { bX[i] = pX - 1; bY[i] = p1Y; bActive[i] = true; break; }
      }
      delay(50);
    }
    if (digitalRead(6) == LOW) {
      for(int i=0; i<MAX_BULLETS; i++) {
        if(!bActive[i]) { bX[i] = pX - 1; bY[i] = p2Y; bActive[i] = true; break; }
      }
      delay(50);
    }

    for(int i=0; i<MAX_BULLETS; i++) {
      if(bActive[i]) {
        bX[i]--;
        
        if(bX[i] >= bossX && bX[i] <= bossX + 5 && bY[i] >= 3 && bY[i] <= 5) {
          bossHP -= 3;
          bActive[i] = false;
          updateLCD();
        } 
        else if(bX[i] < 0) {
          bActive[i] = false;
        } else {
          mx->setPoint(bY[i], bX[i], true);
        }
      }
    }

    lcd.setCursor(0, 0);
    lcd.print("BOSS HP: "); lcd.print(bossHP); lcd.print("  ");
    lcd.setCursor(0, 1);
    lcd.print("TEMPO: "); lcd.print(timeLeft); lcd.print("s  ");

    delay(30);
  }

  mx->control(MD_MAX72XX::INTENSITY, 15);
  for(int x=bossX; x<bossX+6; x++) {
    for(int y=3; y<=5; y++) mx->setPoint(y, x, true);
  }
  delay(100);

  const int NUM_PART = 40;
  float pX_f[NUM_PART], pY_f[NUM_PART];
  float vX[NUM_PART], vY[NUM_PART];
  bool pAlive[NUM_PART];
  
  for(int i=0; i<NUM_PART; i++) {
    pX_f[i] = bossX + 3; 
    pY_f[i] = 4;
    vX[i] = (random(-150, 150) / 40.0); 
    vY[i] = (random(-200, 50) / 50.0);
    pAlive[i] = true;
  }

  for(int t=0; t<35; t++) {
    mx->clear();
    bool anyAlive = false;
    for(int i=0; i<NUM_PART; i++) {
      if(!pAlive[i]) continue;

      pX_f[i] += vX[i];
      pY_f[i] += vY[i];
      vY[i] += 0.25;
      
      if(pX_f[i] < 0 || pX_f[i] >= 32 || pY_f[i] >= 8) {
        pAlive[i] = false;
      } else if(pY_f[i] >= 0) {
        mx->setPoint((int)pY_f[i], (int)pX_f[i], true);
        anyAlive = true;
      }
    }
    if(!anyAlive) break;
    delay(40);
  }
  
  mx->clear();
  mx->control(MD_MAX72XX::INTENSITY, 5); 
  delay(1000);

  mx->clear();

  mx->setPoint(1, 4, true); mx->setPoint(1, 5, true);
  mx->setPoint(2, 6, true); mx->setPoint(2, 7, true);
  mx->setPoint(3, 8, true); mx->setPoint(3, 9, true);
  mx->setPoint(4, 10, true); mx->setPoint(4, 11, true);
  for(int x=5; x<12; x++) mx->setPoint(6, x, true);
  mx->setPoint(4, 7, true); mx->setPoint(5, 7, true);
  mx->setPoint(4, 8, true); mx->setPoint(5, 8, true);

  mx->setPoint(1, 27, true); mx->setPoint(1, 26, true);
  mx->setPoint(2, 25, true); mx->setPoint(2, 24, true);
  mx->setPoint(3, 23, true); mx->setPoint(3, 22, true);
  mx->setPoint(4, 21, true); mx->setPoint(4, 20, true);

  for(int x=20; x<27; x++) mx->setPoint(6, x, true);

  mx->setPoint(4, 23, true); mx->setPoint(5, 23, true);
  mx->setPoint(4, 24, true); mx->setPoint(5, 24, true);

  for(int p=0; p<5; p++) {
    mx->control(MD_MAX72XX::INTENSITY, 15);
    delay(80);
    mx->control(MD_MAX72XX::INTENSITY, 3);
    delay(80);
  }
  mx->control(MD_MAX72XX::INTENSITY, 5);

  lcd.clear();
  lcd.setCursor(1, 0);
  String msgVolta = "EU VOLTAREI....!   ";
  for (int i = 0; i < msgVolta.length() + 16; i++) {
    lcd.setCursor(0, 0);
    String displayMsg = "";
    if (i < 16) {
      for(int s=0; s<(16-i); s++) displayMsg += " ";
      displayMsg += msgVolta.substring(0, i);
    } else {
      displayMsg = msgVolta.substring(i - 16, i);
    }
    lcd.print(displayMsg);
    delay(120);
  }
  
  delay(500);
  lcd.clear();

  P.displayClear();
  P.print("YEEY");
  String parabens = "P A R A B E N S !";
  lcd.setCursor(0, 0);
  for (int i = 0; i < parabens.length(); i++) {
    lcd.print(parabens[i]);
    delay(200);
  }

  lcd.setCursor(0, 1);
  for (int i = 0; i < 16; i++) {
    lcd.print(">");
    delay(100);
  }

  while(true) {
    lcd.setCursor(0, 0);
    lcd.print("P A R A B E N S !");
    delay(800);
    lcd.setCursor(0, 0);
    lcd.print("                ");
    delay(400);
  }
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

  int p1Clicks = 0;
  int p2Clicks = 0;

  while(true) {

    if (digitalRead(3) == LOW) {
      p1Clicks++;
      delay(200); 
    }

    if (p1Clicks >= 2 && digitalRead(6) == LOW) {
      p2Clicks++;
      delay(200);
    }

    if (p1Clicks == 2 && p2Clicks == 3) {
      lcd.clear();
      
      String msg = "Como ousam me acordar...   ";
      for (int i = 0; i < msg.length() + 16; i++) {
        lcd.setCursor(0, 0);
        String displayMsg = "";
        if (i < 16) {
          for(int s=0; s<(16-i); s++) displayMsg += " ";
          displayMsg += msg.substring(0, i);
        } else {
          displayMsg = msg.substring(i - 16, i);
        }
        lcd.print(displayMsg);
        delay(110);
      }

      lcd.clear();
      delay(500);

      for (int i = 0; i < 4; i++) {
        lcd.setCursor(4, 0);
        lcd.print("ATAQUE-ME!");
        delay(400);
        lcd.clear();
        delay(200);
      }

      startBossBattle();
      break; 
    }
  }
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