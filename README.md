# 🐭 Muse – A Wearable Motion-Controlled Mouse for Disabled Creators (WIP)

**Muse** is an open-source, wearable mouse created **for disabled creators**. It empowers users to access the internet and express their creativity, no matter their mobility challenges. Muse is designed to be affordable, customizable, and adaptable — because assistive technology should serve people, not exploit them.

## ✨ What is Muse?

Muse is a motion-controlled mouse that can be worn on the **head, arm, or any other limb** — tailored to suit each individual’s abilities. Using a gyroscope and accelerometer, it translates body motion into mouse movement, enabling hands-free navigation, communication, and creation.

## 🎯 Key Features

- 🧠 **Head or limb tracking** using the MPU-6050 motion sensor  
- 📡 **Powered by ESP32-Wrover** for flexibility and performance  
- 💡 **LCD and LEDs** for interface and feedback (may include haptic motor in future versions)  
- 🛠️ **3D-printed clamp shell and strap** for wearable comfort  
- 🧩 **Modular & open-source**: adaptable for various types of disabilities  
- 🔌 **USB-powered** (wireless and battery options are being explored)

## 🧰 Hardware Components

- [ESP32-Wrover](https://www.espressif.com/en/products/socs/esp32-wrover)
- [MPU-6050 Gyroscope + Accelerometer](https://invensense.tdk.com/products/motion-tracking/6-axis/mpu-6050/)
- LCD Display
- LEDs
- (Optional) Haptic motor for tactile feedback
- 3D-printed clampshell case
- Wearable strap
- USB cable for wired use

## 🔧 How It Works

1. **Wear Muse** on your head, arm, or other preferred body part.
2. The **MPU-6050 sensor** detects orientation and movement.
3. The **ESP32** processes this input and sends mouse movement signals to a connected computer.
4. Visual or tactile feedback is provided via **LEDs, an LCD**, or **(possibly) a haptic motor**.
5. Muse communicates via USB or Bluetooth (depending on future revisions).

## 🚀 Getting Started

1. Flash the firmware to the ESP32-Wrover using [PlatformIO](https://platformio.org/) or Arduino IDE.
2. Assemble the hardware based on the included schematic in `/hardware/`.
3. Plug the device into a computer via USB.
4. Strap it on, adjust the fit, and you're ready to navigate.

## 🤲 Why Muse?

Muse was created because too many assistive technologies are closed, overpriced, and inflexible. People with disabilities deserve tools that are **designed with them in mind**, not products marked up by corporations taking advantage of their needs.

Muse is:

- **Customizable** to individual needs
- **Open-source** and hackable
- **Affordable** and replicable
- **Designed to empower**, not to profit

This project is about **freedom, accessibility, and dignity** — built with care and community in mind.

## 💬 Contributing

Contributions are welcome! Whether it’s software bugs, hardware tweaks, documentation, or design, your help can make Muse better for someone who needs it.

```bash
# Clone the repo
git clone https://github.com/MadRobin13/Muse.git
