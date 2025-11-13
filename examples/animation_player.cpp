// Example: Play an animated GIF on the display
// This shows how to use the converted bitmap animations

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "definitions.h"
#include "animations/smile_bitmap.h"  // Include your converted animation

// Create display object
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Animation state
int currentFrame = 0;
unsigned long lastFrameTime = 0;

void setup() {
  Serial.begin(115200);
  Serial.println("Animation Player Example");
  
  // Initialize I2C and display
  Wire.begin(I2C_SDA, I2C_SCL);
  
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    while(1);
  }
  
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(20, 28);
  display.println(F("Animation Ready!"));
  display.display();
  delay(1000);
  
  Serial.println(F("Display initialized"));
  Serial.print(F("Animation frames: "));
  Serial.println(SMILE_FRAMES);
}

void loop() {
  unsigned long currentTime = millis();
  
  // Get frame duration from PROGMEM
  uint16_t frameDuration = pgm_read_word(&smile_durations[currentFrame]);
  
  // Check if it's time to advance to next frame
  if (currentTime - lastFrameTime >= frameDuration) {
    lastFrameTime = currentTime;
    
    // Clear display
    display.clearDisplay();
    
    // Get frame bitmap from PROGMEM
    const unsigned char* frame = (const unsigned char*)pgm_read_ptr(&smile_frames[currentFrame]);
    
    // Draw the frame (centered)
    display.drawBitmap(0, 0, frame, SMILE_WIDTH, SMILE_HEIGHT, SSD1306_WHITE);
    
    // Display it
    display.display();
    
    // Advance to next frame (loop back to 0 after last frame)
    currentFrame = (currentFrame + 1) % SMILE_FRAMES;
    
    // Debug output
    Serial.print(F("Frame: "));
    Serial.print(currentFrame);
    Serial.print(F(" / "));
    Serial.println(SMILE_FRAMES);
  }
}

