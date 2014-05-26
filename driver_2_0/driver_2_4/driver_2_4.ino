/*

Timelapse Dolly - Automation, Systemer og Komponenter
@version 2.1
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
const int PIN_DIST_SCALE_POT = A2;
const float Pi = 3.14159;

const float STEP_RES = 16.0;  // Stepper resolution
const float STEPS_PR_ROUND = 200.0*STEP_RES; // Total steps pr. round
const float RADIUS = 0.0272; // [m]
const float DIST_PR_STEP = (2.0*RADIUS*Pi)/STEPS_PR_ROUND; //[cm] Omkreds/steps pr round


float distance = 0.0;  // [m] Record the number dist we've taken
float distance_to_go = 5.0; // [m] MAX:4.295*10^9 MIN:5.3407*10^-5
float DISTANCE_TO_GO_MAX = 20.0; // [m]
unsigned long steps_to_go = (distance_to_go)/DIST_PR_STEP; // total amount of steps to the distance_to_go
float time_to_go = 60.0*60.0; // [s] time to do the distance
float TIME_TO_GO_MAX = 60.0*60.0; // [s] max time to go on pottentiometer
unsigned long steps = 0;  // Total count of steps
int run = 0;   // Control var for start/stop fcn
int dir = 1;   // Control direction
int sensorValue = 0; // Time scale pot sensor
int stepState = 0; // Step output state

String state = "IDLE", next_state = "IDLE";  // Statemachine control

float time_pr_step;
float time_pr_step_ms;
float time_pr_step_micros;
long time_start = 0;
long time_end = 0;
int use_micros_time = 0;

unsigned long current_time = 0;
unsigned long prev_time = 0;

void setup() {
    Serial.begin(115200);  
    /*while(!Serial){
     ; // wait for serial to begin 
    }*/
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

  run = digitalRead(PIN_RUN); // Start/Stop knap
  
  // Statemachine!  
    if (state == "IDLE"){
      // Wait for action
      
      // Read time potentiometer
      int timePotValue = analogRead(PIN_TIME_SCALE_POT);
      // Calc time to go
      float time_to_go = timePotValue * (TIME_TO_GO_MAX / 1023.0);
      
      // Read time potentiometer
      int distPotValue = analogRead(PIN_DIST_SCALE_POT);
      // Calc time to go
      float distance_to_go = distPotValue * (DISTANCE_TO_GO_MAX / 1023.0);
      Serial.println(distance_to_go);
      
      // Calc delay time
      time_pr_step = time_to_go/steps_to_go; // the delay time between steps [s]/[step]
      time_pr_step_ms = time_pr_step*pow(10,3); // Convert to millisec [ms]/[step]
      time_pr_step_micros = time_pr_step*pow(10,6); // Convert to millisec [micros]/[step]
      if (time_pr_step_ms < 1) time_pr_step_ms = 1;  // Don't go too fast! saturation
      if (time_pr_step_micros < 1) time_pr_step_micros = 1;  // Don't go too fast! saturation
      
      if(time_to_go < 60*60) use_micros_time = 1; // 60[m]*60[s] use micros instead of millis
      
      // Print Parameters
      /*Serial.print("Dist: ");
      Serial.print(distance_to_go);
      Serial.print(", Steps to go:");
      Serial.print(steps_to_go);
      Serial.print(", Time: ");
      Serial.print(time_to_go);
      Serial.print(", Step Time: ");
      Serial.println(time_pr_step_ms);*/
     
      // Change state
      // Start running
      if(run == LOW){
        enableMotor();
        time_start = millis();
        next_state = "MOVE";
      }
      else{
        next_state = "IDLE"; 
      }
    
    }
    else if (state == "MOVE"){
      next_state = "MOVE";
      // MOVE
      // Check to see if we are at the end of our move
      if ((steps*DIST_PR_STEP < distance_to_go) && (run == LOW)) {
        
        
        if(use_micros_time == 1){ 
          current_time = micros();    
          if(current_time-prev_time >= time_pr_step_micros/2){
            prev_time = current_time;
            // Step toogle
            if(stepState == 0){
              digitalWrite(PIN_STEP, HIGH); // step high
              stepState = 1;
            }
            else if(stepState == 1){
              digitalWrite(PIN_STEP, LOW); // step low
              stepState = 0;
              steps = 1+steps; // Incremetent steps 
              //Serial.println(steps_to_go-steps);
            } // end step state
            
          } // if time
            
        } // if use micro
        else{  
          current_time = millis();
          if(current_time-prev_time >= time_pr_step_ms/2){
            prev_time = current_time;
            // Step toogle
            if(stepState == 0){
              digitalWrite(PIN_STEP, HIGH); // step high
              stepState = 1;
            }
            else if(stepState == 1){
              digitalWrite(PIN_STEP, LOW); // step low
              stepState = 0;
              steps = 1+steps; // Incremetent steps 
              //Serial.println(steps_to_go-steps);
            } // end step state
            
          } // if time
        }// else use micros   
     
      } // if dist <
       
      // If dist is done go to wait state 
      if (steps*DIST_PR_STEP >= distance_to_go){
         next_state = "DONE";
         disableMotor(); // disable motor
         // calc run time
         time_end = millis();
         Serial.print("Time: ");
         Serial.println((time_end-time_start)*pow(10,-3)); //[s]
         steps = 0; // reset dist
         
      } // if dist >=
      
      
      // Change state
      if(run == HIGH){  // Stop running      
        next_state = "IDLE";
        disableMotor(); // disable motor
      }

    } // state  
   
    else if(state == "DONE"){
      next_state = "DONE";
      // If off go to idle
      if(run == HIGH) next_state = "IDLE";
     
    } // state DONE 
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
|| | 2.4 2014-05-26 - Peter Savnik : Fix potentiometer control of time
|| | 2.2 2014-05-08 - Peter Savnik : Time function fix
|| | 2.1 2014-05-08 - Peter Savnik : Dist is calibrated 
|| | 2.0 2014-05-06 - Peter Savnik : Added state machine & Millis as delay
|| | 1.3 2014-05-05 - Peter Savnik : Added Potentiometer control of time
|| | 1.2 2014-05-01 - Peter Savnik : Added Debug function, enable serial output when debugging
|| | 1.1 2014-05-01 - Peter Savnik : Added direction control
|| | 1.0 2014-05-01 - Peter Savnik : Initial Release
|| #
*/


