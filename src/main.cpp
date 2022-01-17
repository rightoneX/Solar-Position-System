/*ver 20211194
 * 
 * Created by Alex Ustinov @2021/10/16 for all requirements contact by auco.co.nz
 * The code is design to control a solar mirror by horizontal and vertical position based on the signals from array of four Photo Sensors.
 * During the day time, when the lux level is high the system will track the sun movement and adjust the mirror position accordingly.
 * After the sun set the solar reflector will be return to start point.
 * Based on the special requirements received from client the solar reflector will be raised to the top position be for return it to the start point.
 * all names and numbers had been set based if you were stating at front of the solar reflector and looking on it.
 */

#include <Arduino.h>

const int photoSensorTL = A0;   // photo sensor top left
const int photoSensorTR = A1;   // photo sensor top right
const int photoSensorBL = A2;   // photo sensor bottom left
const int photoSensorBR = A3;   // photo sensor bottom right

int averageSensorTime = 3000;  // average photo sensor reading before command the motor control

long photoSensorValueTL;   // avarge sensor reading top left
long photoSensorValueTR;   // avarge sensor reading top right
long photoSensorValueBL;   // avarge sensor reading bottom left
long photoSensorValueBR;   // avarge sensor reading bottom right

const int collectorTemp = A4;   // solar collector temperature

const int horizontalMotionLeft = 6;    // motor signal to drive system horizontally to left postion
const int horizontalMotionRight = 7;    // motor signal to drive system horizontally to right postion
const int verticalMotionTop = 8;    // motor signal to drive system vertically to top postion
const int verticalMotionBottom = 9;    // motor signal to drive system vertically to bottom positton

const int horizontalMotionLeftStop = 2;    // motor signal to drive system horizontally to left postion
const int horizontalMotionRightStop = 3;    // motor signal to drive system horizontally to right postion
const int verticalMotionTopStop = 4;    // motor signal to drive system vertically to top postion
const int verticalMotionBottomStop = 5;    // motor signal to drive system vertically to bottom positton

int motionControlSignalH = 0;   //Horizontal Motor Control Signal: Stop = 0, Left = -1, Right = +1
int motionControlSignalV = 0;   //Vertical Motor Control Signal: Stop = 0, Down = -2, UP = +2

int hRuler = 0; //measure the working area of reflector in horizontal area
int vRuler = 0; //measure the working area of reflector in vertical area

int tempReading;
int TEMPCOEFFICIENT = 220;

int actionDelay = 2; // 

int startCount = 0;  //run calibration task on a start

//declaration of subfunction for CPP IDE only
void collectorTempCalc();    
void photoSensorAverageReading(); // get average photo sensor reading
void photoSensorCommand();        // produce the motor command against the photo sensor reading
void limitPositionCheck();        // check if the reflector reached the motion limit
void motorControl();              // adjust position of the reflector based on photo sensor signal
void comportDisplay();            // used for testing data, please comment after production period



void setup() {
  // reflector motor control
  pinMode(horizontalMotionLeft, OUTPUT);     // set pin to output
  digitalWrite(horizontalMotionLeft, LOW);   // turn output off
  pinMode(horizontalMotionRight, OUTPUT);     // set pin to output
  digitalWrite(horizontalMotionRight, LOW);   // turn output off
  pinMode(verticalMotionTop, OUTPUT);     // set pin to output
  digitalWrite(verticalMotionTop, LOW);   // turn output off
  pinMode(verticalMotionBottom, OUTPUT);     // set pin to output
  digitalWrite(verticalMotionBottom, LOW);   // turn output off
  
  // reflector limit switches
  pinMode(horizontalMotionLeftStop, INPUT);  // set pin to input
  pinMode(horizontalMotionRightStop, INPUT);  // set pin to input
  pinMode(verticalMotionTopStop, INPUT);  // set pin to input
  pinMode(verticalMotionBottomStop, INPUT);  // set pin to input
  
  // comport used for testing only
  Serial.begin(9600);
}


  // the main loop 
  void loop() {

//  if (startCount == 0) { systemCalibration(); }  // calibration of the system, start once on booting
 
  collectorTempCalc();    
  photoSensorAverageReading(); // get average photo sensor reading
  photoSensorCommand();        // produce the motor command against the photo sensor reading
  limitPositionCheck();        // check if the reflector reached the motion limit
  motorControl();              // adjust position of the reflector based on photo sensor signal
  comportDisplay();            // used for testing data, please comment after production period
}



void photoSensorAverageReading() {
     
  for (int i = 0; i < averageSensorTime; i++) {
    photoSensorValueTL = photoSensorValueTL + analogRead(photoSensorTL);
    photoSensorValueTR = photoSensorValueTR + analogRead(photoSensorTR);
    photoSensorValueBL = photoSensorValueBL + analogRead(photoSensorBL);
    photoSensorValueBR = photoSensorValueBR + analogRead(photoSensorBR); 
    
    limitPositionCheck();        // check if the reflector reached the motion limit
    delay(actionDelay);
  }
  
    photoSensorValueTL = photoSensorValueTL / averageSensorTime;
    photoSensorValueTR = photoSensorValueTR / averageSensorTime;
    photoSensorValueBL = photoSensorValueBL / averageSensorTime;
    photoSensorValueBR = photoSensorValueBR / averageSensorTime; 
}

// photoSensorCommand returns motor control signal
void photoSensorCommand() {
  /*
   * read lux level from the photo sensors
   * the sensors assembled as square position
   * TL | TR
   * BL | BR
   * 
   * motionControlSignal signal = 0 no motion, signal = 1 horizontal motion to left, signal = -1 horizontal motion to right, signal = 2 vertical motion to top, signal = -2 vertical motion to bottom
   */  
    motionControlSignalH = 0;   //Horizontal Motor Control Signal: Stop = 0, Left = -1, Right = +1
    motionControlSignalV = 0;   //Vertical Motor Control Signal: Stop = 0, Down = -2, Up = +2
    
   if (photoSensorValueTL > photoSensorValueTR && photoSensorValueBL > photoSensorValueBR) { 
    // system turns to left
    motionControlSignalH = -1;
   }

   if (photoSensorValueTL < photoSensorValueTR && photoSensorValueBL < photoSensorValueBR) { 
    // system turns to right
   motionControlSignalH = 1;  
   }

   if (photoSensorValueTL >  photoSensorValueBL && photoSensorValueTR > photoSensorValueBR) { 
    // system turns to top
    motionControlSignalV = 2;
   }

   if (photoSensorValueTL <  photoSensorValueBL && photoSensorValueTR < photoSensorValueBR) { 
    // system turns to bottom
   motionControlSignalV = -2;
   }
}



void motorControl() {

  // turn left if no stop switch reached yet
    if (motionControlSignalH == -1 && digitalRead(horizontalMotionLeftStop) == LOW) {
      digitalWrite(horizontalMotionLeft, HIGH);   // turn output on
      digitalWrite(horizontalMotionRight, LOW);   // turn output off
    }
  
  // turn right if no stop switch reached yet
    if (motionControlSignalH == 1 && digitalRead(horizontalMotionRightStop) == LOW) {
      digitalWrite(horizontalMotionLeft, LOW);    // turn output off
      digitalWrite(horizontalMotionRight, HIGH);  // turn output on    
    }

  // turn motors off
    if (motionControlSignalH == 0) {
      digitalWrite(horizontalMotionLeft, LOW);     // turn output off
      digitalWrite(horizontalMotionRight, LOW);    // turn output off   
    }

  // turn up if no stop switch reached yet
    if (motionControlSignalV == 2 && digitalRead(verticalMotionTopStop) == LOW) {
      digitalWrite(verticalMotionTop, HIGH);      // turn output on
      digitalWrite(verticalMotionBottom, LOW);    // turn output off            
    }

  // turn down if no stop switch reached yet
    if (motionControlSignalV == -2 && digitalRead(verticalMotionBottomStop) == LOW) {
      digitalWrite(verticalMotionTop, LOW);        // turn output off 
      digitalWrite(verticalMotionBottom, HIGH);    // turn output on           
    }

  // turn motors off
    if (motionControlSignalV == 0) {
      digitalWrite(verticalMotionTop, LOW);     // turn output off
      digitalWrite(verticalMotionBottom, LOW);    // turn output off   
    }
  
}



void limitPositionCheck() {
  // check if the reflector reached the limit

   // turn left if no stop switch reached yet
    if (digitalRead(horizontalMotionLeftStop) == HIGH) {
      digitalWrite(horizontalMotionLeft, LOW);   // turn output on
      digitalWrite(horizontalMotionRight, LOW);   // turn output off 
    }
  
  // turn right if no stop switch reached yet
    if (digitalRead(horizontalMotionRightStop) == HIGH) {
      digitalWrite(horizontalMotionLeft, LOW);    // turn output off
      digitalWrite(horizontalMotionRight, LOW);  // turn output on      
    }

  // turn up if no stop switch reached yet
    if (digitalRead(verticalMotionTopStop) == HIGH) {
      digitalWrite(verticalMotionTop, LOW);      // turn output on
      digitalWrite(verticalMotionBottom, LOW);    // turn output off            
    }

  // turn down if no stop switch reached yet
    if (digitalRead(verticalMotionBottomStop) == HIGH) {
      digitalWrite(verticalMotionTop, LOW);        // turn output off 
      digitalWrite(verticalMotionBottom, LOW);    // turn output on           
    }
}


void systemCalibration() {
  // measure the motion areas of the reflector
   startCount++; //runs once on startup
  
  hRuler = 0;
  vRuler = 0;

  motionControlSignalV = -2; // set to down 
  do {
    motorControl();
    Serial.println("Vertical Positioning to the point");
  }while (!digitalRead(verticalMotionBottomStop));
  
  motionControlSignalV = 2; // set to top
  do {
    motorControl();
    vRuler++;              // count the total time motion
    Serial.println("Vertical Motion Time is : " + String(vRuler));
  }while (!digitalRead(verticalMotionTopStop));
  
  motionControlSignalH = -1; // set to left 
  do {
    motorControl();
    Serial.println("Horizontal  Positioning to the point");
  }while (!digitalRead(horizontalMotionLeftStop));

  motionControlSignalH = 1; // set to right 
  do {
    motorControl();
    hRuler++;               // count the total time motion
    Serial.println("Horizontal Motion Time is : " + String(hRuler));
  }while (!digitalRead(horizontalMotionRightStop));
  
}


void collectorTempCalc() {
  // calculating the receiver temperature
  // currently used for statistic only
  tempReading = TEMPCOEFFICIENT / ((1023 / analogRead(collectorTemp))  - 1);
}


void comportDisplay() {
  // for testing perpuse only
  if (startCount == 1){ Serial.println("System in testing mode ...."); }
  
  Serial.println("Receiver Temp : " + String(tempReading) + " C");
  
  Serial.println(".....");  //
  
  Serial.println("Photo Sensor Top Left : " + String(photoSensorValueTL) + " Lux");
  Serial.println("Photo Sensor Top Right : " + String(photoSensorValueTR) + " Lux");
  Serial.println("Photo Sensor Bottom Left : " + String(photoSensorValueBL) + " Lux");
  Serial.println("Photo Sensor Bottom Right : " + String(photoSensorValueBR) + " Lux");
  
  Serial.println(".....");  //
  
  Serial.print("Horizontal Motor Signal : ");
  switch (motionControlSignalH) {
  case 1:
    Serial.print("Right");   // right
    break;
  case -1:
    Serial.print("Leftt");  // left
    break;
  default:
    Serial.print("Stop");  // stop
    break;
  } 
  Serial.print("  | Vertical Motor Signal : ");
  switch (motionControlSignalV) {
  case 2:
    Serial.println("Up");  // up
    break;
  case -2:
    Serial.println("Down");  // down
    break;
  default:
    Serial.println("Stop");  // stop
    break;
  }
  
  Serial.println(".....");  //
  
  Serial.println("Left Stop: " + String(digitalRead(horizontalMotionLeftStop)));  // set pin to input
  Serial.println("Right Stop: " + String(digitalRead(horizontalMotionRightStop)));  // set pin to input
  Serial.println("Top Stop: " + String(digitalRead(verticalMotionTopStop)));  // set pin to input
  Serial.println("Bottom Stop: " + String(digitalRead(verticalMotionBottomStop)));  // set pin to input
  Serial.println("........................................................................");  //
}