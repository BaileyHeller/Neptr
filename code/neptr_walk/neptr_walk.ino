#include "ServoEasing.h"

const byte SERVO_PIN[] = {3,2,8,9,7,6,5,4};

const byte SERVO_MAX[] = {73, 86, 140, 16,    84, 170,  63,   10};//73, 96, 147, 6,    34, 170,  63,   60
const byte SERVO_MIN[] = {172, 0, 47,  98,  20,  10,    177,  140};//152, 0, 67,  109,  0,  10,    177,  120

#define SERVO_MAP_VALUE 100000

#define SERVO_TOLERANCE 400

long servoPositionRaw[] = {80000, 80000, 80000, 80000,00000,100000,100000,50000};//60000, 80000, 100000, 80000
//long servoPositionRaw[] = {100000, 60000, 75000, 70000,00000,0000,50000,50000};//100000, 37000, 75000, 50000,00000,00000,50000,50000
byte servoPosition[] = {0, 0, 0, 0,0,0,0,0};
long servoTarget[] = {0, 0, 0, 0,0,0,0,0};
int servoSpeed[] = {0, 0, 0, 0,0,0,0,0};

#define NUMBER_OF_FRAMES 8
const long SERVO_STEP_FRAMES[8][NUMBER_OF_FRAMES] = { //if less than zero,hold position
  {-1,-1,90,70,80,-1,-1,100},//pause, pause, lift,front,drop,pause,pause,back,
  {-1,-1,10,0,90,-1,-1,80},
  {80,-1,-1,100,-1,-1,100,70},//drop, pause, pause,back,pause,pause,lift,front,
  {90,-1,-1,80,-1,-1,10,0},
  {0,0,0,0,0,0,0,0},
  {-1,0,-1,-1,-1,100,-1,-1},//tail
  {-1,50,-1,100,-1,50,-1,100},
  {50,50,50,50,50,50,50,50}
};
byte stepFrame = 0;//current frame in animation (resets to zero on first pass)

#define STEP_SPEED 1000 //the bigger the slower

#define STATUS_LED_R 10
#define STATUS_LED_G 11
#define BATTERY_MONITOR A0
#define BATTERY_MAX 836
#define BATTERY_MIN 637

#define MOSFET 12 //auto on/off switch
#define LEFT_TILT_SENSOR 13
#define RIGHT_TILT_SENSOR A2

ServoEasing Servo1;
ServoEasing Servo2;
ServoEasing Servo3;
ServoEasing Servo4;
ServoEasing Servo5;
ServoEasing Servo6;
ServoEasing Servo7;
ServoEasing Servo8;

void setup() {
  Serial.begin(9600);
  Servo1.attach(SERVO_PIN[0]);
  Servo2.attach(SERVO_PIN[1]);
  Servo3.attach(SERVO_PIN[2]);
  Servo4.attach(SERVO_PIN[3]);
  Servo5.attach(SERVO_PIN[4]);
  Servo6.attach(SERVO_PIN[5]);
  Servo7.attach(SERVO_PIN[6]);
  Servo8.attach(SERVO_PIN[7]);
  setEasingTypeForAllServos(EASE_CUBIC_IN_OUT);
  pinMode(STATUS_LED_R,OUTPUT);
  pinMode(STATUS_LED_G,OUTPUT);
  digitalWrite(STATUS_LED_R,HIGH);
  digitalWrite(STATUS_LED_G,LOW);

  pinMode(MOSFET, OUTPUT);
  //MonitorBattery();
  digitalWrite(MOSFET,HIGH);

  //pinMode(LEFT_TILT_SENSOR,INPUT_PULLUP);
  
  
  for(byte i=0;i<8;i++){
    //servoPosition[i] = map(servoPositionRaw[i], 0, SERVO_MAP_VALUE, SERVO_MIN[i], SERVO_MAX[i]);
    MoveTo(i, servoPositionRaw[i]);
  }

  /*Servo6.write(20);
  delay(1000);
  Servo6.write(160);
  delay(1000);
  Servo6.write(20);
  delay(1000);
  Servo6.write(160);
  delay(9999999999);*/

  //delay(999999999);
  delay(2000);

  randomSeed(analogRead(1));
}

void loop() {
  Animate();
  /*while(1){
    TestIfFallenOver();
    delay(10);
    Serial.print(digitalRead(LEFT_TILT_SENSOR));
    Serial.print(" right: ");
    Serial.println(analogRead(RIGHT_TILT_SENSOR));
  }*/
  //Step();
  //Step();
  //delay(999999999);
  //TestIfFallenOver();
  HeadMovement();
  MonitorBattery();
}

void Step(){ //move to the next keyframe of the walk animation
  if(stepFrame >= NUMBER_OF_FRAMES -1){
    stepFrame = 0;
  }else{
    stepFrame++;
  }
  
  for(byte i=0;i<8;i++){
    if(SERVO_STEP_FRAMES[i][stepFrame]<0){
      servoSpeed[i]=0;
    }else{
      servoTarget[i] = (SERVO_STEP_FRAMES[i][stepFrame]*SERVO_MAP_VALUE)/100;
      servoSpeed[i] = (servoTarget[i] - servoPositionRaw[i])/STEP_SPEED;
    }
  }
  
  while(1) {
    for(byte i=0;i<8;i++){     
      if(servoPositionRaw[i] < servoTarget[i] - SERVO_TOLERANCE || servoPositionRaw[i] > servoTarget[i] + SERVO_TOLERANCE){
        servoPositionRaw[i] += servoSpeed[i];
        MoveTo(i,servoPositionRaw[i]);
      }else{
        servoSpeed[i]=0;
      }
    }
    if(servoSpeed[0]==0 && servoSpeed[1]==0 && servoSpeed[2]==0 && servoSpeed[3]==0 && servoSpeed[5]==0 && servoSpeed[6]==0){
      return;
    }
    delay(1);
  }
}

void HeadMovement(){
  //if(stepFrame == 0 || stepFrame == 3){//if both feet are on the ground
    //if(random(8)>=5){
      for(byte i = random(2,5); i>0;i--){
        RandomizeSpeeds();
        Servo7.setEaseTo(random(SERVO_MAX[6],SERVO_MIN[6])-30);
        Servo8.setEaseTo(random(SERVO_MAX[7],SERVO_MIN[7]));
        synchronizeAllServosStartAndWaitForAllServosToStop();

        for(byte i=random(1,3);i>0;i--){
          MappedEaseTo(5,random(20,80));
        }
      }
    //}
  //}
}

void Animate(){
  RandomizeSpeeds();
  
  switch(random(4)){
    case 0://head down scan
      SetMappedEaseTo(6,0);
      SetMappedEaseTo(7,random(0,20));
      synchronizeAllServosStartAndWaitForAllServosToStop();
      for(byte i= random(1,2);i>0;i--){
        delay(random(500,2000));
        RandomizeSpeeds();
        SetMappedEaseTo(7,random(80,100));
        delay(random(500,2000));
        RandomizeSpeeds();
        SetMappedEaseTo(7,random(0,20));
      }
      break;
      
    case 1://bob
      SetMappedEaseTo(0,70);
      SetMappedEaseTo(1,50);
      SetMappedEaseTo(2,70);
      SetMappedEaseTo(3,50);
      SetMappedEaseTo(6,70);
      SetMappedEaseTo(7,random(30,70));
      synchronizeAllServosStartAndWaitForAllServosToStop();
      RandomizeSpeeds();
      for(byte i= random(2,5);i>0;i--){
        delay(500);
        SetMappedEaseTo(0,80);
        SetMappedEaseTo(1,80);
        SetMappedEaseTo(2,80);
        SetMappedEaseTo(3,80);
        SetMappedEaseTo(6,50);
        synchronizeAllServosStartAndWaitForAllServosToStop();
        delay(500);
        SetMappedEaseTo(0,70);
        SetMappedEaseTo(1,50);
        SetMappedEaseTo(2,70);
        SetMappedEaseTo(3,50);
        SetMappedEaseTo(6,70);
        synchronizeAllServosStartAndWaitForAllServosToStop();
      }
      break;

    case 2://wag
      MappedEaseTo(7,random(30,70));
      Servo6.setSpeed(80);
      for(byte i=random(2,4);i>0;i--){
        MappedEaseTo(5,10);
        MappedEaseTo(5,90);
      }
      break;
      
    default://shake head  
      SetMappedEaseTo(6,80);
      SetMappedEaseTo(7,50);
      synchronizeAllServosStartAndWaitForAllServosToStop();
      delay(500);
      Servo8.setSpeed(80);
      for(byte i=0;i<3;i++){
        MappedEaseTo(7,70);
        MappedEaseTo(7,30);
      }
      MappedEaseTo(7,50);
      break;
      
  }
  SetMappedEaseTo(0,80);
  SetMappedEaseTo(1,80);
  SetMappedEaseTo(2,80);
  SetMappedEaseTo(3,80);
  synchronizeAllServosStartAndWaitForAllServosToStop();
}

void RandomizeSpeeds(){
  byte legSpeed = random(50,80);
  Servo1.setSpeed(legSpeed);
  Servo2.setSpeed(legSpeed);
  Servo3.setSpeed(legSpeed);
  Servo4.setSpeed(legSpeed);
  Servo6.setSpeed(random(30,50));
  Servo7.setSpeed(random(30,50));
  Servo8.setSpeed(random(20,80));
}

void TestIfFallenOver(){
  byte counter = 0;
  for(byte i = 0;i<20;i++){
    if(digitalRead(LEFT_TILT_SENSOR) || analogRead(RIGHT_TILT_SENSOR)<1000){
      counter++;
    }
    delay(2);
  }
  if(counter <=18){
    //FallenOver();
    Serial.println("fell over!");
  }
}

void SetMappedEaseTo(byte motor,  long target){
  byte targetUnmapped = map(target, 0, 100, SERVO_MIN[motor], SERVO_MAX[motor]);
  //Serial.println(targetUnmapped);
  if (targetUnmapped <= SERVO_MAX[motor] && targetUnmapped >= SERVO_MIN[motor] || targetUnmapped >= SERVO_MAX[motor] && targetUnmapped <= SERVO_MIN[motor]) {
    switch(motor){
      case 0:
        Servo1.setEaseTo(targetUnmapped);
        break;
      case 1:
        Servo2.setEaseTo(targetUnmapped);
        break;
      case 2:
        Servo3.setEaseTo(targetUnmapped);
        break;
      case 3:
        Servo4.setEaseTo(targetUnmapped);
        break;
      case 4:
        Servo5.setEaseTo(targetUnmapped);
        break;
      case 5:
        Servo6.setEaseTo(targetUnmapped);
        break;
      case 6:
        Servo7.setEaseTo(targetUnmapped);
        break;
      case 7:
        Servo8.setEaseTo(targetUnmapped);
        break;
    }
  }
}

void MappedEaseTo(byte motor,  long target){
  byte targetUnmapped = map(target, 0, 100, SERVO_MIN[motor], SERVO_MAX[motor]);
  //Serial.println(targetUnmapped);
  if (targetUnmapped <= SERVO_MAX[motor] && targetUnmapped >= SERVO_MIN[motor] || targetUnmapped >= SERVO_MAX[motor] && targetUnmapped <= SERVO_MIN[motor]) {
    switch(motor){
      case 0:
        Servo1.easeTo(targetUnmapped);
        break;
      case 1:
        Servo2.easeTo(targetUnmapped);
        break;
      case 2:
        Servo3.easeTo(targetUnmapped);
        break;
      case 3:
        Servo4.easeTo(targetUnmapped);
        break;
      case 4:
        Servo5.easeTo(targetUnmapped);
        break;
      case 5:
        Servo6.easeTo(targetUnmapped);
        break;
      case 6:
        Servo7.easeTo(targetUnmapped);
        break;
      case 7:
        Servo8.easeTo(targetUnmapped);
        break;
    }
  }
}

void FallenOver(){
  setSpeedForAllServos(random(10,20));
  byte randomness = 3;
  float range = 0.5;
  if(random(randomness)==1){Servo1.setEaseTo(random(SERVO_MIN[0]*(1-range),SERVO_MAX[0]*range));}
  if(random(randomness)==1){Servo2.setEaseTo(random(SERVO_MIN[1]*range,SERVO_MAX[1]*(1-range)));}
  if(random(randomness)==1){Servo3.setEaseTo(random(SERVO_MIN[2]*range,SERVO_MAX[2]*(1-range)));}
  if(random(randomness)==1){Servo4.setEaseTo(random(SERVO_MIN[3]*(1-range),SERVO_MAX[3]*range));}
  if(random(randomness)==1){Servo5.setEaseTo(random(SERVO_MIN[4]*range,SERVO_MAX[4]*(1-range)));}
  if(random(randomness)==1){Servo6.setEaseTo(random(SERVO_MIN[5]*range,SERVO_MAX[5]*(1-range)));}
  if(random(randomness)==1){Servo7.setEaseTo(random(SERVO_MIN[6]*(1-range),SERVO_MAX[6]*range));}

  synchronizeAllServosStartAndWaitForAllServosToStop();
  int delayLoops = random(500,2000);
  for(byte i=0; i<delayLoops/40;i++){
    TestIfFallenOver;
  }
}

void MoveTo(byte motor,  long target){
  byte targetUnmapped = map(target, 0, SERVO_MAP_VALUE, SERVO_MIN[motor], SERVO_MAX[motor]);
  //Serial.println(targetUnmapped);
  if (targetUnmapped <= SERVO_MAX[motor] && targetUnmapped >= SERVO_MIN[motor] || targetUnmapped >= SERVO_MAX[motor] && targetUnmapped <= SERVO_MIN[motor]) {
    servoPositionRaw[motor] = target;
    servoPosition[motor] = targetUnmapped;

    switch(motor){
      case 0:
        Servo1.write(servoPosition[0]);
        break;
      case 1:
        Servo2.write(servoPosition[1]);
        break;
      case 2:
        Servo3.write(servoPosition[2]);
        break;
      case 3:
        Servo4.write(servoPosition[3]);
        break;
      case 4:
        Servo5.write(servoPosition[4]);
        break;
      case 5:
      Serial.println(servoPosition[5]);
        Servo6.write(servoPosition[5]);
        break;
      case 6:
        Servo7.write(servoPosition[6]);
        break;
      case 7:
        Servo8.write(servoPosition[7]);
        break;
    }  
  }
}

void MonitorBattery(){
  int tot=0;
  for(byte i = 0;i<20;i+=0){
    int temp = analogRead(BATTERY_MONITOR);
    if (temp >0){
      tot += temp;
      i++;
    }
    delay(2);
  }
  int batLevel = map((tot^2)/(21^2),BATTERY_MIN^2,BATTERY_MAX^2,0,100);
  Serial.print("Battery Level: ");
  Serial.println(batLevel);

  if(batLevel > 100){
    digitalWrite(STATUS_LED_G,HIGH);
    digitalWrite(STATUS_LED_R,LOW);
  } else if (batLevel <= 0){
    LowPowerMode();
  } else{
  analogWrite(STATUS_LED_G, map(batLevel,0,100,0,255));
  analogWrite(STATUS_LED_R, map(batLevel,0,100,255,0));
  }
}

void LowPowerMode(){
  digitalWrite(STATUS_LED_G,LOW);
  digitalWrite(MOSFET,LOW);
  while(1){
    digitalWrite(STATUS_LED_R,HIGH);
    delay(700);
    digitalWrite(STATUS_LED_R,LOW);
    delay(700);
  }
}

void LilDance(){
  setSpeedForAllServos(22);
  setEasingTypeForAllServos(EASE_BACK_IN);
  
  long timerStart = millis();
  Servo1.startEaseTo(map(7500, 0, SERVO_MAP_VALUE, SERVO_MIN[0], SERVO_MAX[0]));
  Servo2.startEaseTo(map(4800, 0, SERVO_MAP_VALUE, SERVO_MIN[1], SERVO_MAX[1]));
  Servo3.startEaseTo(map(7000, 0, SERVO_MAP_VALUE, SERVO_MIN[2], SERVO_MAX[2]));
  Servo4.startEaseTo(map(4300, 0, SERVO_MAP_VALUE, SERVO_MIN[3], SERVO_MAX[3]));

  updateAndWaitForAllServosToStop();
  delay(50);

  Serial.println(millis() - timerStart);
  
  timerStart = millis();
  Servo1.startEaseTo(map(6000, 0, SERVO_MAP_VALUE, SERVO_MIN[0], SERVO_MAX[0]));
  Servo2.startEaseTo(map(3300, 0, SERVO_MAP_VALUE, SERVO_MIN[1], SERVO_MAX[1]));
  Servo3.startEaseTo(map(8000, 0, SERVO_MAP_VALUE, SERVO_MIN[2], SERVO_MAX[2]));
  Servo4.startEaseTo(map(5300, 0, SERVO_MAP_VALUE, SERVO_MIN[3], SERVO_MAX[3]));

  updateAndWaitForAllServosToStop();
  delay(50);
  Serial.println(millis() - timerStart);
}
