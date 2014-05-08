/*

Timelapse Dolly - Automation, Systemer og Komponenter
@version 2.0
@date 2014-05-05
@author Peter Savnik

@Description
Make a step motor control motion of a Camera mounted on a dolly.
Input: Distance and Time


*/
#include <math.h>
#include <Keypad.h>

const int DEBUG = 0;  // Toogle Serial output
const int PIN_RUN = 8;
const int PIN_STEP = 9;
const int PIN_ENABLE = A0; 
const int PIN_TIME_SCALE_POT = A1;

const int STEP_RES = 1;  // Stepper resolution
const int STEPS_PR_ROUND = 200*STEP_RES; // Total steps pr. round
const float RADIUS = 0.003; // [m]
const float DIST_PR_STEP = (2*RADIUS*PI)/STEPS_PR_ROUND; // Omkreds/steps pr round


float distance = 0.0;  // [m] Record the number of steps we've taken
float distance_to_go = 1; // [m]
int steps_to_go = distance_to_go/DIST_PR_STEP; // total amount of steps to the distance_to_go
float time_to_go = 60; // [s] time to do the distance  
int steps = 0;  // Total count of steps
int run = 0;   // Control var for start/stop fcn
int dir = 1;   // Control direction
int sensorValue = 0; // Time scale pot sensor
int stepState = 0; // Step output state

String state = "IDLE", next_state = "IDLE";  // Statemachine control

float time_pr_step;
float time_pr_step_ms;

unsigned long current_time = 0;
unsigned long prev_time = 0;

void setup() {
    Serial.begin(9600);  
    while(!Serial){
     ; // wait for serial to begin 
    }
    Serial.println('Timelapse Dolly Start');
    
  pinMode(PIN_RUN, INPUT_PULLUP);     
  pinMode(PIN_STEP, OUTPUT);
  pinMode(PIN_ENABLE, OUTPUT);
  pinMode(PIN_TIME_SCALE_POT, INPUT);
  digitalWrite(PIN_STEP, LOW);
  digitalWrite(PIN_ENABLE, HIGH);
  
  //establishContact();  // send a byte to establish contact until receiver responds 
}

void loop() {

  run = digitalRead(PIN_RUN);
  
  // Statemachine!  
    if (state == "IDLE"){
      // Wait for action
      
      // Calc delay time
      float time_pr_step = time_to_go/steps_to_go; // the delay time between steps
      float time_pr_step_ms = time_pr_step*pow(10,3); // Convert to millisec
      if (time_pr_step_ms < 1) time_pr_step_ms = 1;  // Don't go too fast!
      
      // Print Parameters
      Serial.print("Dist: ");
      Serial.print(distance_to_go);
      Serial.print(", Time: ");
      Serial.print(time_to_go);
      Serial.print(", Step Time: ");
      Serial.println(time_pr_step_ms);
      
      // Change state
      // Start running
      if(run == LOW){
        Serial.println("Next state: MOVE!");
        digitalWrite(PIN_ENABLE, LOW);
        next_state = "MOVE";
      }
      else{
        next_state = "IDLE"; 
      }
    
    }
    else if (state == "MOVE"){
      // MOVE
      // Check to see if we are at the end of our move
      time_pr_step = 100/16;
      if ((steps*DIST_PR_STEP < distance_to_go) && (run == LOW)) {
        current_time = millis();
        if(current_time-prev_time >= time_pr_step/2){
          // Step toogle
          if(stepState == 0){
            digitalWrite(PIN_STEP, HIGH); // step high
            stepState = 1;
          }
          else if(stepState == 1){
            digitalWrite(PIN_STEP, LOW); // step low
            stepState = 0;
            steps = 1+steps; // Incremetent steps 
            Serial.println(steps);
          }
          prev_time = current_time;
        }    
     
      }
       
      // Change state
      if(run == HIGH){  // Stop running
        next_state = "IDLE";
        digitalWrite(PIN_ENABLE, HIGH);
      }
      else{
        next_state = "MOVE"; // Stay in state 
      }

    }    
    else {
       Serial.print("Error in statemachine: ");
       Serial.println(state); 
    }
    if(state != next_state) Serial.println(next_state);
  state = next_state;
  
  


  
 /* 
  // If dist is done disable
  if ((steps*DIST_PR_STEP >= distance_to_go)){
    disableMotor(); 
    steps = 0; 
  }
  */
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




void establishContact() {
  while (Serial.available() <= 0) {
    Serial.println("0,0,0");   // send an initial string
    delay(300);
  }
}

/*
|| @changelog
|| | 2.0 2014-05-06 - Peter Savnik : Added state machine & Millis as delay
|| | 1.3 2014-05-05 - Peter Savnik : Added Potentiometer control of time
|| | 1.2 2014-05-01 - Peter Savnik : Added Debug function, enable serial output when debugging
|| | 1.1 2014-05-01 - Peter Savnik : Added direction control
|| | 1.0 2014-05-01 - Peter Savnik : Initial Release
|| #
*/


