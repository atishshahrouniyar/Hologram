#include "Wire.h"
#include "SoftwareSerial.h"

/*
 * Accelerator
 * g(bit) --> sesitivity in lsb/g
 * 2g(0b00000000) --> 16384
 * 4g(0b00001000) --> 8192
 * 8g(0b00010000) --> 4096
 * 16g(0b00011000) --> 2048
 * 
 * Gyroscope
 * deg/sec(bit) --> sensitivity in lsb/deg/s
 * 250(0b00000000) --> 131
 * 500(0b00001000) --> 65.5
 * 1000(0b00010000) --> 32.8
 * 2000(0b00011000) --> 16.4
*/

const int Mpu_LOCATION = 0b1101000;
const unsigned char Accelerator_RANGE = 0b00000000;
const float Accelerator_SENSE = 16384;
const unsigned char Gyroscope_RANGE = 0b00000000;
const float Gyroscope_SENSE = 131;

SoftwareSerial Bluetooth(2, 3);

float g_force_x, g_force_y, g_force_z;
float alpha, beta, gamma;

void setup() {
  Serial.begin(9600);
  Wire.begin();
  Bluetooth.begin(9600);

  // disable SLEEP mode
  Wire.beginTransmission(Mpu_LOCATION);
    Wire.write(0x6B);
    Wire.write(0b00000000);
  Wire.endTransmission();

  // Setting up Accel
  Wire.beginTransmission(Mpu_LOCATION);
    Wire.write(0x1C);
    Wire.write(Accelerator_RANGE);
  Wire.endTransmission();
  
  // Setting up Gyro
  Wire.beginTransmission(Mpu_LOCATION);
    Wire.write(0x1B);
    Wire.write(Gyroscope_RANGE);
  Wire.endTransmission();
}

void print3(const char* title, float x, float y, float z) {
  Serial.print(title);
  Serial.print("(");
  Serial.print(x);
  Serial.print(", ");
  Serial.print(y);
  Serial.print(", ");
  Serial.print(z);
  Serial.println(")");
}

void loop() {
  // Reading from Accel register
  Wire.beginTransmission(Mpu_LOCATION);
    Wire.write(0x3B);
  Wire.endTransmission();
  Wire.requestFrom(Mpu_LOCATION, 6);
  while (Wire.available() < 6);
  int acc_x = Wire.read() << 8 | Wire.read();
  int acc_y = Wire.read() << 8 | Wire.read();
  int acc_z = Wire.read() << 8 | Wire.read();
  alpha = (float)acc_x / Accelerator_SENSE;
  beta = (float)acc_y / Accelerator_SENSE;
  gamma = (float)acc_z / Accelerator_SENSE;

  // Reading from Gyro register
  Wire.beginTransmission(Mpu_LOCATION);
    Wire.write(0x43);
  Wire.endTransmission();
  Wire.requestFrom(Mpu_LOCATION, 6);
  while (Wire.available() < 6);
  int gyro_x = Wire.read() << 8 | Wire.read();
  int gyro_y = Wire.read() << 8 | Wire.read();
  int gyro_z = Wire.read() << 8 | Wire.read();
  g_force_x = (float)gyro_x / Gyroscope_SENSE;
  g_force_y = (float)gyro_y / Gyroscope_SENSE;
  g_force_z = (float)gyro_z / Gyroscope_SENSE;

//  print3("Acc", alpha, beta, gamma);
//  print3("Gyro", g_force_x, g_force_y, g_force_z);
    while (Bluetooth.available()) {
      Serial.print(Bluetooth.read());
    }
  delay(500);
}
