using namespace std; 
#ifndef lcd_hpp
#define lcd_hpp

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <pcf8574.h>
#include <lcd.h>
#include <time.h>

 
class MYLCD {
public:
    MYLCD();
    void printCPUTemperature();
    void printDataTime();
    void printDHT11(float humidity, float temperature);
    int detectI2C(int addr);

private:
    int lcdhd;
    int pcf8574_address;
};

#endif