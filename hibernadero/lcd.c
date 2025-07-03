
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <pcf8574.h>
#include "lcd.h"
#include "mylcd.h"
#include <time.h>

#define MYBASE64    64
#define RS      (MYBASE64 + 0)
#define RW      (MYBASE64 + 1)
#define EN      (MYBASE64 + 2)
#define LED     (MYBASE64 + 3)
#define D4      (MYBASE64 + 4)
#define D5      (MYBASE64 + 5)
#define D6      (MYBASE64 + 6)
#define D7      (MYBASE64 + 7)

int lcdhd;// used to handle LCD
MYLCD::MYLCD(){
    wiringPiSetup();
    int i;  
    int pcf8574_address = 0x27;        // PCF8574T:0x27, PCF8574AT:0x3F

    if(detectI2C(0x27)){
        pcf8574_address = 0x27;
    }else if(detectI2C(0x3F)){
        pcf8574_address = 0x3F;
    }else{
        printf("No correct I2C address found, \n"
        "Please use command 'i2cdetect -y 1' to check the I2C address! \n"
        "Program Exit. \n");
    }
    pcf8574Setup(MYBASE64,pcf8574_address);//initialize PCF8574
    for(i=0;i<8;i++){
        pinMode(MYBASE64+i,OUTPUT);     //set PCF8574 port to output mode
    } 
    digitalWrite(LED,HIGH);     //turn on LCD backlight
    digitalWrite(RW,LOW);       //allow writing to LCD

	lcdhd = lcdInit(2,16,4,RS,EN,D4,D5,D6,D7,0,0,0,0);// initialize LCD and return “handle” used to handle LCD
    if(lcdhd == -1){
        printf("lcdInit failed !");
    }    
}

void MYLCD::printCPUTemperature(){// sub function used to print CPU temperature
    FILE *fp;
    char str_temp[15];
    float CPU_temp;
    // CPU temperature data is stored in this directory.
    fp=fopen("/sys/class/thermal/thermal_zone0/temp","r");
    fgets(str_temp,15,fp);      // read file temp
    CPU_temp = atof(str_temp)/1000.0;   // convert to Celsius degrees
    printf("CPU's temperature : %.2f \n",CPU_temp);
    lcdPosition(lcdhd,0,0);     // set the LCD cursor position to (0,0) 
    lcdPrintf(lcdhd,"CPU:%.2fC",CPU_temp);// Display CPU temperature on LCD
    fclose(fp);
}
void MYLCD::printDataTime(){//used to print system time 
    time_t rawtime;
    struct tm *timeinfo;
    time(&rawtime);// get system time
    timeinfo = localtime(&rawtime);//convert to local time
    printf("%s \n",asctime(timeinfo));
    lcdPosition(lcdhd,0,1);// set the LCD cursor position to (0,1) 
    lcdPrintf(lcdhd,"Time:%02d:%02d:%02d",timeinfo->tm_hour,timeinfo->tm_min,timeinfo->tm_sec); //Display system time on LCD
}

void MYLCD::printDHT11(float humidity, float tmp){
    printf("HUMEDAD: %f", humidity);
	lcdPosition(lcdhd,0,0);
    lcdPrintf(lcdhd, "H: %.2f %%", humidity);
	lcdPosition(lcdhd,0,1);
    lcdPrintf(lcdhd, "T: %.2f *C", tmp);
}

int MYLCD::detectI2C(int addr){
    int _fd = wiringPiI2CSetup (addr);   
    if (_fd < 0){		
        printf("Error address : 0x%x \n",addr);
        return 0 ;
    } 
    else{	
        if(wiringPiI2CWrite(_fd,0) < 0){
            printf("Not found device in address 0x%x \n",addr);
            return 0;
        }
        else{
            printf("Found device in address 0x%x \n",addr);
            return 1 ;
        }
    }
}