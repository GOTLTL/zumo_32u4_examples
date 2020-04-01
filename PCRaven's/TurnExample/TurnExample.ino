#include <Wire.h>
#include <Zumo32U4.h>
#include "TurnSensor.h"

Zumo32U4LCD lcd;
L3G gyro;
Zumo32U4Motors motors;
Zumo32U4ButtonA buttonA;
Zumo32U4ButtonB buttonB;
Zumo32U4ButtonC buttonC;
Zumo32U4Encoders encoders;

int turnSpeed = 100;
int motorSpeed = 250;
int16_t countsLeft;
float COUNTPERCM = 100.0;

void moveForward(int16_t Zspeed, float distance){
  int countMax = int (COUNTPERCM * distance);
  int angle = (((int32_t)turnAngle >> 16) * 360) >> 16;
  turnSensorReset();
  while (encoders.getCountsLeft()<countMax) {
    motors.setSpeeds(Zspeed + (angle * 5), Zspeed - (angle * 5));
  }
  motors.setSpeeds(0, 0);
}

void setup() {
  turnSensorSetup();
  delay(500);
  turnSensorReset();
  lcd.clear();
}

// Turn left
void turnLeft(int degrees) {
  turnSensorReset();
  motors.setSpeeds(-turnSpeed, turnSpeed);
  int angle = 0;
  do {
    delay(1);
    turnSensorUpdate();
    angle = (((int32_t)turnAngle >> 16) * 360) >> 16;
    lcd.gotoXY(0, 0);
    lcd.print(angle);
    lcd.print(" ");
  } while (angle < degrees);
  motors.setSpeeds(0, 0);
}

// Turn right
void turnRight(int degrees) {
  turnSensorReset();
  motors.setSpeeds(turnSpeed, -turnSpeed);
  int angle = 0;
  do {
    delay(1);
    turnSensorUpdate();
    angle = (((int32_t)turnAngle >> 16) * 360) >> 16;
    lcd.gotoXY(0, 0);
    lcd.print(angle);
    lcd.print(" ");
  } while (angle > -degrees);
  motors.setSpeeds(0, 0);
}

void loop() {
  turnSensorUpdate();

  // If we press A, turn left.
  bool buttonPress = buttonA.getSingleDebouncedPress();
  if(buttonPress) {
    delay(500);
    turnLeft(179);
  }

  // If we press B, turn right
  buttonPress = buttonB.getSingleDebouncedPress();
  if(buttonPress) {
    delay(500);
    turnRight(180);
  }

  // If we press C, dance a jig
  buttonPress = buttonC.getSingleDebouncedPress();
  if (buttonPress) {
    delay(500);
    countsLeft = encoders.getCountsLeft();
    lcd.clear();
    lcd.print("Counts: ");
    lcd.setCursor(0, 1);
    lcd.print(countsLeft);
    moveForward(50,10.0);
  }
}
