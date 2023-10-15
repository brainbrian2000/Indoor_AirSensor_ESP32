#include <Servo.h>
Servo servo1;
void setup() {
  // put your setup code here, to run once:
  //servo in
  servo1.attach(18);
  servo2.attach(19);
  //servo out
  servo3.attach(20);
  servo4.attach(21);
  Serial.begin(115200);
}
class Pipe{
    public:
        int   floor = 1;
        int   door_in=0,door_out=0; //state of pipe door, in is the pipe suck the air into indoor.
        int   fan_in =0,fan_out=0;  //state of pipe fan
        float press_in=1013.25,press_out=1013.25; //pressure inside and outside
        Pipe(){};
        ~Pipe(){};
        void set_door_fan(){
          if(door_in==0){
            servo1.write(90); //angle set by tested
            servo2.write(90);
          }
          else if(door_in==1){
            servo1.write(0);
            servo2.write(0);
          }
          if(door_out==0){
            servo3.write(0);
            servo4.write(5);
          }
          else if (door_out==1){
            servo3.write(100);
            servo4.write(115);
          }
          // using Relay to control fan
        };
};
int ang=0;
String angStr;
void loop() {
  // put your main  code here, to run repeatedly:
  if(Serial.available()){
    angStr = Serial.readString();
    ang = angStr.toInt();
    Serial.println(ang);
    servo1.write(ang);
  }
}
