#include <Wire.h>
#include <ZumoShield.h> // Pololu's Zumo Shield library

ZumoIMU imu;
ZumoMotors motors;
Pushbutton button(ZUMO_BUTTON);

void setup() {
  Serial.begin(9600);
  Wire.begin();

  // Initialize the IMU (includes magnetometer)
  if (!imu.init()) {
    Serial.println("Failed to initialize IMU.");
    while (1);
  }

  // Enable the magnetometer
  imu.enableDefault();

  button.waitForButton();
  motors.setLeftSpeed(100);
  motors.setRightSpeed(-100);
}

void loop() {
  // Read magnetometer data
  imu.read();

  // Calculate the compass heading (in degrees)
  float heading = atan2(imu.m.y, imu.m.x) * 180.0 / PI;

  // Ensure the heading is within 0-360 degrees
  if (heading < 0) {
    heading += 360.0;
  }

  // Print the heading to the Serial Monitor
  Serial.print("Heading: ");
  Serial.print(heading);
  Serial.print("° x:");
  Serial.print(imu.m.x);
  Serial.print(" y:");
  Serial.println(imu.m.y);
  

  delay(500);
}
