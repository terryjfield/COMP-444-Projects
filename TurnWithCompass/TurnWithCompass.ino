/* This example uses the Zumo Shield's onboard magnetometer to help
 * the Zumo make precise 90-degree turns and drive in squares. It uses
 * ZumoMotors, Pushbutton, and ZumoIMU.
 *
 * This program first calibrates the compass to account for offsets in
 *  its output. Calibration is accomplished in setup().
 *
 * In loop(), The driving angle then changes its offset by 90 degrees
 * from the heading every second. Essentially, this navigates the
 * Zumo to drive in square patterns.
 *
 * It is important to note that stray magnetic fields from electric
 * current (including from the Zumo's own motors) and the environment
 * (for example, steel rebar in a concrete floor) might adversely
 * affect readings from the compass and make them less reliable.
 */

#include <Wire.h>
#include <ZumoShield.h>

#define SPEED           200 // Maximum motor speed when going straight; variable speed when turning
#define TURN_BASE_SPEED 100 // Base speed when turning (added to variable speed)


#define CALIBRATION_SAMPLES 70  // Number of compass readings to take when calibrating

// Allowed deviation (in degrees) relative to target angle that must be achieved before driving straight
#define DEVIATION_THRESHOLD 5

ZumoMotors motors;
Pushbutton button(ZUMO_BUTTON);
ZumoIMU imu;

ZumoIMU::vector<int16_t> m_max; // maximum magnetometer values, used for calibration
ZumoIMU::vector<int16_t> m_min; // minimum magnetometer values, used for calibration

// Setup will calibrate our compass by finding maximum/minimum magnetic readings
void setup()
{
  // The highest possible magnetic value to read in any direction is 32767
  // The lowest possible magnetic value to read in any direction is -32767
  ZumoIMU::vector<int16_t> running_min = {32767, 32767, 32767}, running_max = {-32767, -32767, -32767};
  unsigned char index;

  Serial.begin(9600);

  // Initialize the Wire library and join the I2C bus as a master
  Wire.begin();

  // Initialize IMU
  imu.init();

  // Enables accelerometer and magnetometer
  imu.enableDefault();

  imu.configureForCompassHeading();

  button.waitForButton();

  Serial.println("starting calibration");

  // To calibrate the magnetometer, the Zumo spins to find the max/min
  // magnetic vectors. This information is used to correct for offsets
  // in the magnetometer data.
  motors.setLeftSpeed(SPEED);
  motors.setRightSpeed(-SPEED*-1);

  for(index = 0; index < CALIBRATION_SAMPLES; index ++)
  {
    // Take a reading of the magnetic vector and store it in compass.m
    imu.readMag();

    running_min.x = min(running_min.x, imu.m.x);
    running_min.y = min(running_min.y, imu.m.y);

    running_max.x = max(running_max.x, imu.m.x);
    running_max.y = max(running_max.y, imu.m.y);

    Serial.println(index);

    delay(50);
  }

  motors.setLeftSpeed(0);
  motors.setRightSpeed(0);

  Serial.print("max.x   ");
  Serial.print(running_max.x);
  Serial.println();
  Serial.print("max.y   ");
  Serial.print(running_max.y);
  Serial.println();
  Serial.print("min.x   ");
  Serial.print(running_min.x);
  Serial.println();
  Serial.print("min.y   ");
  Serial.print(running_min.y);
  Serial.println();

  // Store calibrated values in m_max and m_min
  m_max.x = running_max.x;
  m_max.y = running_max.y;
  m_min.x = running_min.x;
  m_min.y = running_min.y;

  button.waitForButton();
}

void loop()
{
  float heading, relative_heading;
  int speed;
  static float target_heading = averageHeading();

  // Heading is given in degrees away from the magnetic vector, increasing clockwise
  heading = averageHeading();

  // This gives us the relative heading with respect to the target angle
  relative_heading = relativeHeading(heading, target_heading);

  Serial.print("Target heading: ");
  Serial.print(target_heading);
  Serial.print("    Actual heading: ");
  Serial.print(heading);
  Serial.print("    Difference: ");
  Serial.print(relative_heading);

  // If the Zumo has turned to the direction it wants to be pointing, go straight and then do another turn
  if(abs(relative_heading) < DEVIATION_THRESHOLD)
  {
    motors.setSpeeds(SPEED, SPEED*-1);

    Serial.print("   Straight");

    delay(1000);

    // Turn off motors and wait a short time to reduce interference from motors
    motors.setSpeeds(0, 0);
    delay(100);

    // Turn 90 degrees relative to the direction we are pointing.
    // This will help account for variable magnetic field, as opposed
    // to using fixed increments of 90 degrees from the initial
    // heading (which might have been measured in a different magnetic
    // field than the one the Zumo is experiencing now).
    // Note: fmod() is floating point modulo
    target_heading = fmod(averageHeading() + 90, 360);
  }
  else
  {
    // To avoid overshooting, the closer the Zumo gets to the target
    // heading, the slower it should turn. Set the motor speeds to a
    // minimum base amount plus an additional variable amount based
    // on the heading difference.

    speed = SPEED*relative_heading/180;

    if (speed < 0)
      speed -= TURN_BASE_SPEED;
    else
      speed += TURN_BASE_SPEED;

    motors.setSpeeds(speed, -speed*-1);

    Serial.print("   Turn");
  }
  Serial.println();
}

// Converts x and y components of a vector to a heading in degrees.
// This calculation assumes that the Zumo is always level.
template <typename T> float heading(ZumoIMU::vector<T> v)
{
  float x_scaled =  2.0*(float)(v.x - m_min.x) / (m_max.x - m_min.x) - 1.0;
  float y_scaled =  2.0*(float)(v.y - m_min.y) / (m_max.y - m_min.y) - 1.0;

  float angle = atan2(y_scaled, x_scaled)*180 / M_PI;
  if (angle < 0)
    angle += 360;
  return angle;
}

// Yields the angle difference in degrees between two headings
float relativeHeading(float heading_from, float heading_to)
{
  float relative_heading = heading_to - heading_from;

  // constrain to -180 to 180 degree range
  if (relative_heading > 180)
    relative_heading -= 360;
  if (relative_heading < -180)
    relative_heading += 360;

  return relative_heading;
}

// Average 10 vectors to get a better measurement and help smooth out
// the motors' magnetic interference.
float averageHeading()
{
  ZumoIMU::vector<int32_t> avg = {0, 0, 0};

  for(int i = 0; i < 10; i ++)
  {
    imu.readMag();
    avg.x += imu.m.x;
    avg.y += imu.m.y;
  }
  avg.x /= 10.0;
  avg.y /= 10.0;

  // avg is the average measure of the magnetic vector.
  return heading(avg);
}
