// Simple Thermostat 
// Pratik Gangwani
// Section A
#include "mbed.h"
#include "TMP36.h"
#include "TextLCD.h"
#include "PinDetect.h"
#include "Speaker.h"
// must add your new class code to the project file Shiftbrite.h
#include "Shiftbrite.h"

// use class to setup temperature sensor pins
TMP36 myTMP36(p15);  //Analog in

// use class to setup the LCD
TextLCD myLCD(p22, p23, p24, p25, p26, p27); // rs, e, d4-d7

// use class to setup pushbuttons pins
PinDetect pb1(p28);
PinDetect pb2(p29);
PinDetect pb3(p30);

// use class to setup speaker pin
Speaker mySpeaker(p21); //PWM out

// use class to setup Shiftbrite pins
Shiftbrite myShiftbrite(p16, p9, p11, p12, p13);// ei li di n/c ci

// use class to setup Mbed's four on-board LEDs
DigitalOut myLED1(LED1);
DigitalOut myLED2(LED2);
DigitalOut myLED3(LED3);
DigitalOut myLED4(LED4);

// heat or code mode jumper - removed when pushbuttons added
DigitalIn jumper_wire(p14);

//also setting any unused analog input pins to digital outputs reduces A/D noise a bit
//see http://mbed.org/users/chris/notebook/Getting-best-ADC-performance/
//DigitalOut P16(p16);
DigitalOut P17(p17);
DigitalOut P18(p18);
DigitalOut P19(p19);
DigitalOut P20(p20);




// Global variables used in callbacks and main program
// C variables in interrupt routines should use volatile keyword

int volatile temp_set = 75;
int volatile heat_set = temp_set+1; // heat to temp
int volatile cool_set = temp_set-1; // cool to temp
int volatile mode = 0; // heat or cool mode; 0 = off, heat = 1, cool = 2;
int volatile setpoint_cursor = 14;

// Callback routine is interrupt activated by a debounced pb1 hit
void pb1_hit_callback (void)
{
    temp_set++;
    heat_set++;
    cool_set++;
    myLCD.locate(setpoint_cursor, 1);    //2nd line    purpose        
    myLCD.printf("%2d", temp_set); 
}
// Callback routine is interrupt activated by a debounced pb2 hit
void pb2_hit_callback (void)
{
    temp_set--;
    heat_set--;
    cool_set--;
    myLCD.locate(setpoint_cursor, 1);      //2nd line on purpose         
    myLCD.printf("%2d", temp_set);
}
// Callback routine is interrupt activated by a debounced pb3 hit
 void pb3_hit_callback (void)
{

 }

int main()
{
    // Use internal pullups for the three pushbuttons
    pb1.mode(PullUp);
    pb2.mode(PullUp);
    pb3.mode(PullUp);
    // Delay for initial pullup to take effect
    wait(.01);
    // Setup Interrupt callback functions for a pb hit
    pb1.attach_deasserted(&pb1_hit_callback);
    pb2.attach_deasserted(&pb2_hit_callback);
    pb3.attach_deasserted(&pb3_hit_callback);
    // Start sampling pb inputs using interrupts
    pb1.setSampleFrequency();
    pb2.setSampleFrequency();
    pb3.setSampleFrequency();
    // pushbuttons now setup and running
    // State machine code below 
    while (1) {
        {
            enum Statetype { HEAT_OFF = 0, HEAT_ON };
            Statetype state = HEAT_OFF;
            while(1) {
                float Current_tempC = myTMP36.read();
                //convert to degrees F
                float Current_tempF = ((9.0*Current_tempC)/5.0) + 32.0;
                switch (state) {
                    case HEAT_OFF:
                        myLED3 = 0;
                        myShiftbrite.write(0,0,255);
                        setpoint_cursor = 14;
                        myLCD.cls();
                        myLCD.locate(0,0);
                        myLCD.printf("Temp=%5.2FF HOFF\n",Current_tempF);
                        myLCD.locate(0,1);
                        myLCD.printf("Holding above %d",temp_set);
                        if ( Current_tempF <= cool_set ){
                            state = HEAT_ON;
                            mySpeaker.PlayNote(600,0.5,0.5);
                        }
                        break;
                    case HEAT_ON:
                        myLED3 = 1;
                        myShiftbrite.write(0,0,255);
                        setpoint_cursor = 11;
                        myLCD.cls();
                        myLCD.locate(0,0);
                        myLCD.printf("Temp=%5.2FF Heat\n",Current_tempF);
                        myLCD.locate(0,1);
                        myLCD.printf("Heating to %d   ", temp_set);
                        if ( Current_tempF >= heat_set ) {
                            state = HEAT_OFF;
                            mySpeaker.PlayNote(400,0.5,0.5);
                        }
                        break;
                }
                wait(0.33);
                // heartbeat LED - common debug tool
                // blinks as long as code is running and not locked up
                myLED1=!myLED1;
            }
        }
    }
}