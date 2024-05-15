#include "mylcd.h"
#include <wiringPi.h>   // Include for WiringPi
#include "DHT.hpp"
#include "led.hpp"

int main(){
    DHT dht;
    MYLCD lcd;
    LED led;
    
    while (1){
        
        lcd.printDHT11(dht.humidity,dht.temperature);
        dht.readDHT11(0); 
        if(dht.humidity > 70 || dht.temperature > 27){      
            led.turnOffGreen();
            led.turnOnRed();
        }
        else{
            led.turnOnGreen();
            led.turnOffRed();
        }
        delay(1000);
    }

   return 0;
}