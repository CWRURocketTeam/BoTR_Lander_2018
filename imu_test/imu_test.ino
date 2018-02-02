#include <BMI160Gen.h>
#include <math.h>
#include <elapsedMillis.h>

const int select_pin = 10;
const int i2c_addr = 0x68;
const double earth_gravity = 9.8;
const double sample_rate = 1.0 / 1600.0;

double accel_conversion_factor;
double total_distance;
double vx;
double vy;
double vz;
int raw_gravx;
int raw_gravy;
int raw_gravz;
int last_ax;
int last_ay;
int last_az;

elapsedMillis sample_time;

int i;

int ax, ay, az;

void setup()
{
  double raw_gravity;
  
  Serial.begin(9600); // initialize Serial communication
  while (!Serial);    // wait for the serial port to open

  BMI160.begin(BMI160GenClass::I2C_MODE, i2c_addr);
  BMI160.setAccelRate(12); //1600Hz

  //Calibrate the accelerometer using earth's gravity
  delay(500);
  Serial.println("Calibrating...");
  BMI160.readAccelerometer(ax, ay, az);
  Serial.println(ax);
  Serial.println(ay);
  Serial.println(az);
  raw_gravx = ax;
  raw_gravy = ay;
  raw_gravz = az;
  raw_gravity = vector_length(ax, ay, az);
  Serial.println(raw_gravity);
  accel_conversion_factor = earth_gravity / raw_gravity;

  Serial.println(convert_raw_accel(raw_gravx));
  Serial.println(convert_raw_accel(raw_gravy));
  Serial.println(convert_raw_accel(raw_gravz));

  total_distance = 0;
  vx = 0;
  vy = 0;
  vz = 0;
  i = 0;
  last_ax = ax;
  last_ay = ay;
  last_az = az;
}

void loop()
{
  double converted_ax, converted_ay, converted_az;
  sample_time = 0;

  BMI160.readAccelerometer(ax, ay, az);
  //Serial.println(curr_accel);

  delay(1);


  total_distance += vector_length(vx, vy, vz) * sample_time / 1000.0;
  converted_ax = convert_raw_accel((last_ax + ax)/2 - raw_gravx);
  converted_ay = convert_raw_accel((last_ay + ay)/2 - raw_gravy);
  converted_az = convert_raw_accel((last_az + az)/2 - raw_gravz);

  if (abs(converted_ax) < 0.1)
    converted_ax = 0.0;
  if (abs(converted_ay) < 0.1)
    converted_ay = 0.0;
  if (abs(converted_az) < 0.1)
    converted_az = 0.0;
  
  vx += converted_ax * sample_time / 1000.0;
  vy += converted_ay * sample_time / 1000.0;
  vz += converted_az * sample_time / 1000.0;

  if (i % 1000 == 0)
  {
    Serial.println("Accel:");

    Serial.println(converted_ax);
    Serial.println(converted_ay);
    Serial.println(converted_az);
    
    Serial.print("Total distance: ");
    Serial.println(total_distance);

    Serial.println("Velocity: ");

    Serial.println(vx);
    Serial.println(vy);
    Serial.println(vz);
  }
  i++;

  last_ax = ax;
  last_ay = ay;
  last_az = az;
}

double vector_length (double x, double y, double z)
{
  return sqrt(x*x + y*y + z*z);
}

double convert_raw_accel(int raw_accel)
{
  return raw_accel * accel_conversion_factor;
}

