//#include <header.h>
#include <Arduino.h>
#include <PID_v1.h>
#include <TM1628.h>
#include <TM16XXFonts.h>

// PINOUT
//*1= GND 8, 22@
//2= Serial
//3= Serial
//*4= NC
//5= Serial
//*6= Nc
//*7= NC
//*8= swAir L=on
//*9= swIron L=on
//*10=swMan L=Auto
//*11=airEl H=off
//*12=ironTc 0,0- 2.4v=480
//*13=ironPot 0=0
//*14=hotairPot 0=0
//*15=ironEl H=off
//*16=hotairTc 0,0 2.4v=480
//*17=hotFan H=off [LED MAYBE]
//*18=hall L=saddle
//*19=fanOut L=on
//20=POS


//Swapping lows and highs. 5v=low 0v=high
#define HIGH 0x0
#define LOW 0x1

//Declareing varables.
double HotairPot, HotairTc, AirOut;
double IronPot, IronTc, IronOut;
float OldAir, OldIron; //BOOLEAN Inputs

//setting Analog Inputs.
int flop = 1000;
int hotairTc = A0;
int ironTc = A1;
int ironPot = A2;
int hotairPot = A3;

//DIGITAL inputs
int swMan = 2;
int swIron =3;
int swAir = 4;
int hall = 7;
int fanLed= 8;

//DIGITAL outs PWM
int ironEl = 5;
int airEl = 6;
int fanOut = 9;
int led = 11;
int heat = false;

//OUTPUTS PID settings.
PID hotairPID(&HotairTc, &AirOut, &HotairPot,2,5,1,P_ON_M, DIRECT);
PID ironPID(&IronTc, &IronOut, &IronPot,2,5,1,P_ON_M, DIRECT);
// define - data pin D10, clock pin D12 and strobe pin D13
// обьявление портов: DIO - порт D10, CLK - D12, STB - D13

// Led settup.
TM1628 dvdLED(10, 12, 13);


//Setting up the code.
void setup() {
      // put your setup code here, to run once:
    Serial.begin(9600);
    digitalWrite(ironEl, LOW);
    digitalWrite(airEl, LOW);
    ironPID.SetMode(MANUAL);
    hotairPID.SetMode(MANUAL);

    dvdLED.begin(ON, 2); // this may be a pin number????
    OldAir = HotairPot;
    OldIron = IronPot;
    }


//The controller.
void loop() {


    //Read switches====================================================
    int SwMan = digitalRead(swMan);
    int SwIron = digitalRead(swIron);
    int SwAir = digitalRead(swAir);
    int Hall = digitalRead(hall);
    HotairTc = analogRead(hotairTc);
    HotairPot = analogRead(hotairPot);
    IronTc = analogRead(ironTc);
    IronPot = analogRead(ironPot);
    HotairTc = map(HotairTc, 0, 1023, 0, 1000);
    HotairPot = map(HotairPot, 0, 1023, 0, 480);
    IronTc = map(IronTc, 0, 1023, 0, 1000);
    IronPot = map(IronPot, 0, 1023, 0, 480);
    IronOut = airEl;
    AirOut = ironEl;
    fanLed = fanOut;

    //Read switches====================================================
      ironPID.Compute();
      hotairPID.Compute();

    if(Hall == LOW || SwMan == LOW){
      heat = true;
    }
    else {(heat = false);}

      if (SwAir == HIGH && heat == true){
        hotairPID.SetMode(AUTOMATIC); //on element
    }
    else {hotairPID.SetMode(MANUAL);
      digitalWrite(airEl, LOW);
    } // off element

//This is the cool down loop for the hot air.
    if (airEl == HIGH || HotairTc >=100){
      digitalWrite(fanOut, HIGH);
    }
    else {digitalWrite(fanOut, LOW);}

//Iron controller loop.
      if (SwIron == HIGH){
        ironPID.SetMode(AUTOMATIC);
      }
    else {ironPID.SetMode(MANUAL);
      digitalWrite(ironEl, LOW);
    }

//This is for the led driver.
    //for (int z=0; z <5; z++){
    int z = 0;
    dvdLED.clear();
    dvdLED.print(IronTc);
    dvdLED.setLED(z);
    delay(500);
    dvdLED.clear();
    dvdLED.print(HotairTc);
    dvdLED.setLED(z);
    delay(500);
  //}

    if (HotairPot - OldAir > 3){
      dvdLED.clear();
      dvdLED.print(HotairPot);
      dvdLED.setLED(2);
      delay(flop);
      OldAir = HotairPot;
  }
    if (IronPot - OldIron > 3){
      dvdLED.clear();
      dvdLED.print(IronPot);
      delay(flop);
      OldIron = IronPot;
  }

}
