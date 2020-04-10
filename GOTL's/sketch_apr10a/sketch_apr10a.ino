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

// --- Constants ---

const uint16_t motorSpeed = 200;
const uint16_t turnSpeed = 100;
const int acceleration = 1;

// --- Global Variables ---

int curSpeed = 0;
float COUNTPERCM = 100.0;

// Go forward
void forward(int16_t Zspeed, float distance) {
  int countMax = int (COUNTPERCM * distance);
  while(encoders.getCountsLeft()<countMax) {
    motors.setSpeeds(Zspeed,Zspeed);
    delay(100);
  }
  motors.setSpeeds(0,0);
  encoders.getCountsAndResetLeft();
}

void moveForward() {
  turnSensorUpdate();
  motors.setSpeeds(motorSpeed,motorSpeed);
}

// Back up!
void reverse(int16_t Zspeed, float distance) {
  int countMax = int (COUNTPERCM * distance);
  while(encoders.getCountsLeft()<countMax) {
    motors.setSpeeds(-Zspeed,-Zspeed);
    delay(100);
  }
  motors.setSpeeds(0,0);
  encoders.getCountsAndResetLeft();
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

  buttonA.waitForButton();
}

// --- Main Loop ---
void loop() {

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


  int i = encoders.getCountsAndResetRight();
  while (centerLeftSensor<=6&&centerRightSensor<=6) {
    forward(100, 5.0);
  }
  forward(100, 15.0);
  stop();
  int recorded_left_length = i;
  delay(50000);
  
}
