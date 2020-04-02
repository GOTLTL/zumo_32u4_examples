#include <Wire.h>
#include <Zumo32U4.h>
#include "TurnSensor.h"

Zumo32U4LCD lcd;
Zumo32U4ProximitySensors proxSensors;
Zumo32U4ButtonA buttonA;
Zumo32U4Motors motors;
Zumo32U4Encoders encoders;
LSM303 lsm303; // Accelerometer
L3G gyro;

// --- Enums ---

// This enum is used for our "state machine."
enum State {
  pause_state,
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
float COUNTPERCM = 100.0;

// --- Support Functions ---

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

// Stop
void stop() {
  motors.setSpeeds(0, 0);
}

// Go forward
void forward(float distance) {
  // Get out heading
  turnSensorUpdate();
  int angle = (((int32_t)turnAngle >> 16) * 360) >> 16;
  // Move foward, adjusting motor speed to hold heading
  motors.setSpeeds(curSpeed + (angle * 5), curSpeed - (angle * 5));
}

void moveForward() {
  motors.setSpeeds(curSpeed,curSpeed);
}

// Back up!
void reverse(float distance) {
  turnSensorUpdate();
  int angle = (((int32_t)turnAngle >> 16) * 360) >> 16;
  // Move foward, adjusting motor speed to hold heading
  motors.setSpeeds(-curSpeed + (angle * 5), -curSpeed - (angle * 5));
}

void deposit() {
  motors.setSpeeds(400,400);
  forward(20);
  reverse(10);
  turnLeft(180);
  motors.setSpeeds(200,200);
  forward(10); //now back at the beginning of the box
}

// --- Setup ---
void setup() {

  // Proximity sensors
  proxSensors.initThreeSensors();

  // Accelerometer
  Wire.begin();
  lsm303.init();
  lsm303.enableDefault();

  // Gyrometer
  turnSensorSetup();
  delay(500);
  turnSensorReset();
}

// --- Main Loop ---
void loop() {

  // Buttons
  bool buttonPress = buttonA.getSingleDebouncedPress();

  // Proximity sensors
  proxSensors.read();

  int left_sensor = proxSensors.countsLeftWithLeftLeds();
  int centerLeftSensor = proxSensors.countsFrontWithLeftLeds();
  int centerRightSensor = proxSensors.countsFrontWithRightLeds();
  int right_sensor = proxSensors.countsRightWithRightLeds();

  // Accelerometer
  lsm303.read();
  int16_t x = lsm303.a.x;
  int16_t y = lsm303.a.y;
  int32_t magnitudeSquared = (int32_t)x * x + (int32_t)y * y;

   // record left side length during first left side scan
  turnLeft(90);
  int i = encoders.getCountsAndResetLeft();
  while (centerLeftSensor <= 6) {
    moveForward();
  }
  forward(2);
  stop();
  int recorded_left_length = i;
  delay(500);

  // record vertical length during first vertical length scan
  turnRight(90);
  int j = encoders.getCountsAndResetLeft();
  while (centerLeftSensor <= 6) {
    moveForward();
  }
  forward(2);
  stop();
  int recorded_vertical_length = j;
  delay(500);

  //first return to deposit
  turnRight(180);
  motors.setSpeeds(100,100);
  forward(recorded_vertical_length);
  turnLeft(90);
  forward(recorded_left_length);
  turnRight(90);
  deposit();

  //scoop+scan the full left side
  int a = 0;
  while (a <= 6) {
    turnLeft(90);
    forward(recorded_left_length - a);
    turnRight(90);
    forward(recorded_vertical_length);
    motors.setSpeeds(100,100);
    turnRight(180);
    forward(recorded_vertical_length);
    turnLeft(90);
    forward(recorded_left_length - a);
    turnRight(90);
    deposit();
    a++;
  }

  //scoop+scan middle
  forward(recorded_vertical_length);
  motors.setSpeeds(100,100);
  turnRight(180);
  forward(recorded_vertical_length);
  deposit();

  //record right side length during first right side scan
  turnRight(90);
  int l = encoders.getCountsAndResetRight();
  while (centerRightSensor <= 6) {
    moveForward();
  }
  forward(2);
  stop();
  int recorded_right_length = l;
  delay(500);

  //first scoop and scan right
  turnLeft(90);
  forward(recorded_vertical_length);
  motors.setSpeeds(100,100);
  turnLeft(180);
  forward(recorded_vertical_length);
  turnRight(90);
  forward(recorded_right_length);
  turnLeft(90);
  deposit();

  //scoop+scan the full right side
  int b = 0;
  while (b <= 6) {
    turnRight(90);
    forward(recorded_right_length - b);
    turnLeft(90);
    forward(recorded_vertical_length);
    motors.setSpeeds(100,100);
    turnLeft(180);
    forward(recorded_vertical_length);
    turnRight(90);
    forward(recorded_right_length - b);
    turnLeft(90);
    deposit();
    b++;
  }
}

//MIT License (for the code of turning angles and moving forward (int distance))

//Copyright (c) 2016 Paul V Craven

//Permission is hereby granted, free of charge, to any person obtaining a copy
//of this software and associated documentation files (the "Software"), to deal
//in the Software without restriction, including without limitation the rights
//to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//copies of the Software, and to permit persons to whom the Software is
//furnished to do so, subject to the following conditions:

//The above copyright notice and this permission notice shall be included in all
//copies or substantial portions of the Software.

//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//SOFTWARE.
