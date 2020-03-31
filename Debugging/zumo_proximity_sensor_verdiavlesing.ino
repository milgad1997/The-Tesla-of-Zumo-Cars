#include <Zumo32U4.h> 

Zumo32U4ProximitySensors proxSensors;


void setup()
{
    Serial.begin(115200);
    int leftReading;
    int rightReading;
}
 
void loop()
{
    proxSensors.read();

    leftReading = proxSensors.countsFrontWithLeftLeds();
    rightReading = proxSensors.countsFrontWithRightLeds();

    Serial.print("Left: ")
    Serial.print(leftReading);
    Serial.print("\tRight: ")
    Serial.println(rightReading);

    delay(200);
}