const int PIN_DIR = 8;
const int PIN_STEP = 9;
const int PIN_ENABLE = A0;  

const int STEP_RES = 1;  // Stepper resolution
const int STEPS_PR_ROUND = 200*STEP_RES; // Total steps pr. round
const float RADIUS = 0.005; // [m]
const float DIST_PR_STEP = (2*RADIUS*PI)/STEPS_PR_ROUND; // Omkreds/steps pr round

float time_pr_step_ms = 1875;
float time_pr_step = 0.5655;

void setup() {
  pinMode(PIN_DIR, OUTPUT);     
  pinMode(PIN_STEP, OUTPUT);
  pinMode(PIN_ENABLE, OUTPUT);
  digitalWrite(PIN_DIR, HIGH);
  digitalWrite(PIN_STEP, LOW);
  digitalWrite(PIN_ENABLE, LOW);
}


void loop() {
    digitalWrite(PIN_STEP, HIGH);
    //delayMicroseconds(time_pr_step_ms/2); // Micro sec
    delay(250);
    digitalWrite(PIN_STEP, LOW); 
    //delayMicroseconds(time_pr_step_ms/2); //Micro sec   
   delay(250); 
}
