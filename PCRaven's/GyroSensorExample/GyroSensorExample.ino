#include <Wire.h>
#include <Zumo32U4.h>
#include "TurnSensor.h"

L3G gyro;
Zumo32U4LCD lcd;
Zumo32U4Motors motors;
Zumo32U4ButtonA buttonA;

// --- Helper functions
int32_t getAngle() {
  // turnAngle is a variable defined in TurnSensor.cpp
  // This fancy math converts the number into degrees turned since the
  // last sensor reset.
  return (((int32_t)turnAngle >> 16) * 360) >> 16;
}
// This enum is used for our "state machine."
enum State {
  pause_state,
  forward_left_state,
  forward_right_state,
  scan_left_state,
  scan_right_state,
  reverse_state,
  forward_state
};

// --- Constants ---

const uint16_t motorSpeed = 200;
const uint16_t turnSpeed = 200;
const int acceleration = 1;

// --- Global Variables ---

State state = pause_state;
int curSpeed = 0;

// --- Setup Method
void setup() {
  turnSensorSetup();
  delay(500);
  turnSensorReset();
  lcd.clear();
}

// --- Main program loop
void loop() {
  
  // Read the sensors
  turnSensorUpdate();
  int32_t angle = getAngle();

  // Update the display
  lcd.gotoXY(0, 0);
  lcd.print(angle);
  lcd.print(" ");
}

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
