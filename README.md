# 🐭 Muse – A Wearable Motion-Controlled Mouse for Disabled Creators
<img src="https://github.com/MadRobin13/Assets/blob/cf013104932ca5b4a5696f4017aa68dd73726bf3/Muse/Muse_white_with_blackBG_wide_logo.png" width="10000px" margin="0 auto"/>
___
**Muse** is an open-source, wearable mouse created **for disabled creators**. It empowers users to access the internet and express their creativity, no matter their mobility challenges. Muse is designed to be affordable, customizable, and adaptable — because assistive technology should serve people, not exploit them.

## ✨ What is Muse?

Muse is a motion-controlled mouse that can be worn on the **head, arm, or any other limb** — tailored to suit each individual’s abilities. Using a gyroscope and accelerometer, it translates body motion into mouse movement, enabling hands-free navigation, communication, and creation.

## 🎯 Key Features

- 🧠 **Head, Arm,and Hand tracking** using the MPU-6050 6-axis gyroscope sensor  
- 📡 **Powered by an ESP32-Wrover** for flexibility and performance  
- 💡 **LCD and DIY Haptic Motor** for interface and feedback
- 🛠️ **3D-printed clamp shell, base, and elastic strap** for wearable comfort and stable structure
- 🧩 **Modular & open-source**: adaptable for various types of disabilities  
- 🔌 **USB-powered** (Currently uses USB-C to draw 5V for power)

## 🧰 Hardware Components

- [ESP32-Wrover](https://www.espressif.com/en/products/socs/esp32-wrover)
- [MPU-6050 Gyroscope + Accelerometer](https://invensense.tdk.com/products/motion-tracking/6-axis/mpu-6050/)
- LCD Display
- Haptic motor for tactile feedback (Uses the small DC motor found in a [Sg90 9g Micro Servo Motor](https://www.amazon.ca/Micro-Helicopter-Airplane-Remote-Control/dp/B072V529YD?th=1))
- [3 way mini toggle switch](https://www.aircraftspruce.ca/catalog/elpages/minitoggle.php?srsltid=AfmBOorh_ywSO5pY5MYrz0jfd-931al7PfeetMWVtHb0YzVKly1kAKRf)
- 3D-printed clampshell case (All files found in the [CAD_designs](https://github.com/MadRobin13/Muse/tree/main/CAD_designs) folder)
- Wearable strap (made using a dollar store elastic strap)
- USB-C to USB-A cable for wired use

## 🔧 How It Works

1. **Wear Muse** on your head, arm, or hand (make sure you select the correct limb using the switch).
3. The **MPU-6050 sensor** detects orientation and movement.
4. The **ESP32** processes this input and sends mouse movement signals to a connected computer.
5. Tactile feedback is provided via the **haptic motor**.
6. Muse communicates via Bluetooth (the LCD will tell you if bluetooth is connected).

## 🚀 Getting Started

1. Flash the firmware to the ESP32-Wrover using [PlatformIO](https://platformio.org/) or the Arduino IDE.
2. Assemble the hardware by inserting the LCD and switch into the lid, putting the gyro and haptic motor (made by combining the 3D-printed weight with the DC motor) into the 3D-printed headband case, attaching the elastic to the headband case, wiring everything, and shoving the components in the big 3D-printed box, while also putting the 3D-printed lid on the headband case.
3. Plug the device into a computer via USB.
4. Strap it on, adjust the fit, and you're ready to go!

## 🤲 Why Muse?

Muse was created because too many assistive technologies are closed, overpriced, and inflexible. People with disabilities deserve tools that are **designed with them in mind**, not products marked up by corporations taking advantage of their needs and marketing rigid, one-size-fits-all solutions.

Muse is:

- **Customizable** to individual needs
- **Open-source** and modifiable
- **Affordable** and replicable
- **Designed to help**, not to profit

This project is about **freedom, accessibility, and dignity** — built with care and community in mind.

## 💬 Contributing

Contributions are welcome! Whether it’s software bugs, hardware tweaks, documentation, or CAD designs, your help can make Muse better for someone who needs it.

First:
```bash
# Clone the repo
git clone https://github.com/MadRobin13/Muse.git
```


Then:

  Flash [main.cpp](https://github.com/MadRobin13/Muse/blob/main/src/main.cpp) to the ESP-32

