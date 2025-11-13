#ifndef DEFINITIONS_H
#define DEFINITIONS_H

// ===== WIFI CONFIGURATION =====
#define WIFI_SSID "Mochi-Control"
#define WIFI_PASSWORD "mochi123"

// ===== HARDWARE PIN CONFIGURATION =====
// SSD1306 OLED Display (I2C)
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1  // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C  // 0x3C for 0.96" and some 1.3" displays

#define I2C_SDA 20  // GPIO20 - SDA
#define I2C_SCL 21  // GPIO21 - SCL

// Touch Sensor
#define TOUCH_PIN 8  // GPIO8 - TTP233 touch sensor output

// Speaker/Buzzer
#define BUZZER_PIN 10  // GPIO10 - Buzzer

// ===== EMOJI DISPLAY CONFIGURATION =====
#define ANIMATION_FPS 10  // Frames per second for animations
#define IDLE_CHANGE_MIN 3000  // Minimum ms before changing idle expression
#define IDLE_CHANGE_MAX 8000  // Maximum ms before changing idle expression

// ===== EMOJI TYPES =====
enum EmojiType {
  EMOJI_IDLE,
  EMOJI_HAPPY,
  EMOJI_SAD,
  EMOJI_SURPRISED,
  EMOJI_ANGRY,
  EMOJI_SLEEPY,
  EMOJI_LOVE,
  EMOJI_COOL,
  EMOJI_THINKING,
  EMOJI_LAUGHING,
  EMOJI_COUNT  // Total number of emoji types
};

// Serial Debug
#define SERIAL_DEBUG

#ifdef SERIAL_DEBUG
#define DEBUG_PRINT(...) Serial.print(__VA_ARGS__)
#define DEBUG_PRINTLN(...) Serial.println(__VA_ARGS__)
#define DEBUG_PRINTF(...) Serial.printf(__VA_ARGS__)
#else
#define DEBUG_PRINT(...)
#define DEBUG_PRINTLN(...)
#define DEBUG_PRINTF(...)
#endif

#endif
