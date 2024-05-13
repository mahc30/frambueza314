
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <pcf8574.h>
#include <lcd.h>
#include <time.h>
#include "DHT.hpp"

#define DHT11_Pin  0
#define  redLedPin    3	
#define  greenLedPin  29

DHT::DHT(){
    wiringPiSetup();
}
//Function: Read DHT sensor, store the original data in bits[]
// return values:DHTLIB_OK   DHTLIB_ERROR_CHECKSUM  DHTLIB_ERROR_TIMEOUT
int DHT::readSensor(int pin,int wakeupDelay){
	int mask = 0x80;
	int idx = 0;
	int i ;
	int32_t t;
	for (i=0;i<5;i++){
		bits[i] = 0;
	}
	// Clear sda
	pinMode(pin,OUTPUT);
	digitalWrite(pin,HIGH);
	delay(500);
	// Start signal
	digitalWrite(pin,LOW);
	delay(wakeupDelay);
	digitalWrite(pin,HIGH);
	// delayMicroseconds(40);
	pinMode(pin,INPUT);
	
	int32_t loopCnt = DHTLIB_TIMEOUT;
	t = micros();
	// Waiting echo
	while(1){
		if(digitalRead(pin)==LOW){
			break;
		}
		if((micros() - t) > loopCnt){
			return DHTLIB_ERROR_TIMEOUT;
		}
	}
	
	loopCnt = DHTLIB_TIMEOUT;
	t = micros();
	// Waiting echo low level end
	while(digitalRead(pin)==LOW){
		if((micros() - t) > loopCnt){
			return DHTLIB_ERROR_TIMEOUT;
		}
	}
	
	loopCnt = DHTLIB_TIMEOUT;
	t = micros();
	// Waiting echo high level end
	while(digitalRead(pin)==HIGH){
		if((micros() - t) > loopCnt){
			return DHTLIB_ERROR_TIMEOUT;
		}
	}
	for (i = 0; i<40;i++){
		loopCnt = DHTLIB_TIMEOUT;
		t = micros();
		while(digitalRead(pin)==LOW){
			if((micros() - t) > loopCnt)
				return DHTLIB_ERROR_TIMEOUT;
		}
		t = micros();
		loopCnt = DHTLIB_TIMEOUT;
		while(digitalRead(pin)==HIGH){
			if((micros() - t) > loopCnt){
				return DHTLIB_ERROR_TIMEOUT;
			}
		}
		if((micros() - t ) > 60){
			bits[idx] |= mask;
		}
		mask >>= 1;
		if(mask == 0){
			mask = 0x80;
			idx++;
		}
	}
	pinMode(pin,OUTPUT);
	digitalWrite(pin,HIGH);
	//printf("bits:\t%d,\t%d,\t%d,\t%d,\t%d\n",bits[0],bits[1],bits[2],bits[3],bits[4]);
	return DHTLIB_OK;
}
//Function：Read DHT sensor, analyze the data of temperature and humidity
//return：DHTLIB_OK   DHTLIB_ERROR_CHECKSUM  DHTLIB_ERROR_TIMEOUT
int DHT::readDHT11Once(int pin){
	int rv ; 
	uint8_t sum;
	rv = readSensor(pin,DHTLIB_DHT11_WAKEUP);
	if(rv != DHTLIB_OK){
		humidity = DHTLIB_INVALID_VALUE;
		temperature = DHTLIB_INVALID_VALUE;
		return rv;
	}
	humidity = bits[0];
	temperature = bits[2] + bits[3] * 0.1;
	sum = bits[0] + bits[1] + bits[2] + bits[3];
	if(bits[4] != sum)
		return DHTLIB_ERROR_CHECKSUM;
	return DHTLIB_OK;
}

int DHT::readDHT11(int pin){
	int chk = DHTLIB_INVALID_VALUE;
	for (int i = 0; i < 15; i++){
		chk = readDHT11Once(pin);	//read DHT11 and get a return value. Then determine whether data read is normal according to the return value.
		if(chk == DHTLIB_OK){
			return DHTLIB_OK;
		}
		delay(100);
	}
	return chk;
}

// DHT
int pcf8574_address = 0x27;        // PCF8574T:0x27, PCF8574AT:0x3F
#define BASE 64         // BASE any number above 64
//Define the output pins of the PCF8574, which are directly connected to the LCD1602 pin.
#define RS      BASE+0
#define RW      BASE+1
#define EN      BASE+2
#define LED     BASE+3
#define D4      BASE+4
#define D5      BASE+5
#define D6      BASE+6
#define D7      BASE+7

int lcdhd;// used to handle LCD
void printCPUTemperature(){// sub function used to print CPU temperature
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
void printDataTime(){//used to print system time 
    time_t rawtime;
    struct tm *timeinfo;
    time(&rawtime);// get system time
    timeinfo = localtime(&rawtime);//convert to local time
    printf("%s \n",asctime(timeinfo));
    lcdPosition(lcdhd,0,1);// set the LCD cursor position to (0,1) 
    lcdPrintf(lcdhd,"Time:%02d:%02d:%02d",timeinfo->tm_hour,timeinfo->tm_min,timeinfo->tm_sec); //Display system time on LCD
}

void printDHT11(float humidity, float tmp){
	lcdPosition(lcdhd,0,0);
    lcdPrintf(lcdhd, "H: %.2f %%", humidity);
	lcdPosition(lcdhd,0,1);
    lcdPrintf(lcdhd, "T: %.2f *C", tmp);
}

int detectI2C(int addr){
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
int main(){
    DHT dht;
    int chk, counts;		//chk:read the return value of sensor; sumCnt:times of reading sensor
    int i; 
    pinMode(redLedPin, OUTPUT);
    pinMode(greenLedPin, OUTPUT);
    
    if(detectI2C(0x27)){
        pcf8574_address = 0x27;
    }else if(detectI2C(0x3F)){
        pcf8574_address = 0x3F;
    }else{
        printf("No correct I2C address found, \n"
        "Please use command 'i2cdetect -y 1' to check the I2C address! \n"
        "Program Exit. \n");
        return -1;
    }
    pcf8574Setup(BASE,pcf8574_address);//initialize PCF8574
    for(i=0;i<8;i++){
        pinMode(BASE+i,OUTPUT);     //set PCF8574 port to output mode
    } 
    digitalWrite(LED,HIGH);     //turn on LCD backlight
    digitalWrite(RW,LOW);       //allow writing to LCD
	lcdhd = lcdInit(2,16,4,RS,EN,D4,D5,D6,D7,0,0,0,0);// initialize LCD and return “handle” used to handle LCD
    if(lcdhd == -1){
        printf("lcdInit failed !");
        return 1;
    }    while (1){
        counts++; //counting number of reading times
        //printf("Measurement counts : %d \n", counts);
        for (int i = 0; i < 15; i++){
            chk = dht.readDHT11(DHT11_Pin);	//read DHT11 and get a return value. Then determine whether data read is normal according to the return value.
            if(chk == DHTLIB_OK){
                //printf("DHT11,OK! \n");
                break;
            }
            delay(100);
        }
        printDHT11(dht.humidity,dht.temperature);
		
        if(dht.humidity > 70 || dht.temperature > 27){      
		    digitalWrite(redLedPin, HIGH);
		    digitalWrite(greenLedPin, LOW); 
        }
        else{
		    digitalWrite(redLedPin, LOW);
		    digitalWrite(greenLedPin, HIGH);
        }
        delay(1000);
    }

   return 0;
}