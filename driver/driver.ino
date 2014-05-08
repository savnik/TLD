/*

Timelapse Dolly - Automation, Systemer og Komponenter
@version 1.3
@date 2014-05-05
@author Peter Savnik

@Description
Make a step motor control motion of a Camera mounted on a dolly.
Input: Distance and Time


*/
#include <math.h>
#include <Keypad.h>

const int DEBUG = 1;  // Toogle Serial output
const int PIN_DIR = 8;
const int PIN_STEP = 9;
const int PIN_ENABLE = A0; 
const int PIN_TIME_SCALE_POT = A1;

const int STEP_RES = 1;  // Stepper resolution
const int STEPS_PR_ROUND = 200*STEP_RES; // Total steps pr. round
const float RADIUS = 0.003; // [m]
const float DIST_PR_STEP = (2*RADIUS*PI)/STEPS_PR_ROUND; // Omkreds/steps pr round
const float TIME_SCALE_MAX = 60;  // [minutter] Max time on time scale slider
const int TIME_SCALE_POT_MAX = 1023;  // Calib to 5v
const int TIME_SCALE_POT_MIN = 66; // Calib to 0v

float distance = 0.0;  // [m] Record the number of steps we've taken
float distance_to_go = 1; // [m]
int steps_to_go = distance_to_go/DIST_PR_STEP; // total amount of steps to the distance_to_go
float time_to_go = 0; // [s] time to do the distance  
int steps = 0;  // Total count of steps
int run = 0;   // Control var for start/stop fcn
int dir = 0;   // Control direction
int sensorValue = 0; // Time scale pot sensor

float time_pr_step;
float time_pr_step_ms;

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
  pinMode(PIN_TIME_SCALE_POT, INPUT);
  digitalWrite(PIN_DIR, HIGH);
  digitalWrite(PIN_STEP, LOW);
  digitalWrite(PIN_ENABLE, HIGH);
  
  //establishContact();  // send a byte to establish contact until receiver responds 
}

void loop() {
  // Get inputs
  char customKey = customKeypad.getKey();
  
  if(run == 0){
  // read the analog in value TIME_SCALE_POT
  sensorValue = analogRead(PIN_TIME_SCALE_POT);          
  // map it to the range of the analog out:
  time_to_go = map(sensorValue-TIME_SCALE_POT_MIN, 0, TIME_SCALE_POT_MAX-TIME_SCALE_POT_MIN, 0, TIME_SCALE_MAX*60); // -TIME_CSCALE_POT_MIN from calibration
  
  // Calc delay time
  float time_pr_step = time_to_go/steps_to_go; // the delay time between steps
  float time_pr_step_ms = time_pr_step*pow(10,3); // Convert to millisec
  if (time_pr_step_ms < 1) time_pr_step = 1;  // Don't go too fast!

  // Print Parameters
      Serial.print("Dist[m]: ");
      Serial.print(distance_to_go);
      Serial.print(", Time[s]: ");
      Serial.print(time_to_go);
      Serial.print(", Step Time[ms]: ");
      Serial.println(time_pr_step_ms);
  }
  
  if (customKey && (DEBUG == 1)){
    Serial.println(customKey);
  }
  
  // Change direction
  if(customKey == '1'){
     changeDirection();
  }
    
  
    
  
  // Start running
  if(customKey == '0'){
    enableMotorToogle();
  }
  
  // Check to see if we are at the end of our move
  if ((steps*DIST_PR_STEP < distance_to_go) && (run == 1))
  {    
    digitalWrite(PIN_STEP, HIGH);
    //delay(time_pr_step_ms/2); // Micro sec
    delay(1000); // Micro sec
    digitalWrite(PIN_STEP, LOW); 
    //delay(time_pr_step_ms/2); //Micro sec    
    delay(1000); // Micro sec
    steps = steps + 1;   // record this step
    Serial.println(steps);
  }

  // If dist is done disable
  if ((steps*DIST_PR_STEP >= distance_to_go)){
    disableMotor(); 
    steps = 0; 
    run = 0;
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
|| | 1.3 2014-05-05 - Peter Savnik : Added Potentiometer control of time
|| | 1.2 2014-05-01 - Peter Savnik : Added Debug function, enable serial output when debugging
|| | 1.1 2014-05-01 - Peter Savnik : Added direction control
|| | 1.0 2014-05-01 - Peter Savnik : Initial Release
|| #
*/


