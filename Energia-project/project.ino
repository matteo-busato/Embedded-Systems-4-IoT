#include "graphics.h"
#include "buzzer.h"
#include "game.h"
#include "accelerometer.h"
#include "button.h"
#include "msp.h"
#include "driverlib.h"

void setup() {
  analogReadResolution(12);  
  _initButtonS1();  
  _accelInit(accwin);
  _initScreen();    
  _initBuzzer();
  initGame();
  startUpSound();
  Serial.begin(9600);  
  delay(1000);  
}

void loop() {
  
  if (loose == 0 && win == 0){    
    switch (mode) {
      case 0: { //initialPage
          break;
        }
      case 1: { //gamepage               
          getAccelData();     //get data from accelerometer sensor.
          drawAccelBar(accwin);   //draw a visual representation of the accelerometer sensor data.
          manageMovement(accwin);   //manage the movement of the ball on the screen.                  
          break;
        }
      case 2: { //pause          
          break;
        }
    }
  }  
  delay(100);

}
