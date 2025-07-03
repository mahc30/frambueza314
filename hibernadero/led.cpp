#define  redLedPin    3	
#define  greenLedPin  29
#include <wiringPi.h>
#include "led.hpp"

LED::LED(){
    wiringPiSetup();
    
    pinMode(redLedPin, OUTPUT);
    pinMode(greenLedPin, OUTPUT);
}

void LED::turnOnRed(){
		    digitalWrite(redLedPin, HIGH);
}

void LED::turnOffRed(){
		    digitalWrite(redLedPin, LOW);
}

void LED::turnOnGreen(){
    digitalWrite(greenLedPin, HIGH);
}

void LED::turnOffGreen(){
	digitalWrite(greenLedPin, LOW);
}