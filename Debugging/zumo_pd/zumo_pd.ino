#include <Zumo32U4.h>               //Importerer Zumo-biblioteket
#include <Wire.h>
#include <LSM303.h>

Zumo32U4Motors motors;              //Oppretter instans av motorane
Zumo32U4Encoders encoders;          //Oppretter instans av kodarane
Zumo32U4LineSensors lineSensors;    //Oppretter instans av linjesensorane
Zumo32U4ButtonA buttonA;            //Oppretter instans av knapp A
Zumo32U4ButtonB buttonB;            //Oppretter instans av knapp A
Zumo32U4ButtonC buttonC;            //Oppretter instans av knapp A
Zumo32U4LCD lcd;                    //Oppretter instans av LCD-display
Zumo32U4Buzzer buzzer;              //Oppretter instans av buzzeren
LSM303 compass;                     //Oppretter instans av akselerometer
L3G gyro;




class SelfDriving
{
    private:

        unsigned int lineSensorValues[5];   //Verdien til kvar linjesensor

        int leftSpeed;
        int rightSpeed;

        int PD(int input, int last, int target, int speed)
        {
            int error = target - input;

            int adjust = 2.0*error*((float)speed/(float)target) + (error-last)*0.5;

            int left = constrain(speed - adjust, -400, 400);
            int right = constrain(speed + adjust, -400, 400);

            motors.setSpeeds(left, right);

            return error;
        }

    public:

        void calibrateSensors() 
        {
            lineSensors.initFiveSensors();                  //Starter 5-linjesensorkonfigurasjonen

            for (int t = 0; t <= 200; t++) {                //Varer i 4000ms der t er tid i ms

                lineSensors.calibrate();                    //Kalibrerer sensor

                int speed = 400*sin(PI/100*t);              //Farten er beskrive av sinus-uttrykk med periode 4000ms
                
                motors.setSpeeds(speed, -speed);            //Bilen roterer med varierande fart
                delay(1);                                   //Delay for å styre tida
            }
        }

        void followLinePD(int speed)
        {
            static int last = 0;
            int position = lineSensors.readLine(lineSensorValues);          //Reads position from lineSensors
            last = PD(position, last, 2000, speed);                         //Adjusts based on position and stores return value
        }
};

SelfDriving drive;                  //Instans for sjølvkjøring


void setup() 
{
    buttonA.waitForPress();
    delay(1000);
    drive.calibrateSensors();
}



void loop()
{

    drive.followLinePD(200);
    
}