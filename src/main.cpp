#include <Wire.h>
#include <MPU6050.h>
#include <BleMouse.h>
#include <USB.h>
#include <esp_bt.h>
#include <esp_pm.h>
#include <esp_wifi.h>
#include <LiquidCrystal.h>
#include <string.h>


MPU6050 mpu;
const int rs = 26, en = 27, d4 = 5, d5 = 4, d6 = 0, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

BleMouse bleMouse("Muse");

float elapsedTime, currentTime, previousTime;
float accAngleX, roll, yaw = 0.0;

bool isConnected = false;

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
      bleMouse.move(10, 0);
    } else if (millis() % 1000 == 500) {
      bleMouse.move(-10, 0);
    }
  }
}

void updateLCD(bool isConnected, String type, LiquidCrystal &lcd) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("BT: ");
  lcd.print(isConnected ? "Connected" : "Disconnected");
  lcd.setCursor(0, 1);
  lcd.print("Type: ");
  lcd.print(type[0,sizeof(type)], 1);
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

  lcd.begin(16, 2);
  lcd.setCursor(0, 1);
  // lcd.print("Muse Mouse");

  pinMode(12, INPUT);
  pinMode(14, INPUT);
}

void loop() {
  currentTime = millis();
  elapsedTime = (currentTime - previousTime) / 1000.0;
  previousTime = currentTime;

  mpu.getMotion6(&accX, &accY, &accZ, &gyroX, &gyroY, &gyroZ);

  accAngleX = atan(accY / sqrt(pow(accX, 2) + pow(accZ, 2))) * 180 / PI;
  roll = (0.96 * (roll + gyroX * elapsedTime) + 0.04 * accAngleX) * 180 / PI;
  yaw += gyroZ * elapsedTime * 180 / PI;

  gyroX = joystickFilter(gyroX, prevGyroX, 6);
  gyroY = joystickFilter(gyroY, prevGyroY, 20);
  prevGyroX = gyroX;
  prevGyroY = gyroY;

  if (bleMouse.isConnected()) {
    isConnected = true;
    float sensitivityX = 0.001;
    float sensitivityY = 0.0007;
    int8_t moveX = -(int8_t)(gyroX * sensitivityX);
    int8_t moveY = (int8_t)(gyroY * sensitivityY);

    if (abs(moveX) > motionThreshold || abs(moveY) > motionThreshold) {
      bleMouse.move(moveX, moveY);
      // lastRealMotionTime = millis();
    }

    if (abs(roll) > 50) {
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

  else {
    isConnected = false;
  }

    // digitalWrite(15, HIGH);
    // sleep(1);
    // digitalWrite(15, LOW);
    // sleep(1);
  // lcd.clear();
  // lcd.setCursor(0, 0);
  // lcd.print("Muse Mouse");
  lcd.setCursor(0, 0);
  lcd.print("BT: ");
  lcd.print(isConnected ? "Connected" : "Disconnected");
  lcd.clear();

  if (digitalRead(12) == HIGH) {
    // updateLCD(isConnected, "Head", lcd);
    lcd.setCursor(0, 1);
    lcd.print("Type: ");
    lcd.print("Head");
  } else if (digitalRead(14) == HIGH) {
    // updateLCD(isConnected, "Hand", lcd);
    lcd.setCursor(0, 1);
    lcd.print("Type: ");
    lcd.print("Hand");
  } else {
    // updateLCD(isConnected, "Arm", lcd);
    lcd.setCursor(0, 1);
    lcd.print("Type: ");
    lcd.print("Arm");
  }

}