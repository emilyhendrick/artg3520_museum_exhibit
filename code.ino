// Motor driver code from SparkFun demo: https://learn.sparkfun.com/tutorials/hookup-guide-for-the-qwiic-motor-driver 

#include <Arduino.h>
#include <stdint.h>
#include "SCMD.h"
#include "SCMD_config.h" 
#include "Wire.h"

SCMD myMotorDriver; 
int fsrAnalogPin = 0; // fabric is connected to analog 0
int fsrReading;      // the analog reading from the fabric

void setup()
{
  pinMode(8, INPUT_PULLUP); // Use to halt motor movement (ground)

  Serial.begin(9600);
  Serial.println("Starting sketch.");

  //***** Configure the Motor Driver's Settings *****//
  //  .commInter face is I2C_MODE 
  myMotorDriver.settings.commInterface = I2C_MODE;

  //  set address if I2C configuration selected with the config jumpers
  myMotorDriver.settings.I2CAddress = 0x5D; //config pattern is "1000" (default) on board for address 0x5D

  //  set chip select if SPI selected with the config jumpers
  myMotorDriver.settings.chipSelectPin = 10;

  //*****initialize the driver get wait for idle*****//
  while ( myMotorDriver.begin() != 0xA9 ) //Wait until a valid ID word is returned
  {
    // Serial.println( "ID mismatch, trying again" );
    delay(500);
  }
  Serial.println( "ID matches 0xA9" );

  //  Check to make sure the driver is done looking for peripherals before beginning
  Serial.print("Waiting for enumeration...");
  while ( myMotorDriver.ready() == false );
  Serial.println("Done.");
  Serial.println();

  //*****Set application settings and enable driver*****//

  //Uncomment code for motor 1 inversion
  while ( myMotorDriver.busy() ); //Waits until the SCMD is available.
  myMotorDriver.inversionMode(1, 1); //invert motor 1

  while ( myMotorDriver.busy() );
  myMotorDriver.enable(); //Enables the output driver hardware

}

#define LEFT_MOTOR 0
#define RIGHT_MOTOR 1
void loop()
{
  myMotorDriver.setDrive( LEFT_MOTOR, 0, 0); //Stop motor
  myMotorDriver.setDrive( RIGHT_MOTOR, 0, 0); //Stop motor
  while (digitalRead(8) == 0); // Hold if jumper is placed between pin 8 and ground

  // read from fabric sensor
  fsrReading = analogRead(fsrAnalogPin);
  Serial.print("Analog reading = ");
  Serial.println(fsrReading);

  //**** Calculating the scale ****//
  // Magnitude:motor-level -> 1:1 2:2 3:4 4:8 5:16 6:32 7:64 8:128 9:256

  // ignoring pressures below 500
  // pressure low = 500; pressure high = 1100
  // motor low = 0; motor high = 255

  if (fsrReading > 500) {
    const int motorLevel = fsrReading / 2.35; // motor drive level scaled from the pressure measured
    myMotorDriver.setDrive( LEFT_MOTOR, 0, motorLevel);
    myMotorDriver.setDrive( RIGHT_MOTOR, 0, 20 + (motorLevel / 2));
  }

  delay(100);

}
