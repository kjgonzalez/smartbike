/*
2020-dec-16
check how interrupts are done using wiringPi
src: http://wiringpi.com/reference/priority-interrupts-and-threads/
*/

#include <iostream>
#include <wiringPi.h>

void isr(){
  std::printf("button pressed\n");
}

int main(int argc, char **argv){
  int pin;
  if(argc<=1) pin=3;
  else pin=std::stoi(argv[1]);
  std::printf("using pin %d\n",pin);
  wiringPiSetupGpio();
  pinMode(pin,INPUT);
  wiringPiISR(pin,INT_EDGE_FALLING,isr);
  
  
  
  while(true){
  std::printf("hello world\n");
  delay(1000);
  }
  return 0;
}



