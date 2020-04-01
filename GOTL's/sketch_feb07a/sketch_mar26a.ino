void setup() {
  // put your setup code here, to run once:

}

void deposit() {
  motors.setSpeed(400,400);
  forward(recorded_length * 2);
  reverse(recorded_length);
  turnLeft(180);
  motors.setSpeed(200,200);
  forward(recorded_length);
}

void loop() {
  
  // record left side length during first left side scan
  turnLeft(90);
  int i = getCountsAndResetLeft();
  while (prox_sensors_frontL <= 6) {
    moveForward();
  }
  foward( insert_specific_length_here);
  stop();
  int recorded_left_length = i;
  delay(500);

  // record vertical length during first vertical length scan
  turnRight(90);
  int j = getCountsAndResetLeft();
  while (prox_sensors_frontL <= 6) {
    moveForward();
  }
  foward( insert_specific_length_here);
  motors.setSpeed(0,0);
  int recorded_vertical_length = j;
  delay(500)

  //first collect and deposit
  turnRight(180);
  motors.setSpeed(100,100);
  forward(recorded_vertical_length);
  turnLeft(90);
  forward(recorded_left_length);
  turnRight(90);
  deposit();  
}
