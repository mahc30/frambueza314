#ifndef led_hpp
#define led_hpp

#define  redLedPin    3	
#define  greenLedPin  29

class LED{
    public:
        LED();
        void turnOffGreen();
        void turnOffRed();
        void turnOnRed();
        void turnOnGreen();
    private:
};

#endif