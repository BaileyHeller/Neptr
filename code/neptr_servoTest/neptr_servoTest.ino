#include "ServoEasing.h"

#define SERVO1_PIN 5
#define SERVO2_PIN 6
#define POT1_PIN A0
#define POT2_PIN A5
int pot1_reading;
int pot2_reading;

#define SERVO1_MAX 162 //162
#define SERVO1_MIN 75 //75
#define SERVO2_MAX 96 //96
#define SERVO2_MIN 0 //0

#define POT_TOLERANCE 4

byte servo1_position;
byte servo2_position;

ServoEasing Servo1;
ServoEasing Servo2;

void setup() {
  Serial.begin(9600);
  Servo1.attach(SERVO1_PIN);
  Servo2.attach(SERVO2_PIN);

  /*setSpeedForAllServos(30);
  setEasingTypeForAllServos(EASE_CUBIC_IN_OUT);*/

  /*Servo1.startEaseTo(135, 40, false);
    do {
        delayAndUpdateAndWaitForAllServosToStop(100);
    } while (Servo1.isMoving() || Servo2.isMoving());*/
}

void loop() {
  if(pot1_reading > (analogRead(POT1_PIN)+POT_TOLERANCE) || pot1_reading < (analogRead(POT1_PIN)-POT_TOLERANCE)){
    pot1_reading = analogRead(POT1_PIN);
    servo1_position = map(pot1_reading,0,1023, SERVO1_MIN,SERVO1_MAX);
    Servo1.write(servo1_position);
    
    Serial.print(servo1_position);
    Serial.print(" | ");
    Serial.println(servo2_position);
  }
  if(pot2_reading > (analogRead(POT2_PIN)+POT_TOLERANCE) || pot2_reading < (analogRead(POT2_PIN)-POT_TOLERANCE)){
    pot2_reading = analogRead(POT2_PIN);
    servo2_position = map(pot2_reading,0,1023, SERVO2_MIN,SERVO2_MAX);
    Servo2.write(servo2_position);
    
    Serial.print(servo1_position);
    Serial.print(" | ");
    Serial.println(servo2_position);
  }
}
