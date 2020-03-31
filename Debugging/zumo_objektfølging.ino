#include <Zumo32U4.h>                   //Importerer Zumo-biblioteket

Zumo32U4Motors motors;                  //Oppretter instans av motorane
Zumo32U4Encoders encoders;              //Oppretter instans av kodarane
Zumo32U4LineSensors lineSensors;        //Oppretter instans av linjesensorane
Zumo32U4ButtonA buttonA;                //Oppretter instans av knapp A
Zumo32U4LCD lcd;                        //Oppretter instans av LCD-display
Zumo32U4Buzzer buzzer;                  //Oppretter instans av buzzeren
Zumo32U4ProximitySensors proxSensors;   //Create instance of the proximity sensors

unsigned int lineSensorValues[5];       //Verdien til kvar linjesensor



class SelfDriving
{
    private:

        bool lineInit;                                      //Static bool to save variable value through program 
        bool proxInit;                        
 
 
    public:

        void calibrateSensors() 
        {
            lineInit = false;                               //Linesensors is now configured, no need to do it again 
            proxInit = true;                                //unless prox. sensors config has been run 
 
            lineSensors.initFiveSensors();                  //Starter 5-linjesensorkonfigurasjonen

            for (int t = 0; t <= 200; t++) {                //Varer i 4000ms der t er tid i ms
                lineSensors.calibrate();                    //Kalibrerer sensor

                int speed = 400*sin(PI/100*t);              //Farten er beskrive av sinus-uttrykk med periode 4000ms
                
                motors.setSpeeds(speed, -speed);            //Bilen roterer med varierande fart
                delay(1);                                   //Delay for å styre tida
            }
        }


        void followLine(int value, bool fastMode, int batteryLevel)
        {
            if (lineInit && !proxInit) { 
                lineSensors.initFiveSensors(); 
 
                lineInit = false;                            //Sensor init interlock 
                proxInit = true; 
            } 
 
            int leftSpeed;
            int rightSpeed;

            float batteryCorr = 1.00E+00 - exp(-1.00E-01*batteryLevel); //Korreksjonsfaktor for batterinivå

            if (fastMode) {                                             //Rask modus
                leftSpeed = 4.00E+02 - 8.00E+02*exp(-2.50E-03*value);   //Farten er bestemt av eksponentialfunksjonar
                rightSpeed = 4.00E+02 - 3.63E-02*exp(+2.50E-03*value);
            }
            else {
                leftSpeed =                             //Venstre fart er lik eit 4.gradspolynom der variabelen 
                    -4.00E+02*pow(value, 0)             //er sensorverdien med Df = [0, 4000].
                    +1.04E+00*pow(value, 1)             //Polynomet er stigande for dette intervallet med 
                    -6.63E-04*pow(value, 2)             //verdiar Vf = [-400, 400].
                    +1.80E-07*pow(value, 3)             //Polynomet har terrassepunkt i (2000, 200).
                    -1.67E-11*pow(value, 4);

                rightSpeed =                            //Høgre fart er lik eit 4.gradspolynom der variabelen er
                    +4.00E+02*pow(value, 0)             //sensorverdien med Df = [0, 4000].
                    -1.07E-01*pow(value, 1)             //Polynomet er synkande for dette intervallet med 
                    -1.03E-04*pow(value, 2)             //verdiar Vf = [-400, 400].
                    +8.67E-08*pow(value, 3)             //Polynomet har terrassepunkt i (2000, 200).
                    -1.67E-11*pow(value, 4);
            }

            leftSpeed *= batteryCorr;                   //Korrigerer med batterinivået
            rightSpeed *= batteryCorr;
            
            motors.setSpeeds(leftSpeed, rightSpeed);    //Setter fart til utrekna, korrigerte verdiar
        }


        void followObject()                             //Follow an object within prox. sensor range (~30-40 cm) as it would follow lines on ground
        {
            if (!lineInit && proxInit) { 
                proxSensors.initFrontSensor();          //Need to call this function in order to use the front prox. sensor
                                                        //Can only use front when full line sensor array in use, ref. p. 20 in Pololu User Guide
                proxInit = false;                       //Keep init from running more than once 
                lineInit = true; 
            }                       

            int driveSpeed = 200;
            int turnSpeed = 400;

            proxSensors.read();                                          //Function that reads reflected IR from nearby object, takes ~ 3 ms to run

            int leftReading = proxSensors.countsFrontWithLeftLeds();     //Returns an integer based on reflected IR light
            int rightReading = proxSensors.countsFrontWithRightLeds();

            while ((leftReading + rightReading) == 0) {                  //As long as no object is detected, turn until detected
                proxSensors.read();                                      //Read sensor values for every iteration of the while loop

                motors.setSpeeds(driveSpeed, 0);

                leftReading = proxSensors.countsFrontWithLeftLeds();     //Get value from front left prox. sensor
                rightReading = proxSensors.countsFrontWithRightLeds();   //Get value from front right prox. sensor

                if((leftReading + rightReading) != 0) {                  //If one of the two sensors detected something, stop turning and exit while loop
                    motors.setSpeeds(0,0);
                    delay(30);                                           //Motor safety delay time
                }
            }

            if (((leftReading + rightReading) / 2) == 2) motors.setSpeeds(driveSpeed, driveSpeed);   //Object is close to front of Zumo and directly in front of it, drive towards object

            else if (leftReading >= 4 || rightReading >= 4) motors.setSpeeds(0, 0);                  //Object must be very close to an object, stop motors to prevent damage as preventive measure 

            else if (leftReading > rightReading) motors.setSpeeds(driveSpeed, turnSpeed);            //Object is to the left, turn left 

            else if (leftReading < rightReading) motors.setSpeeds(turnSpeed, driveSpeed);            //Objct is to the right, turn right 
        }
};




SelfDriving drive;                  //Instans for sjølvkjøring


void setup()
{  
    delay(1000);
    drive.calibrateSensors();                                   //Kalibrerer sensorane på kommando
}



void loop()
{
    start:
    
    unsigned long time = millis();

    int position = lineSensors.readLine(lineSensorValues);      //Leser av posisjonen til zumoen

    drive.followLine(position, true, batteryLevel);             //Korrigerer retning basert på posisjon

    if (time > 10000)
    {
        drive.followObject();

        if (buttonA.isPressed()) goto start;
    }
}


