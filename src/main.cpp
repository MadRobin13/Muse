#include <Wire.h>
#include <MPU6050.h>
#include <BleMouse.h>
#include <USB.h>
#include <esp_bt.h>
#include <esp_pm.h>
#include <esp_wifi.h>

MPU6050 mpu;
BleMouse bleMouse("Muse");

float elapsedTime, currentTime, previousTime;

int16_t accX, accY, accZ;
int16_t gyroX, gyroY, gyroZ;

int16_t prevGyroX = 0;
int16_t prevGyroY = 0;

int16_t joystickFilter(int current, int previous, int coefficient) {
  return (previous * (coefficient - 1) + current) / coefficient;
}

void keepMouseALive() {
  if (bleMouse.isConnected()) {
    if (millis() % 1000 == 0) {
      bleMouse.move(0.0001, 0);
    } else if (millis() % 1000 == 500) {
      bleMouse.move(-0.0001, 0);
    }
  }
}

static unsigned long lastKeepAliveTime = 0;
const unsigned long keepAliveInterval = 3000;

static unsigned long lastRealMotionTime = 0;
const unsigned long motionThreshold = 0;

void setup() {
  Serial.begin(115200);
  pinMode(15, OUTPUT);

  esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT);
  setCpuFrequencyMhz(240);

  #ifdef CONFIG_PM_ENABLE
    esp_pm_config_t pm_config = {
      .max_freq_mhz = 240,
      .min_freq_mhz = 240,
      .light_sleep_enable = false
    };
    esp_pm_configure(&pm_config);
  #endif

  esp_wifi_set_ps(WIFI_PS_NONE);

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

  gyroX = joystickFilter(gyroX, prevGyroX, 6);
  gyroY = joystickFilter(gyroY, prevGyroY, 20);
  prevGyroX = gyroX;
  prevGyroY = gyroY;

  if (bleMouse.isConnected()) {
    float sensitivityX = 0.001;
    float sensitivityY = 0.0005;
    int8_t moveX = -(int8_t)(gyroX * sensitivityX);
    int8_t moveY = (int8_t)(gyroY * sensitivityY);

    if (abs(moveX) > motionThreshold || abs(moveY) > motionThreshold) {
      bleMouse.move(moveX, moveY);
      // lastRealMotionTime = millis();
    }

    if (abs(accY) > 6000) {
      if (!bleMouse.isPressed(MOUSE_LEFT)) {
        bleMouse.press(MOUSE_LEFT);
      }
      digitalWrite(15, HIGH);
    } else {
      if (bleMouse.isPressed(MOUSE_LEFT)) {
        bleMouse.release(MOUSE_LEFT);
      }
      digitalWrite(15, LOW);
    }

    // if (millis() - lastKeepAliveTime > keepAliveInterval && millis() - lastRealMotionTime > keepAliveInterval) {
    // bleMouse.move(10, 0);
    // delay(1000);
    // bleMouse.move(-10, 0);
    // delay(1000);
      // lastKeepAliveTime = millis();
    // }

    keepMouseALive();
  }
}