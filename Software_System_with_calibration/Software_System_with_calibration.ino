#include <HX711_ADC.h> // Load Cell
#include <Wire.h> // Must include Wire library for I2C
#include <SparkFun_MMA8452Q.h> // Accelerometer : Includes the SFE_MMA8452Q library
#include <math.h>

// PIN DECLATIONS
const int HX711_dout = 5; //mcu > HX711 dout pin
const int HX711_sck = 4; //mcu > HX711 sck pin

// ACCELEROMETER
MMA8452Q accel;
float angleData;
int startingAngle = 0;

// LOAD CELL
HX711_ADC LoadCell(HX711_dout, HX711_sck);
double loadCellData;
float calibrationValue = 75.84;
// FIRST 16 VALUES MUST BE REMOVED

// OTHER
static boolean newDataReady = 0;
const int serialPrintInterval = 0; //increase value to slow down serial print activity
unsigned long t = 0;

void setup() {
  Serial.begin(9600); delay(10);
  Serial.println("| ---- | Welcome to AbilityMade Stiffness Testing Machine Software Module | ---- |");
  Serial.println("| ---- |                    System Setup Will Now Occur                   | ---- |");
  delay(2000);
  Serial.println("Starting Load Cell calibration...");

  unsigned long stabilizingtime = 5000; // preciscion right after power-up can be improved by adding a few seconds of stabilizing time
  boolean _tare = true; //set this to false if you don't want tare to be performed in the next step
  LoadCell.start(stabilizingtime, _tare);
  LoadCell.setCalFactor(calibrationValue); // set calibration value (float)
  Serial.println("Load Cell has been successfully calibrated!");

  accel.init();
  LoadCell.begin();
  delay(1000);
  Serial.println("Starting Angle Sensor calibration...");
  // Angle Sensor Calibration
  int i = 0;
  int angle; 
  int sum = 0;
  // Take the average of 5 initial angle readings
  while (i < 5){
    accel.read();
    angle = atan(accel.cz/sqrt(pow(accel.cx,2) + pow(accel.cy,2)))*180/M_PI;
    sum = sum + angle;
    i = i+ 1;
  }
  //Find the calibration angle
  startingAngle = sum/i;
  Serial.println("Angle Sensor has been successfully calibrated!");
  delay(1000);
  Serial.println("Please begin Excel data stream. No other instructions will be printed to the Serial Monitor.");
  delay(5000);
  //LoadCell.setReverseOutput(); //uncomment to turn a negative output value to positive
}

void loop() {
  // get smoothed value from the dataset:
  if (millis() > t + serialPrintInterval) {
    newDataReady = true;
    if (LoadCell.update()) {
      loadCellData = LoadCell.getData();
    } else { newDataReady = false; }

    if (accel.available()) {
      //Serial.println("here");
      accel.read();
      //Serial.println(accel.cz);
      angleData = atan(accel.cz/sqrt(pow(accel.cx,2) + pow(accel.cy,2)))*180/M_PI - startingAngle;
    } else { newDataReady = false; }

    if (newDataReady) {
      Serial.print(loadCellData, 3);
      Serial.print(",");
      Serial.println(angleData, 3);
    }
    t = millis();
  }
}