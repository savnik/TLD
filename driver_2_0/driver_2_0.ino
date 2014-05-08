/*

Timelapse Dolly - Automation, Systemer og Komponenter
@version 2.0
@date 2014-05-02
@author Peter Savnik

@Description
Make a step motor control motion of a Camera mounted on a dolly.
Input: Distance and Time


*/
#include <math.h>
#include <Keypad.h>

const int DEBUG = 0;  // Toogle Serial output
const int PIN_DIR = 8;
const int PIN_STEP = 9;
const int PIN_ENABLE = A0;  

const int STEP_RES = 1;  // Stepper resolution
const int STEPS_PR_ROUND = 200*STEP_RES; // Total steps pr. round
const float RADIUS = 0.005; // [m]
const float DIST_PR_STEP = (2*RADIUS*PI)/STEPS_PR_ROUND; // Omkreds/steps pr round

float distance = 0.0;  // [m] Record the number of steps we've taken
float distance_to_go = 2; // [m]
int steps_to_go = distance_to_go/DIST_PR_STEP; // total amount of steps to the distance_to_go
float time_to_go = 2.0*60.0*60.0; // [s] time to do the distance  
int steps = 0;  // Total count of steps
int run = 0;   // Control var for start/stop fcn
int dir = 1;   // Control direction

// Calc delay time
float time_pr_step = time_to_go/steps_to_go; // the delay time between steps in secs

long interval = time_pr_step*pow(10,3); // Convert to milliSec
unsigned long previousMillis = millis(); // Will store last time
unsigned long currentMillis = millis();  // Will store current milliSec 


// Setup keypad ------------------
const byte ROWS = 4; //four rows
const byte COLS = 4; //four columns
//define the cymbols on the buttons of the keypads
char hexaKeys[ROWS][COLS] = {
  {'0','1','2','3'},
  {'4','5','6','7'},
  {'8','9','A','B'},
  {'C','D','E','F'}
};
// 0  Start/Stop
// 1  Direction control
// 2  Manuel move
// 3  Manuel move
byte rowPins[ROWS] = {3, 2, 1, 0}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {7, 6, 5, 4}; //connect to the column pinouts of the keypad

//initialize an instance of class NewKeypad
Keypad customKeypad = Keypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); 

void setup() {
    if(DEBUG == 1){  
    Serial.begin(9600);  
    while(!Serial){
     ; // wait for serial to begin 
    }
    Serial.println('Timelapse Dolly Start');
  }
  pinMode(PIN_DIR, OUTPUT);     
  pinMode(PIN_STEP, OUTPUT);
  pinMode(PIN_ENABLE, OUTPUT);
  digitalWrite(PIN_DIR, HIGH);
  digitalWrite(PIN_STEP, LOW);
  digitalWrite(PIN_ENABLE, HIGH);
  
  //establishContact();  // send a byte to establish contact until receiver responds 
}

void loop() {
  
  char customKey = customKeypad.getKey();
  
  if (customKey && (DEBUG == 1)){
    Serial.println(customKey);
  }
  
  // Change direction
  if(customKey == '1'){
     changeDirection();
  }
    
  // Manuel move
  /*if(customKeypad.key['2'].kstate == PRESSED){
    //Serial.println("test");
    if(run == 0) enableMotor();
    digitalWrite(PIN_DIR,LOW);  // Set direction
    // Do a step
    digitalWrite(PIN_STEP, HIGH);
    delayMicroseconds(1000);
    digitalWrite(PIN_STEP, LOW);
    delayMicroseconds(1000);
    
  }*/
    
  
  // Start running
  if(customKey == '0'){
    enableMotorToogle();
  }
  
  // Check to see if we are at the end of our move
  if ((steps*DIST_PR_STEP < distance_to_go) && (run == 1))
  { 
    currentMillis = millis();
    if(currentMillis - previousMillis > interval){
       previousMillis = currentMillis;   
      digitalWrite(PIN_STEP, HIGH);
      delay(1);
      digitalWrite(PIN_STEP, LOW); 
      steps = steps + 1;   // record this step
    }    
  }

  // If dist is done disable
  if ((steps*DIST_PR_STEP >= distance_to_go)){
    disableMotor(); 
    steps = 0; 
  }
}


void enableMotor(){
 // Enable motor driver
       digitalWrite(PIN_ENABLE, LOW);
      if(DEBUG == 1) Serial.println("Start");
}

void disableMotor(){
 // Enable motor driver
       digitalWrite(PIN_ENABLE, HIGH); 
       if(DEBUG == 1) Serial.println("Stop");
}


void enableMotorToogle(){
 // Enable motor driver
     if(run == 0){
       enableMotor();
       run = 1;
     }
     else if(run == 1){
       disableMotor();
       run = 0;
     } 
}

void changeDirection(){
  if(dir == 1){
        dir = 0;
        digitalWrite(PIN_DIR,LOW);
     }
    else{
       dir = 1;
       digitalWrite(PIN_DIR,HIGH);
    } 
}


void establishContact() {
  while (Serial.available() <= 0) {
    Serial.println("0,0,0");   // send an initial string
    delay(300);
  }
}

/*
|| @changelog
|| | 2.0 2014-05-01 - Peter Savnik : Change from delay to timer control
|| | 1.2 2014-05-01 - Peter Savnik : Added Debug function, enable serial output when debugging
|| | 1.1 2014-05-01 - Peter Savnik : Added direction control
|| | 1.0 2014-05-01 - Peter Savnik : Initial Release
|| #
*/


