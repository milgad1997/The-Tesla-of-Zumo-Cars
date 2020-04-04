#include <Zumo32U4.h>               //Importerer Zumo-biblioteket

Zumo32U4Motors motors;              //Oppretter instans av motorane
Zumo32U4Encoders encoders;          //Oppretter instans av kodarane
Zumo32U4LineSensors lineSensors;    //Oppretter instans av linjesensorane
Zumo32U4ButtonA buttonA;            //Oppretter instans av knapp A
Zumo32U4ButtonB buttonB;            //Oppretter instans av knapp A
Zumo32U4ButtonC buttonC;            //Oppretter instans av knapp A
Zumo32U4LCD lcd;                    //Oppretter instans av LCD-display
Zumo32U4Buzzer buzzer;              //Oppretter instans av buzzeren



class SelfDriving
{
    private:

        unsigned int lineSensorValues[5];   //Verdien til kvar linjesensor

        int leftSpeed;
        int rightSpeed;


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
            //delay(800*abs(degrees)/360);                         //Går ut ifrå 800ms ved 200 gir 360 graders rotasjon
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


        void followLine(int batteryLevel = 100, bool emergencyPower = false, bool fastMode = false)
        {
            int value = lineSensors.readLine(lineSensorValues);         //Leser av posisjonen til zumoen 
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

            if (batteryLevel <= 10 && !emergencyPower) {//Viss batterinivået er 10% og nødbatteri ikkje er aktivert
                leftSpeed = 0;                          //Setter fartane til null
                rightSpeed = 0;
            }
            
            motors.setSpeeds(leftSpeed, rightSpeed);    //Setter fart til utrekna, korrigerte verdiar
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
};



class Interface
{
    private:

        bool force = true;      //Variable that controls whether or not buttons are required to prompt configuration.


    public:

        int* command()
        {
            static int config[] = {0, 0};                                       //Array stores mode and configuration.

            char* modes[] = {                                                   //Array with names of modes.
                "Calibrate linesensors",
                "Line follower",
                "Object follower",
                "Square",
                "Circle",
                "Back and forth",
                "Slalom"    
            };

            if (usbPowerPresent() && (buttonA.isPressed() || buttonC.isPressed() || force)) {   //Prompts configuration in serial monitor.
                if (!Serial) Serial.begin(9600);                                                //Initiates serial monitor.
                while (!Serial);                                                                //Waits for serial communication.

                lcd.clear();

                Serial.print(                                                                   //Prints selection of modes on monitor.
                    "Select mode:\n\n"
                    "q: Quit serial communication\n"
                    "0: Calibrate linesensors\n"
                    "1: Line follower\n"
                    "2: Object follower\n"
                    "3: Square\n"
                    "4: Circle\n"
                    "5: Back and forth\n"
                    "6: Slalom\n\n"
                    );
                
                while (Serial) {
                    if (Serial.available()) {                                           //If received bytes larger than zero.
                        config[0] = Serial.parseInt();                                  //Reads integer.

                        switch (config[0]) {
                            case 'q':
                                Serial.print("Quiting...\n\n");
                                Serial.end();                                           //Ends serial communication.
                                break;

                            case 1:                                                     //Prompts line follower configuration.
                                Serial.print(                                           //Prints selection of configurations.
                                    "Configure mode:\n\n"
                                    "   0: Normal\n"
                                    "   1: PD-regulated\n\n"
                                    "Selected configuration: "
                                    );
                                break;

                            case 6:                                                     //Prompts slalom configuration.
                                Serial.print("Enter centimeters between cones: ");      //Asks for distance.
                                break;
                        }

                        while (Serial && (config[0] == 1 || config[0] == 6)) {          //If mode need configuration.
                            if (Serial.available()) {
                                config[1] = Serial.parseInt();                          //Stores configuration.

                                Serial.print(config[1]);                                //Prints configuration to monitor.
                                break;
                            }
                        }
                        
                        Serial.print(
                            "\n\nConfiguration done.\n\n"
                            "Press button B to activate.\n\n"
                            );       
                        lcd.print("Ready.");

                        buttonB.waitForRelease();   //Waits for button B before initializing configuration.
                        break;        
                    }
                }
            }

            if (buttonB.getSingleDebouncedRelease()) {                          //Pauses if button B is pressed.
                lcd.clear();
                lcd.print("B to continue");
                lcd.gotoXY(0, 1);
                lcd.print("A or C to configure");
                
                while (true) {                                                  //Continuously checks if somthing happens.
                    if (buttonB.getSingleDebouncedRelease()) return config;     //Continues if button B is pressed again.
                    if (buttonA.isPressed() || buttonC.isPressed()) break;      //Any other button prompts selection of modes.
                } 
            }
            
            if (buttonA.getSingleDebouncedRelease() || buttonC.getSingleDebouncedRelease() || force) {  //Prompts selection of modes.
                config[1] = 0;                                                                          //Resets configuration.

                lcd.clear();
                lcd.gotoXY(0, 1);
                lcd.print("<A B^ C>");
                lcd.gotoXY(0, 0);

                while (true) {                                                          //Continuously checks if somthing happens.                                                     
                    lcd.print(modes[config[0]]);                                        //Prints current mode.

                    if (buttonA.getSingleDebouncedRelease()) config[0]--;               //Toggles back to previous mode.
                    if (buttonC.getSingleDebouncedRelease()) config[0]++;               //Toggles forward to following mode.
                    if (buttonB.getSingleDebouncedRelease()) {                          //Chooses current mode.
                        while (config[0] == 1) {                                        //Prompts line follower configuration.
                            if (config[1] == 0) lcd.print("Normal");                    //Prints current configuration.
                            if (config[1] == 1) lcd.print("PID");
                                        
                            if (buttonA.getSingleDebouncedRelease()) config[1]--;       //Toggles back to previous configuration.
                            if (buttonC.getSingleDebouncedRelease()) config[1]++;       //Toggles forward to following configuration.
                            if (buttonB.getSingleDebouncedRelease()) break;             //Chooses current configuration.

                            if (config[1] < 0) config[1] = 1;                           //Toggles to rightmost configuration.
                            if (config[1] > 1) config[1] = 0;                           //Toggles to leftmost configuration.
                        }

                        while (config[0] == 6) {                                        //Prompts slalom configuration.
                            lcd.print("cm: ");
                            lcd.gotoXY(4, 0);
                            lcd.print(config[1]);                                       //Prints current distance between cones.
                            lcd.gotoXY(0, 0);

                            if (buttonA.getSingleDebouncedRelease()) config[1] -= 10;   //Increments distance between cones.
                            if (buttonC.getSingleDebouncedRelease()) config[1] += 10;   //Decrements distance between cones.
                            if (buttonB.getSingleDebouncedRelease()) break;             //Chooses current distance.
                                
                            if (config[1] < 0) config[1] = 100;                         //Toggles to rightmost configuration.
                            if (config[1] > 100) config[1] = 0;                         //Toggles to leftmost configuration.
                        }

                        break;                                                          //Leaves selection of modes.
                    }

                    if (config[0] < 0) config[0] = 6;                                   //Toggles to rightmost mode.
                    if (config[0] > 6) config[0] = 0;                                   //Toggles to leftmost mode.
                }
            }
            
            if (force) force = false;               //Turns of forcing.
            return config;                          //Returns pointer to array that stores configuration.
        }


        void printMessage(char message[])
        {
            lcd.clear();

            for (byte x = 0, y = 0; x < constrain(sizeof(message)-1, 0, 16); x++)   //Itererer gjennom stringen
            {
                if (x >= 8) {                                   //Viss stringen er lenger enn første rad
                    y = 1;                                      //Starter på neste rad.
                    lcd.gotoXY(x-8, y);                         
                    lcd.print(message[x]);                      //Printer bokstav
                }
                else {
                    lcd.gotoXY(x, y);
                    lcd.print(message[x]);                      //Printer bokstav
                }
            }
        }


        void enableForceConfig()
        {
            force = true;           //Forces to configure a single time after call.
        }       
};



SelfDriving drive;                  //Instans for sjølvkjøring
Interface intf;                     //Instans for brukargrensesnitt



void setup() {}



void loop()
{
    int* config = intf.command();

    switch (config[0]) {
        case 0:
            drive.calibrateSensors();                                   //Kalibrerer sensorane på kommando
            intf.enableForceConfig();
            break;

        case 1:
            drive.followLine();                   //Korrigerer retning basert på posisjon

            lcd.clear();
            if (config[1] == 0) lcd.print("Normal");
            if (config[1] == 1) lcd.print("PD");
            break;

        case 2:
            //Konfigurer objektfølgar
            break;
        
        case 3:
            drive.square();
            intf.enableForceConfig();
            break;

        case 4:
            drive.circle();
            intf.enableForceConfig();
            break;

        case 5:
            drive.backAndForth();
            intf.enableForceConfig();
            break;

        case 6:
            drive.slalom();
            intf.enableForceConfig();
            break;

        default:
            break;
    }
}
