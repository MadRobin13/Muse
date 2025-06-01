#include <Wire.h>
#include <MPU6050.h>
#include <BleMouse.h>

MPU6050 mpu;

BleMouse bleMouse;

float accAngleX, accAngleY;
float gyroAngleX, gyroAngleY, gyroAngleZ;
float roll, pitch, yaw;
float elapsedTime, currentTime, previousTime;
int16_t gyroX, gyroY, gyroZ;
int16_t accX, accY, accZ;

void setup() {

  Serial.begin(115200);

  bleMouse.begin();

  Wire.begin();

  mpu.initialize();

  previousTime = millis();

}

void loop() {
  currentTime = millis();
  elapsedTime = (currentTime - previousTime) / 1000.0;
  previousTime = currentTime;

  mpu.getMotion6(&accX, &accY, &accZ, &gyroX, &gyroY, &gyroZ);

  accAngleX = atan(accY / sqrt(pow(accX, 2) + pow(accZ, 2))) * 180 / PI;
  accAngleY = atan(-accX / sqrt(pow(accY, 2) + pow(accZ, 2))) * 180 / PI;

  gyroAngleX += gyroX / 131.0 * elapsedTime;
  gyroAngleY += gyroY / 131.0 * elapsedTime;
  gyroAngleZ += gyroZ / 131.0 * elapsedTime;

  roll = 0.96 * (roll + gyroX / 131.0 * elapsedTime) + 0.04 * accAngleX;
  pitch = 0.96 * (pitch + gyroY / 131.0 * elapsedTime) + 0.04 * accAngleY;
  yaw += gyroZ / 131.0 * elapsedTime;

  Serial.print(" Roll: "); Serial.print(roll);
  Serial.print(" Pitch: "); Serial.print(pitch);
  Serial.print(" Yaw: "); Serial.println(yaw);

  if(bleMouse.isConnected()) {
    bleMouse.move(gyroX * 0.0001, gyroZ * 0.0001);
  }
}
