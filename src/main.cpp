#include <Wire.h>
// #include <MPU6050.h>
#include <Adafruit_MPU6050.h>
#include <BleMouse.h>
#include <USB.h>
#include <esp_bt.h>
#include <esp_pm.h>
#include <esp_wifi.h>
#include <LiquidCrystal.h>
#include <string.h>


// MPU6050 mpu;
Adafruit_MPU6050 mpu;
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
  // mpu.initialize();
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }

  previousTime = millis();

  lcd.begin(16, 2);
  lcd.setCursor(0, 1);
  // lcd.print("Muse Mouse");

  pinMode(12, INPUT_PULLDOWN);
  pinMode(14, INPUT_PULLDOWN);
}

void loop() {
  currentTime = millis();
  elapsedTime = (currentTime - previousTime) / 1000.0;
  previousTime = currentTime;

  // mpu.getMotion6(&accX, &accY, &accZ, &gyroX, &gyroY, &gyroZ);
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  // accAngleX = atan(accY / sqrt(pow(accX, 2) + pow(accZ, 2))) * 180 / PI;
  // roll = (int)((0.96 * (roll + gyroX * elapsedTime) + 0.04 * accAngleX) * 180 / PI) % 360;
  // yaw += (int)(gyroZ * elapsedTime * 180 / PI);
  // yaw = (int)yaw % 360;

  accX = (int16_t)a.acceleration.x * 1000;
  accY = (int16_t)a.acceleration.y * 1000;
  accZ = (int16_t)a.acceleration.z * 1000;
  gyroX = (int16_t)g.gyro.x * 1000;
  gyroY = (int16_t)g.gyro.y * 1000;
  gyroZ = (int16_t)g.gyro.z * 1000;

  roll = atan2(accY, sqrt(pow(accX, 2) + pow(accZ, 2))) * 180 / PI;
  yaw += gyroZ * elapsedTime * 180 / PI;
  yaw = fmod(yaw, 360.0);
  
  gyroX = joystickFilter(gyroX, prevGyroX, 6);
  gyroY = joystickFilter(gyroY, prevGyroY, 20);
  prevGyroX = gyroX;
  prevGyroY = gyroY;

    Serial.printf("Acc: X=%d, Y=%d, Z=%d | Gyro: X=%d, Y=%d, Z=%d | Roll: %.2f | Yaw: %.2f\n",
                  accX, accY, accZ, gyroX, gyroY, gyroZ, roll, yaw);

  if (bleMouse.isConnected()) {
    isConnected = true;
    float sensitivityX = 0.02;
    float sensitivityY = 0.04;
    int8_t moveX = -(int8_t)(gyroX * sensitivityX);
    int8_t moveY = (int8_t)(gyroY * sensitivityY);

    if (abs(moveX) > motionThreshold || abs(moveY) > motionThreshold) {
      bleMouse.move(moveX, moveY);
      // lastRealMotionTime = millis();
    }

    if (accY > 5000) {
      bleMouse.press(MOUSE_RIGHT);
      // digitalWrite(15, HIGH);
    } else {
        bleMouse.release(MOUSE_RIGHT);
        // digitalWrite(15, LOW);
    }

    if (accY < -3000) {
      bleMouse.press(MOUSE_LEFT);
      // digitalWrite(15, HIGH);
    } else {
        bleMouse.release(MOUSE_LEFT);
        // digitalWrite(15, LOW);
    }

    keepMouseALive();
  } else {
    isConnected = false;
    digitalWrite(15, LOW);
  }

  lcd.setCursor(0, 0);
  lcd.print("BT: ");
  lcd.print(isConnected ? "Connected" : "Disconnected");
  lcd.setCursor(0, 1);
  lcd.print("Type: ");

  if (digitalRead(12) == HIGH && digitalRead(14) == LOW) {
    // updateLCD(isConnected, "Head", lcd);
    lcd.print("Head");
  } else if (digitalRead(14) == HIGH && digitalRead(12) == LOW) {
    // updateLCD(isConnected, "Hand", lcd);
    lcd.print("Hand");
  } else if (digitalRead(12) == LOW && digitalRead(14) == LOW) {
    // updateLCD(isConnected, "Arm", lcd);
    lcd.print("Arm");

  } else {
    lcd.setCursor(0, 1);
    lcd.print("Error");
  }

  lcd.clear();

}