#include <Zumo32U4.h>               //Importerer Zumo-biblioteket

Zumo32U4Motors motors;              //Oppretter instans av motorane
Zumo32U4Encoders encoders;          //Oppretter instans av kodarane
Zumo32U4ButtonA buttonA;            //Oppretter instans av knapp A


int leftSpeed;
int rightSpeed;


void setup() 
{
  buttonA.waitForButton();
  delay(1000);
}


void loop() //Fjern kommentar på testeobjekt
{
  //rotate(360);
  //rotate(-360);
  //rotate(180);
  //rotate(90);

  //square();
  //circle();
  //backAndForth();
  //slalom()

  buttonA.waitForButton();
  delay(1000);
}


void rotate(int degrees)
{
    //Teoretisk count per meter er 909.7(2*pi*r)=7425

    int counts = encoders.getCountsLeft();
    float arcCounts = 0.267*7425*degrees/360;       //Buelengda på 360-rotasjon er 0.267m
            
    rightSpeed = 200*degrees/abs(degrees);          //Endrer forteiknet avhengig av forteikn på vinkel
    leftSpeed = -rightSpeed;

    motors.setSpeeds(0, 0);
    delay(50);

    while (abs(encoders.getCountsLeft() - counts) < abs(arcCounts)) {
        motors.setSpeeds(leftSpeed, rightSpeed);
    }

    motors.setSpeeds(0, 0);
    delay(50);

    //motors.setSpeeds(leftSpeed, rightSpeed);
    //delay(800*abs(degrees)/360);                   //Går ut ifrå 800ms ved 200 gir 360 graders rotasjon
}


void square()
{
    for (byte n = 0; n < 4; n++) {
        motors.setSpeeds(200, 200);
        delay(2000);
        rotate(90);
    }
}


void circle()
{
    motors.setSpeeds(300, 150);
    delay(3000);
    motors.setSpeeds(0, 0);
}


void backAndForth()
{
    motors.setSpeeds(200, 200);
    delay(2000);
           
    rotate(180);
            
    motors.setSpeeds(200, 200);
    delay(2000);
    motors.setSpeeds(0, 0);

}


void slalom()
{
    motors.setSpeeds(0, 0);
    delay(50);
            
    int counts = encoders.getCountsLeft();
    float coneCounts = 0.5*7425;            //Kjegledistanse på 0.5m
    unsigned long time = millis();

    while (encoders.getCountsLeft() - counts < coneCounts) {
        motors.setSpeeds(200, 200);
    }

    time = millis() - time;
            
    for (byte i = 0; i < 10; i++){          //10 kjegler
        rotate(90);
        motors.setSpeeds(200, 200);
        delay(time);
        rotate(-90);
        motors.setSpeeds(200, 200);
        delay(time);
        rotate(-90);
        motors.setSpeeds(200, 200);
        delay(time);
        rotate(90);
    }
}