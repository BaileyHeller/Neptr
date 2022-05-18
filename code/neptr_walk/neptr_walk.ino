#include "ServoEasing.h"

const byte SERVO_PIN[] = {5, 6,10,11};
#define SERVO1_PIN 5
#define SERVO2_PIN 6
#define SERVO3_PIN 10
#define SERVO4_PIN 11
#define POT1_PIN A0
#define POT2_PIN A5
int pot1_reading;
int pot2_reading;

const byte SERVO_MAX[] = {162, 96,144,111};
const byte SERVO_MIN[] = {75, 0,57,6};
#define SERVO1_MAX 162
#define SERVO1_MIN 75
#define SERVO2_MAX 96
#define SERVO2_MIN 0

#define POT_TOLERANCE 4
#define SERVO_TOLERANCE 1

int servoPositionRaw[] = {0, 0, 0, 0};
byte servoPosition[] = {0, 0, 0, 0};
byte servoTarget[] = {0, 0, 0, 0};
int servoSpeed[] = {0, 0, 0, 0};
byte servo1_position;
byte servo2_position;


const byte SERVO1_SEQUENCE[] = {159, 141, 96, 98, 101, 106, 109, 114, 118, 123, 127, 131, 134, 137, 140, 145, 147, 150, 152};
const byte SERVO2_SEQUENCE[] = {77,   19,  0, 15, 17,  24,  24,  30,  34,  38,  43,  51,  54,  58,  66,  72,  75,  93,  89};
const byte STEP_ANIMATION_LENGTH = sizeof(SERVO1_SEQUENCE)/sizeof(SERVO1_SEQUENCE[0]) -3;
byte index = STEP_ANIMATION_LENGTH;

#define KEYFRAME_SUBDIVISION 50 //number of substeps between keyframe sequence

ServoEasing Servo1;
ServoEasing Servo2;
ServoEasing Servo3;
ServoEasing Servo4;

void setup() {
  Serial.begin(9600);
  Servo1.attach(SERVO1_PIN);
  Servo2.attach(SERVO2_PIN);
  Servo3.attach(SERVO3_PIN);
  Servo4.attach(SERVO4_PIN);

  servoPosition[0] = 122;
  servoPosition[1] = 38;
  servoPosition[2] = 122;
  servoPosition[3] = 38;
  servoPositionRaw[0] = servoPosition[0]*100;
  servoPositionRaw[1] = servoPosition[1]*100;
  servoPositionRaw[2] = servoPosition[2]*100;
  servoPositionRaw[3] = servoPosition[3]*100;
  Servo1.write(servoPosition[0]);
  Servo2.write(servoPosition[1]);
  Servo3.write(servoPosition[2]);
  Servo4.write(servoPosition[3]);
  delay(500);

  /*setSpeedForAllServos(30);
    setEasingTypeForAllServos(EASE_CUBIC_IN_OUT);*/

  /*Servo1.startEaseTo(135, 40, false);
    do {
        delayAndUpdateAndWaitForAllServosToStop(100);
    } while (Servo1.isMoving() || Servo2.isMoving());*/
}

void loop() {
  while(1){
    Step();
  }
  for (byte i = STEP_ANIMATION_LENGTH; i > 1; i--) { //each keyframe
    servoTarget[0] = SERVO1_SEQUENCE[i];
    servoTarget[1] = SERVO2_SEQUENCE[i];
    
    while ((servoPositionRaw[0] > 100 * (servoTarget[0] + SERVO_TOLERANCE) || servoPositionRaw[0] < 100 * (servoTarget[0] - SERVO_TOLERANCE)) || (servoPositionRaw[1] > 100 * (servoTarget[1] + SERVO_TOLERANCE) || servoPositionRaw[1] < 100 * (servoTarget[1] - SERVO_TOLERANCE))) {
      SetMotorSpeeds(i);
      
      servoPositionRaw[0] += servoSpeed[0];
      servoPositionRaw[1] += servoSpeed[1];
      servoPosition[0] = servoPositionRaw[0] / 100;
      servoPosition[1] = servoPositionRaw[1] / 100;
      if (servoPosition[0] < SERVO_MAX[0] && servoPosition[0] > SERVO_MIN[0]) {
        Servo1.write(servoPosition[0]);
      }
      if (servoPosition[1] < SERVO_MAX[1] && servoPosition[1] > SERVO_MIN[1]) {
        Servo2.write(servoPosition[1]);
      }
      delay(1);

    }
    Servo1.write(servoTarget[0]);
    Servo2.write(servoTarget[1]);

    Serial.println(index);
    Serial.print(servoSpeed[0]);
    Serial.print("  |  ");
    Serial.println(servoSpeed[1]);
    Serial.print(servoPosition[0]);
    Serial.print(" | ");
    Serial.println(servoPosition[1]);


    
    if (index == 0) {
      delay(500);
    }
    index--;

  }
  index = STEP_ANIMATION_LENGTH;

  /*if(pot1_reading > (analogRead(POT1_PIN)+POT_TOLERANCE) || pot1_reading < (analogRead(POT1_PIN)-POT_TOLERANCE)){
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
    }*/
}

void SetMotorSpeeds(byte i){
  for (byte j = 0; j <= 1; j++) {
    int speed_temp = (servoTarget[j] * 100 - servoPositionRaw[j]) / KEYFRAME_SUBDIVISION;
    /*if (speed_temp == 0) {
      if (i == 0) {
        servoSpeed[j] = 1;
      } else {
        servoSpeed[j] = -1;
      }
    } else {*/
      servoSpeed[j] = speed_temp;
    //}
    //Serial.println(servoPosition[j]);
  }
}

void Step(){
  servoSpeed[0] = -3;
  servoSpeed[1] = -4;
  servoSpeed[2] = -3;
  servoSpeed[3] = -4;
  servoPositionRaw[0] = 14700;//9500
  servoPositionRaw[1] = 7500;//1000
  servoPositionRaw[2] = 10400;//12400
  servoPositionRaw[3] = 10100;//10100
  servoPosition[0] = servoPositionRaw[0] / 100;
  servoPosition[1] = servoPositionRaw[1] / 100;
  servoPosition[2] = servoPositionRaw[2] / 100;
  servoPosition[3] = servoPositionRaw[3] / 100;
  Servo1.write(servoPosition[0]);
  Servo2.write(servoPosition[1]);
  Servo3.write(servoPosition[2]);
  Servo4.write(servoPosition[3]);
  delay(300);
  while (servoPositionRaw[0] >9500 && servoPositionRaw[1] >1000) { //<14700 <7500
      servoPositionRaw[0] += servoSpeed[0];
      servoPositionRaw[1] += servoSpeed[1];
      servoPositionRaw[2] += servoSpeed[2];
      servoPositionRaw[3] += servoSpeed[3];
      servoPosition[0] = servoPositionRaw[0] / 100;
      servoPosition[1] = servoPositionRaw[1] / 100;
      servoPosition[2] = servoPositionRaw[2] / 100;
      servoPosition[3] = servoPositionRaw[3] / 100;
      if (servoPosition[0] < SERVO_MAX[0] && servoPosition[0] > SERVO_MIN[0]) {
        Servo1.write(servoPosition[0]);
      }
      if (servoPosition[1] < SERVO_MAX[1] && servoPosition[1] > SERVO_MIN[1]) {
        Servo2.write(servoPosition[1]);
      }
      if (servoPosition[2] < SERVO_MAX[2] && servoPosition[2] > SERVO_MIN[2]) {
        Servo3.write(servoPosition[2]);
      }
      if (servoPosition[3] < SERVO_MAX[3] && servoPosition[3] > SERVO_MIN[3]) {
        Servo4.write(servoPosition[3]);
      }
      delay(1);
  }

  servoSpeed[0] = 3;
  servoSpeed[1] = 4;
  servoSpeed[2] = 3;
  servoSpeed[3] = 4;
  servoPositionRaw[0] = 11500;//9500
  servoPositionRaw[1] = 1000;//1000
  servoPositionRaw[2] = 7200;//12400
  servoPositionRaw[3] = 3600;//10100
  servoPosition[0] = servoPositionRaw[0] / 100;
  servoPosition[1] = servoPositionRaw[1] / 100;
  servoPosition[2] = servoPositionRaw[2] / 100;
  servoPosition[3] = servoPositionRaw[3] / 100;
  Servo1.write(servoPosition[0]);
  Servo2.write(servoPosition[1]);
  Servo3.write(servoPosition[2]);
  Servo4.write(servoPosition[3]);
  delay(300);
  while (servoPositionRaw[0] <16700 && servoPositionRaw[1] <7500) { //<14700 <7500
      servoPositionRaw[0] += servoSpeed[0];
      servoPositionRaw[1] += servoSpeed[1];
      servoPositionRaw[2] += servoSpeed[2];
      servoPositionRaw[3] += servoSpeed[3];
      servoPosition[0] = servoPositionRaw[0] / 100;
      servoPosition[1] = servoPositionRaw[1] / 100;
      servoPosition[2] = servoPositionRaw[2] / 100;
      servoPosition[3] = servoPositionRaw[3] / 100;
      if (servoPosition[0] < SERVO_MAX[0] && servoPosition[0] > SERVO_MIN[0]) {
        Servo1.write(servoPosition[0]);
      }
      if (servoPosition[1] < SERVO_MAX[1] && servoPosition[1] > SERVO_MIN[1]) {
        Servo2.write(servoPosition[1]);
      }
      if (servoPosition[2] < SERVO_MAX[2] && servoPosition[2] > SERVO_MIN[2]) {
        Servo3.write(servoPosition[2]);
      }
      if (servoPosition[3] < SERVO_MAX[3] && servoPosition[3] > SERVO_MIN[3]) {
        Servo4.write(servoPosition[3]);
      }
      delay(1);
  }

}

void Step1(){
  servoSpeed[0] = -3;
  servoSpeed[1] = -4;
  servoSpeed[2] = 3;
  servoSpeed[3] = 4;
  servoPositionRaw[0] = 14700;//9500
  servoPositionRaw[1] = 7500;//1000
  servoPositionRaw[2] = 7200;//12400
  servoPositionRaw[3] = 3600;//10100
  servoPosition[0] = servoPositionRaw[0] / 100;
  servoPosition[1] = servoPositionRaw[1] / 100;
  servoPosition[2] = servoPositionRaw[2] / 100;
  servoPosition[3] = servoPositionRaw[3] / 100;
  Servo1.write(servoPosition[0]);
  Servo2.write(servoPosition[1]);
  Servo3.write(servoPosition[2]);
  Servo4.write(servoPosition[3]);
  delay(300);
  while (servoPositionRaw[0] >9500 && servoPositionRaw[1] >1000) { //<14700 <7500
      servoPositionRaw[0] += servoSpeed[0];
      servoPositionRaw[1] += servoSpeed[1];
      servoPositionRaw[2] += servoSpeed[2];
      servoPositionRaw[3] += servoSpeed[3];
      servoPosition[0] = servoPositionRaw[0] / 100;
      servoPosition[1] = servoPositionRaw[1] / 100;
      servoPosition[2] = servoPositionRaw[2] / 100;
      servoPosition[3] = servoPositionRaw[3] / 100;
      if (servoPosition[0] < SERVO_MAX[0] && servoPosition[0] > SERVO_MIN[0]) {
        Servo1.write(servoPosition[0]);
      }
      if (servoPosition[1] < SERVO_MAX[1] && servoPosition[1] > SERVO_MIN[1]) {
        Servo2.write(servoPosition[1]);
      }
      if (servoPosition[2] < SERVO_MAX[2] && servoPosition[2] > SERVO_MIN[2]) {
        Servo3.write(servoPosition[2]);
      }
      if (servoPosition[3] < SERVO_MAX[3] && servoPosition[3] > SERVO_MIN[3]) {
        Servo4.write(servoPosition[3]);
      }
      delay(1);
  }
  Servo1.write(141);//141
  Servo2.write(0);//0
  Servo3.write(78);//78
  Servo4.write(111);//111
  delay(300);

}
