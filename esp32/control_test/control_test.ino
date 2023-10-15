/*define thresholes*/
float co2_level1 = 1200;
float co2_level2 = 3000;
float pm_level1  = 25;
float pm_level2  = 90;
float tvoc_level1 = 25;
float tvoc_level2 = 90;

/*class definition (copied from code & simplified for test)*/
class Sensor{
  public:
    int _pm025;
    float _temp, _hum, _press, _co2, _tvoc;
    Sensor(){};
    ~Sensor(){};

    int state = 0; //sensor state: 0=:), 1=:|, 2=:(
    /*state determining functions*/
    int co2_state(){
      if(_co2>co2_level2) return 2;
      else if(_co2>co2_level1) return 1;
      else return 0;
    };
    int pm_state(){
      if(_pm025>pm_level2) return 2;
      else if(_pm025>pm_level1) return 1;
      else return 0;
    };
    int tvoc_state(){
      if(_tvoc>tvoc_level2) return 2;
      else if(_tvoc>tvoc_level1) return 1;
      else return 0;
    };
      
    int update_state(){
      int co2, tvoc, pm;
      co2 =   co2_state();
      tvoc =  tvoc_state();
      pm =    pm_state();
      // state = max(co2, max(tvoc, pm));
      state = co2>=(tvoc>=pm?tvoc:pm)?co2:(tvoc>=pm?tvoc:pm);
    };
    /*end of new added functions*/



    void set_data(float temp, float hum, float press, float co2, float tvoc,int pm025,int pm100){
        _temp = temp;
        _hum = hum;
        _press = press;
        _co2 = co2;
        _tvoc = tvoc;
        _pm025 = pm025;
        update_state();
    };
};

class Pipe{
  public:
  int   door_in=0,door_out=0; //state of pipe door, in is the pipe suck the air into indoor.
  int   fan_in =0,fan_out=0;  //state of pipe fan
  float press_in=1013.25,press_out=1013.25; //pressure inside and outside
  int condition=0; //condition of the wall: is inner sensor better than outer sensor?
  Pipe(){};
  ~Pipe(){};
  void set_state(int _door_in, int _door_out, int _fan_in, int _fan_out){
    door_in = _door_in;
    door_out = _door_out;
    fan_in = _fan_in;
    fan_out = _fan_out;
  }
};
float co2_weight = 1, pm_weight = 1, tvoc_weight = 1;

class Control_Logic{
  public:
  /*user defined thresholes*/
  /*weightings for compare*/

  /*condition array: -1, close; 1, open; 0, compare*/
  int condition_array[3][3] = {{-1,1,1},{-1,0,1},{-1,-1,0}};

  /*calculate threshole according to sensors*/
  int calculate_threshole(Sensor sensor_i, Sensor sensor_o){
    int result = 0; //too high : open, inside-outside.
    result += (sensor_i._co2 - sensor_o._co2)*co2_weight;
    result += (sensor_i._pm025 - sensor_o._pm025)*pm_weight;
    result += (sensor_i._tvoc - sensor_o._tvoc)*tvoc_weight;
    if(result>3) return 1;
    else return -1;
  }
  /*compare 2 sensor states*/
  int compare_sensors(Sensor sensor_i, Sensor sensor_o){
    int condition = condition_array[sensor_o.state][sensor_i.state];
    //condition:compare
    if(!condition) condition = calculate_threshole(sensor_i,sensor_o);
    return condition;
  }
  /*single pipe control logic*/
  //assign states according to current input value
  //directly update pipe status
  void update_sigle_pipe_state(Pipe &pipe, Sensor sensor_i, Sensor sensor_o){
    int condition = compare_sensors(sensor_i, sensor_o);
    switch(condition){
      case -1: //close
        pipe.set_state(0,0,0,0);
        return;
      case 1:  //open
        pipe.set_state(1,1,1,1);
        return;
    }
  }
  /*two pipe control logic: consider convection*/
  void update_two_pipe_status(Pipe &pipe1, Sensor sensor_i1, Sensor sensor_o1, Pipe &pipe2, Sensor sensor_i2, Sensor sensor_o2){
    pipe1.condition = compare_sensors(sensor_i1, sensor_o1);
    pipe2.condition = compare_sensors(sensor_i2, sensor_o2);

    if(pipe1.condition == -1 && pipe2.condition == -1){ //inner better: close all pipes to advoid energy loss
      pipe1.set_state(0,0,0,0);
      pipe2.set_state(0,0,0,0);
      return;
    }
    else if(pipe1.condition == 1 && pipe2.condition == -1){
      //determine pipe1 state
      if(pipe1.press_out > pipe1.press_in) pipe1.set_state(1,0,0,0);
      else pipe1.set_state(1,0,1,0);
      //determine pipe2 state
      if(pipe2.press_in > pipe2.press_out) pipe2.set_state(0,1,0,0);
      else pipe2.set_state(0,1,0,1);
      return;
    }
    else if(pipe1.condition == -1 && pipe2.condition == 1){
      //determine pipe1 state
      if(pipe1.press_in > pipe1.press_out) pipe1.set_state(0,1,0,0);
      else pipe1.set_state(0,1,0,1);
      //determine pipe2 state
      if(pipe2.press_out > pipe2.press_in) pipe2.set_state(1,0,0,0);
      else pipe2.set_state(1,0,1,0);
      return;
    }
    else{
      pipe1.set_state(1,1,1,1);
      pipe2.set_state(1,1,1,1);
      return;
    }
  }


};

void setup(){
  Serial.begin(115200);
  Serial.println();
  Serial.println("== start of controller logic test ==");

  /*fake data*/
  Sensor sensor_i, sensor_o1, sensor_o2;
  Pipe pipe_1, pipe_2;

  /*single pipe control logic*/
}
 
void loop(){  
}