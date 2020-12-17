/*
2020-dec-16
read in from a pin
*/

#include <iostream>
#include <wiringPi.h>

int main(int argc, char **argv){
  wiringPiSetupGpio();
  std::printf("gpio pins w/ wiringpi\n");
  int pin = argc>1 ? std::stoi(argv[1]) : 4;
  std::printf("using pin %d\n",pin);
  pinMode(pin,INPUT);
  while(true){
    std::printf("current value: %d\n",digitalRead(pin));
    delay(1000);
  }
}


