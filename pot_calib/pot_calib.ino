/*

Potentiometer Calibration - Automation, Systemer og Komponenter
@version 1.0
@date 2014-05-05
@author Peter Savnik

@Description
Calibate input from potentiometer.
Analog in out serial


*/

// These constants won't change.  They're used to give names
// to the pins used:
const int analogInPin = A1;  // Analog input pin that the potentiometer is attached to

int sensorValue = 0;        // value read from the pot
int outputValue = 0;  

void setup() {
  // initialize serial communications at 9600 bps:
  Serial.begin(9600); 
  pinMode(analogInPin, INPUT);
}

void loop() {
  // read the analog in value:
  sensorValue = analogRead(analogInPin);            
  // map it to the range of the analog out:
  outputValue = map(sensorValue, 0, 1023, 0, 255);  
  // print the results to the serial monitor:
  Serial.print("sensor = " );                       
  Serial.println(sensorValue);        

  // wait 2 milliseconds before the next loop
  // for the analog-to-digital converter to settle
  // after the last reading:
  delay(2);                     
}
