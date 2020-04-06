/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
//Include sensor- and servo-libraries here

char auth[] = "YourAuthToken";
char ssid[] = "YourNetworkName";
char pass[] = "YourPassword";

BlynkTimer timer;



class Sensor
{
    protected:

        byte pin;

    public:

        void setPin(byte pinNumber) {
            pin = pinNumber;
            pinMode(pinNumber, INPUT);
        }

        void read() {
            analogRead(pin);
        }
};


class TMP36TemperatureSensor: public Sensor
{
    public:
    
        TMP36TemperatureSensor(byte pinNumber) {
            setPin(pinNumber);
        }

        //More functions related to this class
};


class HCSR04UltrasonicSensor: public Sensor
{
    private:

        byte trigger;

    public:

        HCSR04UltrasonicSensor(byte echoPin, byte trigPin)
        {
            setPin(echoPin);

            trigger = trigPin;
            pinMode(trigPin, OUTPUT);
        }

        //More functions related to this class
};


class VL6180XTimeOfFlight
{
    private:

    public:

        //More functions related to this class
};




// This function sends Arduino's up time every second to Virtual Pin (5).
// In the app, Widget's reading frequency should be set to PUSH. This means
// that you define how often to send data to Blynk App.
void myTimerEvent1()
{
  // You can send any value at any time.
  // Please don't send more that 10 values per second.
  Blynk.virtualWrite(V5, millis() / 1000);
}


void myTimerEvent2()
{
  // You can send any value at any time.
  // Please don't send more that 10 values per second.
  Blynk.virtualWrite(V5, millis() / 60000);
}


// This function will be called every time widget
// in Blynk app writes values to the Virtual Pin V1
BLYNK_WRITE(V1)
{
  int pinValue = param.asInt(); // assigning incoming value from pin V1 to a variable

  // process received value
}


// This function is called when there is a widget
// which is requesting data from Virtual Pin V5
BLYNK_READ(V5)
{
  // This command writes Arduino's uptime in seconds to Virtual Pin (5)
  Blynk.virtualWrite(V5, millis() / 1000);
}


void setup()
{
  // Debug console
  Serial.begin(9600);

  Blynk.begin(auth, ssid, pass);
  // You can also specify server:
  //Blynk.begin(auth, ssid, pass, "blynk-cloud.com", 80);
  //Blynk.begin(auth, ssid, pass, IPAddress(192,168,1,100), 8080);

  // Setup a function to be called every second and minute
  timer.setInterval(1000L, myTimerEvent1);
  timer.setInterval(60000L, myTimerEvent2);
}


void loop()
{
  Blynk.run();
  timer.run(); // Initiates BlynkTimer
}
