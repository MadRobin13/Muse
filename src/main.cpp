#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <BleMouse.h>
#include <USB.h>
#include <esp_bt.h>
#include <esp_pm.h>
#include <esp_wifi.h>
#include <LiquidCrystal.h>
#include <string.h>

Adafruit_MPU6050 mpu;
const int rs = 26, en = 27, d4 = 5, d5 = 4, d6 = 0, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

BleMouse bleMouse("Muse");

float elapsedTime, currentTime, previousTime;
float roll, yaw = 0.0;

bool isConnected = false;
bool rightButtonPressed = false;
bool leftButtonPressed = false;
bool usingHead = true;
bool usingHand = false;
bool usingArm = false;

int16_t accX, accY, accZ;
int16_t gyroX, gyroY, gyroZ;

int16_t prevGyroX = 0;
int16_t prevGyroY = 0;
int16_t prevGyroZ = 0;

int16_t joystickFilter(int current, int previous, int coefficient)
{
	return (previous * (coefficient - 1) + current) / coefficient;
}

void keepMouseAlive()
{
	if (bleMouse.isConnected())
	{
		if (millis() % 1000 == 0)
		{
			bleMouse.move(1, 0);
		}
		else if (millis() % 1000 == 500)
		{
			bleMouse.move(-1, 0);
		}
	}
}

void setup()
{
	Serial.begin(115200);
	pinMode(15, OUTPUT);
	analogWrite(15, 0);

	esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT);
	setCpuFrequencyMhz(240);

#ifdef CONFIG_PM_ENABLE
	esp_pm_config_t pm_config = {
		.max_freq_mhz = 240,
		.min_freq_mhz = 240,
		.light_sleep_enable = false};
	esp_pm_configure(&pm_config);
#endif

	esp_wifi_set_ps(WIFI_PS_NONE);
	bleMouse.begin();

	Wire.begin();
	if (!mpu.begin())
	{
		Serial.println("Failed to find MPU6050 chip");
		while (1)
			delay(10);
	}

	lcd.begin(16, 2);
	lcd.clear();
	previousTime = millis();

	pinMode(12, INPUT_PULLDOWN);
	pinMode(14, INPUT_PULLDOWN);
}

void loop()
{
	currentTime = millis();
	elapsedTime = (currentTime - previousTime) / 1000.0;
	previousTime = currentTime;

	sensors_event_t a, g, temp;
	mpu.getEvent(&a, &g, &temp);

	accX = (int16_t)(a.acceleration.x * 1000);
	accY = (int16_t)(a.acceleration.y * 1000);
	accZ = (int16_t)(a.acceleration.z * 1000);
	gyroX = (int16_t)(g.gyro.x * 1000);
	gyroY = (int16_t)(g.gyro.y * 1000);
	gyroZ = (int16_t)(g.gyro.z * 1000);

	roll = atan2(accY, sqrt(pow(accX, 2) + pow(accZ, 2))) * 180 / PI;
	yaw += gyroZ * elapsedTime * 180 / PI;
	yaw = fmod(yaw, 360.0);

	if (bleMouse.isConnected())
	{
		isConnected = true;

		if (usingHead)
		{
			gyroX = joystickFilter(gyroX, prevGyroX, 6);
			gyroY = joystickFilter(gyroY, prevGyroY, 2);
			prevGyroX = gyroX;
			prevGyroY = gyroY;

			float sensitivityX = 0.01;
			float sensitivityY = 0.01;
			int8_t moveX = -(int8_t)(gyroX * sensitivityX);
			int8_t moveY = (int8_t)(gyroY * sensitivityY);

			if (abs(moveX) > 1 || abs(moveY) > 1)
			{
				bleMouse.move(moveX, moveY);
			}

			if (accY > 5000 && !rightButtonPressed)
			{
				rightButtonPressed = true;
				bleMouse.press(MOUSE_RIGHT);
				analogWrite(15, 60);
			}
			else if (accY < 5000 && rightButtonPressed)
			{
				rightButtonPressed = false;
				bleMouse.release(MOUSE_RIGHT);
				analogWrite(15, 0);
			}

			if (accY < -5000 && !leftButtonPressed)
			{
				leftButtonPressed = true;
				bleMouse.press(MOUSE_LEFT);
				analogWrite(15, 60);
			}
			else if (accY > -5000 && leftButtonPressed)
			{
				leftButtonPressed = false;
				bleMouse.release(MOUSE_LEFT);
				analogWrite(15, 0);
			}
		}
		else if (usingHand)
		{
			gyroX = joystickFilter(gyroX, prevGyroX, 5);
			gyroY = joystickFilter(gyroY, prevGyroY, 5);
			prevGyroX = gyroX;
			prevGyroY = gyroY;

			float sensitivityX = 0.005;
			float sensitivityY = 0.005;
			int8_t moveX = (int8_t)(gyroZ * sensitivityX);
			int8_t moveY = (int8_t)(gyroY * sensitivityY);

			if (abs(moveX) > 1 || abs(moveY) > 1)
			{
				bleMouse.move(moveX, moveY);
			}

			if (gyroX < -55 && !rightButtonPressed)
			{
				rightButtonPressed = true;
				bleMouse.press(MOUSE_RIGHT);
				analogWrite(15, 60);
			}
			else if (gyroX > -55 && rightButtonPressed)
			{
				rightButtonPressed = false;
				bleMouse.release(MOUSE_RIGHT);
				analogWrite(15, 0);
			}

			if (gyroX > 55 && !leftButtonPressed)
			{
				leftButtonPressed = true;
				bleMouse.press(MOUSE_LEFT);
				analogWrite(15, 60);
			}
			else if (gyroX < 55 && leftButtonPressed)
			{
				leftButtonPressed = false;
				bleMouse.release(MOUSE_LEFT);
				analogWrite(15, 0);
			}
		}
		else if (usingArm)
		{
			gyroZ = joystickFilter(gyroZ, prevGyroZ, 20);
			gyroY = joystickFilter(gyroY, prevGyroY, 10);
			prevGyroZ = gyroZ;
			prevGyroY = gyroY;

			float sensitivityX = 0.005;
			float sensitivityY = 0.005;
			int8_t moveX = -(int8_t)(gyroZ * sensitivityX);  // Use third axis for X
			int8_t moveY = -(int8_t)(gyroY * sensitivityY);

			if (abs(moveX) > 1 || abs(moveY) > 1)
			{
				bleMouse.move(moveX, moveY);
			}

			if (accY > 5000 && !rightButtonPressed)  // Use accX for click now
			{
				rightButtonPressed = true;
				bleMouse.press(MOUSE_RIGHT);
				analogWrite(15, 60);
			}
			else if (accY < 5000 && rightButtonPressed)
			{
				rightButtonPressed = false;
				bleMouse.release(MOUSE_RIGHT);
				analogWrite(15, 0);
			}

			if (accY < -5000 && !leftButtonPressed)
			{
				leftButtonPressed = true;
				bleMouse.press(MOUSE_LEFT);
				analogWrite(15, 60);
			}
			else if (accY > -5000 && leftButtonPressed)
			{
				leftButtonPressed = false;
				bleMouse.release(MOUSE_LEFT);
				analogWrite(15, 0);
			}
		}

		keepMouseAlive();
	}
	else
	{
		isConnected = false;
		analogWrite(15, 0);
	}

	// LCD Output

	if (millis() - previousTime > 1000)
	{
		lcd.clear();
		previousTime = millis();
	}

	lcd.setCursor(0, 0);
	lcd.print("BT: ");
	lcd.print(isConnected ? "Connected " : "Disconnected");

	lcd.setCursor(0, 1);
	lcd.print("Type: ");

	if (digitalRead(12) == HIGH && digitalRead(14) == LOW)
	{
		lcd.print("Head   ");
		usingHead = true;
		usingHand = false;
		usingArm = false;
	}
	else if (digitalRead(14) == HIGH && digitalRead(12) == LOW)
	{ 
		lcd.print("Hand  ");
		usingHead = false;
		usingHand = true;
		usingArm = false;
	}
	else if (digitalRead(12) == LOW && digitalRead(14) == LOW)
	{
		lcd.print("Arm    ");
		usingHead = false;
		usingHand = false;
		usingArm = true;
	}
	else
	{
		lcd.print("Error  ");
	}
}
