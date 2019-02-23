
#include <Arduino.h>
#include "base_functions.h"

#include <MPU6050_tockn.h>
#include <Wire.h>

MPU6050 mpu6050(Wire);

long timer = 0;

void setup() {
  Serial.begin(115200);
  Wire.begin();
  mpu6050.begin();
  // mpu6050.calcGyroOffsets(true);
  mpu6050.setGyroOffsets(-1.84, 0.08, -0.04);
}

void loop() {
  mpu6050.update();

  if(millis() - timer > 1000){

    DL("=======================================================");
    DF("temp: %.2f\n", mpu6050.getTemp());
    DF("acc:      (%.2f, %.2f, %.2f)\n", mpu6050.getAccX(), mpu6050.getAccY(), mpu6050.getAccZ());
    DF("gyro:     (%.2f, %.2f, %.2f)\n", mpu6050.getGyroX(), mpu6050.getGyroY(), mpu6050.getGyroZ());
    DF("accAngle: (%.2f, %.2f)\n", mpu6050.getAccAngleX(), mpu6050.getAccAngleY());

    DF("gyroAngle: (%.2f, %.2f, %.2f)\n", mpu6050.getGyroAngleX(), mpu6050.getGyroAngleY(), mpu6050.getGyroAngleZ());
    DF("angle: (%.2f, %.2f, %.2f)\n", mpu6050.getAngleX(), mpu6050.getAngleY(), mpu6050.getAngleZ());

    timer = millis();
  }
}
