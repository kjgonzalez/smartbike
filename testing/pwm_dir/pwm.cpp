/*
practice creating a pwm signal

compile: g++ pwm.cpp -l wiringPi -l pthread

*/

#include <iostream>
#include <wiringPi.h>
#include <softTone.h>

void note(int freq, int dur_ms,int pin = 9){
    softToneWrite(pin,freq);
    delay(dur_ms);
}

int main(int argc, char **argv){
    int pin = argc>1 ? std::stoi(argv[1]) : 9;
    int freq = argc>2 ? std::stoi(argv[2]) : 262;
    wiringPiSetupGpio();
    pinMode(pin,OUTPUT);
    softToneCreate(pin);
    if(argc==1){
        // play special and exit
        note(587,161);
        note(784,323);
        note(0,50);
        note(784,161);
        note(622,161);
        note(784,323);
        note(659,161);
        note(698,161);
        note(587,161);
        note(0,10);
        return 0;
    }
    std::printf("pin: %d, freq: %d\n",pin, freq);
    softToneWrite(pin,freq);
    delay(100);
    softToneWrite(pin,0);
    delay(100);
    softToneWrite(pin,freq);
    delay(100);
    //softToneWrite(pin,349);
    //delay(500);
    //softToneWrite(pin,392);
    //delay(500);
    //softToneWrite(pin,440);
    //delay(500);
    //softToneWrite(pin,494);
    //delay(500);
    //softToneWrite(pin,523);
    //delay(500);

    //softToneWrite(pin,0);
    return 0;
}



// eof
