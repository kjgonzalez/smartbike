/*
2020-dec-16
make simple spedometer w/ magnet interrupt code
compile: g++ spedsim.cpp -l wiringPi
run: ./a.out 4
*/

#include <iostream>
#include <wiringPi.h>
constexpr float pi = 3.14159;
constexpr float diam = 0.675; // wheel diameter in meters
constexpr float circumf = diam * pi; // circumference

struct Spedometer{
  unsigned int tprev=millis();
  unsigned int elapsed=800000;
  unsigned int tStart = millis();
  float elapsedMin = 8e5;
  int counter=0; // total rotations since start
  unsigned int stopThresh = 2500; // if elapsed>thresh, effectively stopped
  Spedometer(){}
  void lap(){
    // sensor detects a full rotation
    counter++; // update 'total' variables
    // update 'current' variables
    elapsed=millis()-tprev;
    tprev=millis();
    if(elapsed<elapsedMin) elapsedMin = elapsed;
  }
  void startTimer(){tStart = millis(); }
  float getVelCurr(){ 
    /* return current speed, m/s */
    if(millis()-tprev > stopThresh) return 0;
    else return circumf / elapsed * 1000;

  }
  float getDistTotal(){ return circumf * counter; } // meters
  float getTimeTotal() { return (millis()/1000 - tStart/1000); } // seconds
  float getVelTotal(){ return circumf * counter * 1000 / (millis() - tStart); } // m/s, average
  float getVelMax() { return circumf / elapsedMin * 1000; } // m/s
  
};

Spedometer s;

void isr(){
  s.lap();
}

int main(int argc, char **argv){
  int pin = argc<=1 ? 4 : std::stoi(argv[1]);
  std::printf("using pin %d\n",pin);
  wiringPiSetupGpio();
  pinMode(pin,INPUT);
  wiringPiISR(pin,INT_EDGE_FALLING,isr);
  s.startTimer();
  
  while(true){
    std::printf("----------------------\n");
    std::printf("vel_curr %5.2f\t vel_max %5.2f \n",s.getVelCurr(),s.getVelMax());
    std::printf("vel_tot %5.2f\t dist_tot %5.2f\t time_tot %5.2f \n",
      s.getVelTotal(), s.getDistTotal(), s.getTimeTotal());
    delay(1000);
  }
  return 0;
}
