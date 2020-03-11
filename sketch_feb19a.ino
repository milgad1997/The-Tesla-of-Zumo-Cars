#include <Zumo32U4.h>

Zumo32U4Motors motors;
Zumo32U4LineSensors lineSensors;
Zumo32U4ButtonA btnA;
Zumo32U4ButtonB btnB;
Zumo32U4ButtonC btnC;
Zumo32U4LCD lcd;
Zumo32U4Encoders encoders;

const int traverseSpeed = 350;
int lineSensorValues[5];
bool driving = false, stopped = false;
unsigned long timer;
float distance = 0.0;

void updateDisplay(){
  lcd.clear();
  lcd.print(lineSensors.readLine(lineSensorValues));
  lcd.gotoXY(4,0);
  lcd.print(power());
  lcd.gotoXY(0,1);
  lcd.print(getDistance());
  timer = millis();
}

void displayMenu() {
  lcd.clear();
  lcd.print("A-drive");
  lcd.gotoXY(0,1);
  lcd.print("C-calib");
}

float getDistance(){
  int cLeft = encoders.getCountsAndResetLeft(), cRight = encoders.getCountsAndResetRight(), middledDistance;
  if (cLeft < cRight) middledDistance = cLeft + (cRight-cLeft)/2;
  else if (cLeft > cRight) middledDistance = cRight + (cLeft-cRight)/2;
  else middledDistance = cLeft;
  distance += middledDistance/(7425);
  return distance;
}

float power(){
  return 100.0 - getDistance()*10;
}

void sensorCalibrate() {
  delay(500);
  motors.setSpeeds(200, -200);
  timer = millis();
  while (millis() - timer < 1500) lineSensors.calibrate();
  motors.setSpeeds(0,0);
  delay(40);
  motors.setSpeeds(-200, 200);
  while (millis() - timer < 3000) lineSensors.calibrate();
}

void drive() {
  if (power() <= 0) return;
  int sValue = lineSensors.readLine(lineSensorValues);
  if (sValue > 3999 || sValue < 1) {
    if (!stopped) {
      motors.setSpeeds(0,0);
      delay(20);
      stopped = true;
    }
    if (sValue < 2000) motors.setSpeeds(-150, 150);
    else motors.setSpeeds(100, -100);
    return;
  }
  int turnSpeed = 2000-sValue;
  if (turnSpeed < 0) motors.setSpeeds(traverseSpeed, traverseSpeed+turnSpeed/4);
  else motors.setSpeeds(traverseSpeed-turnSpeed/4, traverseSpeed);
  stopped = false;
}

void setup() {
  lineSensors.initFiveSensors();
  displayMenu();
}

void loop() {
  if (driving) {
    drive();
    if (millis() - timer > 500) updateDisplay();
  }
  if (btnA.getSingleDebouncedPress()) {
    driving = !driving;
    delay(500);
    if (!driving) displayMenu();
  }
  if (btnB.getSingleDebouncedPress()) {
    while (getDistance() < 1.0) {
      motors.setSpeeds(200,175);
      updateDisplay();
    } 
    motors.setSpeeds(0,0);
  }
  if (btnC.getSingleDebouncedPress() && !driving) sensorCalibrate();
}
