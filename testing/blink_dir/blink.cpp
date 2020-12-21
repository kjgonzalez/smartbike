/* 
2020-dec-16
goal: just do a simple blink on gpio pins for external LED

*/

#include <iostream>
#include <wiringPi.h>
#include <csignal>

bool RUNNING = true;
int led =22;

void blink_led(int led, int time=100){
  digitalWrite(led,HIGH);
  std::printf("HIGH... \n");
  delay(time);
  digitalWrite(led,LOW);
  std::printf("LOW ... \n");
  delay(time);
}

void my_handler(int s){
  std::printf("Detected CTRL-C signal #%d\n",s);
  RUNNING=false;
  digitalWrite(led,LOW);
}

int main(int argc, char **argv){
  std::signal(SIGINT,my_handler);
  wiringPiSetupGpio();
  led = argc>1 ? std::stoi(argv[1]) : 10;
  int tim = argc>2 ? std::stoi(argv[2]) : 1000;
  std::printf("using pin %d\n",led);
  std::printf("time is %d\n",tim);

  pinMode(led,OUTPUT);

  while(RUNNING){
  blink_led(led,tim);
  }
  return 0;
}






