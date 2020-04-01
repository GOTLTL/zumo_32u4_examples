#include <Wire.h>
#include <Zumo32U4.h>

Zumo32U4LCD lcd;
Zumo32U4ProximitySensors proxSensors;

// --- set up proxSensors
void setup()
{
  proxSensors.initThreeSensors();
  proxSensors.setPeriod(420);
  proxSensors.setPulseOnTimeUs(421);
  proxSensors.setPulseOffTimeUs(300);
}

// --- Main program loop
void loop() {

  // --- Read the sensors
  proxSensors.read();    
  uint8_t left_sensor = proxSensors.countsLeftWithLeftLeds();
  uint8_t center_left_sensor = proxSensors.countsFrontWithLeftLeds();
  uint8_t center_right_sensor = proxSensors.countsFrontWithRightLeds();
  uint8_t right_sensor = proxSensors.countsRightWithRightLeds();

  // Update the display
  lcd.gotoXY(0, 1);
  lcd.print(left_sensor);
  lcd.print(" ");
  lcd.print(center_left_sensor);
  lcd.print(" ");
  lcd.print(center_right_sensor);
  lcd.print(" ");
  lcd.print(right_sensor);
}
