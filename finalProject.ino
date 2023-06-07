#include <Adafruit_CircuitPlayground.h>
#include <AsyncDelay.h>

int game, buttonCountL, buttonCountR, bothButtonCount;
int difficulty = 0;
int points = 0;
int lives = 3;
int buttonPinL = 4;
int buttonPinR = 5;
int switchPin = 7;
float X, Y, Z;
volatile bool buttonFlagL = 0;
volatile bool buttonFlagR = 0;
volatile bool switchFlag = 0;
bool switchState, gameStart, gamePause;

float midi[127];
int a440 = 440;

AsyncDelay delay_1s;
AsyncDelay delay_3s;
AsyncDelay delay_5s;
AsyncDelay delay_10s;

void setup() {
  // put your setup code here, to run once:
  CircuitPlayground.begin();
  attachInterrupt(digitalPinToInterrupt(buttonPinL), leftButtonISR, RISING);
  attachInterrupt(digitalPinToInterrupt(buttonPinR), rightButtonISR, RISING);
  attachInterrupt(digitalPinToInterrupt(switchPin), switchISR, CHANGE);
  Serial.begin(9600);

  generateMIDI();
}

void loop() {
  // put your main code here, to run repeatedly:
  X = CircuitPlayground.motionX();
  Y = CircuitPlayground.motionY();
  Z = CircuitPlayground.motionZ();

    if (switchFlag == 1) {
      gamePause = 1;
      switchState = digitalRead(switchPin);
      Serial.println("Game paused.");
      switchFlag = 0;
    }

    if (!switchState) {
      delay(5);
      game = random(1,2);
      if (game == 1) {
        game3();
      } else if (game == 2) {
        game2();
      }
      
      if (points == 3 || points == 6 || points == 9 || points == 12 || points ==  15) {
        difficulty++;
      }
  }
}

void game1() {
  //Don't move anything.
  delay(5);
  Serial.println("Don't move!");
  gameStart = 1;
    if (difficulty > 0 && difficulty < 2) {
      delay_1s.start(1000, AsyncDelay::MILLIS);
      while (!(delay_1s.isExpired())) {
        X = CircuitPlayground.motionX();
        Y = CircuitPlayground.motionY();
        Z = CircuitPlayground.motionZ();
      }
      if (delay_1s.isExpired()) {
        if ((X > (X - 30) && X < (X + 30)) && (Y > (Y - 30) && Y < (Y + 30)) && (Z > (Z - 30) && Z < (Z + 30))) {
          win();
        } else {
          loss();
        }
      }
      delay(50);
    } else if (difficulty > 2 && difficulty < 5) {
      delay_3s.start(3000, AsyncDelay::MILLIS);
      while (!(delay_3s.isExpired())) {
        X = CircuitPlayground.motionX();
        Y = CircuitPlayground.motionY();
        Z = CircuitPlayground.motionZ();
      }
      if (delay_3s.isExpired()) {
        if ((X > (X - 20) && X < (X + 20)) && (Y > (Y - 20) && Y < (Y + 20)) && (Z > (Z - 20) && Z < (Z + 20))) {
          win();
        } else {
          loss();
        }
      } 
      delay(50);
    } else if (difficulty > 5) {
      delay_5s.start(5000, AsyncDelay::MILLIS);
      while (!(delay_5s.isExpired())) {
          X = CircuitPlayground.motionX();
          Y = CircuitPlayground.motionY();
          Z = CircuitPlayground.motionZ();
      }
      if (delay_5s.isExpired()) {
        if ((X > (X - 10) && X < (X + 10)) && (Y > (Y - 10) && Y < (Y + 10)) && (Z > (Z - 10) && Z < (Z + 10))) {
          win();
        } else {
          loss();
        }
      }
    delay(50);
  }
}

void game2() {
  //Press left button 5 times.
  Serial.println("Press left button 5 times.");
  if (difficulty > 0 && difficulty < 3) {
      delay_3s.start(3000, AsyncDelay::MILLIS);
      while (!(delay_3s.isExpired())) {
        if (buttonFlagL == 1) {
          buttonCountL++;
          buttonFlagL = 0;
          delay(5);
        }
      }
      if (delay_3s.isExpired()) {
        if (buttonCountL == 5) {
          win();
          buttonCountL = 0;
        } else {
          loss();
          buttonCountL = 0;
        }
      }
      delay(50);
    } else {
      delay_1s.start(1000, AsyncDelay::MILLIS);
      while (!(delay_1s.isExpired())) {
        if (buttonFlagL == 1) {
          
          buttonCountL++;
          buttonFlagL = 0;
        }
      }
      if (delay_1s.isExpired()) {
        if (buttonCountL == 5) {
          win();
          buttonCountL = 0;
        } else {
          loss();
          buttonCountL = 0;
        }
      }
    delay(50);
  }
}

void game3() {
  //Press right button 5 times.
  Serial.println("Press right button 5 times.");
  if (difficulty > 0 && difficulty < 3) {
      delay_5s.start(5000, AsyncDelay::MILLIS);
      while (!(delay_5s.isExpired())) {
        if (buttonFlagR == 1) {
          buttonCountR++;
          buttonFlagL = 0;
          delay(5);
        }
      }
      if (delay_5s.isExpired()) {
        if (buttonCountR == 5) {
          win();
          buttonCountR = 0;
        } else {
          loss();
          buttonCountR = 0;
        }
      }
    } else {
      delay_3s.start(3000, AsyncDelay::MILLIS);
      while (!(delay_3s.isExpired())) {
        if (buttonFlagR == 1) {
          
          buttonCountR++;
          buttonFlagR = 0;
        }
      }
      if (delay_3s.isExpired()) {
        if (buttonCountR == 5) {
          win();
          buttonCountR = 0;
        } else {
          loss();
          buttonCountR = 0;
        }
      }
  }
}

void win() {
  points = points + 1;
  
  for (int i = 0; i < 10; i++) {
    CircuitPlayground.setPixelColor(i, 0, 255, 0);
  }
  
  CircuitPlayground.playTone(midi[56], 50);
  delay(50);
  CircuitPlayground.playTone(midi[63], 50);
  delay(50);
  CircuitPlayground.playTone(midi[68], 50);
  delay(50);
  
  for (int i = 0; i < 10; i++) {
    CircuitPlayground.setPixelColor(i, 0, 0, 0);
  }

  Serial.print("Score: ");
  Serial.println(points);
  Serial.print("Lives: ");
  Serial.println(lives);
  gameStart = 0;
  delay(1000);
}

void loss() {
  lives = lives - 1;

  for (int i = 0; i < 10; i++) {
    CircuitPlayground.setPixelColor(i, 255, 0, 0);
  }
  CircuitPlayground.playTone(midi[68], 50);
  delay(50);
  CircuitPlayground.playTone(midi[63], 50);
  delay(50);
  CircuitPlayground.playTone(midi[56], 50);
  delay(50);
  for (int i = 0; i < 10; i++) {
    CircuitPlayground.setPixelColor(i, 0, 0, 0);
  }

  Serial.print("Lives: ");
  Serial.println(lives);
  Serial.print("Score: ");
  Serial.println(points);
  
  if (lives == 0) {
    for (int beeps = 0; beeps < 3; beeps++) {
      for (int i = 0; i < 10; i++) {
        CircuitPlayground.setPixelColor(i, 255, 0, 0);
      }
      CircuitPlayground.playTone(midi[56], 500);
      for (int i = 0; i < 10; i++) {
        CircuitPlayground.setPixelColor(i, 0, 0, 0);
      }
    }
    Serial.println("You lost.");
    Serial.println("Score: ");
    Serial.println(points);
    lives = 3;
  }
  delay(1000);
}

void leftButtonISR() {
  buttonFlagL = 1;
}

void rightButtonISR() {
  buttonFlagR = 1;
}

void switchISR() {
  switchFlag = 1;
}

void generateMIDI() {
  for (int x = 0; x < 127; ++x)
  {
    midi[x] = (a440 / 32.0) * pow(2.0, ((x - 9.0) / 12.0));
    Serial.println(midi[x]);
  }
}