echo "building ..."
g++ pwm.cpp -l wiringPi -l pthread $1
if [ $? -eq 0 ]
 then
  ./a.out
fi

