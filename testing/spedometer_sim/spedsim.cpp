/*
2020-dec-16
make simple spedometer w/ magnet interrupt code
compile: g++ spedsim.cpp -l wiringPi
run: ./a.out 4
*/

#include <iostream>
#include <wiringPi.h>
struct Spedometer{
  unsigned int tprev=millis();
  unsigned int elapsed=0;
  int counter=0;
  Spedometer(){}
  void lap(){
    elapsed=millis()-tprev;
    tprev=millis();
    counter++;
  }
  unsigned int getElapsed() {return elapsed;}
};

Spedometer s;

void isr(){
  std::printf("button pressed\n");
  s.lap();
}

int main(int argc, char **argv){
  int pin;
  if(argc<=1) pin=3;
  else pin=std::stoi(argv[1]);
  std::printf("using pin %d\n",pin);
  wiringPiSetupGpio();
  pinMode(pin,INPUT);
  // Spedometer s(pin);
  wiringPiISR(pin,INT_EDGE_FALLING,isr);
  
  while(true){
    std::printf("hello world (%d, %d)\n", s.counter,s.getElapsed() );
    delay(1000);
  }
  return 0;
}
