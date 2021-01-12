/*
tracker program for the bike-mounted pi

implemented functions:
  * total speed (average)
  * total distance
  * total time
  * current speed
  * max speed
  * brake status

missing functions: 
  * total elevation change
  * current elevation
  * user-controlled start of tracking
  * user-controlled stop of tracking
  * brake light activation
  * save data to file
  * save data to BINARY file
  * turn on brake light
  * use turn-lights


*/

#include <iostream> // debugging help
#include <csignal> // help with debugging
#include <fstream>
#include <time.h>
#include <wiringPi.h> // hardware interaction

// software constants
constexpr float pi = 3.14159;
constexpr float diam = 0.675; // wheel diameter
constexpr float circ = diam * pi; // wheel circumference
// hardware constants
constexpr int pinLED = 10;
constexpr int pinWHL = 4;
constexpr int pinBRL = 17;
constexpr int pinBRR = 27;

std::string tstamp(); // declare function here for ease of use later
void isr_wheel(); // declare here to avoid issues
void isr_brake();

// classes
struct LedMgr{
    bool state=false;
    int pin=-1;
    void (*pSetter)(int,int) = nullptr;
    LedMgr(void hw_setter(int,int),int pinNum){
        /* hw_setter models after digitalWrite(int pinNum,int value) */
        pSetter = hw_setter;
        pin = pinNum;
    }
    void on(){state=true;pSetter(pin,state);}
    void off(){state=false;pSetter(pin,state);}
    void toggle(){state=!state;pSetter(pin,state);}
};

struct Spedometer{
    unsigned int tprev=0;
    unsigned int elapsed=800000;
    unsigned int tStart = 0;
    float elapsedMin = 8e5; // minimum elapsed time
    int counter=0; // total rotations since start
    unsigned int stopThresh = 2500; // if elapsed>thresh, effectively stopped
    unsigned int (*pMills)() = nullptr;
    Spedometer(unsigned int mills()){
        /* mills models after millis() */
        pMills = mills;
    }
    void lap(){
        /* sensor detects a full rotation */
        counter++; // update 'total' variables
        // update 'current' variables
        elapsed=pMills()-tprev;
        tprev=pMills();
        if(elapsed<elapsedMin) elapsedMin = elapsed;
    }
    void startTimer(){tStart = pMills(); }
    float getVelCurr(){
        /* return current speed, m/s */
        if (counter == 0) return 0; // prevent noise output
        if(pMills()-tprev > stopThresh) return 0; // have detected approximate stop
        else return circ / elapsed * 1000;
    }
    float getDistTotal(){ return circ * counter; } // meters
    float getTimeTotal() { return (pMills()/1000 - tStart/1000); } // seconds
    float getVelTotal(){ return circ * counter * 1000 / (pMills() - tStart); } // m/s, average
    float getVelMax() { return circ / elapsedMin * 1000; } // m/s
    void print(){
        std::printf("vel_curr %5.2f\t vel_max %5.2f \n",getVelCurr(),getVelMax());
        std::printf("vel_tot %5.2f\t dist_tot %5.2f\t time_tot %5.2f \n",
            getVelTotal(), getDistTotal(), getTimeTotal());
    }
};

struct Braking{
    bool stateL = false;
    bool stateR = false;
    int pinL = -1;
    int pinR = -1;
    int (*pReader)(int) = nullptr;
    Braking(int hw_reader(int), int pinLeft, int pinRight){
        /* hw_reader modeled after digitalRead(int pin) */
        pinL = pinLeft;
        pinR = pinRight;
        pReader = hw_reader;
    }
    void update(){
        stateL = pReader(pinL);
        stateR = pReader(pinR);
    }

    void print(){
        std::printf("Left: %d | Right: %d\n",stateL,stateR);
    }
};

struct CsvSimple {
    /* simple, per-item / per-line csv writer */
    std::ofstream fout;
    CsvSimple(std::string filename){
        fout.open(filename);
    }
    ~CsvSimple(){ close(); }
    void addval(int val){ fout << val<< ','; }
    void addval(float val){ fout << val<< ','; }
    void addval(double val){ fout << val<< ','; }
    void addval(std::string val){ fout << val.c_str() << ','; }
    void endl(){ fout << std::endl; }
    void close(){ endl(); fout.close();}
};

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
    light.toggle();
    delay(166);
    light.toggle(); // blink 1
    delay(166);
    light.toggle();
    delay(166);
    light.toggle(); // blink 2
    delay(166);
    light.toggle();
    delay(166);
    light.toggle(); // blink 3
}

// variable / object initializations
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
int main(){
    // initialization step
    initialize_hw();
    announce_on(led);
    std::signal(SIGINT,isr_ctrlc); // enable CTRL+C graceful exit
    CsvSimple f(tstamp() + "_out.csv");
    f.addval("Time-"+tstamp()+" (s)");
    f.addval("Dist (m)");
    f.addval("CurrSpeed (m/s)");
    f.addval("brL");
    f.addval("brR");
    f.endl();

    
    // looping step
    while (RUNNING) {
        if (millis() % 1000 == 0) {
            delay(1); // ensure don't accidentally repeat all tasks if finished in under the loop time
            led.toggle();
            //sped.print();
            //brakes.print();
            //std::printf("time elapsed: %d\n", millis());
            std::printf("current time: %s\n", tstamp().c_str()); 

            f.addval(sped.getTimeTotal());
            f.addval(sped.getDistTotal());
            f.addval(sped.getVelCurr());
            f.addval(brakes.stateL);
            f.addval(brakes.stateR);
            f.endl();

        } // if-millis
    } // while-loop
    f.close();

    return 0;
} // main





// eof
