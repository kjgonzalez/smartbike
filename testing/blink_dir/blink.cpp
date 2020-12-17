/* 
2020-dec-16
goal: just do a simple blink on gpio pins for external LED

*/

#include <iostream>
#include <wiringPi.h>
#include <csignal>

bool RUNNING = true;

void blink_led(int led, int time=100){
  digitalWrite(led,HIGH);
  delay(time);
  digitalWrite(led,LOW);
  delay(time);
}

void my_handler(int s){
  std::printf("Detected CTRL-C signal #%d\n",s);
  RUNNING=false;
}

int main(int argc, char **argv){
  std::signal(SIGINT,my_handler);
  wiringPiSetupGpio();
  printf("Controlling GPIO pins w/ wiringPi\n");
  int led;
  if(argc>1) led=(int) argv[1];
  else led =22;

  pinMode(led,OUTPUT);

  while(RUNNING){
  blink_led(led,300);
  }
  return 0;
}






