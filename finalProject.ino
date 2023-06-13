#include <Adafruit_CircuitPlayground.h>

//Global variables.
int difNum, points, greenLED, yellowLED1, yellowLED2, count, randLED;
int difficulty[17] = {100, 90, 80, 70, 60, 50, 44, 40, 34, 30, 24, 20, 14, 10, 8, 4, 2}; //Array of difficulties to increment through.
int lives = 3;
int buttonPinL = 4;
int buttonPinR = 5;
int switchPin = 7;
volatile bool buttonFlagL = 0;
volatile bool buttonFlagR = 0;
volatile bool switchFlag = 0;
bool switchState, game;

float midi[127];
int a440 = 440;

void setup() {
  //Intiallizing CircuitPlayground, interrupts, serial monitor, and MIDI.
  CircuitPlayground.begin();
  attachInterrupt(digitalPinToInterrupt(buttonPinL), leftButtonISR, RISING);
  attachInterrupt(digitalPinToInterrupt(buttonPinR), rightButtonISR, RISING);
  attachInterrupt(digitalPinToInterrupt(switchPin), switchISR, CHANGE);
  Serial.begin(9600);

  generateMIDI();
}

void loop() {
  if (switchFlag == 1) //When the switch moves, the game pauses.
    delay(5);
    off();
    switchState = digitalRead(switchPin);
    switchFlag = 0;
  }

  if (!switchState) {
    //The game itself.
    delay(5);
    game = 1;

    greenLED = random(0,9); //Declaring where the LED's are going to go.
    
    yellowLED1 = greenLED - 1;
    if (yellowLED1 < 0) {
      yellowLED1 = 9;
    }

    yellowLED2 = greenLED + 1;
    if (yellowLED2 > 10) {
      yellowLED2 = 0;
    }

    for (int i = 0; i < 10; i++) { 
      //Looping through the LEDS, checks to implement the green/yellow LEDS.
      if(i == greenLED) {
        CircuitPlayground.setPixelColor(i, 0, 200, 0);
      } else if (i == yellowLED1 || i == yellowLED2) {
        CircuitPlayground.setPixelColor(i, 200, 200, 0);
      } else {
        CircuitPlayground.setPixelColor(i, 200, 0, 0);
      }
    }

    while (game == 1) {
    //The game itself.
      for (int i = 0; i < 10; i++) {
        if (switchFlag == 1) { 
          //Checks the switch every loop so you can pause.
          game = 0;
          break;
        }

        CircuitPlayground.setPixelColor(i, 200, 200, 200);
        delay(difficulty[difNum]); //Delay between LED's corresponds to the current difficulty of the game.

        if(i == greenLED) { //Sets the LED's back where they were, after setting the LED's white.
          CircuitPlayground.setPixelColor(i, 0, 200, 0);
        } else if (i == yellowLED1 || i == yellowLED2) {
          CircuitPlayground.setPixelColor(i, 200, 200, 0);
        } else {
          CircuitPlayground.setPixelColor(i, 200, 0, 0);
        }
        delay(difficulty[difNum]);
        
        if (buttonFlagR == 1 || buttonFlagL == 1) { //Checks for button presses.
          delay(5);
          if (i == greenLED) { 
            //Checks if the button is pressed on the green LED, if so, gain 2 points.
            points = points + 2;
            count = count + 2;
            scorePointsGreen();
          } else if (i == yellowLED1 || i == yellowLED2) { 
            //Checks if the button is pressed on the yellow LED's, if so, gain a point.
            points++;
            count++;
            scorePointsYellow();
          } else if (i != greenLED || i != yellowLED1 || i != yellowLED2) { 
            //Checks if the button is pressed on the red LED's, if so, lose a life.
            lives--;
            if (lives > 0) { 
              //Checks if lives are greater than 0, if so, lose a life.
              loseLife();
            } else { 
              //If you have zero lives, you lose.
              Serial.print("You lost.\n");
              loss();
              lives = 3;
              points = 0;
              difNum = 0;
              count = 0;
            }
          }

          if (count > 2) { 
            /*Increments difficulty/delay time every two points.
            Once the difficulty is greater then the number of delay times, it stops incrementing.*/
            if (difNum >= sizeof(difficulty)/sizeof(int)) {
              count = 0;
            } else {
              difNum++;
              count = 0;
            }
          }
          //Resets flags and stops looping after the button is pressed, and all checks are in place.
          buttonFlagR = 0;
          buttonFlagL = 0;
          game = 0;
          break;
        }
      }
      delay(difficulty[difNum]);
    }
      Serial.print("Score: ");
      Serial.println(points);
      Serial.print("Lives: ");
      Serial.println(lives);
  }
}

void scorePointsGreen() {
  //Light/sound pattern when you land on green.
  for (int i = 0; i < 10; i++) {
    CircuitPlayground.setPixelColor(i, 0, 255, 0);
  }
  
  CircuitPlayground.playTone(midi[56], 40);
  delay(40);
  CircuitPlayground.playTone(midi[63], 40);
  delay(40);
  CircuitPlayground.playTone(midi[68], 40);
  delay(40);
  
  for (int i = 0; i < 10; i++) {
    CircuitPlayground.setPixelColor(i, 0, 0, 0);
  }
}

void scorePointsYellow() {
  //Light/sound pattern when you land on yellow.
  for (int i = 0; i < 10; i++) {
    CircuitPlayground.setPixelColor(i, 255, 255, 0);
  }
  
  CircuitPlayground.playTone(midi[68], 40);
  delay(40);
  CircuitPlayground.playTone(midi[63], 40);
  delay(40);
  CircuitPlayground.playTone(midi[68], 40);
  delay(40);
  
  for (int i = 0; i < 10; i++) {
    CircuitPlayground.setPixelColor(i, 0, 0, 0);
  }
}

void loseLife() {
  //Light/sound pattern when you land on red.
  for (int i = 0; i < 10; i++) {
    CircuitPlayground.setPixelColor(i, 200, 0, 0);
  }
  CircuitPlayground.playTone(midi[68], 40);
  delay(40);
  CircuitPlayground.playTone(midi[63], 40);
  delay(40);
  CircuitPlayground.playTone(midi[56], 40);
  delay(40);
  for (int i = 0; i < 10; i++) {
    CircuitPlayground.setPixelColor(i, 0, 0, 0);
  }
}

void loss() {
  //Light/sound pattern when you lose a life.
  for (int i = 0; i < 10; i++) {
    CircuitPlayground.setPixelColor(i, 200, 0, 0);
  }
  CircuitPlayground.playTone(midi[68], 40);
  delay(40);

  for (int i = 0; i < 10; i++) {
    CircuitPlayground.setPixelColor(i, 200, 0, 0);
  }
  CircuitPlayground.playTone(midi[63], 40);
  delay(40);

  for (int i = 0; i < 10; i++) {
    CircuitPlayground.setPixelColor(i, 200, 0, 0);
  }
  CircuitPlayground.playTone(midi[56], 1500);
  delay(40);

  for (int i = 0; i < 10; i++) {
    CircuitPlayground.setPixelColor(i, 0, 0, 0);
  }
}

void off() {
  //Turns off all LED's when game is paused.
  for (int i = 0; i < 10; i++){
    CircuitPlayground.setPixelColor(i, 0, 0, 0);
  }
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
