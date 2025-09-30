# Water Management System (ESP-IDF)

This repository contains the **ESP-IDF equivalent** of my Arduino IDE (C++) based Water Management System, which is also available on my GitHub.  

Key highlights:
- Reimplemented drivers for **LCD over I2C** and **Blynk Cloud** (using REST API + HTTP client).
- Converted the original Arduino C++ codebase to **pure ESP-IDF C**.
- Explored ESP32 at a lower level by diving into ESP-IDF source code and writing custom drivers instead of relying on prebuilt Arduino libraries.

⚠️ **Note:**  
This project was originally developed over 2 years ago. Hardware is not currently available to me, so this ESP-IDF codebase has not been fully hardware-tested. However, it **successfully compiles** with ESP-IDF.

The main purpose of this project was to gain **hands-on experience with ESP32 and ESP-IDF**, including:
- Understanding the internal implementation of libraries.
- Practicing bare-metal style driver writing.
- Bridging Arduino-style code to native ESP-IDF.

Feel free to explore, use, or contribute. If you run into issues or have questions, please reach out!
