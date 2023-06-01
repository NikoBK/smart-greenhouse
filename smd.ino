#include "time.h"
#include "string.h"
#include "arduino.h"
#include <stdio.h>
#include <stdlib.h>


const int plantlen = 2;
const int tanklen = 2;
int buttonpin=1;
plante plants[plantlen];
restank tanks[tanklen];
screne gui ;
long int time;
long int lasttime=0;
ui interfaces;
void setup() {
  // put your setup code here, to run once:
  //setup
  Serial.begin(9600);
  
        
       
        interfaces = ui();
        gui = screne();
        
        



}

void loop() 
{
  time+=(millis()-lasttime)/1000;// gets time in seconds
  lasttime=millis();
  // put your main code here, to run repeatedly:
  for (int i = 0; i < plantlen; i++)
    {
      plants[i].updatestate();
    }

  for (int i = 0; i < tanklen; i++)
    {
      if (tanks[i].cheklevel())
      {

        gui.refiltank();
                    
                        //make noise
                    


      }

  }
  long int timestamp = time;
  bool buttonclick=false;
  while (timestamp + 100000 > time)
  {
  if (digitalRead(buttonpin))
  {
    gui.showmaxmenu();
  }

  }
}

void test1(){
    int testvar1=0; //curent time
    // koden er ment til at dette altid er et voksende tal, se hvad der sker med tilfældige tal, eller faldende verdier

     int testvar2=0; //moisture
/*
presets for reusage
 Resusage(){
      lwt=0;
      dryt=300;
      mindryt=7200;
      lastdryt=0;
      dry=false;
      WPc=40;
      FPc=40;
      WCt=6*7200;
      FCt=6*7200;
    }



*/


    Resusage re=Resusage();

   bool b=re.shouldwater(testvar1,testvar2);

    Serial.println(String(b));

}
void test2(){

float testvar1=0;

restank re=restank(1,"vand",2,50,1);

bool b=re.cheklevel(testvar1);
    Serial.println(String(b));

}





class Resusage
{
    float lwt;// last timestamp of when plant was watered
    float dryt; //minimum dryness threshold
    float mindryt;// the amount of time the plant should be under the drynes theshold
    float lastdryt;// the timestamp of the beginning of the last dry period
    bool dry;// if the plant is curently dry
    float WPc;// water usage pr watering cycle
    float FPc;// fertilizer usage per cycle 
    float WCt;// water cycle time
    float FCt;//fertilizer cycle time

    int curentid=0;
    int lastswitch = 0;
    int timestamps[20];
    int waterusage[20];

    public:
    Resusage(){
      lwt=0;
      dryt=300;
      mindryt=7200;
      lastdryt=0;
      dry=false;
      WPc=40;
      FPc=40;
      WCt=6*7200;
      FCt=6*7200;
    }


    bool apx(int a,int b,int thesh)
    {
        if (a - b < thesh & -(a - b) < thesh)
        {
            return true;
        }
        return false;
    }

    bool shouldwater(int cctime,int moist)
    {
        if (lwt + WCt < cctime)
        { // if it has been an entire water cycle since last time the plant was watered
            int clt = 0;
            for (int i = 0; i < 18; i++)
            {
                int v1 = (i + curentid) % 20;
                int v2 = (i + curentid - 1) % 20;



                if (apx((timestamps[v1] - timestamps[v2]),WCt,5))
                {

                    clt++;
                }

            }
            if (clt > 14)
            {
                lastswitch = curentid;
                WCt = WCt * 1.1;
            }

            return true;
        }
        else if(moist<dryt && dry==true && cctime - lastdryt > mindryt)
        { // if the plant has ben below its drynes theshold for a dangerous amount of time
            int clt = 0;
            for (int i = 0; i < 18; i++)
            {
                int v1 = (i + curentid) % 20;
                int v2 = (i + curentid - 1) % 20;



                if (timestamps[v1]-timestamps[v2]<WCt)
                {
                    
                    clt++;
                }

            }

            if (clt>8)
            {
                lastswitch = curentid;
                WCt = WCt * 0.9;
            }


            return true;

        }
        else
        {// else the plant should not be watered
            if (dry==false)
            {
                if (moist < mindryt)
                {
                    dry = true;
                    lastdryt = cctime;
                }

            }
            return false;
        }
    }
     float estematewaterusage(int cctime)
    {
        

        
         
    }
    int getwpc(int cctime)
    {
        return WCt; // maby needs to be rounded to an int



    }

    float nextWATER(int cctime)
    {
        return lwt + WCt - cctime;
    }
    void waterplant(int water,int cctime)
    {
        lwt = cctime;
        dry = false;
        timestamps[curentid] = cctime;
        waterusage[curentid] = water;
        curentid++;
        if (curentid >= 20)
        {
            curentid = 0;
        }
           
    }
    String usage(){
        String ret;

        
        int clt = INT16_MAX;
        int cht=0;
        int sum=0;
            for (int i = 0; i < 20; i++)
            {
              sum+=waterusage[i];
                if(timestamps[i]<clt){
                    clt=timestamps[i];
                }
                if(timestamps[i]>cht){
                    cht=timestamps[i];
                }
            }
            
            int tim20= cht-clt;
            int fact=604800/tim20;
            //one dose watering 
            ret="";
            ret+=","+String(fact*sum)+"";

            

        
        return ret;
    }
};

class plante
{
      bool active;
        public:
        int plantposition;
        public:
        String plantname;
        public:
        String plantspices;
        int age;
        Resusage requirements;
        int sensorpin;
        int valvepin;


        //
        public:
        plante()
        {
          active = false;
        plantposition=0;
        plantname = "empty";
        plantspices = "none";
        age =0;
        requirements=Resusage();
        sensorpin=0;
        valvepin=0;

        }
        public:
        plante(int pos, String names, String type, int iage, Resusage res,int pin,int valve)
        {
          active=true;
            plantposition = pos;
            plantname = names;
            plantspices = type;
            age = iage;
            requirements = res;
            valvepin=valve;
        }

        float waterplant()
        {
        //start motor
        //

        }

        void updatestate()
        {
        if (requirements.shouldwater(time,analogRead(sensorpin)))
            waterplant();

        }
        String plantdata(){
          return plantname + " who is a " + plantspices + " is " + age + " in pot number " + plantposition;
        }
        String plantuse1(){
            
            return String(requirements.nextWATER(time));
        }
        String platuse2(){
            return requirements.usage();
        }


};

class restank
{
    int sensorpin;
    String resourcename;
    float minval;
    float maxval;
    int id;

    public:
    restank(){
      sensorpin=0;
      resourcename="none";
      minval=0;
      maxval=0;
      id=0;
    }

    public:
    restank(int pin,String recource,float minvalue,float maxvalue,int tankid)
    {
        sensorpin = pin;
        resourcename = recource;
        minval = minvalue;
        maxval = maxvalue;
        id = tankid;


    }
   bool cheklevel(float dist)
    {
        //get recource level
        float lvl = maxval-dist;
        if (lvl < minval)
        {
            return true;
        }
        return false;
    }
    String identification()
    {
        return "tank label " + String(id) + " containing " + resourcename;
    }

};

class ui
{
    bool addplant()
    {

    }
    bool removeplant()
    {

    }
    plante viewplant()
    {

    }


};

class screne
{
  bool showmaxmenu()
  {
    Serial.flush();
    Serial.println("type the number of the option you wish to selct");
    Serial.println("1:ad a plant");
    Serial.println("2:show plant data");
    Serial.println("3:remove a plant");
    Serial.println("4:show greenhouse resource usage");
    while (true)
    {
      if (Serial.available())
      {
        String msg = Serial.readString();

      if (msg = 'b')
      {

        return false;

      }
      else
      {
          switch (msg[0])
                    {


                       

                        case '1':
                            addplants();
                            break;
                        case '2':
                           
                            showplantmenu(plants,plantlen);
                            break;
                        case '3':

                          
                            break;
                        case '4':
                          

                            break;
                        default:
                          Serial.println("that isnt a valid input");
                    }

                }





            }


        }
    }

    bool showplantmenu(plante plants[], int len)
    {
        Serial.flush();
        Serial.println("This menu will show you information about your plants");
        Serial.println("write the index of a plant to show the plants resource use");
        for (int i = 0; i < len; i++)
        {
            Serial.println(i + "-<" + plants[i].plantdata());
        }
        Serial.print("id: ");
        while (true)
        {

            if (Serial.available())
            {
              String  msg = Serial.readString();
               if (msg = 'b')
                {

                    return false;
                }
                if (msg = 'r')
                {

                    Serial.println("This menu will show you information about your plants");
                    Serial.println("write the index of a plant to show the plants resource use");
                    for (int i = 0; i < len; i++)
                    {
                        Serial.println(i + "-<" +plants[i].plantdata());
                    Serial.print("id: ");
                }
                plante p = plants[Serial.parseInt()];
                String pname = p.plantname;
                
                Serial.println("showing data for" + pname);
                
                Serial.println(pname + " has used " + p.platuse2() + "in the last week");
                Serial.println(pname + " will be watered in " + p.plantuse1() + " minutes");

                Serial.println("type the id of anóther plant to wiew it / type r to reload the menu / type b to go back to the previous menu");

            }
            else
            {


            }
        }
    }
    }

    bool addplants()
    {
        Serial.flush();
        Serial.println("This menu will help you add a plant to your greenhouse");
        Serial.println("Input b if you wish to go back to a previous menu");

        String msg = Serial.readString();

        


        int plantposition;
        String plantname;
        String plantspices;
        int age;
        Resusage requirements;
        int stc=0;
        while (true)
        {
            
            if (Serial.available())
            { msg = Serial.readString();

                if (msg = 'b')
                {

                    return false;

                }
                else
                {
                    switch(stc)
                    {
                        case 0:
                            //input a plant type
                            Serial.println("What type of plant do you wish to add.");

                            stc = 1;



                            break;
                        case 1:

                            //input a plant type
                            plantspices = msg;
                            Serial.println("plant type was set to: " + msg);

                            Serial.println("y/n do you want to continue with this name?: " + msg);
                            bool b=getYN();
                            if(b){
                              stc=2;
                            }

                        
                            Serial.println("what would you like to call your plant?");
                            Serial.println("type your plants name");
                            Serial.print("name: ");
                            break;

                        case 2:
                            //input plant name
                            plantname = msg;
                            Serial.println("plant name was set to: " + msg);

                            Serial.println("y/n do you want to continue with this name?: " + msg);
                            if (getYN())
                            {
                                stc = 3;
                                Serial.println("how many days since you planted this plant");
                                Serial.print("age: ");

                            }
                            else
                            {
                                stc = 2;
                                Serial.println("type your plants name");
                                Serial.print("name: ");
                            }



                            break;

                        case 3:
                            //input plant age
                            age = Serial.parseInt();
                            Serial.println("plant age was set to: " + msg);

                            Serial.println("y/n do you want to continue with this age?: " + msg);
                            if (getYN())
                            {
                                stc = 4;
                                Serial.println("type in the position of this plant");
                                Serial.print("pos: ");
                            }
                            else
                            {
                                stc = 3;
                                Serial.println("type your plants age");
                                Serial.print("age: ");
                            }

                            break;
                        case 4:
                            //input plant age
                            plantposition =atoi(msg.c_str());
                            Serial.println("plant age was set to: " + msg);

                            Serial.println("y/n do you want to continue with this position?: " + msg);
                            if (getYN())
                            {
                                stc = 5;
                                requirements = Resusage();
                                plante plat = plante(age, plantname, plantspices, plantposition, requirements,1,2);
                                return true;

                            }
                            else
                            {
                                stc = 4;
                                Serial.println("type your plants position in the greenhouse");
                                Serial.print("pos: ");
                            }

                            break;

                    }

                }





            }
        }


    }
    bool removeplants()
    {

            

    }
 bool showrecourcemenu()
    {

    }

    bool refiltank(restank r)
    {
        Serial.println("refil tank the tank with this discription"+r.identification());
    }

};
   




bool getYN()
{
    bool runs = true;
    bool ans = true;
  while (runs)
    {
        if (Serial.available())
        {
          String msg=Serial.readString();
            if (msg == "y")
            {
                ans = true;
                runs = false;
            }
            else if (msg == "n")
            {
                ans = false;
                runs = false;
            }
            else
            {
                Serial.write("please type y or n for yes or no");
            }
        }
    }
    return ans;

};
