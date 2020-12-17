/*
2020-dec-16
read in from a pin
*/

#include <iostream>
#include <wiringPi.h>

int main(){
  wiringPiSetupGpio();
  printf("gpio pins w/ wiringpi\n");
  int pin3 = 3;
  pinMode(pin3,INPUT);
  while(true){
  std::printf("current value: %d\n",digitalRead(pin3));
  delay(1000);
  }
}


