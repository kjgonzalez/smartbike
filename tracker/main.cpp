/*
tracker program for the bike-mounted pi

implemented functions:
  * total speed (average)
  * total distance
  * total time
  * current speed
  * max speed
  * brake status
  * save data to file
  * move classes / constants / functions to bikeClasses.h

missing functions: 
  * total elevation change
  * current elevation
  * user-controlled start of tracking
  * user-controlled stop of tracking
  * brake light activation
  * save data to BINARY file
  * turn on brake light
  * use turn-lights


*/

#include <iostream> // debugging help
#include <csignal> // help with debugging
#include <fstream>
#include <time.h>
#include <wiringPi.h> // hardware interaction
#include "bikeClasses.h"

// software constants
constexpr bool TOFILE = false;

// hardware constants
constexpr int pinLED = 10;
constexpr int pinWHL = 4;
constexpr int pinBRL = 17;
constexpr int pinBRR = 27;

void isr_wheel(); // declare here to avoid issues
void isr_brake();

// functions
std::string tstamp() {
    /* return current time as a string in format YYYYMMMDD-HHmmSS */
    time_t timer;
    tm* curr_tm;
    time(&timer);
    curr_tm = localtime(&timer);
    char date_string[100];
    strftime(date_string, 50, "%Y%b%d-%H%M%S", curr_tm);
    std::string str(date_string);
    return str;
}

void initialize_hw(){
    // any code directly initializing hardware is initialized here
    wiringPiSetupGpio();
    pinMode(pinLED,OUTPUT);
    pinMode(pinWHL,INPUT);
    pinMode(pinBRL,INPUT);
    pinMode(pinBRR,INPUT);
    wiringPiISR(pinWHL,INT_EDGE_FALLING,isr_wheel);
    wiringPiISR(pinBRL,INT_EDGE_BOTH,isr_brake);
    wiringPiISR(pinBRR,INT_EDGE_BOTH,isr_brake);
}

void announce_on(LedMgr light) {
    /* announce to user that program is on, using available outputs */
    std::printf("Tracker program starting... \n");
    light.blink(3);
}

// global variable / object initializations
LedMgr led(digitalWrite,pinLED); // external led object
bool RUNNING=true; // graceful end via ctrl+c
Spedometer sped(millis); // velocity, distance, and time tracking via hall sensor
Braking brakes(digitalRead,pinBRL,pinBRR);

// functions (separate from other functions)
void isr_ctrlc(int s){
    /* try to exit gracefully, write to hardware directly */
    RUNNING=false;
    std::printf("exiting...\n");
    digitalWrite(pinLED,LOW);
}

void isr_wheel(){ sped.lap(); }
void isr_brake(){ brakes.update(); }

// main function
int main(int argc, char **argv){
    // parse argument(s)
    std::string base_dir = argc > 1 ? argv[1] : "/home/pi/smartbike/output/";
    // initialization step
    initialize_hw();
    announce_on(led);
    std::signal(SIGINT,isr_ctrlc); // enable CTRL+C graceful exit
    CsvSimple f;
    std::printf("planned location: ");
    std::string fname = base_dir + tstamp() + "_out.csv";
    std::printf("%s\n", fname.c_str());
    if(TOFILE){
        f.open("/home/pi/smartbike/output/" + tstamp() + "_out.csv");
        f.addval("Time-"+tstamp()+" (s)");
        f.addval("Dist (m)");
        f.addval("CurrSpeed (m/s)");
        f.addval("brL");
        f.addval("brR");
        f.endl();
    }

    // looping step
    while (RUNNING) {
        if (millis() % 1000 == 0) {
            delay(1); // ensure don't accidentally repeat all tasks if finished in under the loop time
            led.toggle();
            std::printf("%f,%f,%f,%d,%d,\n",
                sped.getTimeTotal(),sped.getDistTotal(),
                sped.getVelCurr(), brakes.stateL, brakes.stateR);

            if (TOFILE) {
                f.addval(sped.getTimeTotal());
                f.addval(sped.getDistTotal());
                f.addval(sped.getVelCurr());
                f.addval(brakes.stateL);
                f.addval(brakes.stateR);
                f.endl();
            }

        } // if-millis
    } // while-loop
    // f.close();

    return 0;
} // main





// eof
