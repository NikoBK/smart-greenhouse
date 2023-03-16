#include "string.h"
#include "arduino.h"
#include "time.h"
float pumpspeed; //the amount of fluid the pump can transport per second

class Crop{
 public:
  String names; //displayname of the plant in question
  float fluid_requirements; //the amount of liquid the plant is estemated to need once per sycle
  int watering_period; //the length of each cycle
  int max_hydration_limit; //the maximum the hydration sensor can be before the plant is estemated to not need water, thus the plant will not be watered even if the cycle is complete 
  int min_hydration_limit; //the minimum value of the hydration sensor where the plant will be watered imeadeatly instead of waiting for the cycle to end
  int last_cycle_time;
  int connected_pin; // the pin connected to this plants electric valve
  int sensor_pin; // the sensor reading this plants water level
  Crop( String plantname,float fluidreq,int period,int maxhydro,int minhydro,int sensor,int valve){
    
    names=plantname;
    fluid_requirements=fluidreq;
    watering_period=period*1000;
    max_hydration_limit=maxhydro;
    min_hydration_limit=minhydro;
    sensor_pin=sensor;
    connected_pin=valve;
    last_cycle_time=millis();

    
  }
   Crop( ){
    
    names="";
    fluid_requirements=0;
    watering_period=100*1000;
    max_hydration_limit=1022;
    min_hydration_limit=0;
    sensor_pin=0;
    connected_pin=0;
    last_cycle_time=millis();

    
  }
  
};

Crop crops[2];
int crops_length;

void setup() {
  // put your setup code here, to run once:
 Serial.begin(9600);
 crops[0]= Crop("chilli",100,60*60*4,600,10,15,16);
 crops[1]= Crop("peper",100,60*60*4,600,10,17,18);
for(int i=0;i<crops_length;i++){
  
  pinMode(crops[i].sensor_pin,OUTPUT);
  
  }
  

}

void loop() {
  // put your main code here, to run repeatedly:


  for(int i=0;i<crops_length;i++){
    
   struct Crop cr=crops[i];
    int sensorread= analogRead(cr.sensor_pin);
    if(cr.min_hydration_limit<sensorread){
      cr.last_cycle_time+=10000;

      
      
    }
    if(cr.min_hydration_limit<sensorread){
      float timedif = (millis()-cr.watering_period);

      
      cr.last_cycle_time=millis();
      digitalWrite(cr.connected_pin,HIGH);
      
      delay((cr.fluid_requirements/pumpspeed));
      
      digitalWrite(cr.connected_pin,LOW);
      
    }
    else if(millis()-cr.watering_period>cr.last_cycle_time){
      
      cr.last_cycle_time=millis();
      
      digitalWrite(cr.connected_pin,HIGH);
      
      delay((cr.fluid_requirements/pumpspeed));
      
      digitalWrite(cr.connected_pin,LOW);
    }
    else{
      delay(5000);
    }
    
  }

}
