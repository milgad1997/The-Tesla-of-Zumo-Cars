#include <Zumo32U4.h>               //Importerer Zumo-biblioteket

Zumo32U4Motors motors;              //Oppretter instans av motorane
Zumo32U4LineSensors lineSensors;    //Oppretter instans av linjesensorane
Zumo32U4ButtonA buttonA;            //Oppretter instans av knapp A


unsigned int lineSensorValues[5];   //Verdien til kvar linjesensor


void setup() 
{
    lineSensors.initFiveSensors();                  //Starter 5-linjesensorkonfigurasjonen

    Serial.begin(9600);
    while(!Serial);

    Serial.println("Press A to calibrate\n");

    buttonA.waitForButton();
    delay(1000);

    Serial.print("Calibrating...\t");
    for (int i = 0; i < 1000; i++) {

        lineSensors.calibrate();

        delay(1);
    }

    Serial.println("Done!\n");
    Serial.println("Press A to test\n");

    buttonA.waitForButton();
    delay(1000);
}


void loop()
{
    Serial.print("Alle: ");
    Serial.print(lineSensors.readLine(lineSensorValues));
    Serial.print("\t");

    Serial.print("1: ");
    Serial.print(lineSensorValues[0]);
    Serial.print("\t");

    Serial.print("2: ");
    Serial.print(lineSensorValues[1]);
    Serial.print("\t");

    Serial.print("3: ");
    Serial.print(lineSensorValues[2]);
    Serial.print("\t");

    Serial.print("4: ");
    Serial.print(lineSensorValues[3]);
    Serial.print("\t");

    Serial.print("5: ");
    Serial.print(lineSensorValues[4]);
    Serial.print("\n");


    if (buttonA.isPressed()) {
        delay(2000);
        buttonA.waitForButton();
        delay(1000);
    }

    delay(200);
}
