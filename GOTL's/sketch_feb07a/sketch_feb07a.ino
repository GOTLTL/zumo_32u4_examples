#include <Wire.h>
#include <Zumo32U4.h>
#include "TurnSensor.h"

Zumo32U4LCD lcd;
Zumo32U4ProximitySensors proxSensors;
Zumo32U4ButtonA buttonA;
Zumo32U4Motors motors;
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
float COUNTPERCM = 100.0

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

// Back up!
void reverse() {
  motors.setSpeeds(-motorSpeed, -motorSpeed);
}

// Look left
void scanLeft() {
  motors.setSpeeds(-curSpeed, curSpeed);
}

// Look right
void scanRight() {
  motors.setSpeeds(curSpeed, -curSpeed);
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

  // Change states
  if (state == pause_state) {
    if (buttonPress) {
      state = forward_state;
      turnSensorReset();
    }
  }
  else if (buttonPress) {
    state = pause_state;
  }
}


  // Sensor display
  lcd.gotoXY(0, 1);
  lcd.print(left_sensor);
  lcd.print(" ");
  lcd.print(centerLeftSensor);
  lcd.print(" ");
  lcd.print(centerRightSensor);
  lcd.print(" ");
  lcd.print(right_sensor);

  delay(2);
}

//MIT License

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
