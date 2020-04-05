/*
Under arbeid...
*/

#include <Zumo32U4.h>               //Importerer Zumo-biblioteket

Zumo32U4LCD lcd;                    //Oppretter instans av LCD-display

void setup() 
{
    lcd.clear();
    lcd.gotoXY(0, 1);
    lcd.print("<A B^ C>");
    lcd.gotoXY(0, 0;
    lcd.autoscroll();
}

void loop() {}

void print(char* string)
{
    static unsigned long timer = millis();              //Variabel som lagrer tida for siste print
    static int counter;
    static int* ptr = nullptr;

    if (millis() - timer > 100) {                       //Printer ikkje oftare enn kvart 100ms for lesbarhet
        if (counter == 0) {
            lcd.print(string);                               //Printer tal utan desimalar

            counter = 8;
            ptr = &string;
        }

        else if (counter > 7 && string[counter] != "\0") {
            lcd.print(string[counter];
            counter++;
        }

        else counter = 0;
    
        timer = millis();                               //Lagrer tida
    }

    
    
    int size = sizeof(string)/8 - 1;

    if (size > 8) {

    }


}