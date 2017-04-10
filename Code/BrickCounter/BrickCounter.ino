#include <LedControl.h>

/*Lunchbox Electronics 2016
 *Code to run an assembly line to automatically package Build Upons
 *The system uses two motors to run belts that move the bricks through the line
 *The bricks are counted with a IR sensor and then bagged for kitting
 *Created by Jason Brownstein
 */
 
#define stp 2                   // Defines stepper motor step pin
#define dir 4                   // Defines stepper motor direction pin
#define MS1 7                   // Defines stepper motor microstep1 pin
#define MS2 16                  // Defines stepper motor microstep2 pin, on analog in rail
#define EN  17                  // Defines stepper motor enable pin, on analog in rail
#define SLP 18                  // Defines stepper motor sleep pin, on analog in rail
int ledPin = 12;                // LED is connected to pin 12
int IRgate = 0;                 // IR gate is connected to pin A0
int val;                        // variable for reading the pin status
int newState;                   // variable to hold temp state status
int bricks = 0;                 // how many bricks have passed
int IRstate;                    // variable to hold the IR gate state
int offVal;                    // value when the gate is open
int onVal;                     // value when the gate is closed
int bagbricks = 3;             // variable for number of bricks per bag
int setpin = 11;                 // initiates the state to set number of bricks per bag mode
int setState;                   // variable to hold set number of bricks per bag state mode
int potpin = 1;                 // sets analog pin to read a potentiometer
int prefeederpin = 3;           // sets a pwm pin to control the motor speed on the prefeeder
int buttonpin = 13;             // set a digital pin to read the status of the big red button
int buttonState = 1;            // initiate the state of the button
int buttonlight = 5;           // LED in button is on digital pin 10
int convayorpin = 6;           // Convayor belt motor on digital pin 11
long timer = 0;                // Keep a reference time for prefeeder motor delay
long timer2;                   // Keep a second reference time for delay after bagbrick limit reached
bool limit = false;            // initiate a condition that bagbricks limit reached
int photopin = 5;              // define pin for photoresistor
int photo;
int prefeederspeed = 80;
int convayorspeed = 120;


LedControl counter = LedControl(10, 8, 9, 1);  // (DIN, CLK, LOAD, number of chips)

void setup() {
  Serial.begin(9600);                 // Set up serial communication
  counter.shutdown(0, false);         // turns on display
  counter.setIntensity(0, 5);        // 15 = brightest
  pinMode(ledPin, OUTPUT);            // Set the LED pin as OUTPUT
  pinMode(setpin, INPUT);             // set the set pin as INPUT to detect change
  setState = digitalRead(setpin);     // set initial state of mode switch counter or set # of bricks mode
  pinMode(prefeederpin, OUTPUT);      // set the pwm pin to OUTPUT
  pinMode(buttonpin, INPUT);          // set the button pin to INPUT to detect change
  pinMode(buttonlight, OUTPUT);       // set the button light pin to OUTPUT
  pinMode(convayorpin, OUTPUT);       // set the convayor belt pin to OUTPUT
  offVal = analogRead(IRgate);        // set initial reading base value of the IR gate
  onVal = offVal + 100;               // set threshold for level to be triggered when brick passes through
  pinMode(stp, OUTPUT);               // set all stepper motor pins to output
  pinMode(dir, OUTPUT);
  pinMode(MS1, OUTPUT);
  pinMode(MS2, OUTPUT);
  pinMode(EN, OUTPUT);
  pinMode(SLP, OUTPUT);
  resetEDPins();                      // Set step, direction, microstep and enable pins to default states
  digitalWrite(EN, LOW);              // set driver to enable stepper motor
  digitalWrite(SLP, HIGH);            // wake up stepper motor driver
  photo = analogRead(photopin);       // Read the photocell
  while(photo <= 100) {                // while the photocell reads low move the stepper motor
    //Serial.println(photo);
    runmotor();
  }
  delay(100);
  digitalWrite(SLP, LOW);             // sleep stepper driver to minimize power consumption
  printNumber(bricks);
  digitalWrite(ledPin, HIGH);         // Turn on led to indicate all is good
}

void loop() {
  //Serial.println(digitalRead(setpin));
  long currenttime = millis();
  if (digitalRead(setpin) == HIGH) {   // if the mode swtich is activated you can set
    Serial.println("oops");
    bricset();                           // the number of bricks per bag (when the counter cycles over)
  }
  // MOTOR INITIATOR AND KILL SWITCH
  if (digitalRead(buttonpin) == HIGH) {   // detect if the button has been pressed
    buttonState = -1 * buttonState;      // change the value of buttonState to show change
    delay(300);                          // add a delay to debounce button
    if (buttonState == -1) {               // case for button to initate motors
      analogWrite(prefeederpin, prefeederspeed);      // use analog write to drive a lower voltage to the prefeeder motor
      digitalWrite(buttonlight, HIGH);     // turn button light on
      analogWrite(convayorpin, convayorspeed);       // use analog write to drive a lower voltage to the convayor belt motor
      //digitalWrite(prefeederpin, HIGH);
      //digitalWrite(convayorpin, HIGH);
      return;
    }
    else {
      digitalWrite(prefeederpin, LOW);     // opposing case to kill the motors
      digitalWrite(convayorpin, LOW);
      digitalWrite(buttonlight, LOW);      // turn button light off
    }
  }
  //Serial.println(buttonState);
  if (buttonState == -1) {                 // If button is pressed start incremental step of prefeeder belt
    if (currenttime - timer > 200) {       // compare timers and turn off motor when greater than 200 ms
      digitalWrite(prefeederpin, LOW);
      if (currenttime - timer > 700) {     // compare times and turn motor back on 500ms after motor turned off
        analogWrite(prefeederpin, prefeederspeed);
        timer = currenttime;               // update the timer 
      }
    }
  }
  val = analogRead(IRgate);               // read input value and store it in val
  //Serial.println(val);
  // NO BRICK IS PRESENT IN THE GATE
  if (val < offVal) {                     // check if the analog value is less than the gate open threshold
    digitalWrite(ledPin, HIGH);           // turn LED on
    newState = 0;                         // update the state
  }
  // BRICK IS PASSING THROUGH THE GATE
  if (val > onVal) {                      // check if the analog value is greater than the gate closed threshold
    digitalWrite(ledPin, LOW);            // turn LED off
    newState = 1;                         // update the state
  }
  if (newState != IRstate) {              // check the state has changed
    if (val < offVal) {                   // if state has changed count brick once state goes back to open
      bricks++;                           // increment brick counter
      //Serial.println("+1");
      printNumber(bricks);                // feed brick count to the display for visual counting
    }
  if (bricks == bagbricks) {              // check if the brick count is over the bagbrick set number
    printNumber(bricks);                  // feed brick count to the display for visual counting
    timer2 = millis();
    limit = true;
    bricks = 0;                           // cycle over to recount for next bag
  }
  }
  if (currenttime - timer2 > 1025 && limit == true) {
    printNumber(bricks);
    digitalWrite(prefeederpin, LOW);      // code to stop motors for a duration
    digitalWrite(convayorpin, LOW);
    flashbutton();                        // flash button light for 2 sec so person can change to a new bag    
    }
  IRstate = newState;                     // save/update the new state in IRstate
}

// THIS WILL RESET THE VARIABLE BAGBRICKS WHEN SWITCH IS ACTIVATED
void bricset() {
  digitalWrite(prefeederpin, LOW);        // stop the motors if they were running
  digitalWrite(convayorpin, LOW);
  for ( int x = 1; x <= 1; x++) {     // initially scroll on display you are setting the brick count
    set();
    bric();
  }
  while (digitalRead(setpin) == HIGH) {
    bricks = 0;                         // clear brick count
    counter.clearDisplay(0);            // reset the display
    int val = analogRead(potpin);       // store the potentiometer signal to val
    val = map(val, 35, 1019, 0, 30);     // remap the pot values to our scale of 0 - 50 bricks
    printNumber(val);                   // call function to display any number on the display
    delay(500);
    bagbricks = val;                    // reset the variable bagbricks, will change where main loop counter cycles over
  }
  counter.clearDisplay(0);            // clear display to start fresh for counting again
  printNumber(bricks);
  return;
}


// MORE EFFICIENT WAY OF WRITING MULTIPLE CHARACTERS TO THE DISPLAY; Eberhard Fahle
// http://www.wayoda.org/arduino/ledcontrol/index.html
void printNumber(int bricks) {        // read in current brick value
  int brick = bricks;                 // define local variable brick
  int ones;
  int tens;
  int hundreds;
  boolean negative;

  if (brick < -999 || brick > 999)    // allow for range of -999 to 999 on display
    return;
  if (brick < 0) {                    // if less than zero make a negative number
    negative = true;
    brick = brick * -1;
  }
  ones = brick % 10;                  // find remainder of current brick value in each digit position
  brick = brick / 10;
  tens = brick % 10;
  brick = brick / 10;
  hundreds = brick;
  
  counter.setDigit(0, 1, (byte)hundreds, false);   //Now print the number digit by digit
  counter.setDigit(0, 2, (byte)tens, false);
  counter.setDigit(0, 3, (byte)ones, false);
}

//FUNCTION TO DISPLAY SET ON DISPLAY
void set() {
  counter.clearDisplay(0);
  counter.setDigit(0, 0, 5, false);  //S
  counter.setDigit(0, 1, 14, false); //E
  counter.setRow(0, 2, B00001111);   //t
  delay(2000);
}

//FUNCTION TO DISPLAY BRIC ON DISPLAY
void bric() {
  counter.clearDisplay(0);
  counter.setRow(0, 0, B00011111); //b
  counter.setRow(0, 1, B00000101); //r
  counter.setRow(0, 2, B00010000); //i
  counter.setRow(0, 3, B00001101); //c
  delay(2000);
}

//FUNCTION TO FLASH THE LED IN THE BUTTON
void flashbutton() {
  int temp = digitalRead(buttonlight);   //check if button light is on or not
  for (int x = 1; x < 3; x++) {          //loop twice to flash button
    digitalWrite(buttonlight, LOW);
    delay(200);
    digitalWrite(buttonlight, HIGH);
    delay(200);
  }
  digitalWrite(SLP, HIGH);              // wake up stepper motor driver
  delay(1);                             //delay for stability
  jogmotor();                           // move stepper an arbitrary amount to change photocell reading
  photo = analogRead(photopin);         // starting reading the photocell
  while(photo <= 50){                   // while photocell is in dark run stepper motor
    runmotor();                         // effectively moves to next bucket
  }
  delay(500);                           // add a half second pause for smoother transition
  digitalWrite(SLP, LOW);               // sleep stepper motor driver
  if (temp == 0) {                      // if button was off turn it back off
    digitalWrite(buttonlight, LOW);
  }
  if (buttonState == -1) {              // if button state had been "motors on" turn convayor back on
    analogWrite(convayorpin, convayorspeed);
  }
  limit = false;                        // reset limit boolean
}

//Reset Easy Driver pins to default states
void resetEDPins()
{
  digitalWrite(stp, LOW);              
  digitalWrite(dir, LOW);
  digitalWrite(MS1, LOW);
  digitalWrite(MS2, LOW);
  digitalWrite(EN, HIGH);
}

// RUN STEPPER MOTOR FORWARDS TILL NEXT BUCKET (based on photoresistor reading)
void runmotor(){
  digitalWrite(dir, HIGH);   //move "forward"
  digitalWrite(MS1, HIGH);  // High MS1 and High MS2 is 1/8 step
  digitalWrite(MS2, HIGH);
  //STEP TRIGGER
  digitalWrite(stp, HIGH);  //trigger one step forward
  delay(1);
  digitalWrite(stp, LOW);   //pull step pin low so it can be triggered again
  delay(1);
  photo = analogRead(photopin);   //reread photocell to update while condition
}

// JOG STEPPER MOTOR FORWARDS 100/8 steps
void jogmotor(){
  digitalWrite(dir, HIGH); //move "forward"
  digitalWrite(MS1, HIGH);  // High MS1 and High MS2 is 1/8 step
  digitalWrite(MS2, HIGH);
  for (int x= 1; x < 100; x++) {
    digitalWrite(stp, HIGH); //trigger one step forward
    delay(1);
    digitalWrite(stp, LOW); //pull step pin low so it can be triggered again
    delay(1);
  }
}
